#ifndef TEMPERATURE_SENSOR_TEMPERATURE_SENSOR_H
#define TEMPERATURE_SENSOR_TEMPERATURE_SENSOR_H

class TemperatureSensor {
    public:
        virtual void start() = 0;
        virtual void stop() = 0;
        virtual float getTemp() = 0;
        virtual ~TemperatureSensor() = default;
};

#endif //TEMPERATURE_SENSOR_TEMPERATURE_SENSOR_H
