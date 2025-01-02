#ifndef HTTPMANAGER_H
#define HTTPMANAGER_H

#include <QObject>
#include "global.h"
#include "Singleton.h"
#include <QJsonObject>
#include <QJsonDocument>


class httpManager:public QObject,public Singleton<httpManager>,
                    public std::enable_shared_from_this<httpManager>
{
    Q_OBJECT

public:
    ~httpManager();
    void PostHttpReq(QUrl url,QJsonObject json,ReqId req_id,Modules mode);

private:
    httpManager();
    friend class Singleton<httpManager>;
    QNetworkAccessManager _manager;


signals:
    void sig_http_finish(ReqId id,QString res,ErrorCode errcode,Modules mode);
    void sig_reg_mode_finish(ReqId id,QString res,ErrorCode errcode);
    void sig_reset_mode_finish(ReqId id,QString res,ErrorCode errcode);
    void sig_login_mode_finish(ReqId id,QString res,ErrorCode errcode);

private slots:
    void slot_http_finish(ReqId id,QString res,ErrorCode errcode,Modules mode);

};

#endif // HTTPMANAGER_H
