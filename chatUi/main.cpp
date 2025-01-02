#include "mainwindow.h"
#include <QFile>
#include <QApplication>
#include "global.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    QFile qss(":/style/stylesheet.qss");
    if(qss.open(QFile::ReadOnly)){
        qDebug()<<"open qss success";
        QString style=QLatin1String(qss.readAll());
        a.setStyleSheet(style);
        qss.close();
    }else{
        qDebug()<<"open qss failed";
    }

    //获取当前应用的路径
    QString app_path=QCoreApplication::applicationDirPath();

    //拼接文件名
    QString fileName="config.ini";
    QString config_path=QDir::toNativeSeparators(app_path+
                                                   QDir::separator()+fileName);
    QSettings settings(config_path,QSettings::IniFormat);
    QString gate_host=settings.value("GateServer/host").toString();
    QString gate_port=settings.value("GateServer/port").toString();

    gate_url_prefix="http://"+gate_host+":"+gate_port;

    w.show();
    return a.exec();
}
