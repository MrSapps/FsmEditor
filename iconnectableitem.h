#pragma once

#include <QSet>
#include <QGraphicsItem>

class FsmConnectionGraphicsItem;

class IConnectableItem
{
public:
    virtual ~IConnectableItem() { }

    QSet<FsmConnectionGraphicsItem*>& InConnections() { return mInConnections; }
    QSet<FsmConnectionGraphicsItem*>& OutConnections() { return mOutConnections; }

    virtual QGraphicsItem* AsGraphicsItem() = 0;
    virtual bool IsTerminal() const = 0;
    virtual void Save(QDataStream& out) = 0;
    void HandleItemChange(QGraphicsItem::GraphicsItemChange change);
private:
    QSet<FsmConnectionGraphicsItem*> mInConnections;
    QSet<FsmConnectionGraphicsItem*> mOutConnections;
};
