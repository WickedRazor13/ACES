#include "imagescene.h"

ImageScene::ImageScene(QObject *parent)
    : QGraphicsScene{parent}
{

}

void ImageScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        QGraphicsItem *item = itemAt(event->scenePos(), QTransform());
        DifferenceItem *diffItem = dynamic_cast<DifferenceItem*>(item);
        //qDebug() << item;

        if (diffItem) {
            qDebug() << "Hit";
        }
        else {
            qDebug() << "Miss";
            emit clicked();
        }
    }

    QGraphicsScene::mousePressEvent(event);
}
