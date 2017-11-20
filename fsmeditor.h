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
class QCloseEvent;

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

    void on_actionDelete_triggered();

    void on_actionExit_triggered();

    void on_actionSave_As_triggered();

    void on_actionSave_triggered();

    void on_actionNew_triggered();
    void on_actionOpen_triggered();

private:
    virtual void closeEvent(QCloseEvent* event) override;
private:
    Ui::FsmEditor* mUi;
    QUndoStack mUndoStack;
    FsmGraphicsScene* mScene = nullptr;
    FsmStatePropertyEditor* mFsmStatePropertyEditor = nullptr;
    QString mFileName;
};

#endif // FSMEDITOR_H
