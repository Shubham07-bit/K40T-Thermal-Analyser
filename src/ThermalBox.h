#ifndef THERMALBOX_H
#define THERMALBOX_H

#include <QRect>

/**
 * @brief A user-drawn measurement box on a thermal image.
 */
struct ThermalBox
{
    QRect rect;
    float avgTemperature = 0.0f;
    float minTemperature = 0.0f;
    float maxTemperature = 0.0f;
};

#endif // THERMALBOX_H
