#ifndef PNGTIFFLOADER_H
#define PNGTIFFLOADER_H

#include "ThermalImageLoader.h"

/**
 * @brief Loads 8/16-bit grayscale PNG/TIFF images as thermal data.
 *
 * Assumptions (override via sidecar JSON):
 *   - 16-bit values are centi-degrees Celsius (value / 100.0 = C)
 *   - 8-bit values are degrees Celsius
 */
class PngTiffLoader : public ThermalImageLoader
{
public:
    QStringList supportedExtensions() const override;
    QString load(const QString &filePath, ThermalDataModel &outData) override;

private:
    bool readSidecar(const QString &filePath, float &scale, float &offset, QString &unit);
};

#endif // PNGTIFFLOADER_H
