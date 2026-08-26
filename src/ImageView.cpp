#include "ImageView.h"

#include <QGraphicsEllipseItem>
#include <QGraphicsLineItem>
#include <QGraphicsRectItem>
#include <QGraphicsTextItem>
#include <QScrollBar>

ImageView::ImageView(QWidget *parent)
    : QGraphicsView(parent)
{
    setDragMode(QGraphicsView::NoDrag);
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    setResizeAnchor(QGraphicsView::AnchorViewCenter);

    m_scene = new QGraphicsScene(this);
    setScene(m_scene);

    m_pixmapItem = m_scene->addPixmap(QPixmap());
    m_pixmapItem->setZValue(0);

    QPen minPen(QColor(0, 0, 255));
    minPen.setWidth(2);
    QPen maxPen(QColor(255, 0, 0));
    maxPen.setWidth(2);

    m_minMarker = m_scene->addEllipse(0, 0, 10, 10, minPen, QBrush(Qt::NoBrush));
    m_minMarker->setZValue(10);
    m_minMarker->setVisible(false);

    m_maxMarker = m_scene->addEllipse(0, 0, 10, 10, maxPen, QBrush(Qt::NoBrush));
    m_maxMarker->setZValue(10);
    m_maxMarker->setVisible(false);

    QFont labelFont;
    labelFont.setBold(true);
    labelFont.setPointSize(10);

    m_minLabel = m_scene->addText("MIN", labelFont);
    m_minLabel->setDefaultTextColor(QColor(0, 0, 255));
    m_minLabel->setZValue(11);
    m_minLabel->setVisible(false);

    m_maxLabel = m_scene->addText("MAX", labelFont);
    m_maxLabel->setDefaultTextColor(QColor(255, 0, 0));
    m_maxLabel->setZValue(11);
    m_maxLabel->setVisible(false);

    QPen crossPen(QColor(255, 255, 0, 180));
    crossPen.setWidth(1);
    m_crosshairH = m_scene->addLine(0, 0, 0, 0, crossPen);
    m_crosshairV = m_scene->addLine(0, 0, 0, 0, crossPen);
    m_crosshairH->setZValue(5);
    m_crosshairV->setZValue(5);
    m_crosshairH->setVisible(false);
    m_crosshairV->setVisible(false);

    m_activeBoxItem = m_scene->addRect(QRectF(), QPen(QColor(0, 255, 0), 2, Qt::DashLine), QBrush(QColor(0, 255, 0, 30)));
    m_activeBoxItem->setZValue(14);
    m_activeBoxItem->setVisible(false);
}

void ImageView::setThermalData(const ThermalDataModel &data)
{
    m_data = data;
    updateOverlays();
    rebuildUserPointItems();
    rebuildBoxItems();
    if (m_data.isValid()) {
        emit minMaxChanged(m_data.minTemperature(), m_data.maxTemperature());
    }
}

void ImageView::setRenderedImage(const QImage &image)
{
    if (image.isNull()) {
        m_pixmapItem->setPixmap(QPixmap());
        m_scene->setSceneRect(0, 0, 0, 0);
        return;
    }

    m_pixmapItem->setPixmap(QPixmap::fromImage(image));
    m_scene->setSceneRect(image.rect());
    updateOverlays();
    rebuildUserPointItems();
    rebuildBoxItems();
}

void ImageView::clear()
{
    m_data.clear();
    m_pixmapItem->setPixmap(QPixmap());
    m_scene->setSceneRect(0, 0, 0, 0);
    m_minMarker->setVisible(false);
    m_maxMarker->setVisible(false);
    m_minLabel->setVisible(false);
    m_maxLabel->setVisible(false);
    m_crosshairH->setVisible(false);
    m_crosshairV->setVisible(false);
    clearUserPoints();
    setBoxes(QList<ThermalBox>());
}

void ImageView::setShowMinMax(bool show)
{
    m_showMinMax = show;
    updateOverlays();
}

void ImageView::setShowCrosshair(bool show)
{
    m_showCrosshair = show;
    if (!show) {
        m_crosshairH->setVisible(false);
        m_crosshairV->setVisible(false);
    }
}

void ImageView::setBoxDrawingMode(bool enabled)
{
    m_boxDrawingMode = enabled;
    setCursor(enabled ? Qt::CrossCursor : Qt::ArrowCursor);
}

void ImageView::setZoomRange(double minScale, double maxScale)
{
    m_minScale = minScale;
    m_maxScale = maxScale;
}

void ImageView::addUserPoint(const QPoint &pixel)
{
    if (!m_data.isValid())
        return;
    if (pixel.x() < 0 || pixel.x() >= m_data.width() || pixel.y() < 0 || pixel.y() >= m_data.height())
        return;

    m_userPoints.append(pixel);
    rebuildUserPointItems();
    emit userPointsChanged(m_userPoints);
}

void ImageView::removeLastUserPoint()
{
    if (m_userPoints.isEmpty())
        return;
    m_userPoints.removeLast();
    rebuildUserPointItems();
    emit userPointsChanged(m_userPoints);
}

void ImageView::clearUserPoints()
{
    if (m_userPoints.isEmpty())
        return;
    m_userPoints.clear();
    rebuildUserPointItems();
    emit userPointsChanged(m_userPoints);
}

void ImageView::setBoxes(const QList<ThermalBox> &boxes)
{
    m_boxes = boxes;
    rebuildBoxItems();
}

void ImageView::wheelEvent(QWheelEvent *event)
{
    const double scaleFactor = 1.15;
    const double current = currentScale();
    double target;
    if (event->angleDelta().y() > 0) {
        target = current * scaleFactor;
    } else {
        target = current / scaleFactor;
    }
    target = qBound(m_minScale, target, m_maxScale);
    const double applied = target / current;
    scale(applied, applied);
    event->accept();
}

void ImageView::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        const QPointF scenePos = mapToScene(event->pos());
        emitPixelInfo(scenePos);
        QPoint px = imagePixelFromScene(scenePos);

        if (m_boxDrawingMode && m_data.isValid()) {
            m_drawingBox = true;
            m_boxStartPixel = px;
            m_activeBoxItem->setRect(QRectF(px.x(), px.y(), 0, 0));
            m_activeBoxItem->setVisible(true);
            return;
        }

        if (m_data.isValid() && px.x() >= 0 && px.x() < m_data.width() && px.y() >= 0 && px.y() < m_data.height()) {
            addUserPoint(px);
            emit pixelClicked(px.x(), px.y(), m_data.temperatureAt(px.x(), px.y()));
        }
    } else if (event->button() == Qt::MiddleButton || event->button() == Qt::RightButton) {
        m_panning = true;
        m_lastPanPos = event->pos();
        setCursor(Qt::ClosedHandCursor);
    }
    QGraphicsView::mousePressEvent(event);
}

void ImageView::mouseMoveEvent(QMouseEvent *event)
{
    const QPointF scenePos = mapToScene(event->pos());
    emitPixelInfo(scenePos);

    if (m_showCrosshair && m_data.isValid()) {
        m_crosshairH->setLine(0, scenePos.y(), m_data.width(), scenePos.y());
        m_crosshairV->setLine(scenePos.x(), 0, scenePos.x(), m_data.height());
        m_crosshairH->setVisible(true);
        m_crosshairV->setVisible(true);
    }

    if (m_drawingBox && m_data.isValid()) {
        QPoint px = imagePixelFromScene(scenePos);
        px.setX(qBound(0, px.x(), m_data.width() - 1));
        px.setY(qBound(0, px.y(), m_data.height() - 1));
        QRect r = QRect(m_boxStartPixel, px).normalized();
        m_activeBoxItem->setRect(QRectF(r));
        return;
    }

    if (m_panning) {
        const QPoint delta = event->pos() - m_lastPanPos;
        horizontalScrollBar()->setValue(horizontalScrollBar()->value() - delta.x());
        verticalScrollBar()->setValue(verticalScrollBar()->value() - delta.y());
        m_lastPanPos = event->pos();
    }

    QGraphicsView::mouseMoveEvent(event);
}

void ImageView::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && m_drawingBox) {
        m_drawingBox = false;
        m_activeBoxItem->setVisible(false);

        const QPointF scenePos = mapToScene(event->pos());
        QPoint px = imagePixelFromScene(scenePos);
        px.setX(qBound(0, px.x(), m_data.width() - 1));
        px.setY(qBound(0, px.y(), m_data.height() - 1));
        QRect r = QRect(m_boxStartPixel, px).normalized();
        if (r.width() > 0 && r.height() > 0) {
            emit boxDrawn(r);
        }
    } else if (event->button() == Qt::MiddleButton || event->button() == Qt::RightButton) {
        m_panning = false;
        setCursor(m_boxDrawingMode ? Qt::CrossCursor : Qt::ArrowCursor);
    }
    QGraphicsView::mouseReleaseEvent(event);
}

void ImageView::updateOverlays()
{
    if (!m_data.isValid()) {
        m_minMarker->setVisible(false);
        m_maxMarker->setVisible(false);
        m_minLabel->setVisible(false);
        m_maxLabel->setVisible(false);
        return;
    }

    if (m_showMinMax) {
        const QPoint minP = m_data.minTemperaturePixel();
        const QPoint maxP = m_data.maxTemperaturePixel();

        const int r = 10;
        m_minMarker->setRect(minP.x() - r / 2, minP.y() - r / 2, r, r);
        m_maxMarker->setRect(maxP.x() - r / 2, maxP.y() - r / 2, r, r);

        m_minLabel->setPlainText(QString("MIN %1C").arg(m_data.minTemperature(), 0, 'f', 1));
        m_maxLabel->setPlainText(QString("MAX %1C").arg(m_data.maxTemperature(), 0, 'f', 1));
        m_minLabel->setPos(minP.x() + 10, minP.y() - 20);
        m_maxLabel->setPos(maxP.x() + 10, maxP.y() - 20);

        m_minMarker->setVisible(true);
        m_maxMarker->setVisible(true);
        m_minLabel->setVisible(true);
        m_maxLabel->setVisible(true);
    } else {
        m_minMarker->setVisible(false);
        m_maxMarker->setVisible(false);
        m_minLabel->setVisible(false);
        m_maxLabel->setVisible(false);
    }
}

QPoint ImageView::imagePixelFromScene(const QPointF &scenePos) const
{
    return QPoint(static_cast<int>(scenePos.x()), static_cast<int>(scenePos.y()));
}

void ImageView::emitPixelInfo(const QPointF &scenePos)
{
    QPoint px = imagePixelFromScene(scenePos);
    if (m_data.isValid() && px.x() >= 0 && px.x() < m_data.width() && px.y() >= 0 && px.y() < m_data.height()) {
        float t = m_data.temperatureAt(px.x(), px.y());
        emit pixelHovered(px.x(), px.y(), t);
    }
}

void ImageView::rebuildUserPointItems()
{
    for (QGraphicsEllipseItem *item : std::as_const(m_userPointMarkers))
        m_scene->removeItem(item);
    for (QGraphicsTextItem *item : std::as_const(m_userPointLabels))
        m_scene->removeItem(item);
    for (QGraphicsEllipseItem *item : std::as_const(m_userPointMarkers))
        delete item;
    for (QGraphicsTextItem *item : std::as_const(m_userPointLabels))
        delete item;
    m_userPointMarkers.clear();
    m_userPointLabels.clear();

    if (!m_data.isValid())
        return;

    QPen pointPen(QColor(255, 140, 0));   // orange
    pointPen.setWidth(2);
    QFont labelFont;
    labelFont.setBold(true);
    labelFont.setPointSize(9);

    const int r = 10;
    for (int i = 0; i < m_userPoints.size(); ++i) {
        const QPoint p = m_userPoints[i];
        float t = m_data.temperatureAt(p.x(), p.y());

        auto *marker = new QGraphicsEllipseItem(p.x() - r / 2, p.y() - r / 2, r, r);
        marker->setPen(pointPen);
        marker->setBrush(Qt::NoBrush);
        marker->setZValue(12);
        m_scene->addItem(marker);
        m_userPointMarkers.append(marker);

        auto *label = new QGraphicsTextItem(QString("P%1: %2C").arg(i + 1).arg(t, 0, 'f', 1));
        label->setDefaultTextColor(QColor(255, 140, 0));   // orange
        label->setFont(labelFont);
        label->setPos(p.x() + 10, p.y() - 20);
        label->setZValue(13);
        m_scene->addItem(label);
        m_userPointLabels.append(label);
    }
}

void ImageView::rebuildBoxItems()
{
    for (QGraphicsRectItem *item : std::as_const(m_boxRectItems))
        m_scene->removeItem(item);
    for (QGraphicsTextItem *item : std::as_const(m_boxLabelItems))
        m_scene->removeItem(item);
    for (QGraphicsRectItem *item : std::as_const(m_boxRectItems))
        delete item;
    for (QGraphicsTextItem *item : std::as_const(m_boxLabelItems))
        delete item;
    m_boxRectItems.clear();
    m_boxLabelItems.clear();

    if (!m_data.isValid())
        return;

    QPen boxPen(QColor(0, 200, 0));
    boxPen.setWidth(2);
    QFont labelFont;
    labelFont.setBold(true);
    labelFont.setPointSize(9);

    for (int i = 0; i < m_boxes.size(); ++i) {
        const ThermalBox &b = m_boxes[i];
        auto *rect = new QGraphicsRectItem(QRectF(b.rect));
        rect->setPen(boxPen);
        rect->setBrush(QBrush(QColor(0, 200, 0, 30)));
        rect->setZValue(14);
        m_scene->addItem(rect);
        m_boxRectItems.append(rect);

        auto *label = new QGraphicsTextItem(
            QString("B%1: avg %2C, min %3C, max %4C")
                .arg(i + 1)
                .arg(b.avgTemperature, 0, 'f', 1)
                .arg(b.minTemperature, 0, 'f', 1)
                .arg(b.maxTemperature, 0, 'f', 1));
        label->setDefaultTextColor(QColor(0, 200, 0));
        label->setFont(labelFont);
        label->setPos(b.rect.x() + 10, b.rect.y() - 25);
        label->setZValue(15);
        m_scene->addItem(label);
        m_boxLabelItems.append(label);
    }
}

double ImageView::currentScale() const
{
    return transform().m11();
}
