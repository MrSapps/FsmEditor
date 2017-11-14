#ifndef FSMCONNECTIONGRAPHICSITEM_H
#define FSMCONNECTIONGRAPHICSITEM_H

#include <QGraphicsLineItem>
#include <QPolygonF>

#include "iconnectableitem.h"

class FsmStateGraphicsItem;


class FsmConnectionData
{
public:
    IConnectableItem* mStart = nullptr;
    IConnectableItem* mEnd = nullptr;
};

class FsmConnectionSplitter : public IConnectableItem, public QGraphicsItem
{
public:
    FsmConnectionSplitter(QGraphicsItem *pParent = nullptr);
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget);
    QRectF boundingRect() const;
    virtual QVariant itemChange(GraphicsItemChange change, const QVariant& value) override;

    virtual QGraphicsItem* AsGraphicsItem() override
    {
        return this;
    }

    virtual bool IsTerminal() const override
    {
        return false;
    }

private:
    int mRadius = 30;
};

class FsmConnectionGraphicsItem : public QGraphicsLineItem
{
public:
    FsmConnectionGraphicsItem(IConnectableItem* sourceItem, IConnectableItem* destinationItem, QGraphicsItem* parent = nullptr);
    FsmConnectionGraphicsItem(QSharedPointer<FsmConnectionData> connectionData, QGraphicsItem* parent = nullptr);
    ~FsmConnectionGraphicsItem();

    virtual QRectF boundingRect() const override;
    virtual QPainterPath shape() const override;
    virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;
    virtual QVariant itemChange(GraphicsItemChange change, const QVariant& value) override;

    QPointF WhereLineMeetsRect(const QLineF& polyLine, QGraphicsItem* item, const QRectF& bRect);
    void SyncPosition();
    void EnableArrowHead(bool enable);
    QSharedPointer<FsmConnectionData>& ConnectionData() { return mConnectionData; }

    // Connection point or state
    IConnectableItem* SourceItem() { return mSourceItem; }
    IConnectableItem* DestinationItem() { return mDestinationItem; }
    void SetSourceItem(IConnectableItem* item) { mSourceItem = item; }
    void SetDestinationItem(IConnectableItem* item) { mDestinationItem = item; }

private:
    IConnectableItem* mSourceItem = nullptr;
    IConnectableItem* mDestinationItem = nullptr;
    QSharedPointer<FsmConnectionData> mConnectionData;
    bool mEnableArrowHead = false;
    bool mIsSelectionChanging = false;
    QPolygonF mArrowHead;
};


#endif // FSMCONNECTIONGRAPHICSITEM_H
