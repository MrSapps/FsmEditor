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
#include <QCloseEvent>
#include <QFileDialog>
#include "fsmgraphicsscene.h"
#include "fsmstategraphicsitem.h"
#include "propertytreewidget.h"

/*
TODO:
Impl commands:
Open
New
Save
Save as
exit

Help
Show grid
snap to grid
size scene to content
resize scene

add cmds to tool bar/add icons

Serialization
Undo/redo
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

    QList<QAction*> actions;
    actions.push_back(mUndoStack.createUndoAction(this));
    actions.push_back(mUndoStack.createRedoAction(this));
    mUi->menuEdit->insertActions(mUi->actionDelete, actions);

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

void FsmEditor::on_actionExit_triggered()
{
    close();
}

static bool ContinueOnUnsavedChanges(QWidget* pParent)
{
    const auto ret = QMessageBox::question(pParent, "Fsm editor", "There are unsaved changes, continue?", // TODO tr("")
                     QMessageBox::Cancel | QMessageBox::No | QMessageBox::Yes,
                     QMessageBox::Yes);
    return ret == QMessageBox::Yes;
}

void FsmEditor::on_actionOpen_triggered()
{
    bool doOpen = true;
    if (!mUndoStack.isClean())
    {
        doOpen = ContinueOnUnsavedChanges(this);
    }

    if (doOpen)
    {
        QString fileName = QFileDialog::getOpenFileName(this, tr("Open FSM"), "", tr("FSM Files (*.fsm)"));
        if (!fileName.isEmpty())
        {
            DoClean();
            if (mScene->Open(fileName))
            {
                mFileName = fileName;
                mUi->statusBar->showMessage(tr("Loaded ") + mFileName);
            }
        }
    }
}

void FsmEditor::on_actionSave_As_triggered()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save FSM"), "", tr("FSM Files (*.fsm)"));
    if (!fileName.isEmpty())
    {
        mFileName = fileName;
        on_actionSave_triggered();
    }
}

void FsmEditor::on_actionSave_triggered()
{
    if (mFileName.isEmpty())
    {
        on_actionSave_As_triggered();
        return;
    }

    if (mScene->Save(mFileName))
    {
        mUi->statusBar->showMessage(tr("Saved ") + mFileName);
    }
}

void FsmEditor::on_actionNew_triggered()
{
    bool clearDown = true;
    if (!mUndoStack.isClean())
    {
        clearDown = ContinueOnUnsavedChanges(this);
    }

    if (clearDown)
    {
        DoClean();
    }
}

void FsmEditor::DoClean()
{
    mUndoStack.clear();
    mScene->clear();
    mFileName = "";
}

void FsmEditor::closeEvent(QCloseEvent* event)
{
    if (mUndoStack.isClean())
    {
        event->accept();
        return;
    }

    if (ContinueOnUnsavedChanges(this))
    {
        event->accept();
    }
    else
    {
        event->ignore();
    }
}

