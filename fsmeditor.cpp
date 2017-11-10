#include "fsmeditor.h"
#include "ui_fsmeditor.h"

#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsItem>
#include <QGraphicsSceneMouseEvent>
#include <QUndoCommand>
#include <QUndoStack>
#include <QUndoView>
#include <QDockWidget>
#include <QListWidget>

/*
TODO:
Add state items
Add connection items
Add connection split items
Delete circles/merge lines
Deleteing state delete whole connection
Deleting part of a connection delete whole connection
Connecting items follows connection segments
Rename state text
Undo/redo
Serialization
*/

class FsmGraphicsConnection;

constexpr int kRectSize = 200 / 2;

class FsmStateGraphicsItem : public QGraphicsItem
{
public:
    FsmStateGraphicsItem(QGraphicsItem* pParent = nullptr)
     : QGraphicsItem(pParent)
    {
        setFlag(QGraphicsItem::ItemIsMovable, true);
        setFlag(QGraphicsItem::ItemIsSelectable, true);
        setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);

        mRect.setX(-kRectSize);
        mRect.setY(-kRectSize);
        mRect.setWidth(kRectSize);
        mRect.setHeight(kRectSize);
    }

    enum
    {
        Type = UserType + 1
    };

    virtual int type() const override
    {
        return Type;
    }

    virtual QVariant itemChange(GraphicsItemChange change, const QVariant& value) override
    {
        if (change == QGraphicsItem::ItemPositionChange || change == QGraphicsItem::ItemPositionHasChanged)
        {
            foreach (FsmGraphicsConnection* connection, mInboundConnections)
            {
                //connection->UpdatePosition();
            }

            foreach (FsmGraphicsConnection* connection, mOutboundConnections)
            {
                //connection->UpdatePosition();
            }
        }
        return value;
    }

    virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override
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

class FsmConnectionData
{
public:
    FsmStateGraphicsItem* mStart;
    FsmStateGraphicsItem* mEnd;
};

class FsmConnectionGraphicsItem
{
public:
    QSharedPointer<FsmConnectionData> mConnectionData;
};

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

class FsmGraphicsScene : public QGraphicsScene
{
    //Q_OBJECT
public:
    FsmGraphicsScene(QObject* pParent, QUndoStack& undoStack)
      : QGraphicsScene(pParent), mUndoStack(undoStack)
    {
        connect(this, SIGNAL(selectionChanged()), this, SLOT(SelectionChanged()));

        setSceneRect(0, 0, 700, 700);
    }

    virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* pEvent) override
    {
        mUndoStack.push(new AddStateCommand(this, pEvent->scenePos()));
    }

    virtual void mousePressEvent(QGraphicsSceneMouseEvent* pMouseEvent) override
    {
        if (pMouseEvent->button() != Qt::LeftButton)
        {
            return;
        }
        QGraphicsScene::mousePressEvent(pMouseEvent);
    }

    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent* pMouseEvent) override
    {
        QGraphicsScene::mouseMoveEvent(pMouseEvent);
    }

    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent* pMouseEvent) override
    {
        QGraphicsScene::mouseReleaseEvent(pMouseEvent);
    }

//public slots:
    void SelectionChanged()
    {

    }

private:
    QUndoStack& mUndoStack;
};

FsmEditor::FsmEditor(QWidget* parent)
  : QMainWindow(parent), mUi(new Ui::FsmEditor)
{
    mUi->setupUi(this);

    QGraphicsView* graphicsView = new QGraphicsView(this);
    graphicsView->setScene(new FsmGraphicsScene(graphicsView, mUndoStack));
    setCentralWidget(graphicsView);

    {
        QDockWidget* dock = new QDockWidget(tr("Property editor"), this);
        dock->setFeatures(QDockWidget::DockWidgetMovable);

        QListWidget* listWidget = new QListWidget(this);
        listWidget->addItem("Property test");

        dock->setWidget(listWidget);
        addDockWidget(Qt::RightDockWidgetArea, dock);
    }

    {
        QDockWidget* dock = new QDockWidget(tr("Edit history"), this);
        dock->setFeatures(QDockWidget::DockWidgetMovable);

        QUndoView* undoView = new QUndoView(this);
        undoView->setStack(&mUndoStack);

        dock->setWidget(undoView);
        addDockWidget(Qt::RightDockWidgetArea, dock);

    }

    statusBar()->showMessage(tr("Ready"));

    setContextMenuPolicy(Qt::NoContextMenu);
}

FsmEditor::~FsmEditor()
{
    delete mUi;
}
