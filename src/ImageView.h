#ifndef IMAGEVIEW_H
#define IMAGEVIEW_H

#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QMouseEvent>
#include <QPointF>
#include <QWheelEvent>

#include "ThermalDataModel.h"

/**
 * @brief Interactive thermal image viewer.
 *
 * Features:
 *   - Pan and zoom with mouse wheel
 *   - Click to place persistent measurement points
 *   - Overlay min/max markers
 *   - Optional crosshair / cursor position display
 */
class ImageView : public QGraphicsView
{
    Q_OBJECT

public:
    explicit ImageView(QWidget *parent = nullptr);

    void setThermalData(const ThermalDataModel &data);
    void setRenderedImage(const QImage &image);
    void clear();

    void setShowMinMax(bool show);
    void setShowCrosshair(bool show);

    void addUserPoint(const QPoint &pixel);
    void removeLastUserPoint();
    void clearUserPoints();
    QList<QPoint> userPoints() const { return m_userPoints; }

signals:
    void pixelHovered(int x, int y, float temperature);
    void pixelClicked(int x, int y, float temperature);
    void minMaxChanged(float min, float max);
    void userPointsChanged(const QList<QPoint> &points);

protected:
    void wheelEvent(QWheelEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    void updateOverlays();
    QPoint imagePixelFromScene(const QPointF &scenePos) const;
    void emitPixelInfo(const QPointF &scenePos);
    void rebuildUserPointItems();

    QGraphicsScene *m_scene = nullptr;
    QGraphicsPixmapItem *m_pixmapItem = nullptr;

    QGraphicsEllipseItem *m_minMarker = nullptr;
    QGraphicsEllipseItem *m_maxMarker = nullptr;
    QGraphicsLineItem *m_crosshairH = nullptr;
    QGraphicsLineItem *m_crosshairV = nullptr;
    QGraphicsTextItem *m_minLabel = nullptr;
    QGraphicsTextItem *m_maxLabel = nullptr;

    ThermalDataModel m_data;
    bool m_showMinMax = true;
    bool m_showCrosshair = true;
    bool m_panning = false;
    QPoint m_lastPanPos;

    QList<QPoint> m_userPoints;
    QList<QGraphicsEllipseItem *> m_userPointMarkers;
    QList<QGraphicsTextItem *> m_userPointLabels;
};

#endif // IMAGEVIEW_H
