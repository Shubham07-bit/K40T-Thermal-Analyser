#include "BlstIrLoader.h"

#include <QDateTime>
#include <QFile>
#include <QFileInfo>
#include <QImage>

#include "blst_ir_file.h"

QStringList BlstIrLoader::supportedExtensions() const
{
    return QStringList() << "jpg" << "jpeg";
}

QString BlstIrLoader::load(const QString &filePath, ThermalDataModel &outData)
{
    outData.clear();

    uint16_t width = 0;
    uint16_t height = 0;
    size_t count = 0;

    int ret = blst_ir_file_get_info(filePath.toUtf8().constData(), &width, &height, &count);
    if (ret != 0) {
        return QObject::tr("Not a valid BLST IR image (blst_ir_file_get_info returned %1). "
                           "Temperature matrix cannot be read.").arg(ret);
    }

    if (width == 0 || height == 0 || count == 0 || count != static_cast<size_t>(width) * height) {
        return QObject::tr("Invalid BLST IR image dimensions (width=%1, height=%2, count=%3).")
                .arg(width).arg(height).arg(count);
    }

    QVector<float> temps(static_cast<int>(count));
    ret = blst_ir_file_parse_temperature_matrix(filePath.toUtf8().constData(), temps.data(), count);
    if (ret != 0) {
        return QObject::tr("Failed to parse BLST IR temperature matrix (ret=%1).").arg(ret);
    }

    outData.setSize(width, height);
    outData.setFilePath(filePath);
    outData.setFileName(QFileInfo(filePath).fileName());

    for (int y = 0; y < height; ++y) {
        const float *row = temps.constData() + y * width;
        for (int x = 0; x < width; ++x) {
            outData.setTemperature(x, y, row[x]);
        }
    }

    outData.recomputeMinMax();

    outData.setMetadataValue(QObject::tr("Width"), width);
    outData.setMetadataValue(QObject::tr("Height"), height);
    outData.setMetadataValue(QObject::tr("Format"), QObject::tr("K40T IR (proprietary matrix)"));
    outData.setMetadataValue(QObject::tr("Min Temperature"), outData.minTemperature());
    outData.setMetadataValue(QObject::tr("Max Temperature"), outData.maxTemperature());

    // Use the embedded JPEG preview if available.
    QImage preview;
    if (preview.load(filePath)) {
        outData.setPreviewImage(preview.convertToFormat(QImage::Format_RGB888));
    }

    return QString();
}
