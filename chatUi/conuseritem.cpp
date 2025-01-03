#include "conuseritem.h"
#include "ui_conuseritem.h"

ConUserItem::ConUserItem(QWidget *parent) :
    ListItemBase(parent),
    ui(new Ui::ConUserItem)
{
    ui->setupUi(this);
    SetItemType(ListItemType::CONTACT_USER_ITEM);
    ui->red_point->raise();
    ShowRedPoint(false);
}

ConUserItem::~ConUserItem()
{
    delete ui;
}

QSize ConUserItem::sizeHint() const
{
    return QSize(250, 70); // 返回自定义的尺寸
}

void ConUserItem::SetInfo(int uid,QString name, QString head)
{
    info = std::make_shared<UserInfo>(uid,name, name, head, 0);

    // 加载图片
    QPixmap pixmap(info->_icon);

    // 设置图片自动缩放
    ui->icon_lb->setPixmap(pixmap.scaled(ui->icon_lb->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    ui->icon_lb->setScaledContents(true);

    ui->user_name_lb->setText(info->_name);

}

void ConUserItem::SetInfo(std::shared_ptr<AuthInfo> auth_info)
{
    auto is=std::make_shared<UserInfo>(auth_info);
    info=is;
}

void ConUserItem::SetInfo(std::shared_ptr<AuthRsp> auth_rsp)
{
    auto is=std::make_shared<UserInfo>(auth_rsp);
    info=is;
}

void ConUserItem::ShowRedPoint(bool show)
{
    if(show){
        ui->red_point->show();
    }else{
        ui->red_point->hide();
    }

}

std::shared_ptr<UserInfo> ConUserItem::GetInfo()
{
    return info;
}
