#include "iconnectableitem.h"
#include "fsmconnectiongraphicsitem.h"

void IConnectableItem::HandleItemChange(QGraphicsItem::GraphicsItemChange change)
{
    if (change == QGraphicsItem::ItemPositionChange || change == QGraphicsItem::ItemPositionHasChanged)
    {
        foreach (FsmConnectionGraphicsItem* connection, mInConnections)
        {
            connection->SyncPosition();
        }

        foreach (FsmConnectionGraphicsItem* connection, mOutConnections)
        {
            connection->SyncPosition();
        }
    }
}
