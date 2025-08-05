#ifndef MOVABLELINEITEM_H
#define MOVABLELINEITEM_H

#include <QGraphicsLineItem>
#include <QGraphicsSceneMouseEvent>
#include <QObject>

class MovableLineItem : public QObject, public QGraphicsLineItem
{
    Q_OBJECT
public:
    explicit MovableLineItem(QLineF line, QGraphicsItem* parent = nullptr)
        : QObject(), QGraphicsLineItem(line, parent)
    {
        setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemSendsGeometryChanges);
        setCursor(Qt::SizeHorCursor);
    }

signals:
    void positionChanged(qreal x);

protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override {
        if (change == ItemPositionChange) {
            QPointF newPos = value.toPointF();
            // Autoriser uniquement déplacement horizontal
            return QPointF(newPos.x(), pos().y());
        }
        else if (change == ItemPositionHasChanged) {
            emit positionChanged(pos().x());
        }
        return QGraphicsLineItem::itemChange(change, value);
    }

    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override {
        QGraphicsLineItem::mouseMoveEvent(event);
        // positionChanged sera émis dans itemChange
    }
};

#endif // MOVABLELINEITEM_H
