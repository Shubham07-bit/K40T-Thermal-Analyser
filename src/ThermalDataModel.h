#ifndef THERMALDATAMODEL_H
#define THERMALDATAMODEL_H

#include <QImage>
#include <QMap>
#include <QMetaType>
#include <QString>
#include <QVariant>
#include <QVector>
#include <optional>

/**
 * @brief Stores raw thermal data and metadata for one image.
 *
 * Temperatures are stored in degrees Celsius as float.
 * Width/height match the thermal sensor resolution.
 */
class ThermalDataModel
{
public:
    ThermalDataModel();

    bool isValid() const { return !m_temperatures.isEmpty() && m_width > 0 && m_height > 0; }

    void setSize(int width, int height);
    void setTemperature(int x, int y, float celsius);
    float temperatureAt(int x, int y) const;
    std::optional<float> maybeTemperatureAt(int x, int y) const;

    int width() const { return m_width; }
    int height() const { return m_height; }
    const QVector<float> &temperatures() const { return m_temperatures; }

    float minTemperature() const { return m_minTemp; }
    float maxTemperature() const { return m_maxTemp; }
    QPoint minTemperaturePixel() const { return m_minPixel; }
    QPoint maxTemperaturePixel() const { return m_maxPixel; }

    void recomputeMinMax();

    void setFilePath(const QString &path) { m_filePath = path; }
    QString filePath() const { return m_filePath; }

    void setFileName(const QString &name) { m_fileName = name; }
    QString fileName() const { return m_fileName; }

    void setMetadata(const QMap<QString, QVariant> &meta) { m_metadata = meta; }
    QMap<QString, QVariant> metadata() const { return m_metadata; }
    void setMetadataValue(const QString &key, const QVariant &value) { m_metadata[key] = value; }

    void setPreviewImage(const QImage &img) { m_previewImage = img; }
    QImage previewImage() const { return m_previewImage; }

    void clear();

    static float kelvinToCelsius(float k) { return k - 273.15f; }
    static float celsiusToFahrenheit(float c) { return c * 9.0f / 5.0f + 32.0f; }

private:
    int m_width = 0;
    int m_height = 0;
    QVector<float> m_temperatures;
    float m_minTemp = 0.0f;
    float m_maxTemp = 0.0f;
    QPoint m_minPixel;
    QPoint m_maxPixel;
    QString m_filePath;
    QString m_fileName;
    QMap<QString, QVariant> m_metadata;
    QImage m_previewImage;
};

Q_DECLARE_METATYPE(ThermalDataModel)

#endif // THERMALDATAMODEL_H
