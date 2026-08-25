#include "DjiRjpgLoader.h"

#include <QBuffer>
#include <QDateTime>
#include <QFile>
#include <QFileInfo>
#include <QImage>
#include <QImageReader>

namespace {

// Locate the JPEG SOI marker and scan APP segments for a known DJI thermal payload.
// Many DJI thermal JPEGs store a second low-resolution thermal image or raw data
// in an APP segment.  This is a best-effort parser.
bool scanForDjiThermalSegment(const QString &filePath, QByteArray &outPayload)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly))
        return false;

    const QByteArray data = file.readAll();
    if (data.size() < 4 || static_cast<quint8>(data[0]) != 0xFF || data[1] != static_cast<char>(0xD8))
        return false;

    int i = 2;
    while (i < data.size() - 4) {
        if (static_cast<quint8>(data[i]) != 0xFF) {
            ++i;
            continue;
        }

        quint8 marker = static_cast<quint8>(data[i + 1]);
        // Skip padding bytes
        if (marker == 0x00 || (marker >= 0x01 && marker <= 0xFE)) {
            ++i;
            continue;
        }

        // SOF/SOS/EOI markers have no length field in the same way
        if (marker == 0xD9 || marker == 0xDA)
            break;

        int length = (static_cast<quint8>(data[i + 2]) << 8) | static_cast<quint8>(data[i + 3]);
        if (length < 2 || i + 2 + length > data.size())
            break;

        // DJI sometimes uses APP11 (0xEB) for thermal data, or APP1 (0xE1) for metadata.
        if (marker == 0xEB || marker == 0xE1) {
            QByteArray seg = data.mid(i + 4, length - 2);
            // Look for common DJI/thermal signatures (very heuristic).
            if (seg.startsWith("DJI") || seg.contains("THERM") || seg.contains("thermal")) {
                outPayload = seg;
                return true;
            }
        }

        i += 2 + length;
    }

    return false;
}

} // namespace

QStringList DjiRjpgLoader::supportedExtensions() const
{
    return QStringList() << "jpg" << "jpeg" << "rjpg";
}

QString DjiRjpgLoader::load(const QString &filePath, ThermalDataModel &outData)
{
    outData.clear();

    QImage img;
    if (!img.load(filePath)) {
        return QObject::tr("Failed to load image: %1").arg(filePath);
    }

    outData.setFilePath(filePath);
    outData.setFileName(QFileInfo(filePath).fileName());

    // Try to find real radiometric thermal data first.
    if (tryExtractDjiThermalData(filePath, outData)) {
        extractExifMetadata(filePath, outData);
        return QString();
    }

    // No radiometric data found.  Do NOT estimate from the preview.
    return QObject::tr("No radiometric temperature matrix found in '%1'. "
                       "Only K40T IR JPEGs and radiometric JPEGs with embedded thermal data are supported. "
                       "Plain JPEG previews cannot be interpreted as temperatures.")
            .arg(filePath);
}

bool DjiRjpgLoader::tryExtractDjiThermalData(const QString &filePath, ThermalDataModel &outData)
{
    QByteArray payload;
    if (!scanForDjiThermalSegment(filePath, payload) || payload.isEmpty())
        return false;

    // If the payload itself is a valid image (some DJI files embed a TIFF), try loading it.
    QImage embedded;
    if (embedded.loadFromData(payload)) {
        QImage gray = embedded.convertToFormat(QImage::Format_Grayscale16);
        if (!gray.isNull()) {
            const int w = gray.width();
            const int h = gray.height();
            outData.setSize(w, h);
            for (int y = 0; y < h; ++y) {
                const quint16 *line = reinterpret_cast<const quint16 *>(gray.constScanLine(y));
                for (int x = 0; x < w; ++x)
                    outData.setTemperature(x, y, static_cast<float>(line[x]) * 0.01f);
            }
            outData.recomputeMinMax();
            outData.setPreviewImage(embedded.convertToFormat(QImage::Format_RGB888));
            outData.setMetadataValue(QObject::tr("Format"), QObject::tr("Radiometric JPEG (embedded thermal)"));
            return true;
        }
    }

    return false;
}

void DjiRjpgLoader::extractExifMetadata(const QString &filePath, ThermalDataModel &outData)
{
    QFileInfo fi(filePath);
    outData.setMetadataValue(QObject::tr("File Name"), fi.fileName());
    outData.setMetadataValue(QObject::tr("File Path"), fi.absoluteFilePath());
    outData.setMetadataValue(QObject::tr("File Size"), fi.size());
    outData.setMetadataValue(QObject::tr("Modified"), fi.lastModified().toString(Qt::ISODate));

    // Qt does not expose much EXIF.  For a real tool, link libexiv2 or use QExifImageHeader.
    outData.setMetadataValue(QObject::tr("EXIF"), QObject::tr("Basic (use exiv2/exiftool for full EXIF)"));
}
