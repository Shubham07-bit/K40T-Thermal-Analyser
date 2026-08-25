#include "ColorMap.h"
#include <QtMath>

QStringList ColorMap::paletteNames()
{
    return QStringList()
        << QObject::tr("Original")
        << QObject::tr("White Hot")
        << QObject::tr("Black Hot")
        << QObject::tr("Iron Bow")
        << QObject::tr("Rainbow")
        << QObject::tr("Arctic")
        << QObject::tr("Lava")
        << QObject::tr("Sepia");
}

QString ColorMap::paletteName(Palette p)
{
    const QStringList names = paletteNames();
    return names.value(static_cast<int>(p), QObject::tr("Unknown"));
}

QImage ColorMap::render(const ThermalDataModel &data,
                        Palette palette,
                        std::optional<float> min,
                        std::optional<float> max)
{
    if (!data.isValid())
        return QImage();

    const int w = data.width();
    const int h = data.height();
    const float lo = min.value_or(data.minTemperature());
    const float hi = max.value_or(data.maxTemperature());
    const float range = (hi - lo) > 0.001f ? (hi - lo) : 1.0f;

    QImage img(w, h, QImage::Format_RGB32);
    const QVector<float> &temps = data.temperatures();

    for (int y = 0; y < h; ++y) {
        QRgb *line = reinterpret_cast<QRgb *>(img.scanLine(y));
        for (int x = 0; x < w; ++x) {
            float t = temps[y * w + x];
            float norm = qBound(0.0f, (t - lo) / range, 1.0f);
            QColor c = map(palette, norm);
            line[x] = qRgb(c.red(), c.green(), c.blue());
        }
    }
    return img;
}

QColor ColorMap::map(Palette palette, float normalized)
{
    const QVector<QColor> gradient = buildGradient(palette);
    return interpolate(gradient, qBound(0.0f, normalized, 1.0f));
}

QColor ColorMap::interpolate(const QVector<QColor> &gradient, float t)
{
    if (gradient.isEmpty())
        return QColor(0, 0, 0);
    if (gradient.size() == 1)
        return gradient.first();

    const float pos = t * (gradient.size() - 1);
    const int idx = static_cast<int>(qFloor(pos));
    const float frac = pos - idx;

    const QColor &c1 = gradient[qBound(0, idx, gradient.size() - 1)];
    const QColor &c2 = gradient[qBound(0, idx + 1, gradient.size() - 1)];

    const int r = static_cast<int>(c1.red()   * (1.0f - frac) + c2.red()   * frac);
    const int g = static_cast<int>(c1.green() * (1.0f - frac) + c2.green() * frac);
    const int b = static_cast<int>(c1.blue()  * (1.0f - frac) + c2.blue()  * frac);

    return QColor(qBound(0, r, 255), qBound(0, g, 255), qBound(0, b, 255));
}

QVector<QColor> ColorMap::buildGradient(Palette palette)
{
    QVector<QColor> g;
    switch (palette) {
    case RawGrayscale:
        // Linear grayscale mapping of the raw temperature values.
        // No false-color gradient is applied; the normalized value drives R=G=B directly.
        g << QColor(0, 0, 0) << QColor(255, 255, 255);
        break;
    case WhiteHot:
        g << QColor(0, 0, 0) << QColor(255, 255, 255);
        break;
    case BlackHot:
        g << QColor(255, 255, 255) << QColor(0, 0, 0);
        break;
    case IronBow:
        g << QColor(0, 0, 0)
          << QColor(20, 0, 60)
          << QColor(80, 0, 120)
          << QColor(160, 30, 60)
          << QColor(255, 120, 0)
          << QColor(255, 240, 0)
          << QColor(255, 255, 255);
        break;
    case Rainbow:
        g << QColor(0, 0, 255)
          << QColor(0, 255, 255)
          << QColor(0, 255, 0)
          << QColor(255, 255, 0)
          << QColor(255, 0, 0)
          << QColor(255, 0, 255)
          << QColor(255, 255, 255);
        break;
    case Arctic:
        g << QColor(0, 0, 40)
          << QColor(0, 60, 120)
          << QColor(0, 160, 200)
          << QColor(180, 230, 255)
          << QColor(255, 255, 255);
        break;
    case Lava:
        g << QColor(20, 0, 0)
          << QColor(120, 0, 0)
          << QColor(255, 0, 0)
          << QColor(255, 120, 0)
          << QColor(255, 240, 0)
          << QColor(255, 255, 255);
        break;
    case Sepia:
        g << QColor(30, 15, 5)
          << QColor(120, 80, 40)
          << QColor(200, 160, 100)
          << QColor(255, 230, 190)
          << QColor(255, 255, 255);
        break;
    }
    return g;
}
