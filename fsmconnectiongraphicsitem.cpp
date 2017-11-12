#include "fsmconnectiongraphicsitem.h"
#include "fsmstategraphicsitem.h"

#include <QPainter>
#include <QSet>

const qreal Pi = 3.14;
const int kLineWidth = 4;
const int kArrowSize = 20;

FsmConnectionGraphicsItem::FsmConnectionGraphicsItem(FsmStateGraphicsItem* sourceItem, FsmStateGraphicsItem* destinationItem, QGraphicsItem* parent)
    : QGraphicsLineItem(parent), mDestinationItem(destinationItem), mSourceItem(sourceItem)
{
    mConnectionData = QSharedPointer<FsmConnectionData>(new FsmConnectionData());
    mConnectionData->mStart = mSourceItem;
    mConnectionData->mEnd = mDestinationItem;
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setPen(QPen(QColor(0,0,0), kLineWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));

    mSourceItem->ChangeOutConnection(true, this);
    mDestinationItem->ChangeInConnection(true, this);
}

FsmConnectionGraphicsItem::FsmConnectionGraphicsItem(QSharedPointer<FsmConnectionData> connectionData, QGraphicsItem *parent)
    : QGraphicsLineItem(parent), mConnectionData(connectionData)
{
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setPen(QPen(QColor(0,0,0), kLineWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
}

FsmConnectionGraphicsItem::~FsmConnectionGraphicsItem()
{
    /*
    if (mSourceItem)
    {
        mSourceItem->ChangeOutConnection(false, this);
    }

    if (mDestinationItem)
    {
        mDestinationItem->ChangeInConnection(false, this);
    }*/
}

QRectF FsmConnectionGraphicsItem::boundingRect() const
{
    qreal extra = (pen().width() + kArrowSize) / 2.0;

    return QRectF(line().p1(), QSizeF(line().p2().x() - line().p1().x(),
                                      line().p2().y() - line().p1().y()))
        .normalized()
        .adjusted(-extra, -extra, extra, extra);
}

QPainterPath FsmConnectionGraphicsItem::shape() const
{
    QPainterPath path = QGraphicsLineItem::shape();
    if (mEnableArrowHead)
    {
        path.addPolygon(mArrowHead);
    }
    return path;
}

void FsmConnectionGraphicsItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    if (mSourceItem && mSourceItem->collidesWithItem(mDestinationItem))
    {
        return;
    }

    QPen basePen = pen();

    if (isSelected())
    {
        basePen.setColor(QColor(255, 0, 0));
        painter->setBrush(QColor(255, 0, 0));
    }
    else
    {
         basePen.setColor(QColor(0, 0, 0));
         painter->setBrush(QColor(0, 0, 0));
    }

    painter->setPen(basePen);
    painter->drawLine(line());

    if (mEnableArrowHead)
    {
        painter->drawPolygon(mArrowHead);
    }
}

QPointF FsmConnectionGraphicsItem::WhereLineMeetsRect(const QLineF& polyLine, QGraphicsItem* item, const QRectF& bRect)
{
    QVector<QLineF> bRectLines;
    bRectLines.append(QLineF(bRect.topLeft(), bRect.bottomLeft()));     // left
    bRectLines.append(QLineF(bRect.topRight(), bRect.bottomRight()));   // right
    bRectLines.append(QLineF(bRect.topLeft(), bRect.topRight()));       // top
    bRectLines.append(QLineF(bRect.bottomLeft(), bRect.bottomRight())); // bottom

    for (int i = 0; i < bRectLines.count(); ++i)
    {
        bRectLines[i].setP1(mapFromItem(item, bRectLines[i].p1()));
        bRectLines[i].setP2(mapFromItem(item, bRectLines[i].p2()));
    }

    // Find out where/if our line collides with each line of the rect
    QPointF intersectPoint;
    for (int i = 0; i < bRectLines.count(); ++i)
    {
        QLineF::IntersectType intersectType = polyLine.intersect(bRectLines[i], &intersectPoint);
        if (intersectType == QLineF::BoundedIntersection)
        {
            break;
        }
    }
    return intersectPoint;
}

void FsmConnectionGraphicsItem::SyncPosition()
{
    // Line from the middle of each item
    QLineF polyLine(mapFromItem(mDestinationItem, mDestinationItem->boundingRect().center()), mapFromItem(mSourceItem, mSourceItem->boundingRect().center()));

    // Find where the line hits the edge of to item
    QPointF toItemIntersectionPoint = WhereLineMeetsRect(polyLine, mDestinationItem, mDestinationItem->boundingRect());

    // Find where the line hits the edge of from item
    QPointF fromItemIntersectionPoint = WhereLineMeetsRect(polyLine, mSourceItem, mSourceItem->boundingRect());

    // And use these points as our line
    setLine(QLineF(toItemIntersectionPoint, fromItemIntersectionPoint));

    double angle = ::acos(line().dx() / line().length());
    if (line().dy() >= 0)
    {
        angle = (Pi * 2) - angle;
    }

    QPointF arrowP1 = line().p1() + QPointF(sin(angle + Pi / 3) * kArrowSize,      cos(angle + Pi / 3) * kArrowSize);
    QPointF arrowP2 = line().p1() + QPointF(sin(angle + Pi - Pi / 3) * kArrowSize, cos(angle + Pi - Pi / 3) * kArrowSize);

    mArrowHead.clear();
    mArrowHead << line().p1() << arrowP1 << arrowP2;
}

void FsmConnectionGraphicsItem::EnableArrowHead(bool enable)
{
    mEnableArrowHead = enable;
}

QVariant FsmConnectionGraphicsItem::itemChange(GraphicsItemChange change, const QVariant& value)
{
    if (mIsSelectionChanging)
    {
        return value;
    }

    if (change == ItemSelectedChange)
    {
        mIsSelectionChanging = true; // Prevent re-entry else we'll blow the stack via inf recursion
        QSet<FsmConnectionGraphicsItem*> segments;// = IterateSegments(this);
        foreach(FsmConnectionGraphicsItem* segment, segments)
        {
            if (value == true)
            {
                segment->setSelected(true);
            }
            else
            {
                segment->setSelected(false);
            }
        }
        mIsSelectionChanging = false;
    }
    return value;
}