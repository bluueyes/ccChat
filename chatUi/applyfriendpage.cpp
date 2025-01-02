#include "applyfriendpage.h"
#include "ui_applyfriendpage.h"
#include "tcpmgr.h"
#include "global.h"
#include "userdata.h"
#include <QRandomGenerator>
#include "usermgr.h"
#include <QPainter>
#include "authenfriend.h"

std::vector<QString>  strs1 ={"hello world !",
                             "nice to meet u",
                             "New year，new life",
                             "You have to love yourself",
                             "My love is written in the wind ever since the whole world is you"};
std::vector<QString> heads12 = {
    ":/res/head_1.jpg",
    ":/res/head_2.jpg",
    ":/res/head_3.jpg",
    ":/res/head_4.jpg",
    ":/res/head_5.jpg"
};
std::vector<QString> names1 = {
    "llfc",
    "zack",
    "golang",
    "cpp",
    "java",
    "nodejs",
    "python",
    "rust"
};


ApplyFriendPage::ApplyFriendPage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ApplyFriendPage)
{
    ui->setupUi(this);
    connect(ui->apply_friend_list,&ApplyFriendList::sig_show_search,this,&ApplyFriendPage::sig_show_search);

    loadApplyList();
    //接收tcp传递的authrsp信号处理
    connect(TcpMgr::getInstance().get(),&TcpMgr::sig_auth_rsp,this,&ApplyFriendPage::slot_auth_rsp);

}

ApplyFriendPage::~ApplyFriendPage()
{
    delete ui;
}

void ApplyFriendPage::AddNewApply(std::shared_ptr<AddFriendApply> apply)
{
    //先模拟头像随机，以后头像资源增加资源服务器后再显示
    int randomValue = QRandomGenerator::global()->bounded(100); // 生成0到99之间的随机整数
    int head_i = randomValue % heads12.size();
    auto* apply_item = new ApplyFriendItem();
    auto apply_info = std::make_shared<ApplyInfo>(apply->_from_uid,
                                                  apply->_name, apply->_desc,heads12[head_i], apply->_name, 0, 0);
    apply_item->SetInfo( apply_info);
    QListWidgetItem* item = new QListWidgetItem;
    //qDebug()<<"chat_user_wid sizeHint is " << chat_user_wid->sizeHint();
    item->setSizeHint(apply_item->sizeHint());
    item->setFlags(item->flags() & ~Qt::ItemIsEnabled & ~Qt::ItemIsSelectable);
    ui->apply_friend_list->insertItem(0,item);
    ui->apply_friend_list->setItemWidget(item, apply_item);
    apply_item->ShowAddBtn(true);

    _unauth_items[apply->_from_uid] = apply_item;
    //收到审核好友信号
    connect(apply_item, &ApplyFriendItem::sig_auth_friend, [this](std::shared_ptr<ApplyInfo> apply_info) {
        auto* authFriend = new AuthenFriend(this);
        authFriend->setModal(true);
        authFriend->SetApplyInfo(apply_info);
        authFriend->show();
    });
}

void ApplyFriendPage::paintEvent(QPaintEvent *event)
{
    QStyleOption opt;
    opt.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void ApplyFriendPage::loadApplyList()
{
    //添加好友申请
    auto apply_list = UserMgr::getInstance()->GetApplyList();
    for(auto &apply: apply_list){
        int randomValue = QRandomGenerator::global()->bounded(100); // 生成0到99之间的随机整数
        int head_i = randomValue % heads12.size();
        auto* apply_item = new ApplyFriendItem();
        apply->SetIcon(heads12[head_i]);
        apply_item->SetInfo(apply);
        QListWidgetItem* item = new QListWidgetItem;
        //qDebug()<<"chat_user_wid sizeHint is " << chat_user_wid->sizeHint();
        item->setSizeHint(apply_item->sizeHint());
        item->setFlags(item->flags() & ~Qt::ItemIsEnabled & ~Qt::ItemIsSelectable);
        ui->apply_friend_list->insertItem(0,item);
        ui->apply_friend_list->setItemWidget(item, apply_item);
        if(apply->_status){
            apply_item->ShowAddBtn(false);
        }else{
            apply_item->ShowAddBtn(true);
            auto uid = apply_item->GetUid();
            _unauth_items[uid] = apply_item;
        }
        //收到审核好友信号
        connect(apply_item, &ApplyFriendItem::sig_auth_friend, [this](std::shared_ptr<ApplyInfo> apply_info) {
            auto* authFriend = new AuthenFriend(this);
            authFriend->setModal(true);
            authFriend->SetApplyInfo(apply_info);
            authFriend->show();
        });
    }
        // 模拟假数据，创建QListWidgetItem，并设置自定义的widget
        for(int i = 0; i < 13; i++){
            int randomValue = QRandomGenerator::global()->bounded(100); // 生成0到99之间的随机整数
            int str_i = randomValue%strs1.size();
            int head_i = randomValue%heads12.size();
            int name_i = randomValue%names1.size();
            auto *apply_item = new ApplyFriendItem();
            auto apply = std::make_shared<ApplyInfo>(0, names1[name_i], strs1[str_i],
                                                     heads12[head_i], names1[name_i], 0, 1);
            apply_item->SetInfo(apply);
            QListWidgetItem *item = new QListWidgetItem;
            //qDebug()<<"chat_user_wid sizeHint is " << chat_user_wid->sizeHint();
            item->setSizeHint(apply_item->sizeHint());
            item->setFlags(item->flags() & ~Qt::ItemIsEnabled & ~Qt::ItemIsSelectable);
            ui->apply_friend_list->addItem(item);
            ui->apply_friend_list->setItemWidget(item, apply_item);
            //收到审核好友信号
            connect(apply_item, &ApplyFriendItem::sig_auth_friend, [this](std::shared_ptr<ApplyInfo> apply_info){
                auto *authFriend =  new AuthenFriend(this);
                authFriend->setModal(true);
                authFriend->SetApplyInfo(apply_info);
                authFriend->show();
            });
        }
}

void ApplyFriendPage::slot_auth_rsp(std::shared_ptr<AuthRsp> auth_rsp)
{
    auto uid = auth_rsp->_uid;
    auto find_iter = _unauth_items.find(uid);
    if (find_iter == _unauth_items.end()) {
        return;
    }
    find_iter->second->ShowAddBtn(false);
}