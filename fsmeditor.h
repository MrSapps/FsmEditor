#ifndef FSMEDITOR_H
#define FSMEDITOR_H

#include <QMainWindow>
#include <QUndoStack>

namespace Ui {
class FsmEditor;
}

class FsmEditor : public QMainWindow
{
    Q_OBJECT

public:
    explicit FsmEditor(QWidget *parent = 0);
    ~FsmEditor();

private:
    Ui::FsmEditor *ui;
    QUndoStack mUndoStack;
};

#endif // FSMEDITOR_H
