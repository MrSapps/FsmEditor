#include "fsmstategraphicsitem.h"
#include <QPainter>
#include <QDebug>
#include "fsmconnectiongraphicsitem.h"

/*static*/ unsigned int FsmStateGraphicsItem::mInstaceCounter;

FsmStateGraphicsItem::FsmStateGraphicsItem(QGraphicsItem *pParent)
    : QGraphicsItem(pParent)
{
    Init();

    mInstaceCounter++;
    mId = mInstaceCounter;
    mName = "New state(" + QString::number(mInstaceCounter) + ")";
}

FsmStateGraphicsItem::FsmStateGraphicsItem(QString name, quint32 id, QGraphicsItem* pParent)
    : QGraphicsItem(pParent)
{
    Init();
    mId = id;
    mName = name;
}

void FsmStateGraphicsItem::Init()
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
    HandleItemChange(change);
    return value;
}

void FsmStateGraphicsItem::paint(QPainter *painter, const QStyleOptionGraphicsItem* /*option*/, QWidget* /*widget*/)
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

void FsmStateGraphicsItem::Save(QDataStream& out)
{
    qDebug() << "Write state";
    out << mName;
    out << mRect;
    out << mId;
    out << scenePos();
    out << isSelected();
}

void FsmStateGraphicsItem::Load(QDataStream& in)
{
    in >> mName;
    in >> mRect;
    in >> mId;

    QPointF loadedScenePos;
    in >> loadedScenePos;

    bool loadedIsSelected = false;
    in >> loadedIsSelected;

    setPos(mapFromScene(loadedScenePos));
    setSelected(loadedIsSelected);
}

void FsmStateGraphicsItem::SaveStaticData(QDataStream& out)
{
    out << (quint32)mInstaceCounter;
}

void FsmStateGraphicsItem::LoadStaticData(QDataStream& in)
{
    in >> mInstaceCounter;
}
