#ifndef IMAGESCENE_H
#define IMAGESCENE_H

#include <QGraphicsScene>
#include <QObject>
#include <QDebug>
#include "qgraphicssceneevent.h"
#include "QGraphicsItem"
#include "differenceItem.h"

class ImageScene : public QGraphicsScene
{
    Q_OBJECT

public:
    explicit ImageScene(QObject *parent = nullptr);

signals:
    void clicked();

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
};

#endif // IMAGESCENE_H
