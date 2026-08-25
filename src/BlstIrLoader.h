#ifndef BLSTIRLOADER_H
#define BLSTIRLOADER_H

#include "ThermalImageLoader.h"

/**
 * @brief Loader for BLST proprietary IR image files.
 *
 * Uses the BLST SDK functions blst_ir_file_get_info() and
 * blst_ir_file_parse_temperature_matrix() to read the actual temperature
 * matrix stored inside BLST IR JPEGs such as 19700101_001348_I.jpg.
 *
 * If the file is not a valid BLST IR image, the loader returns an error
 * instead of estimating temperatures from the preview.
 */
class BlstIrLoader : public ThermalImageLoader
{
public:
    QStringList supportedExtensions() const override;
    QString load(const QString &filePath, ThermalDataModel &outData) override;
};

#endif // BLSTIRLOADER_H
