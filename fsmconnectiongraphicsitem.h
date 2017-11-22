#ifndef FSMCONNECTIONGRAPHICSITEM_H
#define FSMCONNECTIONGRAPHICSITEM_H

#include <QGraphicsLineItem>
#include <QPolygonF>
#include <QDebug>
#include "iconnectableitem.h"

class FsmStateGraphicsItem;

class FsmConnectionSplitter : public IConnectableItem, public QGraphicsItem
{
public:
    FsmConnectionSplitter(QGraphicsItem* pParent = nullptr);

    virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;
    virtual QRectF boundingRect() const override;
    virtual QVariant itemChange(GraphicsItemChange change, const QVariant& value) override;

    enum
    {
        Type = UserType + 3
    };

    virtual int type() const override
    {
        return Type;
    }

    virtual QGraphicsItem* AsGraphicsItem() override
    {
        return this;
    }

    virtual bool IsTerminal() const override
    {
        return false;
    }

    virtual void Save(QDataStream& out) override
    {
        qDebug() << "Write splitter";
        out << (quint32)Type;
        out << scenePos();
    }

    void Load(QDataStream& in)
    {
        QPointF pos;
        in >> pos;
        setPos(mapFromScene(pos));
    }

private:
    int mRadius = 30;
};

class FsmConnectionGraphicsItem : public QGraphicsLineItem
{
public:
    FsmConnectionGraphicsItem(IConnectableItem* sourceItem, IConnectableItem* destinationItem, QGraphicsItem* parent = nullptr);
    FsmConnectionGraphicsItem(QGraphicsItem* parent = nullptr);
    ~FsmConnectionGraphicsItem();

    enum
    {
        Type = UserType + 1
    };
    virtual int type() const override { return Type; }

    virtual QRectF boundingRect() const override;
    virtual QPainterPath shape() const override;
    virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;
    virtual QVariant itemChange(GraphicsItemChange change, const QVariant& value) override;

    QPointF WhereLineMeetsRect(const QLineF& polyLine, QGraphicsItem* item, const QRectF& bRect);
    void SyncPosition();
    void EnableArrowHead(bool enable);

    // Connection point or state
    IConnectableItem* SourceItem() { return mSourceItem; }
    IConnectableItem* DestinationItem() { return mDestinationItem; }
    void SetSourceItem(IConnectableItem* item) { mSourceItem = item; }
    void SetDestinationItem(IConnectableItem* item) { mDestinationItem = item; }

    void Save(QDataStream& out);
    void Load(QDataStream& in);
private:
    IConnectableItem* mSourceItem = nullptr;
    IConnectableItem* mDestinationItem = nullptr;
    bool mEnableArrowHead = false;
    bool mIsSelectionChanging = false;
    QPolygonF mArrowHead;
};


#endif // FSMCONNECTIONGRAPHICSITEM_H
