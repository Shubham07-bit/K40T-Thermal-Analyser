#include "PngTiffLoader.h"

#include <QFile>
#include <QFileInfo>
#include <QImage>
#include <QJsonDocument>
#include <QJsonObject>
#include <QTextStream>

QStringList PngTiffLoader::supportedExtensions() const
{
    return QStringList() << "png" << "tif" << "tiff";
}

QString PngTiffLoader::load(const QString &filePath, ThermalDataModel &outData)
{
    outData.clear();

    QImage img;
    if (!img.load(filePath)) {
        return QObject::tr("Failed to load image: %1").arg(filePath);
    }

    // Convert to a format that gives us the raw pixel values.
    // 16-bit grayscale is best. Qt's Format_Grayscale16 preserves 16-bit.
    if (img.format() != QImage::Format_Grayscale16) {
        // Try to convert. This loses precision for non-grayscale, but works as a fallback.
        QImage converted = img.convertToFormat(QImage::Format_Grayscale16);
        if (converted.isNull()) {
            converted = img.convertToFormat(QImage::Format_RGB888);
            if (converted.isNull()) {
                return QObject::tr("Unsupported image format for thermal extraction.");
            }
            img = converted;
        } else {
            img = converted;
        }
    }

    const int w = img.width();
    const int h = img.height();
    outData.setSize(w, h);
    outData.setFilePath(filePath);
    outData.setFileName(QFileInfo(filePath).fileName());

    // Read optional sidecar JSON to interpret pixel values.
    float scale = 0.01f;   // default: centi-Celsius
    float offset = 0.0f;
    QString unit = QObject::tr("Celsius");
    readSidecar(filePath, scale, offset, unit);

    const bool is16Bit = (img.format() == QImage::Format_Grayscale16);

    for (int y = 0; y < h; ++y) {
        const quint16 *line16 = is16Bit ? reinterpret_cast<const quint16 *>(img.constScanLine(y)) : nullptr;
        const QRgb *lineRgb = !is16Bit ? reinterpret_cast<const QRgb *>(img.constScanLine(y)) : nullptr;
        for (int x = 0; x < w; ++x) {
            float raw = 0.0f;
            if (is16Bit) {
                raw = static_cast<float>(line16[x]);
            } else {
                // Use red channel as a fallback for RGB images
                raw = static_cast<float>(qRed(lineRgb[x]));
            }
            outData.setTemperature(x, y, raw * scale + offset);
        }
    }

    outData.recomputeMinMax();

    outData.setMetadataValue(QObject::tr("Width"), w);
    outData.setMetadataValue(QObject::tr("Height"), h);
    outData.setMetadataValue(QObject::tr("Bit Depth"), is16Bit ? 16 : 8);
    outData.setMetadataValue(QObject::tr("Temperature Unit"), unit);
    outData.setMetadataValue(QObject::tr("Scale"), scale);
    outData.setMetadataValue(QObject::tr("Offset"), offset);
    outData.setMetadataValue(QObject::tr("Min Temperature"), outData.minTemperature());
    outData.setMetadataValue(QObject::tr("Max Temperature"), outData.maxTemperature());

    // Keep a visible preview (scaled RGB for display if needed)
    outData.setPreviewImage(img.convertToFormat(QImage::Format_RGB888));

    return QString();
}

bool PngTiffLoader::readSidecar(const QString &filePath, float &scale, float &offset, QString &unit)
{
    QFileInfo fi(filePath);
    QString sidecar = fi.path() + "/" + fi.completeBaseName() + ".json";
    QFile file(sidecar);
    if (!file.open(QIODevice::ReadOnly))
        return false;

    QByteArray data = file.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isObject())
        return false;

    QJsonObject obj = doc.object();
    if (obj.contains("scale"))
        scale = static_cast<float>(obj.value("scale").toDouble(scale));
    if (obj.contains("offset"))
        offset = static_cast<float>(obj.value("offset").toDouble(offset));
    if (obj.contains("unit"))
        unit = obj.value("unit").toString(unit);

    return true;
}
