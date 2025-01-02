#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "logindialog.h"
#include "registerdialog.h"
#include "resetdialog.h"
#include "chatdialog.h"
#include "tcpmgr.h"
/******************************************************************************
 *
 * @file       mainwindow.h
 * @brief      主窗口
 *
 * @author     池游
 * @date       2024/12/07
 * @history
 *****************************************************************************/

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void do_changeRegist();
    void SlotSwitchReset();

    void SlotSwitchReg();
    void do_changeReSet();

    void SlotSwitchLogin();
    void SlotSwitchLogin2();

    void SlotSwitchChat();

private:
    Ui::MainWindow *ui;
    loginDialog* m_login;
    RegisterDialog* m_regist;
    ResetDialog* m_reset;
    ChatDialog* m_chat;

};
#endif // MAINWINDOW_H
