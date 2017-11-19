#ifndef FSMGRAPHICSSCENE_H
#define FSMGRAPHICSSCENE_H

#include <QGraphicsScene>
#include <QUndoStack>

class FsmConnectionGraphicsItem;

class FsmGraphicsScene : public QGraphicsScene
{
    //Q_OBJECT
public:
    FsmGraphicsScene(QObject* pParent, QUndoStack& undoStack);

    virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* pEvent) override;

    virtual void mousePressEvent(QGraphicsSceneMouseEvent* pMouseEvent) override;

    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent* pMouseEvent) override;

    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent* pMouseEvent) override;

    void DeleteSelection();

//public slots:
    //void SelectionChanged();

private:
    void DeleteAllSegments(FsmConnectionGraphicsItem* connection);
    FsmConnectionGraphicsItem* GetStartingSegment(FsmConnectionGraphicsItem* pArbitarySegment);

    void SplitLine(FsmConnectionGraphicsItem* pLineToSplit, QPointF splitPos);

    QUndoStack& mUndoStack;
    FsmConnectionGraphicsItem* mInProgressConnection = nullptr;
};

#endif // FSMGRAPHICSSCENE_H
