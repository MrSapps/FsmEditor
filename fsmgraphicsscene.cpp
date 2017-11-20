#include "fsmgraphicsscene.h"
#include "fsmstategraphicsitem.h"

#include <QUndoCommand>
#include <QGraphicsSceneMouseEvent>
#include "fsmconnectiongraphicsitem.h"
#include <QDebug>

class AddStateCommand : public QUndoCommand
{
public:
    AddStateCommand(QGraphicsScene* pScene, QPointF createPos)
     : mScene(pScene), mCreatePos(createPos)
    {
        QString message = "Create new state item at " + QString::number(createPos.x()) + "," + QString::number(createPos.y());
        setText(message);
    }

private:
    virtual void undo() override
    {
        if (mCreatedItem)
        {
            mScene->removeItem(mCreatedItem);
            delete mCreatedItem;
            mCreatedItem = nullptr;
        }
    }

    virtual void redo() override
    {
        mCreatedItem = new FsmStateGraphicsItem();
        mCreatedItem->setPos(mCreatePos);
        mScene->addItem(mCreatedItem);
    }

    FsmStateGraphicsItem* mCreatedItem = nullptr;
    QGraphicsScene* mScene;
    QPointF mCreatePos;
};

class SelectionChangedCommand : public QUndoCommand
{
public:
    SelectionChangedCommand(QGraphicsScene* pScene)
     : mScene(pScene)
    {
        QString message = "Selection changed";
        setText(message);
    }

    virtual void undo() override
    {
        mScene->clearSelection();
        foreach(QGraphicsItem* pItem, mSelected)
        {
            pItem->setSelected(true);
        }
    }

    virtual void redo() override
    {
        mSelected = mScene->selectedItems();
    }

private:
    QGraphicsScene* mScene;
    QList<QGraphicsItem *> mSelected;
};

FsmGraphicsScene::FsmGraphicsScene(QObject *pParent, QUndoStack &undoStack)
    : QGraphicsScene(pParent), mUndoStack(undoStack)
{
    setSceneRect(0, 0, 700, 700);
}

void FsmGraphicsScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* pEvent)
{
    if (pEvent->button() == Qt::LeftButton)
    {
        mUndoStack.push(new AddStateCommand(this, pEvent->scenePos()));
    }
}

void FsmGraphicsScene::SplitLine(FsmConnectionGraphicsItem* pLineToSplit, QPointF splitPos)
{
    clearSelection();

    // Create a circle splitter item at the click pos
    FsmConnectionSplitter* pSplitter = new FsmConnectionSplitter();
    pSplitter->setPos(splitPos);
    addItem(pSplitter);

    pSplitter->setSelected(true);

    // Remove line we are splitting form the destination items list of lines to sync
    auto it = pLineToSplit->DestinationItem()->InConnections().find(pLineToSplit);
    pLineToSplit->DestinationItem()->InConnections().erase(it);

    // Create a new line between the new circle item and the line to splits destination
    FsmConnectionGraphicsItem* newLine = new FsmConnectionGraphicsItem(pSplitter, pLineToSplit->DestinationItem());
    pSplitter->OutConnections().insert(newLine);
    pLineToSplit->DestinationItem()->InConnections().insert(newLine);
    addItem(newLine);

    // Set the split lines new destination to the circle item
    pLineToSplit->SetDestinationItem(pSplitter);

    // Ensure the circle moves the source line
    pSplitter->InConnections().insert(pLineToSplit);

    // Turn off the arrow head on the split line
    pLineToSplit->EnableArrowHead(false);

    // If the new line segment is connected to a state and not another connection point then enable the arrow head
    newLine->EnableArrowHead(newLine->DestinationItem()->IsTerminal());

    // Fix up the positions
    pLineToSplit->SyncPosition();
    newLine->SyncPosition();
}

void FsmGraphicsScene::mousePressEvent(QGraphicsSceneMouseEvent* pMouseEvent)
{
    if (pMouseEvent->button() == Qt::MiddleButton)
    {
        QGraphicsItem* pItemUnderMouse = itemAt(pMouseEvent->scenePos(), QTransform());
        if (pItemUnderMouse && pItemUnderMouse->type() == FsmStateGraphicsItem::Type)
        {
            mInProgressConnection = new FsmConnectionGraphicsItem();
            mInProgressConnection->setLine(QLineF(pMouseEvent->scenePos(), pMouseEvent->scenePos()));
            addItem(mInProgressConnection);
        }
        else if (pItemUnderMouse && pItemUnderMouse->type() == FsmConnectionGraphicsItem::Type)
        {
            FsmConnectionGraphicsItem* pConnection = qgraphicsitem_cast<FsmConnectionGraphicsItem*>(pItemUnderMouse);
            SplitLine(pConnection, pMouseEvent->scenePos());
        }
    }
    else if (pMouseEvent->button() != Qt::LeftButton)
    {
        return;
    }

    QGraphicsScene::mousePressEvent(pMouseEvent);
}

void FsmGraphicsScene::mouseMoveEvent(QGraphicsSceneMouseEvent* pMouseEvent)
{
    if (mInProgressConnection)
    {
        QLineF line = mInProgressConnection->line();
        line.setP2(pMouseEvent->scenePos());
        mInProgressConnection->setLine(line);
    }
    QGraphicsScene::mouseMoveEvent(pMouseEvent);
}

static FsmStateGraphicsItem* GetDestinationState(FsmConnectionGraphicsItem* pConnection)
{
    while (!pConnection->DestinationItem()->IsTerminal())
    {
        pConnection = (*pConnection->DestinationItem()->OutConnections().begin());
    }

    return qgraphicsitem_cast<FsmStateGraphicsItem*>(pConnection->DestinationItem()->AsGraphicsItem());
}

void FsmGraphicsScene::mouseReleaseEvent(QGraphicsSceneMouseEvent* pMouseEvent)
{
    if (mInProgressConnection)
    {
        QList<QGraphicsItem *> startItems = items(mInProgressConnection->line().p1());
        if (startItems.count() && startItems.first() == mInProgressConnection)
        {
            startItems.removeFirst();
        }

        QList<QGraphicsItem *> endItems = items(mInProgressConnection->line().p2());
        if (endItems.count() && endItems.first() == mInProgressConnection)
        {
            endItems.removeFirst();
        }

        removeItem(mInProgressConnection);
        delete mInProgressConnection;
        mInProgressConnection = nullptr;

        // Check we have a source and destination, that they are both FsmStateGraphicsItem's
        // and isn't attempting to connect to itself.
        if (startItems.count() > 0 &&
            endItems.count() > 0 &&
            startItems.first()->type() == FsmStateGraphicsItem::Type &&
            endItems.first()->type() == FsmStateGraphicsItem::Type &&
            startItems.first() != endItems.first())
        {
            FsmStateGraphicsItem* sourceItem = qgraphicsitem_cast<FsmStateGraphicsItem *>(startItems.first());
            FsmStateGraphicsItem* destinationItem = qgraphicsitem_cast<FsmStateGraphicsItem *>(endItems.first());

            // Check there isn't already a connection between source and destination
            bool alreadyExists = false;
            foreach(FsmConnectionGraphicsItem* outConnnection, sourceItem->OutConnections())
            {
                if (GetDestinationState(outConnnection) == destinationItem)
                {
                    clearSelection();
                    outConnnection->setSelected(true);
                    alreadyExists = true;
                    break;
                }
            }

            if (!alreadyExists)
            {
                FsmConnectionGraphicsItem* connection = new FsmConnectionGraphicsItem(sourceItem, destinationItem);
                sourceItem->OutConnections().insert(connection);
                destinationItem->InConnections().insert(connection);
                connection->EnableArrowHead(true);
                connection->setZValue(-1000.0);
                addItem(connection);
                connection->SyncPosition();
            }
        }
    }
    QGraphicsScene::mouseReleaseEvent(pMouseEvent);
}

void FsmGraphicsScene::DeleteAllSegments(FsmConnectionGraphicsItem* connection)
{
    // Disconnect from the source item
    FsmStateGraphicsItem* pSourceState = qgraphicsitem_cast<FsmStateGraphicsItem*>(connection->SourceItem()->AsGraphicsItem());
    pSourceState->OutConnections().remove(connection);
    for (;;)
    {
        IConnectableItem* pDestination = connection->DestinationItem();

        // Remove the line segment
        removeItem(connection);

        if (pDestination->IsTerminal())
        {
            FsmStateGraphicsItem* pDestinationState = qgraphicsitem_cast<FsmStateGraphicsItem*>(pDestination->AsGraphicsItem());
            pDestinationState->InConnections().remove(connection);
            delete connection;
            break;
        }
        else
        {
            FsmConnectionSplitter* pSplitter = qgraphicsitem_cast<FsmConnectionSplitter*>(pDestination->AsGraphicsItem());

            delete connection;

            connection = *pSplitter->OutConnections().begin();

            // Remove the splitter
            removeItem(pSplitter);
            delete pSplitter;

        }
    }
}

FsmConnectionGraphicsItem* FsmGraphicsScene::GetStartingSegment(FsmConnectionGraphicsItem* pArbitarySegment)
{
    FsmConnectionGraphicsItem* pStartingSegment = pArbitarySegment;
    for (;;)
    {
        // We've reached the starting state item so stop just before it
        if (pStartingSegment->SourceItem()->IsTerminal())
        {
            break;
        }
        // Otherwise its a connection point so get its input segment
        pStartingSegment = *pStartingSegment->SourceItem()->InConnections().begin();
    }
    return pStartingSegment;
}

template<class T>
static auto GetItemsOfType(QList<QGraphicsItem*>& items)
{
    QSet<T*> itemsOfT;
    foreach (QGraphicsItem* item, items)
    {
        if (item->type() == T::Type)
        {
            itemsOfT.insert(qgraphicsitem_cast<T*>(item));
        }
    }
    return itemsOfT;
}

template<class T, class Filter>
static auto GetItemsOfTypeFiltered(QList<QGraphicsItem*>& items, Filter filter)
{
    auto itemsOfT = GetItemsOfType<T>(items);
    QSet<T*> filteredItemsOfT;
    foreach (T* item, itemsOfT)
    {
        filteredItemsOfT.insert(filter(item));
    }
    return filteredItemsOfT;
}

void FsmGraphicsScene::DeleteSelection()
{
    // Get all selected state items
    QSet<FsmStateGraphicsItem*> statesToDelete = GetItemsOfType<FsmStateGraphicsItem>(selectedItems());

    // Collect all in/out line segments from selected states
    QSet<FsmConnectionGraphicsItem*> startingSelectedSegments;
    foreach (FsmStateGraphicsItem* item, statesToDelete)
    {
        foreach(FsmConnectionGraphicsItem* pOutConnection, item->OutConnections())
        {
            startingSelectedSegments.insert(GetStartingSegment(pOutConnection));
        }

        foreach(FsmConnectionGraphicsItem* pInConnection, item->InConnections())
        {
            startingSelectedSegments.insert(GetStartingSegment(pInConnection));
        }
    }

    // Get line segments that are selected but the connecte state(s) are not selected
    startingSelectedSegments += GetItemsOfTypeFiltered<FsmConnectionGraphicsItem>(selectedItems(), GetStartingSegment);

    // Erase the line segements
    foreach (FsmConnectionGraphicsItem* item, startingSelectedSegments)
    {
        DeleteAllSegments(item);
    }

    // Erase the states
    foreach (FsmStateGraphicsItem* item, statesToDelete)
    {
        removeItem(item);
        delete item;
    }

    // Collect the connection splitters, these must have been splitters selected that didn't have
    // the connected to state(s) or line segments selected at this point as we have deleted those before this point.
    QSet<FsmConnectionSplitter*> connectionSplitters = GetItemsOfType<FsmConnectionSplitter>(selectedItems());

    // Remove the connection point and "merge" line 2 remaining line segments
    foreach (FsmConnectionSplitter* splitterToDelete, connectionSplitters)
    {
        // Get the items that the circle connects to
        IConnectableItem* pNextDestination = (*splitterToDelete->OutConnections().begin())->DestinationItem();

        // Set the destination of the input from the circle being deleted to the item the circle was connected to
        (*splitterToDelete->InConnections().begin())->SetDestinationItem(pNextDestination);

        (*splitterToDelete->InConnections().begin())->SyncPosition();
        (*splitterToDelete->InConnections().begin())->EnableArrowHead(pNextDestination->IsTerminal());

        pNextDestination->InConnections().remove(*splitterToDelete->OutConnections().begin());
        pNextDestination->InConnections().insert(*splitterToDelete->InConnections().begin());

        // Remove the circle and redundant line segment from the scene
        removeItem(*splitterToDelete->OutConnections().begin());
        removeItem(splitterToDelete);
        delete (*splitterToDelete->OutConnections().begin());
        delete splitterToDelete;
    }
}

bool FsmGraphicsScene::Open(QString fileName)
{
    // TODO
    return false;
}

bool FsmGraphicsScene::Save(QString fileName)
{
    // TODO
    return false;
}

/*
void FsmGraphicsScene::SelectionChanged()
{
    // TODO: Make this work properly
    mUndoStack.push(new SelectionChangedCommand(this));
}
*/
