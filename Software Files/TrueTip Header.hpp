#pragma once
#include <Components/Sensor.hpp>
#include <Application/Constants.hpp>


extern volatile unsigned long lastTipTime;
extern volatile unsigned int tipCount;
extern volatile bool tipUpdated;
extern volatile unsigned long  tipIntervalMicros;

void tippingBucketInterrupt();

struct TippingBucketSensorData {
    double volume; //volume in liters
    double tips; //tips
};

class TippingBucket : public Sensor<TippingBucketSensorData> {
private:
    double volumePerTip; 
    void begin() override {
        tipCount = 0;
        pinMode(HardwarePins:: ANALOG_SENSOR_1, INPUT);
        setUpdateFreq(100);
    }

public:
    double volume = 0;
    double mlpm    = 0;

    void resetVolume() {
        volume = 0;
    }
   /************************************************************************* 
    The TippingBucket::startMeasurement function  sets up
    an interrupt on the ANALOG_SENSOR_1 pin to trigger the tippingBucketInterrupt
    function when a falling edge is detected. This is used to initiate measurement 
    by responding to changes in the tipping bucket sensor.
   ******************************************************************************/
    void startMeasurement() {

        attachInterrupt(digitalPinToInterrupt(HardwarePins:: ANALOG_SENSOR_1), tippingBucketInterrupt, FALLING);
    }

    void stopMeasurement() {
        detachInterrupt(digitalPinToInterrupt(HardwarePins:: ANALOG_SENSOR_1));
    }
    /*************************************
    The TippingBucket::read method  returns sensor data.
     It updates the tipping bucket's volume and calculates the flow rate in milliliters per minute when a 
     tip event occurs, or sets an error code if no new data is available.
    ******************************************* */
    

    SensorData read() override {
        if(tipUpdated) {
            tipUpdated = false;
            auto tipInterval = double(tipIntervalMicros)/60000000;
            mlpm = 1/tipInterval;
            volumePerTip = 12; //8 ml per tip
            volume+= volumePerTip;

            println("Tipping bucket Volume: ", volume, ", mLpm: ", mlpm);
        } else {
            setErrorCode(ErrorCode::notReady);
        }   
        return {volume};
}
};