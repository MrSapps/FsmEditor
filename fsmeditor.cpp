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

#include "fsmgraphicsscene.h"

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

class FsmStateGraphicsItem;

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
