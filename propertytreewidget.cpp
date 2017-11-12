#include "propertytreewidget.h"
#include <QHeaderView>
#include <QLineEdit>
#include <QKeyEvent>
#include <QTimer>

void LineEditProperty::CreateWidget()
{
    mWidget = new QLineEdit();
    connect(mWidget, SIGNAL(destroyed(QObject*)), this, SLOT(WidgetDeleted(QObject*)));
    connect(mWidget, SIGNAL(returnPressed()), this, SLOT(Commit()));
    mWidget->setText(text(1));
    mWidget->selectAll();

    treeWidget()->setItemWidget(this, 1, mWidget);
    mWidget->setFocus();
}

void LineEditProperty::Commit()
{
    if (mWidget)
    {
        emit OnCommit(mWidget->text());
        setText(1, mWidget->text());
    }
    treeWidget()->setItemWidget(this, 1, nullptr);
}

void LineEditProperty::WidgetDeleted(QObject*)
{
    mWidget = nullptr;
}

void ReadOnlyTextProperty::CreateWidget()
{

}

void ReadOnlyTextProperty::Commit()
{

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

    if (index == 1)
    {
        dynamic_cast<IProperty*>(pItem)->CreateWidget();
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
            setItemWidget(item, 1, nullptr);
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
