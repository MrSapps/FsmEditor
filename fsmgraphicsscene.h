#ifndef FSMGRAPHICSSCENE_H
#define FSMGRAPHICSSCENE_H

#include <QGraphicsScene>
#include <QUndoStack>

class FsmGraphicsScene : public QGraphicsScene
{
    Q_OBJECT
public:
    FsmGraphicsScene(QObject* pParent, QUndoStack& undoStack);

    virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* pEvent) override;

    virtual void mousePressEvent(QGraphicsSceneMouseEvent* pMouseEvent) override;

    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent* pMouseEvent) override;

    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent* pMouseEvent) override;

public slots:
    void SelectionChanged();

private:
    QUndoStack& mUndoStack;
};

#endif // FSMGRAPHICSSCENE_H
