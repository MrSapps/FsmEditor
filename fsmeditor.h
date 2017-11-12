#ifndef FSMEDITOR_H
#define FSMEDITOR_H

#include <QMainWindow>
#include <QUndoStack>

namespace Ui
{
    class FsmEditor;
}

class FsmGraphicsScene;
class PropertyTreeWidget;
class QTreeWidgetItem;
class FsmStateGraphicsItem;

class FsmStatePropertyEditor : public QObject
{
    Q_OBJECT
public:
    FsmStatePropertyEditor(PropertyTreeWidget* widget, QObject* parent = nullptr);
    void SetSelection(FsmStateGraphicsItem* selection);
private slots:
    void OnCommitTextProperty(QString text);
private:
    PropertyTreeWidget* mWidget = nullptr;
    FsmStateGraphicsItem* mSelected = nullptr;
};

class FsmEditor : public QMainWindow
{
    Q_OBJECT
public:
    explicit FsmEditor(QWidget* parent = nullptr);
    ~FsmEditor();
public slots:
    void SelectionChanged();
private slots:
    void on_actionAbout_Qt_triggered();

    void on_actionAbout_triggered();

private:
    Ui::FsmEditor* mUi;
    QUndoStack mUndoStack;
    FsmGraphicsScene* mScene = nullptr;
    FsmStatePropertyEditor* mFsmStatePropertyEditor = nullptr;
};

#endif // FSMEDITOR_H
