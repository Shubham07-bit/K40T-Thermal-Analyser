#ifndef THERMALIMAGELOADER_H
#define THERMALIMAGELOADER_H

#include <QString>
#include <QStringList>

#include "ThermalDataModel.h"

/**
 * @brief Abstract base for thermal image loaders.
 *
 * A loader attempts to extract a raw temperature grid + metadata from a file.
 */
class ThermalImageLoader
{
public:
    virtual ~ThermalImageLoader() = default;

    /**
     * @return A list of file extensions this loader recognizes, e.g. "jpg", "tiff".
     */
    virtual QStringList supportedExtensions() const = 0;

    /**
     * @brief Try to load thermal data from the given file.
     * @param filePath Absolute path to the image file.
     * @param outData Populated on success.
     * @return Error message on failure, empty string on success.
     */
    virtual QString load(const QString &filePath, ThermalDataModel &outData) = 0;
};

#endif // THERMALIMAGELOADER_H
