#ifndef LISTITEMBASE_H
#define LISTITEMBASE_H

#include <QObject>
#include "global.h"

class ListItemBase:public QWidget
{
    Q_OBJECT
public:
    explicit ListItemBase(QWidget* parent=nullptr);
    void SetItemType(ListItemType itemType);

    ListItemType GetItemType();
    //void paintEvent(QPaintEvent *event) override;
private:
    ListItemType _itemType;
};

#endif // LISTITEMBASE_H
