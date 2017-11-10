#include "fsmgraphicsscene.h"
#include "fsmstategraphicsitem.h"

#include <QUndoCommand>
#include <QGraphicsSceneMouseEvent>

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
    connect(this, SIGNAL(selectionChanged()), this, SLOT(SelectionChanged()));

    setSceneRect(0, 0, 700, 700);
}

void FsmGraphicsScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *pEvent)
{
    mUndoStack.push(new AddStateCommand(this, pEvent->scenePos()));
}

void FsmGraphicsScene::mousePressEvent(QGraphicsSceneMouseEvent *pMouseEvent)
{
    if (pMouseEvent->button() != Qt::LeftButton)
    {
        return;
    }
    QGraphicsScene::mousePressEvent(pMouseEvent);
}

void FsmGraphicsScene::mouseMoveEvent(QGraphicsSceneMouseEvent *pMouseEvent)
{
    QGraphicsScene::mouseMoveEvent(pMouseEvent);
}

void FsmGraphicsScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *pMouseEvent)
{
    QGraphicsScene::mouseReleaseEvent(pMouseEvent);
}

void FsmGraphicsScene::SelectionChanged()
{
    // TODO: Make this work properly
    mUndoStack.push(new SelectionChangedCommand(this));
}
