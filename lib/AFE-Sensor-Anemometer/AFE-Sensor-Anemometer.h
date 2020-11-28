/* AFE Firmware for smart home devices, Website: https://afe.smartnydom.pl/ */

#ifndef _AFE_Sensor_Wind_h
#define _AFE_Sensor_Wind_h

#include <AFE-Configuration.h>

#ifdef AFE_CONFIG_HARDWARE_ANEMOMETER_SENSOR

#include <arduino.h>
#include <AFE-Data-Access.h>
#include <AFE-Sensor-Binary.h>

#ifdef DEBUG
#include <Streaming.h>
#endif

class AFESensorAnemometer {

public:
  ANEMOMETER configuration;
  float lastSpeedMS =
      0; // used by HTTPs API - stores and gets the lastest value by HTTP API
  float lastSpeedKMH =
      0; // used by HTTPs API - stores and gets the lastest value by HTTP API

#ifndef AFE_CONFIG_API_DOMOTICZ_ENABLED
  char mqttCommandTopic[sizeof(configuration.mqtt.topic) + 5];
#endif

  /* Constructors */
  AFESensorAnemometer();

  /* Init switch */
  boolean begin(AFEDataAccess *, AFESensorBinary *);

  /* Returns the sensor data in JSON format */
  void getJSON(char *json);

  boolean listener(void);

private:
  AFESensorBinary *_Sensor;
  AFEDataAccess *_Data;
  boolean _initialized = false;
  uint32_t startTime = 0;
  float oneImpulseDistanceCM = 0;
};

#endif // AFE_CONFIG_HARDWARE_ANEMOMETER_SENSOR
#endif // _AFE_Sensor_Wind_h
