#include "searchlist.h"
#include "tcpmgr.h"
#include "adduseritem.h"
#include "findsuccessdlg.h"
#include "customizeedit.h"
#include <QJsonDocument>
#include "findfaildlg.h"
#include "usermgr.h"

SearchList::SearchList(QWidget *parent):QListWidget(parent),_find_dlg(nullptr), _search_edit(nullptr), _send_pending(false)
{
    Q_UNUSED(parent);
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    // 安装事件过滤器
    this->viewport()->installEventFilter(this);
    //连接点击的信号和槽
    connect(this, &QListWidget::itemClicked, this, &SearchList::slot_item_clicked);
    //添加条目
    addTipItem();
    //连接搜索条目
    connect(TcpMgr::getInstance().get(), &TcpMgr::sig_user_search, this, &SearchList::slot_user_search);


}

void SearchList::CloseFindDlg()
{

}

void SearchList::SetSearchEdit(QWidget *edit)
{
    _search_edit=edit;
}

void SearchList::waitPending(bool pending)
{
    if(pending){
        _loadingDialog=new LoadingDlg(this);
        _loadingDialog->setModal(true);
        _loadingDialog->show();
        _send_pending=pending;
    }else{
        _loadingDialog->hide();
        _loadingDialog->deleteLater();
        _send_pending=pending;
    }
}

void SearchList::addTipItem()
{
    auto *invalid_item = new QWidget();
    QListWidgetItem *item_tmp = new QListWidgetItem;
    //qDebug()<<"chat_user_wid sizeHint is " << chat_user_wid->sizeHint();
    item_tmp->setSizeHint(QSize(250,10));
    this->addItem(item_tmp);
    invalid_item->setObjectName("invalid_item");
    this->setItemWidget(item_tmp, invalid_item);
    item_tmp->setFlags(item_tmp->flags() & ~Qt::ItemIsSelectable);
    auto *add_user_item = new AddUserItem();
    QListWidgetItem *item = new QListWidgetItem;
    //qDebug()<<"chat_user_wid sizeHint is " << chat_user_wid->sizeHint();
    item->setSizeHint(add_user_item->sizeHint());
    this->addItem(item);
    this->setItemWidget(item, add_user_item);
}

void SearchList::slot_item_clicked(QListWidgetItem *item)
{
    QWidget* widget=this->itemWidget(item); //获取自定义widget
    if(!widget){
        qDebug()<<"slot item clicked widget is nullptr";
        return;
    }
    // 对自定义widget进行操作， 将item 转化为基类ListItemBase
    ListItemBase* customItem=qobject_cast<ListItemBase*>(widget);
    if(!customItem){
        qDebug()<<"slot item clicked widget is nullptr";
        return;
    }

    auto itemType=customItem->GetItemType();
    if(itemType==ListItemType::INVALID_ITEM){
        qDebug()<< "slot invalid item clicked ";
        return;
    }

    if(itemType==ListItemType::ADD_USER_TIP_ITEM){

        if(_send_pending){
            return;
        }

        if(!_search_edit){
            return;
        }

        waitPending(true);
        auto search_edit=dynamic_cast<CustomizeEdit*>(_search_edit);
        auto uid_str=search_edit->text();

        QJsonObject jsonObj;
        jsonObj["uid"]=uid_str;

        QJsonDocument doc(jsonObj);
        QByteArray jsonData=doc.toJson(QJsonDocument::Compact);
        emit TcpMgr::getInstance()->sig_send_data(ReqId::ID_SEARCH_USER_REQ,jsonData);
        return;
    }


    //清除弹出框
    CloseFindDlg();
}

void SearchList::slot_user_search(std::shared_ptr<SearchInfo> si)
{
    waitPending(false);
    if(si==nullptr){
        _find_dlg=std::make_shared<FindFailDlg>(this);
    }else{
        auto self_uid=UserMgr::getInstance()->GetUid();
        //此处分两种情况，一种是搜多到已经是自己的朋友了，一种是未添加好友
        if(self_uid==si->_uid){
            return;
        }
        //查找是否已经是好友 todo...
        bool bExist=UserMgr::getInstance()->CheckFriendById(si->_uid);
        if(bExist){
            emit sig_jump_chat_item(si);
        }

        _find_dlg=std::make_shared<FindSuccessDlg>(this);
        std::dynamic_pointer_cast<FindSuccessDlg>(_find_dlg)->SetSearchInfo(si);

    }
    _find_dlg->show();
}
