#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include "global.h"
#include <QString>
#include <QMap>

namespace Ui {
class loginDialog;
}

class loginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit loginDialog(QWidget *parent = nullptr);
    ~loginDialog();

private slots:
    void slot_login_mod_finish(ReqId id,QString res,ErrorCode errcode);
    void slot_tcp_con_finish(bool success);
    void slot_login_failed(int errid);
    void on_login_btn_clicked();

signals:
    void changeRegist();
    void changeReset();
    void sig_connect_tcp(ServerInfo);
private:
    void initHttpHanlers();
    void initHead();

    void showErr(QString str,bool ok);
    bool checkPwdValid();
    bool checkEmailValid();

    QMap<ReqId,std::function<void(const QJsonObject&)>> _handlers;
    QString token_;
    int uid_;
    Ui::loginDialog *ui;
};

#endif // LOGINDIALOG_H
