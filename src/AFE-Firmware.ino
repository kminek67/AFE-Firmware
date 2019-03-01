/*
AFE Firmware for smarthome devices based on ESP8266/ESP8285 chips

This code combains AFE Firmware versions:
   - T0 and T0 for Shelly-1
   - T1 (DS18B20)
   - T2 (DHTxx)
   - T3 (PIRs)
   - T4 (Up to 4 relays)
   - T5 Gate
   - T6 Wheater station

More about the versions (PL): https://www.smartnydom.pl/afe-firmware-pl/wersje/
LICENSE: https://github.com/tschaban/AFE-Firmware/blob/master/LICENSE
DOC (PL): https://www.smartnydom.pl/afe-firmware-pl/
*/

/* Includes libraries for debugging in development compilation only */
#if defined(DEBUG)
#include <Streaming.h>
#endif

#include <AFE-API-Domoticz.h>
#include <AFE-API-MQTT.h>
#include <AFE-Data-Access.h>
#include <AFE-Device.h>

/* Shelly-1 device does not have LED. Excluding LED related code */
#ifdef CONFIG_HARDWARE_LED
#include <AFE-LED.h>
AFELED Led;
#endif

#include <AFE-Relay.h>
#include <AFE-Switch.h>
#include <AFE-Upgrader.h>
#include <AFE-Web-Server.h>
#include <AFE-WiFi.h>

/* T1 Set up, DS18B20 sensor */
#ifdef CONFIG_HARDWARE_DS18B20
#include <AFE-Sensor-DS18B20.h>
AFESensorDS18B20 Sensor;
#endif

/* T2 Setup, DHxx sensor */
#ifdef CONFIG_HARDWARE_DHXX
#include <AFE-Sensor-DHT.h>
#include <PietteTech_DHT.h>
void dht_wrapper();
PietteTech_DHT dht;
AFESensorDHT Sensor;
#endif

#ifdef CONFIG_TEMPERATURE
float temperature;
#endif

#ifdef CONFIG_HUMIDITY
float humidity;
#endif

AFEDataAccess Data;
AFEDevice Device;
AFEWiFi Network;
AFEMQTT Mqtt;
AFEDomoticz Domoticz;
AFEWebServer WebServer;
AFESwitch Switch[sizeof(Device.configuration.isSwitch)];
AFERelay Relay[sizeof(Device.configuration.isRelay)];
MQTT MQTTConfiguration;

#if defined(T3_CONFIG)
#include <AFE-PIR.h>
AFEPIR Pir[sizeof(Device.configuration.isPIR)];
#endif

#if defined(T5_CONFIG)
#include <AFE-Gate.h>
GATE GateState;
AFEGate Gate;
uint8_t lastPublishedGateStatus = GATE_UNKNOWN;
byte lastPublishedContactronState[sizeof(Device.configuration.isContactron)];
#endif

#ifdef CONFIG_HARDWARE_UART
#include <AFE-I2C-Scanner.h>
AFEI2CScanner I2CScanner;
#endif

#ifdef CONFIG_HARDWARE_BH1750
#include <AFE-Sensor-BH1750.h>
AFESensorBH1750 BH1750Sensor;
#endif

#ifdef CONFIG_HARDWARE_HPMA115S0
#include <AFE-Sensor-BMx80.h>
AFESensorBMx80 BMx80Sensor;
#endif

#ifdef CONFIG_HARDWARE_HPMA115S0
#include <AFE-Sensor-HPMA115S0.h>
AFESensorHPMA115S0 ParticleSensor;
#endif

#ifdef CONFIG_HARDWARE_ADC_VCC
#include <AFE-Analog-Input.h>
AFEAnalogInput AnalogInput;
#endif

#ifdef CONFIG_HARDWARE_SPIFFS
#include <FS.h>
#endif

void setup() {

  Serial.begin(9600);
  delay(10);

/* Turn off publishing information to Serial for production compilation */
#if !defined(DEBUG)
  Serial.swap();
#endif

#ifdef DEBUG
  Serial << endl
         << endl
         << "################################ BOOTING "
            "################################"
         << endl
         << "All classes and global variables initialized";
#endif

#ifdef CONFIG_HARDWARE_SPIFFS
  if (SPIFFS.begin()) {
#ifdef DEBUG
    Serial << endl << "File system mounted";
  } else {
    Serial << endl << "Failed to mount file system";
#endif
  }
#endif

#ifdef DEBUG
  Serial << endl << "Initializing device";
#endif
  Device.begin();

  /* Checking if the device is launched for a first time. If so it loades
   * default configuration to EEPROM */
#ifdef DEBUG
  Serial << endl << "Checking if first time launch: ";
#endif

  if (Device.getMode() == MODE_FIRST_TIME_LAUNCH) {
#ifdef DEBUG
    Serial << "YES";
#endif
    Device.setDevice();
  }
#ifdef DEBUG
  else {
    Serial << "NO";
  }
#endif

  /* Checking if the firmware has been upgraded. Potentially runs post upgrade
   * code */
#ifdef DEBUG
  Serial << endl << "Checking if firmware should be upgraded: ";
#endif
  AFEUpgrader *Upgrader = new AFEUpgrader();
  if (Upgrader->upgraded()) {
#ifdef DEBUG
    Serial << endl << "Firmware is not up2date. Upgrading...";
#endif
    Upgrader->upgrade();
#ifdef DEBUG
    Serial << endl << "Firmware upgraded";
#endif
  }
#ifdef DEBUG
  else {
    Serial << endl << "Firmware is up2date";
  }
#endif
  delete Upgrader;
  Upgrader = NULL;

  /* Initializing relay */
  initRelay();
#ifdef DEBUG
  Serial << endl << "Relay(s) initialized";
#endif

  /* Initialzing network */
  Network.begin(Device.getMode(), &Device);
#ifdef DEBUG
  Serial << endl << "Network initialized";
#endif

/* Initializing LED, checking if LED exists is made on Class level  */
#ifdef CONFIG_HARDWARE_LED
  uint8_t systeLedID = Data.getSystemLedID();
  if (systeLedID > 0) {
    Led.begin(systeLedID - 1);
  }

  /* If device in configuration mode then it starts LED blinking */
  if (Device.getMode() == MODE_ACCESS_POINT) {
    Led.blinkingOn(100);
  }
#ifdef DEBUG
  Serial << endl << "System LED initialized";
#endif
#endif

#ifdef DEBUG
  Serial << endl << "Connecting to the network";
#endif
  Network.listener();
  /* Initializing HTTP WebServer */
  WebServer.handle("/", handleHTTPRequests);
  WebServer.handle("/favicon.ico", handleFavicon);
  WebServer.begin(&Device);
#ifdef DEBUG
  Serial << endl << "WebServer initialized";
#endif

  /* Initializing switches */
  initSwitch();
#ifdef DEBUG
  Serial << endl << "Switch(es) initialized";
#endif

  if (Device.getMode() == MODE_NORMAL) {

/* Initializing Gate */
#if defined(T5_CONFIG)
    Gate.begin();
    GateState = Data.getGateConfiguration();
#ifdef DEBUG
    Serial << endl << "Gate initialized";
#endif
#endif

    /* Initializing DS18B20 or DHTxx sensor */
#if defined(CONFIG_HARDWARE_DS18B20) || defined(CONFIG_HARDWARE_DHXX)
    initSensor();
#ifdef DEBUG
    Serial << endl << "Sensors initialized";
#endif
#endif

/* Initializing T6 sensor */
#ifdef CONFIG_HARDWARE_I2C
    Wire.begin();
#endif

#ifdef CONFIG_HARDWARE_HPMA115S0
    if (Device.configuration.isHPMA115S0) {
      initHPMA115S0Sensor();
    }
#endif

#ifdef CONFIG_HARDWARE_BMX80
    if (Device.configuration.isBMx80 == TYPE_BME680_SENSOR) {
      BMx80Sensor.begin(TYPE_BME680_SENSOR);
    } else if (Device.configuration.isBMx80 == TYPE_BME280_SENSOR) {
      BMx80Sensor.begin(TYPE_BME280_SENSOR);
    } else if (Device.configuration.isBMx80 == TYPE_BMP180_SENSOR) {
      BMx80Sensor.begin(TYPE_BMP180_SENSOR);
    }
#endif

#ifdef CONFIG_HARDWARE_BH1750
    if (Device.configuration.isBH1750) {
      BH1750Sensor.begin();
    }
#endif

#if defined(T3_CONFIG)
    /* Initializing PIRs */
    initPIR();
#ifdef DEBUG
    Serial << endl << "PIR initialized";
#endif
#endif

#ifdef CONFIG_HARDWARE_ADC_VCC
    if (Device.configuration.isAnalogInput) {
      AnalogInput.begin();
    }
#endif
  }

  /* Initializing APIs */
  MQTTInit();
  DomoticzInit();

#if defined(DEBUG) && defined(CONFIG_HARDWARE_I2C)
  /* Scanning I2C for devices */
  if (Device.getMode() == MODE_NORMAL) {
    I2CScanner.scanAll();
  }
#endif

#ifdef DEBUG
  Serial << endl
         << "########################### BOOTING COMPLETED "
            "###########################"
         << endl;
#endif
}

void loop() {

  if (Device.getMode() != MODE_ACCESS_POINT) {
    if (Network.connected()) {
      if (Device.getMode() == MODE_NORMAL) {

        /* Triggerd when connectes/reconnects to WiFi */
        eventsListener();

        /* If MQTT API is on it listens for MQTT messages. If the device is
         * not connected to MQTT Broker, it connects the device to it */
        if (Device.configuration.api.mqtt) {
          Mqtt.listener();
        }

        /* Listens for HTTP requsts. Both for configuration panel HTTP
         * requests or HTTP API requests if it's turned on */
        WebServer.listener();

#if defined(T5_CONFIG)
        /* Listening for gate events */
        Gate.listener();
#endif

        /* Checking if there was received HTTP API Command */
        mainHTTPRequestsHandler();

#if defined(T5_CONFIG)
        /* Gate related events */
        mainGate();
#endif

#if !(defined(T3_CONFIG) || defined(T5_CONFIG) || defined(T6_CONFIG))
        /* Relay related events */
        mainRelay();
#endif

#if defined(CONFIG_HARDWARE_DS18B20) || defined(CONFIG_HARDWARE_DHXX)
        /* Sensor: DS18B20 or DHT related code */
        mainSensor();
#endif

/* Sensor: HPMA115S0 related code  */
#ifdef CONFIG_HARDWARE_HPMA115S0
        mainHPMA115S0Sensor();
#endif

#ifdef CONFIG_HARDWARE_BMX80
        mainBMx80Sensor();
#endif

#ifdef CONFIG_HARDWARE_BH1750
        mainBH1750Sensor();
#endif

#ifdef CONFIG_HARDWARE_ADC_VCC
        analogInputListner();
#endif

#if defined(T3_CONFIG)
        mainPIR();
#endif

      } else { /* Device runs in configuration mode over WiFi */
#ifdef CONFIG_HARDWARE_LED
        if (!Led.isBlinking()) {
          Led.blinkingOn(100);
        }
#endif
        WebServer.listener();
      }
    }

#ifdef CONFIG_HARDWARE_LED
    else {
      if (Device.getMode() == MODE_CONFIGURATION && Led.isBlinking()) {
        Led.blinkingOff();
      }
    }
#endif
    Network.listener();
  } else { /* Deviced runs in Access Point mode */
    Network.APListener();
    WebServer.listener();
  }

  /* Listens for switch events */
  mainSwitchListener();
  mainSwitch();

  /* Led listener */
#ifdef CONFIG_HARDWARE_LED
  Led.loop();
#endif

/* Debug information */
#if defined(DEBUG)
  if (Device.getMode() == MODE_NORMAL) {
    debugListener();
  }
#endif
}
