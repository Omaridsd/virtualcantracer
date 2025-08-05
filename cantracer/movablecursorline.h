// movablecursorline.h
#pragma once

#include <QGraphicsLineItem>

class MovableCursorLine : public QGraphicsLineItem {
public:
    MovableCursorLine(QLineF line) : QGraphicsLineItem(line) {
        setFlags(ItemIsMovable | ItemSendsScenePositionChanges);
        setZValue(10);
    }

protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override {
        if (change == ItemPositionChange) {
            QPointF newPos = value.toPointF();
            // Ne permettre que le mouvement horizontal
            return QPointF(newPos.x(), pos().y());
        }
        return QGraphicsLineItem::itemChange(change, value);
    }
};
