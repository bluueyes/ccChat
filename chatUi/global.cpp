#include "global.h"

std::function<void(QWidget*)> repolish=[](QWidget* w){
    w->style()->unpolish(w);
    w->style()->polish(w);
};

QString gate_url_prefix="";

std::function<QString(QString)> XorString=[](QString str){

    QString strbuf=str;
    int len=str.length();
    len=len%255;
    for(int i=0;i<len;i++){

        //异或操作对密码加密,在异或一次就能获取原来密码.
        strbuf[i]=QChar(static_cast<ushort>(str[i].unicode()^static_cast<ushort>(len)));

    }
    return strbuf;
};
