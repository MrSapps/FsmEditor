#include "propertytreewidget.h"
#include <QHeaderView>
#include <QLineEdit>
#include <QKeyEvent>
#include <QTimer>

void LineEditProperty::CreateEditorWidget()
{
    mWidget = new QLineEdit();
    // When the user clicks off the property text edit in the tree it will auto be deleted
    // so we use destroyed to ensure mWidget does not become dangling.
    connect(mWidget, SIGNAL(destroyed(QObject*)), this, SLOT(QLineEditWidgetDeleted(QObject*)));

    // Wire up pressing enter in the text edit as a short cut to saving the changes.
    connect(mWidget, SIGNAL(returnPressed()), this, SLOT(Commit()));

    // Set the "Value" column text as the line edit text and select all of it
    // so it is deleted/replaced as soon as the user starts typing in it.
    mWidget->setText(text(Columns::eValue));
    mWidget->selectAll();

    treeWidget()->setItemWidget(this, Columns::eValue, mWidget);
    mWidget->setFocus();
}

void LineEditProperty::Commit()
{
    if (mWidget)
    {
        emit OnCommit(mWidget->text());
        setText(Columns::eValue, mWidget->text());
    }
    treeWidget()->setItemWidget(this, Columns::eValue, nullptr);
}

void LineEditProperty::QLineEditWidgetDeleted(QObject*)
{
    mWidget = nullptr;
}

void ReadOnlyTextProperty::CreateEditorWidget()
{
    // Read only so no editor widget to create
}

void ReadOnlyTextProperty::Commit()
{
    // Read only so never anything to update
}

PropertyTreeWidget::PropertyTreeWidget(QWidget* pParent)
 : QTreeWidget(pParent)
{
    Reset();
    connect(this, SIGNAL(itemClicked(QTreeWidgetItem*,int)), this, SLOT(TreeItemClicked(QTreeWidgetItem*,int)));
}

void PropertyTreeWidget::Reset()
{
    clear();
    setColumnCount(2);
    setAlternatingRowColors(true);

    // Set sane default sizes
    header()->resizeSection( 0, 120 );
    header()->resizeSection( 1, 110 );

    setSelectionMode(QAbstractItemView::SingleSelection);
    setSelectionBehavior(QAbstractItemView::SelectRows);

    setHeaderLabels(QStringList{tr("Name"), tr("Value") });

    setIndentation(0);

    mLastClickedItem = nullptr;
}

void PropertyTreeWidget::TreeItemClicked(QTreeWidgetItem* pItem, int index)
{
    if (mLastClickedItem && mLastClickedItem != pItem)
    {
        dynamic_cast<IProperty*>(mLastClickedItem)->Commit();
    }

    mLastClickedItem = pItem;

    if (index == Columns::eValue)
    {
        dynamic_cast<IProperty*>(pItem)->CreateEditorWidget();
    }
    else
    {
        dynamic_cast<IProperty*>(pItem)->Commit();
    }
}

void PropertyTreeWidget::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Escape)
    {
        event->accept();
        auto item = currentItem();
        if ( item )
        {
            setItemWidget(item, Columns::eValue, nullptr);
        }
    }
    else
    {
        QTreeView::keyPressEvent(event);
    }
}

void PropertyTreeWidget::mousePressEvent(QMouseEvent* event)
{
    if ( mLastClickedItem )
    {
        dynamic_cast<IProperty*>(mLastClickedItem)->Commit();
        mLastClickedItem = nullptr;
    }
    QTreeWidget::mousePressEvent(event);
}
