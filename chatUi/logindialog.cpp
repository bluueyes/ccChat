#include "logindialog.h"
#include "ui_logindialog.h"
#include "clickedlab.h"
#include "httpManager.h"
#include <QPainter>
#include <QPainterPath>
#include <QJsonObject>
#include "tcpmgr.h"

loginDialog::loginDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::loginDialog)
{
    ui->setupUi(this);
    ui->pass_edit->setEchoMode(QLineEdit::Password);

    connect(ui->reg_btn,&QPushButton::clicked,this,&loginDialog::changeRegist);

    ui->forget_lab->SetState("normal","hover","","selected","selected_hover","");
    ui->forget_lab->setCursor(Qt::PointingHandCursor);
    connect(ui->forget_lab, &ClickedLab::clicked, this,&loginDialog::changeReset);

    initHttpHanlers();

    connect(httpManager::getInstance().get(),&httpManager::sig_login_mode_finish,this,&loginDialog::slot_login_mod_finish);

    connect(this,&loginDialog::sig_connect_tcp,TcpMgr::getInstance().get(),&TcpMgr::slot_tcp_connect);
    connect(TcpMgr::getInstance().get(), &TcpMgr::sig_con_success, this, &loginDialog::slot_tcp_con_finish);

    QObject::connect(TcpMgr::getInstance().get(),&TcpMgr::sig_login_failed,this,&loginDialog::slot_login_failed);
    initHead();
}



loginDialog::~loginDialog()
{
    delete ui;
}

void loginDialog::slot_login_mod_finish(ReqId id, QString res, ErrorCode errcode)
{
    if(errcode!=ErrorCode::SUCCESS){
        showErr(tr("网络请求错误"),false);
        return;
    }

    //解析JSON字符串
    QJsonDocument jsonDoc=QJsonDocument::fromJson(res.toUtf8());

    //json解析错误
    if(jsonDoc.isNull()){
        showErr(tr("json 解析错误"),false);
        return;
    }

    //json解析错误
    if(!jsonDoc.isObject()){
        showErr(tr("json解析错误"),false);
        return;
    }

    _handlers[id](jsonDoc.object());
    return;
}

void loginDialog::slot_tcp_con_finish(bool success)
{
    if(success){
        showErr(tr("服务器连接成功"),true);
        QJsonObject jsonObj;
        jsonObj["uid"] = uid_;
        jsonObj["token"] = token_;
        QJsonDocument doc(jsonObj);
        QByteArray jsonData=doc.toJson(QJsonDocument::Compact);

        //发送tcp请求给chat server
        TcpMgr::getInstance()->sig_send_data(ReqId::ID_CHAT_LOGIN, jsonData);

    }else{
        showErr(tr("网络异常"),false);
        ui->login_btn->setEnabled(true);
    }
}

void loginDialog::slot_login_failed(int errid)
{
    QString result = QString("登录失败, err is %1")
                         .arg(errid);
    showErr(result,false);
    ui->login_btn->setEnabled(true);
}

void loginDialog::initHttpHanlers()
{
    //注册获得登录回包的逻辑
    _handlers.insert(ReqId::ID_LOGIN_USER,[this](QJsonObject json_obj){
        int error=json_obj["error"].toInt();
        if(error!=ErrorCode::SUCCESS){
            showErr(tr("参数错误"),false);
            return;
        }

        auto email=json_obj["email"].toString();
        showErr(tr("聊天服务连接成功，正在登录..."),true);
        qDebug()<<"email is"<<email;


        ServerInfo si;
        si.Uid = json_obj["uid"].toInt();
        si.Host = json_obj["host"].toString();
        si.Port = json_obj["port"].toString();
        si.Token = json_obj["token"].toString();
        uid_ = si.Uid;
        token_ = si.Token;
        qDebug()<< "user is " << email << " uid is " << si.Uid <<" host is "
                 << si.Host << " Port is " << si.Port << " Token is " << si.Token;
        emit sig_connect_tcp(si);
    });
}

void loginDialog::initHead()
{
    //加载图片
    QPixmap originalPixmap(":/res/head_1.jpg");
    //设置图片自动缩放
    qDebug()<<originalPixmap.size()<<ui->back_lab->size();
    originalPixmap=originalPixmap.scaled(ui->back_lab->size(),
                        Qt::KeepAspectRatio,Qt::SmoothTransformation);
    //创建一个和原始图片相同大小的QPixmap,用于绘制圆角图片
    QPixmap roundedPixmap(originalPixmap.size());
    roundedPixmap.fill(Qt::transparent);    //用透明色填充

    QPainter painter(&roundedPixmap);
    painter.setRenderHint(QPainter::Antialiasing);  //设置抗锯齿，使圆角更平滑
    painter.setRenderHint(QPainter::SmoothPixmapTransform);

    //使用QPainterPath设置圆角
    QPainterPath path;
    path.addRoundedRect(0,0,originalPixmap.width(),originalPixmap.height(),10,10);//最后两个为圆脚的半径
    painter.setClipPath(path);

    //将原来图片会知道roundedPixmap上
    painter.drawPixmap(0,0,originalPixmap);

    //设置绘制好的圆角图片到label上
    ui->back_lab->setPixmap(originalPixmap);
}

void loginDialog::on_login_btn_clicked()
{
    qDebug()<<"login btn clicked";
    if(checkEmailValid()==false){
        showErr(tr("邮箱输入错误"),false);
        return;
    }

    if(checkPwdValid()==false){
        showErr(tr("密码输入错误"),false);
        return;
    }

    auto email=ui->email_edit->text();
    auto pwd=ui->pass_edit->text();

    QJsonObject json_obj;
    json_obj["email"]=email;
    json_obj["passwd"]=XorString(pwd);

    httpManager::getInstance()->PostHttpReq(QUrl(gate_url_prefix+"/user_login"),json_obj,ReqId::ID_LOGIN_USER,Modules::LOGINMOD);
}

void loginDialog::showErr(QString err,bool ok){
    if(ok==false){
        ui->err_tip->setProperty("state","err");
    }else{
        ui->err_tip->setProperty("state","normal");
    }

    ui->err_tip->setText(err);
    repolish(ui->err_tip);
}

bool loginDialog::checkEmailValid(){
    auto email = ui->email_edit->text();
    if(email.isEmpty()){
        qDebug() << "User empty " ;
        return false;
    }
    return true;
}
bool loginDialog::checkPwdValid(){
    auto pwd = ui->pass_edit->text();
    if(pwd.length() < 6 || pwd.length() > 15){
        qDebug() << "Pass length invalid";
        return false;
    }
    return true;
}
