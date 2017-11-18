#ifndef PROPERTYTREEWIDGET_H
#define PROPERTYTREEWIDGET_H

#include <QTreeWidget>

class QLineEdit;

class IProperty
{
public:
    virtual ~IProperty() = default;
    virtual void CreateEditorWidget() = 0;
    virtual void Commit() = 0;
};

class LineEditProperty : public QObject, public QTreeWidgetItem, public IProperty
{
    Q_OBJECT
public:
    using QTreeWidgetItem::QTreeWidgetItem;
    virtual void CreateEditorWidget() override;
public slots:
    virtual void Commit() override;
signals:
    void OnCommit(QString text);
private slots:
    void QLineEditWidgetDeleted(QObject*);
private:
    QLineEdit* mWidget = nullptr;
};

class ReadOnlyTextProperty : public QTreeWidgetItem, public IProperty
{
    using QTreeWidgetItem::QTreeWidgetItem;
    virtual void CreateEditorWidget() override;
    virtual void Commit() override;
};

enum Columns
{
    eName,
    eValue
};

class PropertyTreeWidget : public QTreeWidget
{
    Q_OBJECT
public:
    PropertyTreeWidget(QWidget* pParent = nullptr);
    void Reset();
    virtual void keyPressEvent(QKeyEvent* event) override;
    virtual void mousePressEvent(QMouseEvent* event) override;
private slots:
    void TreeItemClicked(QTreeWidgetItem* pItem,int index);
private:
    QTreeWidgetItem* mLastClickedItem = nullptr;
};

#endif // PROPERTYTREEWIDGET_H
