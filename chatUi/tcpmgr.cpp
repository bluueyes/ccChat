#include "tcpmgr.h"
#include <QJsonDocument>
#include "usermgr.h"
TcpMgr::TcpMgr():host_(""),port_(0),b_recv_pending(false)
    ,message_length_(0),message_id(0){

    QObject::connect(&socket_,&QTcpSocket::connected,this,[this](){

        qDebug()<<"connected to server!";
        emit sig_con_success(true);
    });

    QObject::connect(&socket_,&QTcpSocket::readyRead,this,[&](){
        //当有数据读时，读取所有数据
        //读取所有数据追加到缓冲区
        buffer_.append(socket_.readAll());
        qDebug()<<"READY READ";
        QDataStream stream(&buffer_,QIODevice::ReadOnly);

        forever{
            //先解析头部
            if(!b_recv_pending){
                //检查缓冲区是否足够解析消息头
                if(buffer_.size()<static_cast<int>(sizeof(quint16)*2)){
                    return;
                }

                //将buffer中的前四个字符移除
                stream>>message_id>>message_length_;
                buffer_=buffer_.mid(sizeof(quint16)*2);

                //输出读取的数据
                qDebug()<<"Message ID="<<message_id<<" Message length="<<message_length_;

            }

            if(buffer_.size()<message_length_){
                b_recv_pending=true;
                return;
            }

            b_recv_pending=false;

            //读取消息体
            QByteArray messageBody=buffer_.mid(0,message_length_);
            qDebug()<<"recive body msg is "<<messageBody;

            buffer_=buffer_.mid(message_length_);

            handlerMsg(ReqId(message_id),message_length_,messageBody);
        }

    });

    QObject::connect(&socket_, QOverload<QAbstractSocket::SocketError>::of(&QTcpSocket::errorOccurred), [&](QAbstractSocket::SocketError socketError) {
        Q_UNUSED(socketError)
        qDebug() << "Error:" << socket_.errorString();
    });

    QObject::connect(&socket_,&QTcpSocket::disconnected,this,[](){
        qDebug()<<"Disconnected from server";
    });



    //连接发送信号用来发送数据
    QObject::connect(this,&TcpMgr::sig_send_data,this,&TcpMgr::slot_send_data);


    //注册消息
    initHandlers();
}

void TcpMgr::initHandlers()
{
    handlers_.insert(ID_CHAT_LOGIN_RSP,[this](ReqId id,int len,QByteArray data){
        Q_UNUSED(len);
        qDebug()<<"handler id is"<<id<<"data is "<<data;
        //将QByteArray转换为QJsonDocument
        QJsonDocument jsonDoc=QJsonDocument::fromJson(data);

        //检查是否转换成功
        if(jsonDoc.isNull()){
            qDebug()<<"failed to create QjsonDocument";
            return;
        }

        QJsonObject jsonObj=jsonDoc.object();

        if(!jsonObj.contains("error")){
            int err=ErrorCode::ERR_JSON;
            qDebug()<<"login Failed err is json parse err "<<err;
            emit sig_login_failed(err);
            return;
        }

        int err=jsonObj["error"].toInt();
        if(err!=ErrorCode::SUCCESS){
            qDebug()<<"login Failed ,err is "<<err;
            emit sig_login_failed(err);
            return;
        }
        auto uid = jsonObj["uid"].toInt();
        auto name = jsonObj["name"].toString();
        auto nick = jsonObj["nick"].toString();
        auto icon = jsonObj["icon"].toString();
        auto sex = jsonObj["sex"].toInt();
        auto user_info = std::make_shared<UserInfo>(uid, name, nick, icon, sex);

        UserMgr::getInstance()->SetUserInfo(user_info);
        UserMgr::getInstance()->SetToken(jsonObj["token"].toString());
        if(jsonObj.contains("apply_list")){
            UserMgr::getInstance()->AppendApplyList(jsonObj["apply_list"].toArray());
        }

        //添加好友列表
        if (jsonObj.contains("friend_list")) {
            UserMgr::getInstance()->AppendFriendList(jsonObj["friend_list"].toArray());

        }


        emit sig_switch_chatdlg();
    });

    handlers_.insert(ID_SEARCH_USER_RSP,[this](ReqId id,int len,QByteArray data){
        Q_UNUSED(len);
        qDebug()<<"handler id is"<<id<<"data is "<<data;
        //将QByteArray转换为QJsonDocument
        QJsonDocument jsonDoc=QJsonDocument::fromJson(data);

        //检查是否转换成功
        if(jsonDoc.isNull()){
            qDebug()<<"failed to create QjsonDocument";
            return;
        }

        QJsonObject jsonObj=jsonDoc.object();

        if(!jsonObj.contains("error")){
            int err=ErrorCode::ERR_JSON;
            qDebug()<<"login Failed err is json parse err "<<err;
            emit sig_login_failed(err);
            return;
        }

        int err=jsonObj["error"].toInt();
        if(err!=ErrorCode::SUCCESS){
            qDebug()<<"login Failed ,err is "<<err;
            emit sig_login_failed(err);
            return;
        }

        auto search_info=std::make_shared<SearchInfo>(jsonObj["uid"].toInt(),
                            jsonObj["name"].toString(), jsonObj["nick"].toString(),
                            jsonObj["desc"].toString(), jsonObj["sex"].toInt(),jsonObj["icon"].toString());

         emit sig_user_search(search_info);
    });


    handlers_.insert(ID_NOTIFY_ADD_FRIEND_REQ,[this](ReqId id,int len,QByteArray data){
        Q_UNUSED(len);
        qDebug()<<"handler id is"<<id<<"data is "<<data;
        //将QByteArray转换为QJsonDocument
        QJsonDocument jsonDoc=QJsonDocument::fromJson(data);

        //检查是否转换成功
        if(jsonDoc.isNull()){
            qDebug()<<"failed to create QjsonDocument";
            return;
        }

        QJsonObject jsonObj=jsonDoc.object();

        if(!jsonObj.contains("error")){
            int err=ErrorCode::ERR_JSON;
            qDebug()<<"ID_SEARCH_USER_RSP Failed err is json parse err "<<err;
            emit sig_user_search(nullptr);
            return;
        }

        int err=jsonObj["error"].toInt();
        if(err!=ErrorCode::SUCCESS){
            qDebug()<<"ID_SEARCH_USER_RSP Failed ,err is "<<err;
            emit sig_user_search(nullptr);
            return;
        }

        int from_uid=jsonObj["applyuid"].toInt();
        QString name=jsonObj["name"].toString();
        QString desc=jsonObj["desc"].toString();
        QString icon=jsonObj["icon"].toString();
        QString nick=jsonObj["nick"].toString();
        int sex=jsonObj["sex"].toInt();

        auto apply_info=std::make_shared<AddFriendApply>(from_uid, name, desc,
            icon, nick, sex);

        qDebug()<<"recive ID_NOTIFY_ADD_FRIEND_REQ";
        emit sig_friend_apply(apply_info);
    });

    handlers_.insert(ID_AUTH_FRIEND_RSP,[this](ReqId id,int len,QByteArray data){
        Q_UNUSED(len);
        qDebug()<<"handler id is"<<id<<"data is "<<data;
        //将QByteArray转换为QJsonDocument
        QJsonDocument jsonDoc=QJsonDocument::fromJson(data);

        //检查是否转换成功
        if(jsonDoc.isNull()){
            qDebug()<<"failed to create QjsonDocument";
            return;
        }

        QJsonObject jsonObj=jsonDoc.object();

        if(!jsonObj.contains("error")){
            int err=ErrorCode::ERR_JSON;
            qDebug()<<"ID_SEARCH_USER_RSP Failed err is json parse err "<<err;
            emit sig_user_search(nullptr);
            return;
        }

        int err=jsonObj["error"].toInt();
        if(err!=ErrorCode::SUCCESS){
            qDebug()<<"ID_SEARCH_USER_RSP Failed ,err is "<<err;
            emit sig_user_search(nullptr);
            return;
        }


        QString name=jsonObj["name"].toString();
        int uid=jsonObj["uid"].toInt();
        QString icon=jsonObj["icon"].toString();
        QString nick=jsonObj["nick"].toString();
        int sex=jsonObj["sex"].toInt();

        auto rsp=std::make_shared<AuthRsp>(uid,name,nick,icon,sex);
        qDebug()<<"recive ID_AUTH_FRIEND_RSP";
        emit sig_auth_rsp(rsp);
    });

    handlers_.insert(ID_NOTIFY_AUTH_FRIEND_REQ, [this](ReqId id, int len, QByteArray data) {
        Q_UNUSED(len);
        qDebug() << "handle id is " << id << " data is " << data;
        // 将QByteArray转换为QJsonDocument
        QJsonDocument jsonDoc = QJsonDocument::fromJson(data);
        // 检查转换是否成功
        if (jsonDoc.isNull()) {
            qDebug() << "Failed to create QJsonDocument.";
            return;
        }
        QJsonObject jsonObj = jsonDoc.object();
        if (!jsonObj.contains("error")) {
            int err = ErrorCode::ERR_JSON;
            qDebug() << "Auth Friend Failed, err is " << err;
            return;
        }
        int err = jsonObj["error"].toInt();
        if (err != ErrorCode::SUCCESS) {
            qDebug() << "Auth Friend Failed, err is " << err;
            return;
        }
        int from_uid = jsonObj["fromuid"].toInt();
        QString name = jsonObj["name"].toString();
        QString nick = jsonObj["nick"].toString();
        QString icon = jsonObj["icon"].toString();
        int sex = jsonObj["sex"].toInt();
        auto auth_info = std::make_shared<AuthInfo>(from_uid,name,
                                                    nick, icon, sex);
        emit sig_add_auth_friend(auth_info);
    });
    handlers_.insert(ID_TEXT_CHAT_MSG_RSP, [this](ReqId id, int len, QByteArray data) {
        Q_UNUSED(len);
        qDebug() << "handle id is " << id << " data is " << data;
        // 将QByteArray转换为QJsonDocument
        QJsonDocument jsonDoc = QJsonDocument::fromJson(data);

        // 检查转换是否成功
        if (jsonDoc.isNull()) {
            qDebug() << "Failed to create QJsonDocument.";
            return;
        }

        QJsonObject jsonObj = jsonDoc.object();

        if (!jsonObj.contains("error")) {
            int err = ErrorCode::ERR_JSON;
            qDebug() << "Chat Msg Rsp Failed, err is Json Parse Err" << err;
            return;
        }

        int err = jsonObj["error"].toInt();
        if (err != ErrorCode::SUCCESS) {
            qDebug() << "Chat Msg Rsp Failed, err is " << err;
            return;
        }

        qDebug() << "Receive Text Chat Rsp Success " ;
        //ui设置送达等标记 todo...
    });

    handlers_.insert(ID_NOTIFY_TEXT_CHAT_MSG_REQ, [this](ReqId id, int len, QByteArray data) {
        Q_UNUSED(len);
        qDebug() << "handle id is " << id << " data is " << data;
        // 将QByteArray转换为QJsonDocument
        QJsonDocument jsonDoc = QJsonDocument::fromJson(data);

        // 检查转换是否成功
        if (jsonDoc.isNull()) {
            qDebug() << "Failed to create QJsonDocument.";
            return;
        }

        QJsonObject jsonObj = jsonDoc.object();

        if (!jsonObj.contains("error")) {
            int err = ErrorCode::ERR_JSON;
            qDebug() << "Notify Chat Msg Failed, err is Json Parse Err" << err;
            return;
        }

        int err = jsonObj["error"].toInt();
        if (err != ErrorCode::SUCCESS) {
            qDebug() << "Notify Chat Msg Failed, err is " << err;
            return;
        }

        qDebug() << "Receive Text Chat Notify Success " ;
        auto msg_ptr = std::make_shared<TextChatMsg>(jsonObj["fromuid"].toInt(),
                                                     jsonObj["touid"].toInt(),jsonObj["text_array"].toArray());
        emit sig_text_chat_msg(msg_ptr);
    });
}

void TcpMgr::handlerMsg(ReqId id, int len, QByteArray data)
{
    auto find_iter=handlers_.find(id);
    if(find_iter==handlers_.end()){
        qDebug()<<"not found id {"<<id<<"} to handlers";
        return;
    }

    find_iter.value()(id,len,data);
}

void TcpMgr::slot_tcp_connect(ServerInfo si)
{
    qDebug()<<"recv tcp connect signal";

    qDebug()<<"connect to server...";
    host_=si.Host;
    port_=static_cast<uint16_t>(si.Port.toUInt());
    socket_.connectToHost(si.Host,port_);

}

void TcpMgr::slot_send_data(ReqId reqid, QByteArray bytesData)
{
    uint16_t id=reqid;

    //计算长度
    quint16 len=static_cast<quint16>(bytesData.size());

    QByteArray block;
    QDataStream out(&block,QIODevice::WriteOnly);

    //设置网络字节序
    out.setByteOrder(QDataStream::BigEndian);

    //写入id和长度
    out<<id<<len;

    //添加字符串长度
    block.append(bytesData);
    qDebug()<<block.toStdString();
    //发送数据
    socket_.write(block);

}





