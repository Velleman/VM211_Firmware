#ifndef Tuyav_h
#define Tuyav_h

#include "Arduino.h"
#include "digitalInput.h"
#include "analogInput.h"
#include "digitalOutput.h"
#include "analogOutput.h"
#include "InputClass.h"
#include "OutputClass.h"
#include "TuyaSerial.h"
#include "mcu_api.h"
#include "userValue.h"

#define AV1 119
#define AV2 120
#define AV3 121
#define AV4 122
#define AV5 123
#define AV6 124
#define AV7 125
#define AV8 126
#define AV9 127

class Tuyav
{
  public:
    Tuyav(HardwareSerial* serial);
    Tuyav(SoftwareSerial* serial);

    void setDigitalInputs(int pin1, int pin2, int pin3);
    void setAnalogInputs(int pin1, int pin2, int pin3);
    void setDigitalOutputs(int pin1, int pin2, int pin3, int pin4, int pin5);
    void setAnalogOutputs(int pin1, int pin2, int pin3);
    DigitalOutput getDigitalOutput(int id);
    AnalogOutput getAnalogOutput(int id);
    TuyaSerial get_tuyaSerial();

    void sendUserValue(int TuyaPinID, int newValue);
    void setUserValue(int TuyaPinID, String value);
    void setWifiMode(unsigned char mode);
	  void resetWifi();
    void initialize();
    void tuyaUpdate();
    
    void setUpdateRateMs(unsigned long updateRate);
    void setAV1(String value);
    void setAV2(String value);
    void setAV3(String value);
    void setAV4(String value);
    void setAV5(String value);
    void setAV6(String value);
    void setAV7(String value);
    void setAV8(String value);
    void setAV9(String value);
    int ANALOG_IN[3];
    bool DIGITAL_IN[3];
    int ANALOG_OUT[3];
    bool DIGITAL_OUT[5];

  private:
    DigitalInput _digitalInputs[3];
    AnalogInput _analogInputs[3];
    DigitalOutput _digitalOutputs[5];
    AnalogOutput _analogOutputs[3];
    UserValue _userValues[9];
    TuyaSerial _tuyaSerial;

    void digitalInputInit();
    void digitalOutputInit();
    void analogOutputInit();
    void userValueInit();
    void digitalUpdate();
    void analogUpdate();
    void userValueUpdate();

    unsigned long eventTime1 = 2000;
    unsigned long previousTime1 = 0;

    int _PinID;
    int _TuyaPinID;
    int _value;


};

#endif
