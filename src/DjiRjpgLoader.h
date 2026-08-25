#ifndef DJIRJPGLOADER_H
#define DJIRJPGLOADER_H

#include "ThermalImageLoader.h"

/**
 * @brief Loader for DJI radiometric JPEG (R-JPG) thermal images.
 *
 * DJI embeds raw thermal sensor values inside the JPEG file.  Because the exact
 * embedding differs per payload and firmware, this loader first tries to parse
 * common DJI APP segments; if that fails it falls back to the visible/thermal
 * preview and reports an informative message.
 *
 * For production use with real DJI files, integrate the official DJI Thermal SDK
 * or use exiftool to extract the raw thermal TIFF first.
 */
class DjiRjpgLoader : public ThermalImageLoader
{
public:
    QStringList supportedExtensions() const override;
    QString load(const QString &filePath, ThermalDataModel &outData) override;

private:
    bool tryExtractDjiThermalData(const QString &filePath, ThermalDataModel &outData);
    void extractExifMetadata(const QString &filePath, ThermalDataModel &outData);
};

#endif // DJIRJPGLOADER_H
