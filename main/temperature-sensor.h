#ifndef TEMPERATURE_SENSOR_TEMPERATURE_SENSOR_H
#define TEMPERATURE_SENSOR_TEMPERATURE_SENSOR_H

class TemperatureSensor {
    public:
        virtual float getTemp() = 0;
        virtual ~TemperatureSensor() = default;
};

#endif //TEMPERATURE_SENSOR_TEMPERATURE_SENSOR_H
