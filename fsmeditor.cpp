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
#include <QTreeWidget>
#include <QMessageBox>
#include <QLineEdit>
#include "fsmgraphicsscene.h"
#include "fsmstategraphicsitem.h"
#include "propertytreewidget.h"

/*
TODO:
Delete circles/merge lines
Deleteing state delete whole connection
Deleting part of a connection delete whole connection
Connecting items follows connection segments
Rename state text
Undo/redo
Serialization
*/

FsmStatePropertyEditor::FsmStatePropertyEditor(PropertyTreeWidget* widget, QObject* parent)
 : QObject(parent), mWidget(widget)
{

}

void FsmStatePropertyEditor::SetSelection(FsmStateGraphicsItem* selection)
{
    if (mSelected != selection)
    {
        mWidget->Reset();
        mSelected = selection;
        if (mSelected)
        {
            LineEditProperty* item = new LineEditProperty(QStringList {tr("Name"), selection->Name() });
            connect(item, SIGNAL(OnCommit(QString)), this, SLOT(OnCommitTextProperty(QString)));
            mWidget->addTopLevelItem(item);

            /* TODO: Display target states and allow order re-arranging
            for (int i=0; i<selection->mOutboundConnections.size(); i++)
            {
                ReadOnlyTextProperty* item = new ReadOnlyTextProperty("Connection", selection->mOutboundConnections[i].Target()->Name());
                mWidget->addTopLevelItem(item);
            }*/
        }
    }
}

void FsmStatePropertyEditor::OnCommitTextProperty(QString text)
{
    mSelected->SetName(text);
    mSelected->scene()->invalidate();
}

FsmEditor::FsmEditor(QWidget* parent)
  : QMainWindow(parent), mUi(new Ui::FsmEditor())
{
    mUi->setupUi(this);

    QGraphicsView* graphicsView = new QGraphicsView(this);
    graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

    mScene = new FsmGraphicsScene(graphicsView, mUndoStack);

    graphicsView->setScene(mScene);
    setCentralWidget(graphicsView);

    PropertyTreeWidget* propertyListWidget = new PropertyTreeWidget(this);
    {
        QDockWidget* dock = new QDockWidget(tr("Property editor"), this);
        dock->setFeatures(QDockWidget::DockWidgetMovable);

        dock->setWidget(propertyListWidget);
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

    mFsmStatePropertyEditor = new FsmStatePropertyEditor(propertyListWidget, this);
    connect(mScene, SIGNAL(selectionChanged()), this, SLOT(SelectionChanged()));
}

void FsmEditor::SelectionChanged()
{
    QList<QGraphicsItem*> selection = mScene->selectedItems();
    if (selection.size() == 1 && selection.at(0)->type() == FsmStateGraphicsItem::Type)
    {
        FsmStateGraphicsItem* selectedItem = qgraphicsitem_cast<FsmStateGraphicsItem*>(selection.at(0));
        mFsmStatePropertyEditor->SetSelection(selectedItem);
    }
    else
    {
        mFsmStatePropertyEditor->SetSelection(nullptr);
    }
}

FsmEditor::~FsmEditor()
{
    delete mUi;
}

void FsmEditor::on_actionAbout_Qt_triggered()
{
    QMessageBox::aboutQt(this);
}

void FsmEditor::on_actionAbout_triggered()
{
    QMessageBox::information(this, tr("About"), tr("ALIVE engine object FSM editor."));
}

void FsmEditor::on_actionDelete_triggered()
{
    mScene->DeleteSelection();
}
