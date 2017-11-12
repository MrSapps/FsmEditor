#ifndef FSMCONNECTIONGRAPHICSITEM_H
#define FSMCONNECTIONGRAPHICSITEM_H

#include <QGraphicsLineItem>
#include <QPolygonF>

class FsmStateGraphicsItem;

class FsmConnectionData
{
public:
    FsmStateGraphicsItem* mStart = nullptr;
    FsmStateGraphicsItem* mEnd = nullptr;
};

class FsmConnectionGraphicsItem : public QGraphicsLineItem
{
public:
    FsmConnectionGraphicsItem(FsmStateGraphicsItem* sourceItem, FsmStateGraphicsItem* destinationItem, QGraphicsItem* parent = nullptr);
    FsmConnectionGraphicsItem(QSharedPointer<FsmConnectionData> connectionData, QGraphicsItem* parent = nullptr);
    ~FsmConnectionGraphicsItem();

    virtual QRectF boundingRect() const override;
    virtual QPainterPath shape() const override;
    virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;
    virtual QVariant itemChange(GraphicsItemChange change, const QVariant& value) override;

    QPointF WhereLineMeetsRect(const QLineF& polyLine, QGraphicsItem* item, const QRectF& bRect);
    void SyncPosition();
    void EnableArrowHead(bool enable);
private:
    FsmStateGraphicsItem* mSourceItem = nullptr;
    FsmStateGraphicsItem* mDestinationItem = nullptr;
    QSharedPointer<FsmConnectionData> mConnectionData;
    bool mEnableArrowHead = false;
    bool mIsSelectionChanging = false;
    QPolygonF mArrowHead;
};


#endif // FSMCONNECTIONGRAPHICSITEM_H
