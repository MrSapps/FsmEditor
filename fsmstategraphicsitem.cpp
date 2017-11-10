#include "fsmstategraphicsitem.h"
#include <QPainter>

FsmStateGraphicsItem::FsmStateGraphicsItem(QGraphicsItem *pParent)
    : QGraphicsItem(pParent)
{
    setFlag(QGraphicsItem::ItemIsMovable, true);
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);

    mRect.setX(-kRectSize);
    mRect.setY(-kRectSize);
    mRect.setWidth(kRectSize);
    mRect.setHeight(kRectSize);
}

int FsmStateGraphicsItem::type() const
{
    return Type;
}

QVariant FsmStateGraphicsItem::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value)
{
    if (change == QGraphicsItem::ItemPositionChange || change == QGraphicsItem::ItemPositionHasChanged)
    {
        foreach (FsmGraphicsConnection* connection, mInboundConnections)
        {
            //connection->UpdatePosition();
        }

        foreach (FsmGraphicsConnection* connection, mOutboundConnections)
        {
            //connection->UpdatePosition();
        }
    }
    return value;
}

void FsmStateGraphicsItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->setBrush(QColor(255, 255, 255));
    if (isSelected())
    {
        painter->setPen(QColor(255, 0, 0));
    }
    else
    {
        painter->setPen(QColor(0, 0, 0));
    }

    painter->drawRect(mRect);

    painter->setPen(QColor(0, 0, 0));

    QTextOption textOption(Qt::AlignCenter);
    painter->drawText(mRect, mName, textOption);
}
