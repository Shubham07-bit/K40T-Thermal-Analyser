#ifndef IMAGEVIEW_H
#define IMAGEVIEW_H

#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QPointF>
#include <QWheelEvent>

#include "ThermalBox.h"
#include "ThermalDataModel.h"

/**
 * @brief Interactive thermal image viewer.
 *
 * Features:
 *   - Pan and zoom with mouse wheel
 *   - Click to place persistent measurement points
 *   - Draw measurement boxes
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

    void setBoxDrawingMode(bool enabled);
    bool boxDrawingMode() const { return m_boxDrawingMode; }

    void setZoomRange(double minScale, double maxScale);

    void addUserPoint(const QPoint &pixel);
    void removeLastUserPoint();
    void clearUserPoints();
    QList<QPoint> userPoints() const { return m_userPoints; }

    void setBoxes(const QList<ThermalBox> &boxes);
    QList<ThermalBox> boxes() const { return m_boxes; }

signals:
    void pixelHovered(int x, int y, float temperature);
    void pixelClicked(int x, int y, float temperature);
    void minMaxChanged(float min, float max);
    void userPointsChanged(const QList<QPoint> &points);
    void boxDrawn(const QRect &rect);

protected:
    void wheelEvent(QWheelEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

private:
    void updateOverlays();
    QPoint imagePixelFromScene(const QPointF &scenePos) const;
    void emitPixelInfo(const QPointF &scenePos);
    void rebuildUserPointItems();
    void rebuildBoxItems();
    double currentScale() const;

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

    double m_minScale = 0.05;
    double m_maxScale = 20.0;

    QList<QPoint> m_userPoints;
    QList<QGraphicsEllipseItem *> m_userPointMarkers;
    QList<QGraphicsTextItem *> m_userPointLabels;

    bool m_boxDrawingMode = false;
    bool m_drawingBox = false;
    QPoint m_boxStartPixel;
    QGraphicsRectItem *m_activeBoxItem = nullptr;
    QList<ThermalBox> m_boxes;
    QList<QGraphicsRectItem *> m_boxRectItems;
    QList<QGraphicsTextItem *> m_boxLabelItems;
    QList<QGraphicsEllipseItem *> m_boxMinMarkers;
    QList<QGraphicsEllipseItem *> m_boxMaxMarkers;
};

#endif // IMAGEVIEW_H
