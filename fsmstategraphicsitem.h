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

    void SetName(QString name)
    {
        mName = name;
    }

    const QString& Name() const
    {
        return mName;
    }

    virtual QGraphicsItem* AsGraphicsItem() override
    {
        return this;
    }

    virtual bool IsTerminal() const override
    {
        return true;
    }

    void Save(QDataStream& out);
    void Load(QDataStream& in);

    static void SaveStaticData(QDataStream& out);
    static void LoadStaticData(QDataStream& in);

    quint32 Id() const { return mId; }
private:
    QRect mRect;
    QString mName;
    quint32 mId = 0;
    static unsigned int mInstaceCounter;
};

#endif // FSMSTATEGRAPHICSITEM_H
