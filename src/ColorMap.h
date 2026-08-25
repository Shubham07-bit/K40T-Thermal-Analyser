#ifndef COLORMAP_H
#define COLORMAP_H

#include <QColor>
#include <QImage>
#include <QString>
#include <QStringList>
#include <QVector>

#include "ThermalDataModel.h"

/**
 * @brief Converts raw temperature values into a color-mapped QImage.
 */
class ColorMap
{
public:
    enum Palette
    {
        RawGrayscale,
        WhiteHot,
        BlackHot,
        IronBow,
        Rainbow,
        Arctic,
        Lava,
        Sepia
    };

    static QStringList paletteNames();
    static QString paletteName(Palette p);

    /**
     * @brief Render the thermal data into an RGB QImage.
     * @param data Thermal data model.
     * @param palette Color palette to apply.
     * @param min Optional manual minimum temperature. If null, use data minimum.
     * @param max Optional manual maximum temperature. If null, use data maximum.
     * @return RGB image with the same dimensions as the thermal data.
     */
    static QImage render(const ThermalDataModel &data,
                         Palette palette,
                         std::optional<float> min = std::nullopt,
                         std::optional<float> max = std::nullopt);

    /**
     * @brief Map a normalized value [0,1] to a color using the chosen palette.
     */
    static QColor map(Palette palette, float normalized);

private:
    static QVector<QColor> buildGradient(Palette palette);
    static QColor interpolate(const QVector<QColor> &gradient, float t);
};

#endif // COLORMAP_H
