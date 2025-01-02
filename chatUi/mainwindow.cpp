#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_login = new loginDialog(this);
    setCentralWidget(m_login);
    m_login->show();

    //自定义窗口
    m_login->setWindowFlags(Qt::CustomizeWindowHint|Qt::FramelessWindowHint);


    //连接注册页面的信号与曹
    connect(m_login,&loginDialog::changeRegist,this,&MainWindow::do_changeRegist);

    //连接登录界面忘记密码信号
    connect(m_login, &loginDialog::changeReset, this, &MainWindow::SlotSwitchReset);

    connect(TcpMgr::getInstance().get(),&TcpMgr::sig_switch_chatdlg,this,&MainWindow::SlotSwitchChat);

    //emit TcpMgr::getInstance()->sig_switch_chatdlg();
}



MainWindow::~MainWindow()
{
    delete ui;

}

void MainWindow::do_changeRegist()
{
    m_regist=new RegisterDialog(this);
    m_regist->setWindowFlags(Qt::CustomizeWindowHint|Qt::FramelessWindowHint);
    setCentralWidget(m_regist);

    //连接返回界面的信号与曹
    connect(m_regist,&RegisterDialog::sigSwitchLogin,this,&MainWindow::SlotSwitchLogin);

    m_login->hide();
    m_regist->show();
}

void MainWindow::SlotSwitchReg()
{
    m_regist = new RegisterDialog(this);
    m_regist->hide();

    m_regist->setWindowFlags(Qt::CustomizeWindowHint|Qt::FramelessWindowHint);

    //连接注册界面返回登录信号
    connect(m_regist, &RegisterDialog::sigSwitchLogin, this, &MainWindow::SlotSwitchLogin);
    setCentralWidget(m_regist);
    m_login->hide();
    m_regist->show();
}

void MainWindow::do_changeReSet()
{
    m_reset=new ResetDialog(this);
    m_reset->setWindowFlags(Qt::CustomizeWindowHint|Qt::FramelessWindowHint);
    setCentralWidget(m_reset);

    //连接返回界面的信号与曹
    connect(m_regist,&RegisterDialog::sigSwitchLogin,this,&MainWindow::SlotSwitchLogin);

    m_login->hide();
    m_reset->show();
}

//从注册界面返回登录界面
void MainWindow::SlotSwitchLogin()
{
    //创建一个CentralWidget, 并将其设置为MainWindow的中心部件
    m_login = new loginDialog(this);
    m_login->setWindowFlags(Qt::CustomizeWindowHint|Qt::FramelessWindowHint);
    setCentralWidget(m_login);

    m_regist->hide();
    m_login->show();
    //连接登录界面注册信号
    connect(m_login, &loginDialog::changeRegist, this, &MainWindow::SlotSwitchReg);
    //连接登录界面忘记密码信号
    connect(m_login, &loginDialog::changeReset, this, &MainWindow::SlotSwitchReset);
}

void MainWindow::SlotSwitchReset()
{
    //创建一个CentralWidget, 并将其设置为MainWindow的中心部件
    m_reset = new ResetDialog(this);
    m_reset->setWindowFlags(Qt::CustomizeWindowHint|Qt::FramelessWindowHint);
    setCentralWidget(m_reset);

    m_login->hide();
    m_reset->show();
    //注册返回登录信号和槽函数
    connect(m_reset, &ResetDialog::changeLogin, this, &MainWindow::SlotSwitchLogin2);
}

//从重置界面返回登录界面
void MainWindow::SlotSwitchLogin2()
{
    //创建一个CentralWidget, 并将其设置为MainWindow的中心部件
    m_login = new loginDialog(this);
    m_login->setWindowFlags(Qt::CustomizeWindowHint|Qt::FramelessWindowHint);
    setCentralWidget(m_login);

    m_reset->hide();
    m_login->show();
    //连接登录界面忘记密码信号
    connect(m_login, &loginDialog::changeReset, this, &MainWindow::SlotSwitchReset);
    //连接登录界面注册信号
    connect(m_login, &loginDialog::changeRegist, this, &MainWindow::SlotSwitchReg);


}

void MainWindow::SlotSwitchChat()
{
    m_chat = new ChatDialog(this);
    m_chat->setWindowFlags(Qt::CustomizeWindowHint|Qt::FramelessWindowHint);
    setCentralWidget(m_chat);

    m_login->hide();
    m_chat->show();

    this->setMinimumSize(QSize(850,650));
    this->setMaximumSize(QWIDGETSIZE_MAX,QWIDGETSIZE_MAX);
}

