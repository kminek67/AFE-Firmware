/* AFE Firmware for smart home devices, Website: https://afe.smartnydom.pl/ */

#ifndef _AFE_MQTT_h
#define _AFE_MQTT_h

#include <AFE-Data-Access.h>
#include <AFE-MQTT-Structure.h>
#include <AFE-NETWORK-Structure.h>
#include <PubSubClient.h>
#include <WiFiClient.h>

#ifdef AFE_CONFIG_API_DOMOTICZ_ENABLED
#if AFE_LANGUAGE == 0
#include <pl_PL.h>
#else
#include <en_EN.h>
#endif
#endif // AFE_CONFIG_API_DOMOTICZ_ENABLED

#ifdef DEBUG
#include <Streaming.h>
#endif

#ifdef AFE_CONFIG_HARDWARE_LED
#include <AFE-LED.h>
#endif

class AFEMQTT {

private:
  PubSubClient Broker;
  AFEDataAccess *_Data;
  char *_DeviceName;
  NETWORK NetworkConfiguration;

#ifdef AFE_CONFIG_HARDWARE_LED
  AFELED *_Led;
  unsigned long ledStartTime = 0;
#endif

  unsigned long sleepStartTime = 0;
  boolean sleepMode = false;
  boolean isConfigured =
      true; // if it's falsed it does not connect to MQTT Broker

  uint8_t connections = 0;
  unsigned long delayStartTime = 0;
  boolean eventConnectionEstablished = false;

  /* Sets parameters related to reconnection to MQTT Broker if there was
   * problems to connect to it */
  void setReconnectionParams(
      uint8_t no_connection_attempts,
      uint8_t duration_between_connection_attempts,
      uint8_t duration_between_next_connection_attempts_series);

  /* Connecting to MQTT Broker */
  void connect();

  /* Method turns off MQTT API */
  void disconnect();

public:
#ifdef AFE_CONFIG_API_PROCESS_REQUESTS
  MQTT_MESSAGE message;
#endif
  WiFiClient esp;
  MQTT configuration;

  /* Constructor: it sets all necessary parameters */
  AFEMQTT();

/* Initialization of the class */
#ifdef AFE_CONFIG_HARDWARE_LED
  void begin(AFEDataAccess *, char *, AFELED *);
#endif

  void begin(AFEDataAccess *, char *);


  /* Methods establishes connection from MQTT Broker, subscribed and set relay
 * default values */
  boolean listener();

  /* Method returns true if device connected to MQTT Broker */
  boolean eventConnected();

  /* Methods publishes a message to MQTT Broker */
  boolean publish(const char *topic, const char *message);

#ifdef AFE_CONFIG_API_PROCESS_REQUESTS
  /* Methods subsribes to topic in the MQTT Broker */
  void subscribe(const char *);
#endif

};

#endif