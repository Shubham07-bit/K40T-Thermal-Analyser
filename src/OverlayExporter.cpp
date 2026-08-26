#include "OverlayExporter.h"

#include <QPainter>

OverlayExporter::Result OverlayExporter::render(const ThermalDataModel &data,
                                                ColorMap::Palette palette,
                                                const QList<QPoint> &points,
                                                const QList<ThermalBox> &boxes,
                                                OverlayFlagSet flags,
                                                std::optional<float> min,
                                                std::optional<float> max,
                                                const QImage &baseImage)
{
    Result result;
    if (!data.isValid()) {
        result.error = QObject::tr("No valid thermal data to export.");
        return result;
    }

    QImage img = baseImage.isNull() ? ColorMap::render(data, palette, min, max) : baseImage;
    if (img.isNull()) {
        result.error = QObject::tr("Failed to render thermal image.");
        return result;
    }

    QPainter painter(&img);
    painter.setRenderHint(QPainter::Antialiasing);

    QFont labelFont;
    labelFont.setBold(true);
    labelFont.setPointSize(14);
    painter.setFont(labelFont);

    if (flags & MinMaxMarkers) {
        const QPoint minP = data.minTemperaturePixel();
        const QPoint maxP = data.maxTemperaturePixel();
        const int r = 12;

        // Min marker: blue
        QPen minPen(QColor(0, 0, 255));
        minPen.setWidth(3);
        painter.setPen(minPen);
        painter.setBrush(Qt::NoBrush);
        painter.drawEllipse(minP, r / 2, r / 2);

        // Max marker: red
        QPen maxPen(QColor(255, 0, 0));
        maxPen.setWidth(3);
        painter.setPen(maxPen);
        painter.setBrush(Qt::NoBrush);
        painter.drawEllipse(maxP, r / 2, r / 2);

        painter.setPen(QColor(0, 0, 255));
        painter.drawText(minP.x() + 12, minP.y() - 8,
                         QString("MIN %1C").arg(data.minTemperature(), 0, 'f', 1));
        painter.setPen(QColor(255, 0, 0));
        painter.drawText(maxP.x() + 12, maxP.y() - 8,
                         QString("MAX %1C").arg(data.maxTemperature(), 0, 'f', 1));
    }

    if (flags & UserPoints) {
        const int r = 12;
        QPen pen(QColor(255, 140, 0));   // orange
        pen.setWidth(3);
        painter.setPen(pen);
        painter.setBrush(Qt::NoBrush);

        for (int i = 0; i < points.size(); ++i) {
            const QPoint p = points[i];
            float t = data.temperatureAt(p.x(), p.y());
            painter.drawEllipse(p, r / 2, r / 2);
            painter.setPen(QColor(255, 140, 0));   // orange
            painter.drawText(p.x() + 12, p.y() - 8,
                             QString("P%1: %2C").arg(i + 1).arg(t, 0, 'f', 1));
            painter.setPen(pen);
        }
    }

    // Measurement boxes are always drawn if any exist.
    if (!boxes.isEmpty()) {
        QPen boxPen(QColor(0, 200, 0));
        boxPen.setWidth(3);
        painter.setPen(boxPen);
        painter.setBrush(QBrush(QColor(0, 200, 0, 40)));

        for (int i = 0; i < boxes.size(); ++i) {
            const ThermalBox &b = boxes[i];
            painter.drawRect(b.rect);
            painter.drawText(b.rect.x() + 12, b.rect.y() - 8,
                             QString("B%1: avg %2C, min %3C, max %4C")
                                 .arg(i + 1)
                                 .arg(b.avgTemperature, 0, 'f', 1)
                                 .arg(b.minTemperature, 0, 'f', 1)
                                 .arg(b.maxTemperature, 0, 'f', 1));
        }
    }

    painter.end();

    result.image = img;
    result.success = true;
    return result;
}

QString OverlayExporter::save(const Result &result, const QString &filePath)
{
    if (!result.success)
        return result.error;
    if (result.image.isNull())
        return QObject::tr("No image to save.");

    if (!result.image.save(filePath))
        return QObject::tr("Failed to save image to %1").arg(filePath);

    return QString();
}
