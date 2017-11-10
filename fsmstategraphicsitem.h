#ifndef FSMSTATEGRAPHICSITEM_H
#define FSMSTATEGRAPHICSITEM_H

#include <QGraphicsItem>
#include <QSet>

class FsmGraphicsConnection;

constexpr int kRectSize = 200 / 2;

class FsmStateGraphicsItem : public QGraphicsItem
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

    void ChangeInConnection(bool add, FsmGraphicsConnection* pConnection)
    {
        ChangeConnectionT(add, mInboundConnections, pConnection);
    }

    void ChangeOutConnection(bool add, FsmGraphicsConnection* pConnection)
    {
        ChangeConnectionT(add, mOutboundConnections, pConnection);
    }

private:
    QRect mRect;
    QString mName = "A fan art zzzzzza zzzz zza zzza zzzzzzzz zzazzz zzzz zzgzz zzz zzzzzz zzhgfzz zzzzzzz zzzzz zzzzz Bzzz";
    QSet<FsmGraphicsConnection*> mInboundConnections;
    QSet<FsmGraphicsConnection*> mOutboundConnections;
};

#endif // FSMSTATEGRAPHICSITEM_H
