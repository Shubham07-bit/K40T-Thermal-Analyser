#ifndef OVERLAYEXPORTER_H
#define OVERLAYEXPORTER_H

#include <QImage>
#include <QList>
#include <QPoint>
#include <optional>

#include "ColorMap.h"
#include "ThermalBox.h"
#include "ThermalDataModel.h"

/**
 * @brief Renders a thermal image with optional overlays for export.
 */
class OverlayExporter
{
public:
    enum OverlayFlags
    {
        NoOverlay       = 0x00,
        MinMaxMarkers   = 0x01,
        UserPoints      = 0x02,
        AllOverlays     = MinMaxMarkers | UserPoints
    };
    Q_DECLARE_FLAGS(OverlayFlagSet, OverlayFlags)

    struct Result
    {
        QImage image;
        bool success = false;
        QString error;
    };

    /**
     * @brief Render the thermal data with overlays.
     * @param data Thermal data model.
     * @param palette Color palette.
     * @param points User-defined measurement points.
     * @param flags Which overlays to draw.
     * @param min Optional manual min temperature.
     * @param max Optional manual max temperature.
     * @param baseImage Optional pre-rendered base image. If null, the thermal data
     *        is rendered using the given palette.
     * @return Rendered image and status.
     */
    static Result render(const ThermalDataModel &data,
                         ColorMap::Palette palette,
                         const QList<QPoint> &points,
                         const QList<ThermalBox> &boxes,
                         OverlayFlagSet flags,
                         std::optional<float> min = std::nullopt,
                         std::optional<float> max = std::nullopt,
                         const QImage &baseImage = QImage());

    /**
     * @brief Save the rendered result to a file.
     * @param result Render result.
     * @param filePath Output path (PNG, JPEG, BMP, TIFF supported by Qt).
     * @return Error message on failure, empty on success.
     */
    static QString save(const Result &result, const QString &filePath);
};

Q_DECLARE_OPERATORS_FOR_FLAGS(OverlayExporter::OverlayFlagSet)

#endif // OVERLAYEXPORTER_H
