#include "ThermalDataModel.h"
#include <QtMath>

ThermalDataModel::ThermalDataModel()
{
}

void ThermalDataModel::setSize(int width, int height)
{
    m_width = width;
    m_height = height;
    m_temperatures.resize(width * height);
    m_temperatures.fill(0.0f);
    m_minTemp = 0.0f;
    m_maxTemp = 0.0f;
    m_minPixel = QPoint();
    m_maxPixel = QPoint();
}

void ThermalDataModel::setTemperature(int x, int y, float celsius)
{
    if (x < 0 || x >= m_width || y < 0 || y >= m_height)
        return;
    m_temperatures[y * m_width + x] = celsius;
}

float ThermalDataModel::temperatureAt(int x, int y) const
{
    if (x < 0 || x >= m_width || y < 0 || y >= m_height)
        return qQNaN();
    return m_temperatures[y * m_width + x];
}

std::optional<float> ThermalDataModel::maybeTemperatureAt(int x, int y) const
{
    if (x < 0 || x >= m_width || y < 0 || y >= m_height)
        return std::nullopt;
    return m_temperatures[y * m_width + x];
}

void ThermalDataModel::recomputeMinMax()
{
    if (m_temperatures.isEmpty()) {
        m_minTemp = m_maxTemp = 0.0f;
        m_minPixel = m_maxPixel = QPoint();
        return;
    }

    m_minTemp = m_maxTemp = m_temperatures.first();
    m_minPixel = m_maxPixel = QPoint(0, 0);

    for (int y = 0; y < m_height; ++y) {
        for (int x = 0; x < m_width; ++x) {
            float t = m_temperatures[y * m_width + x];
            if (t < m_minTemp) {
                m_minTemp = t;
                m_minPixel = QPoint(x, y);
            }
            if (t > m_maxTemp) {
                m_maxTemp = t;
                m_maxPixel = QPoint(x, y);
            }
        }
    }
}

void ThermalDataModel::clear()
{
    m_width = 0;
    m_height = 0;
    m_temperatures.clear();
    m_minTemp = m_maxTemp = 0.0f;
    m_minPixel = m_maxPixel = QPoint();
    m_filePath.clear();
    m_fileName.clear();
    m_metadata.clear();
    m_previewImage = QImage();
}
