#include "httpManager.h"
#include <QNetworkReply>

httpManager::~httpManager()
{

}

httpManager::httpManager() {

    connect(this,&httpManager::sig_http_finish,this,&httpManager::slot_http_finish);
}

void httpManager::PostHttpReq(QUrl url, QJsonObject json, ReqId req_id, Modules mode)
{

    //创建http post请求
    QByteArray data=QJsonDocument(json).toJson();

    //通过url构造请求
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader,"application/json");
    request.setHeader(QNetworkRequest::ContentLengthHeader,QByteArray::number(data.length()));

    //发送请求，并响应请求，获取自己的智能指针，构造伪闭包增加智能指针引用技术
    auto self=shared_from_this();
    QNetworkReply* reply=_manager.post(request,data);

    //设置信号和槽等待发送完成
    connect(reply,&QNetworkReply::finished,[reply,self,req_id,mode](){
        if(reply->error()!=QNetworkReply::NoError){
            qDebug()<<reply->errorString();

            emit self->sig_http_finish(req_id,"",ErrorCode::ERR_NETWORK,mode);
            reply->deleteLater();
            return;
        }

        //无错误
        QString ret=reply->readAll();

        //发送信号通知完成
        emit self->sig_http_finish(req_id,ret,ErrorCode::SUCCESS,mode);
        reply->deleteLater();
        return;
    });


}

void httpManager::slot_http_finish(ReqId id, QString res, ErrorCode errcode, Modules mode)
{
    if(mode==Modules::REGISTERMOD){
        //发送信号给指定模块http响应
        emit sig_reg_mode_finish(id,res,errcode);
    }

    if(mode==Modules::RESETMOD){
        //发送信号给指定模块http响应
        emit sig_reset_mode_finish(id,res,errcode);
    }

    if(mode==Modules::LOGINMOD){
        //发送信号给指定的http响应
        emit sig_login_mode_finish(id,res,errcode);
    }
}
