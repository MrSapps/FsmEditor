#ifndef FSMSTATEGRAPHICSITEM_H
#define FSMSTATEGRAPHICSITEM_H

#include <QGraphicsItem>
#include <QSet>

#include "iconnectableitem.h"

class FsmConnectionGraphicsItem;

constexpr int kRectSize = 200 / 2;

class FsmStateGraphicsItem : public IConnectableItem, public QGraphicsItem
{
public:
    FsmStateGraphicsItem(QGraphicsItem* pParent = nullptr);

    enum
    {
        Type = UserType + 1
    };

    virtual int type() const override;

    virtual QVariant itemChange(GraphicsItemChange change, const QVariant& value) override;

    virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

    virtual QRectF boundingRect() const override
    {
        return mRect;
    }

    template<class T, class U>
    void ChangeConnectionT(bool add, T& collection, U* pConnection)
    {
        if (add)
        {
            collection.insert(pConnection);
        }
        else
        {
            auto it = collection.find(pConnection);
            if (it != collection.end())
            {
                collection.erase(it);
            }
        }
    }

    void ChangeInConnection(bool add, IConnectableItem* pConnection)
    {
        ChangeConnectionT(add, mInboundConnections, pConnection);
    }

    void ChangeOutConnection(bool add, IConnectableItem* pConnection)
    {
        ChangeConnectionT(add, mOutboundConnections, pConnection);
    }

    void SetName(QString name) { mName = name; }
    const QString& Name() const { return mName; }

    virtual QGraphicsItem* AsGraphicsItem() override
    {
        return this;
    }

    virtual bool IsTerminal() const override
    {
        return true;
    }

    /*
    virtual void AddOutItem(IConnectableItem* pToAdd) override
    {
        ChangeOutConnection(true, pToAdd);
    }

    virtual void AddInItem(IConnectableItem* pToAdd) override
    {
        ChangeInConnection(true, pToAdd);
    }

    virtual void SwapOutItem(IConnectableItem* pOldSource, IConnectableItem* pNewSource) override
    {
        ChangeOutConnection(false, pOldSource);
        ChangeOutConnection(true, pNewSource);
    }
*/
private:
    QRect mRect;
    QString mName;
    QSet<IConnectableItem*> mInboundConnections;
    QSet<IConnectableItem*> mOutboundConnections;
    static unsigned int mInstaceCounter;
};

#endif // FSMSTATEGRAPHICSITEM_H
