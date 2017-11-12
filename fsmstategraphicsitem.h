#ifndef FSMSTATEGRAPHICSITEM_H
#define FSMSTATEGRAPHICSITEM_H

#include <QGraphicsItem>
#include <QSet>

class FsmConnectionGraphicsItem;

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

    void ChangeInConnection(bool add, FsmConnectionGraphicsItem* pConnection)
    {
        ChangeConnectionT(add, mInboundConnections, pConnection);
    }

    void ChangeOutConnection(bool add, FsmConnectionGraphicsItem* pConnection)
    {
        ChangeConnectionT(add, mOutboundConnections, pConnection);
    }

    void SetName(QString name) { mName = name; }
    const QString& Name() const { return mName; }
private:
    QRect mRect;
    QString mName;
    QSet<FsmConnectionGraphicsItem*> mInboundConnections;
    QSet<FsmConnectionGraphicsItem*> mOutboundConnections;
    static unsigned int mInstaceCounter;
};

#endif // FSMSTATEGRAPHICSITEM_H
