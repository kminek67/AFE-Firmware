/* AFE Firmware for smart home devices
  LICENSE: https://github.com/tschaban/AFE-Firmware/blob/master/LICENSE
  DOC: https://www.smartnydom.pl/afe-firmware-pl/ */

#include "AFE-Defaults.h"

AFEDefaults::AFEDefaults() {}

void AFEDefaults::set() {

  DEVICE deviceConfiguration;
  FIRMWARE firmwareConfiguration;
  NETWORK networkConfiguration;
  MQTT MQTTConfiguration;
  RELAY RelayConfiguration;
  SWITCH SwitchConfiguration;

#ifdef CONFIG_FUNCTIONALITY_REGULATOR
  REGULATOR RegulatorConfiguration;
#endif

#ifdef CONFIG_HARDWARE_DS18B20
  DS18B20 SensorConfiguration;
#endif

#ifdef CONFIG_HARDWARE_DHXX
  DH SensorConfiguration;
#endif

#if defined(T3_CONFIG)
  PIR PIRConfiguration;
#endif

#if defined(T5_CONFIG)
  CONTACTRON ContactronConfiguration;
  GATE GateConfiguration;
#endif

#ifdef CONFIG_HARDWARE_HPMA115S0
  HPMA115S0 SensorHPMA115S0Configuration;
#endif

#ifdef CONFIG_HARDWARE_UART
  SERIALPORT SerialPortConfiguration;
#endif

#ifdef CONFIG_HARDWARE_BMX80
  BMx80 SensorBMx80Configuration;
#endif

#ifdef CONFIG_HARDWARE_BH1750
  BH1750 SensorBH1750Configuration;
#endif

  sprintf(firmwareConfiguration.version, FIRMWARE_VERSION);
  firmwareConfiguration.type = FIRMWARE_TYPE;
  firmwareConfiguration.autoUpgrade = 0;
  firmwareConfiguration.upgradeURL[0] = '\0';

  Data->saveConfiguration(firmwareConfiguration);

  sprintf(deviceConfiguration.name, "AFE-Device");

  /* APIs */
  deviceConfiguration.mqttAPI = false;
  deviceConfiguration.domoticzAPI = false;
  deviceConfiguration.httpAPI = true;

/* Relay presence */
#if !defined(T5_CONFIG)
  for (uint8_t i = 0; i < sizeof(deviceConfiguration.isRelay); i++) {
    deviceConfiguration.isRelay[i] = false;
  }
#endif

  /* Switch presence */
  deviceConfiguration.isSwitch[0] = true;
  for (uint8_t i = 1; i < sizeof(deviceConfiguration.isSwitch); i++) {
    deviceConfiguration.isSwitch[i] = false;
  }

/* LEDs */
#ifdef CONFIG_HARDWARE_LED
  deviceConfiguration.isLED[0] = true;
  for (uint8_t i = 1; i < sizeof(deviceConfiguration.isLED); i++) {
    deviceConfiguration.isLED[i] = false;
  }
#endif

/* DS18B20 */
#ifdef CONFIG_HARDWARE_DS18B20
  deviceConfiguration.isDS18B20 = false;
#endif

/* DHxx */
#ifdef CONFIG_HARDWARE_DHXX
  deviceConfiguration.isDHT = false;
#endif

/* PIR */
#if defined(T3_CONFIG)
  for (uint8_t i = 1; i < sizeof(deviceConfiguration.isPIR); i++) {
    deviceConfiguration.isPIR[i] = false;
  }
#endif

/* Contactron */
#if defined(T5_CONFIG)
  for (uint8_t i = 1; i < sizeof(deviceConfiguration.isContactron); i++) {
    deviceConfiguration.isContactron[i] = false;
  }
#endif

/* HPMA115S0 Sesnor */
#ifdef CONFIG_HARDWARE_HPMA115S0
  deviceConfiguration.isHPMA115S0 = false;
#endif

#ifdef CONFIG_HARDWARE_BMX80
  deviceConfiguration.isBMx80 = 0;
#endif

#ifdef CONFIG_HARDWARE_BH1750
  deviceConfiguration.isBH1750 = 0;
#endif

#ifdef CONFIG_HARDWARE_ADC_VCC
  deviceConfiguration.isAnalogInput = 0;
#endif

  Data->saveConfiguration(deviceConfiguration);

  /* Network default config */
  networkConfiguration.ssid[0] = '\0';
  networkConfiguration.password[0] = '\0';
  networkConfiguration.isDHCP = true;
  networkConfiguration.ip = IPAddress(0, 0, 0, 0);
  networkConfiguration.gateway = IPAddress(0, 0, 0, 0);
  networkConfiguration.subnet = IPAddress(255, 255, 255, 0);
  networkConfiguration.noConnectionAttempts = 10;
  networkConfiguration.waitTimeConnections = 1;
  networkConfiguration.waitTimeSeries = 60;
  Data->saveConfiguration(networkConfiguration);

  /* MQTT Default config */
  MQTTConfiguration.host[0] = '\0';
  MQTTConfiguration.ip = IPAddress(0, 0, 0, 0);
  MQTTConfiguration.user[0] = '\0';
  MQTTConfiguration.password[0] = '\0';
  MQTTConfiguration.port = 1883;
  sprintf(MQTTConfiguration.topic, "/device/");
  Data->saveConfiguration(MQTTConfiguration);

  /* Domoticz config */
  addDomoticzConfiguration();

/* Relay config */
#ifdef T0_SHELLY_1_CONFIG
  RelayConfiguration.gpio = 4;
#else
  RelayConfiguration.gpio = 12;
#endif

#if defined(T5_CONFIG)
  RelayConfiguration.timeToOff = 200;
#else /* Configuration not related to T5 */

#if !(defined(T3_CONFIG) || defined(T6_CONFIG)) /* Not used in T3 & T6*/
  RelayConfiguration.timeToOff = 0;
#endif

  RelayConfiguration.statePowerOn = 3;
  RelayConfiguration.stateMQTTConnected = 0;

#if defined(T0_CONFIG) || defined(T0_SHELLY_1_CONFIG) || defined(T1_CONFIG) || \
    defined(T2_CONFIG) || defined(T6_CONFIG)
  sprintf(RelayConfiguration.name, "switch");
#elif defined(T3_CONFIG) || defined(T4_CONFIG)
  sprintf(RelayConfiguration.name, "switch1");
#endif

  RelayConfiguration.ledID = 0;
  RelayConfiguration.idx = 0;

#if defined(T1_CONFIG) || defined(T2_CONFIG)
  RelayConfiguration.thermalProtection = 0;
#endif

  /* Saving defulat relay state */
  for (uint8_t i = 0; i < sizeof(deviceConfiguration.isRelay); i++) {
    Data->saveRelayState(i, false);
  }

#endif /* End of configuration not related to T5 */

  Data->saveConfiguration(0, RelayConfiguration);

#if defined(T3_CONFIG) || defined(T4_CONFIG)
  RelayConfiguration.gpio = 5;
  sprintf(RelayConfiguration.name, "switch2");
  Data->saveConfiguration(1, RelayConfiguration);

  RelayConfiguration.gpio = 4;
  sprintf(RelayConfiguration.name, "switch3");
  Data->saveConfiguration(2, RelayConfiguration);

  RelayConfiguration.gpio = 15;
  sprintf(RelayConfiguration.name, "switch4");
  Data->saveConfiguration(3, RelayConfiguration);
#endif

/* Regulator config */
#if defined(T1_CONFIG) || defined(T2_CONFIG)
  RegulatorConfiguration.enabled = false;
  RegulatorConfiguration.turnOn = 0;
  RegulatorConfiguration.turnOnAbove = false;
  RegulatorConfiguration.turnOff = 0;
  RegulatorConfiguration.turnOffAbove = true;
#endif

/* Thermostat configuration */
#if defined(CONFIG_FUNCTIONALITY_REGULATOR) &&                                 \
    defined(CONFIG_FUNCTIONALITY_THERMOSTAT)
  Data->saveConfiguration(RegulatorConfiguration, THERMOSTAT_REGULATOR);
#endif

/* Hunidistat confiuration */
#if defined(CONFIG_FUNCTIONALITY_REGULATOR) &&                                 \
    defined(CONFIG_FUNCTIONALITY_HUMIDISTAT)
  Data->saveConfiguration(RegulatorConfiguration, HUMIDISTAT_REGULATOR);
#endif

/* Switch config */
#ifdef T0_SHELLY_1_CONFIG
  SwitchConfiguration.gpio = 5;
  SwitchConfiguration.type = 1;
#else
  SwitchConfiguration.gpio = 0;
  SwitchConfiguration.type = 0;
#endif

  SwitchConfiguration.sensitiveness = 50;
  SwitchConfiguration.functionality = 0;
  SwitchConfiguration.relayID = 1;
  Data->saveConfiguration(0, SwitchConfiguration);

#if defined(T0_CONFIG) || defined(T1_CONFIG) || defined(T2_CONFIG) ||          \
    defined(T5_CONFIG) || defined(T6_CONFIG)
  SwitchConfiguration.gpio = 14;
  SwitchConfiguration.type = 1;
#elif defined(T3_CONFIG) || defined(T4_CONFIG)
  SwitchConfiguration.gpio = 9;
  SwitchConfiguration.relayID = 2;
#endif

#if defined(T0_CONFIG) || defined(T2_CONFIG) || defined(T3_CONFIG) ||          \
    defined(T4_CONFIG) || defined(T5_CONFIG) || defined(T6_CONFIG)
  SwitchConfiguration.functionality = 1;
  Data->saveConfiguration(1, SwitchConfiguration);
#endif

#if defined(T3_CONFIG) || defined(T4_CONFIG)
  SwitchConfiguration.gpio = 10;
  SwitchConfiguration.relayID = 3;
  Data->saveConfiguration(2, SwitchConfiguration);
  SwitchConfiguration.gpio = 14;
  SwitchConfiguration.relayID = 4;
  Data->saveConfiguration(3, SwitchConfiguration);
#endif

/* LEDs configuration */
#ifdef CONFIG_HARDWARE_LED

#if !defined(T5_CONFIG)
  addLEDConfiguration(1, 3);
  addLEDConfiguration(0, 13);

#if defined(T3_CONFIG) || defined(T4_CONFIG)
  addLEDConfiguration(2, 13);
  addLEDConfiguration(3, 13);
  addLEDConfiguration(4, 13);
#endif

#else /* T5 */
  addLEDConfiguration(0, 16);
  addLEDConfiguration(1, 14);
  addLEDConfiguration(2, 13);
#endif
#endif

  addDeviceID();

/* DS18B20 or DHTxx Sensor configuration */
#if defined(CONFIG_HARDWARE_DHXX) || defined(CONFIG_HARDWARE_DS18B20)
  SensorConfiguration.gpio = 14;
  SensorConfiguration.sendOnlyChanges = true;
  SensorConfiguration.interval = 60;
#ifdef CONFIG_HARDWARE_DS18B20
  SensorConfiguration.correction = 0;
  SensorConfiguration.unit = 0;
  SensorConfiguration.idx = 0;
#else
  SensorConfiguration.gpio = 2;
  SensorConfiguration.type = 1;
  SensorConfiguration.temperature.correction = 0;
  SensorConfiguration.humidity.correction = 0;
  SensorConfiguration.temperature.unit = 0;
  SensorConfiguration.publishHeatIndex = false;
  SensorConfiguration.publishDewPoint = false;
  SensorConfiguration.temperatureIdx = 0;
  SensorConfiguration.humidityIdx = 0;
  SensorConfiguration.temperatureAndHumidityIdx = 0;
#endif

  Data->saveConfiguration(SensorConfiguration);
#endif

#if defined(T3_CONFIG)
  PIRConfiguration.gpio = 4;
  PIRConfiguration.howLongKeepRelayOn = 10;
  PIRConfiguration.invertRelayState = false;
  PIRConfiguration.bouncing = 0;
  PIRConfiguration.idx = 0;
  PIRConfiguration.outputDefaultState = PIR_NO;
  PIRConfiguration.ledId = 0;
  for (uint8_t i = 0; i < sizeof(deviceConfiguration.isPIR); i++) {
    sprintf(PIRConfiguration.name, "pir%d", i + 1);
    PIRConfiguration.relayId = i + 1;

    Data->saveConfiguration(i, PIRConfiguration);
  }
#endif

/* T5: Contactron and Gate configuration */
#if defined(T5_CONFIG)
  ContactronConfiguration.outputDefaultState = CONTACTRON_NO;
  ContactronConfiguration.bouncing = 200;
  ContactronConfiguration.gpio = 4;
  ContactronConfiguration.ledID = 2;
  ContactronConfiguration.idx = 0;
  sprintf(ContactronConfiguration.name, "C1");
  Data->saveConfiguration(0, ContactronConfiguration);
  ContactronConfiguration.gpio = 5;
  ContactronConfiguration.ledID = 3;
  sprintf(ContactronConfiguration.name, "C2");
  Data->saveConfiguration(1, ContactronConfiguration);

  GateConfiguration.state[0] = GATE_OPEN;
  GateConfiguration.state[1] = GATE_PARTIALLY_OPEN;
  GateConfiguration.state[2] = GATE_PARTIALLY_OPEN;
  GateConfiguration.state[3] = GATE_CLOSED;
  GateConfiguration.idx = 0;

  Data->saveConfiguration(GateConfiguration);
  Data->saveGateState(0);

#endif

/* T6 HPMA115S0 Sensor */
#ifdef CONFIG_HARDWARE_HPMA115S0
  SensorHPMA115S0Configuration.interval = 60;
  SensorHPMA115S0Configuration.timeToMeasure = 0;
  SensorHPMA115S0Configuration.idx.pm10 = 0;
  SensorHPMA115S0Configuration.idx.pm25 = 0;
  Data->saveConfiguration(SensorHPMA115S0Configuration);
#endif

#ifdef CONFIG_HARDWARE_UART
  SerialPortConfiguration.RXD = 12;
  SerialPortConfiguration.TXD = 14;
  Data->saveConfiguration(SerialPortConfiguration);
#endif

#ifdef CONFIG_HARDWARE_BMX80
  SensorBMx80Configuration.interval = 60;
  SensorBMx80Configuration.i2cAddress = 0;
  SensorBMx80Configuration.idx.temperatureHumidityPressure = 0;
  SensorBMx80Configuration.idx.gasResistance = 0;
  SensorBMx80Configuration.idx.temperature = 0;
  SensorBMx80Configuration.idx.humidity = 0;
  SensorBMx80Configuration.idx.pressure = 0;
  Data->saveConfiguration(SensorBMx80Configuration);
#endif

#ifdef CONFIG_HARDWARE_BH1750
  SensorBH1750Configuration.interval = 60;
  SensorBH1750Configuration.i2cAddress = 0;
  SensorBH1750Configuration.idx = 0;
  SensorBH1750Configuration.mode = 0;
  Data->saveConfiguration(SensorBH1750Configuration);
#endif

#ifdef CONFIG_HARDWARE_LED
  Data->saveSystemLedID(1);
#endif

  Data->saveDeviceMode(2);
  Data->saveLanguage(1);
}

void AFEDefaults::addDomoticzConfiguration() {
  DOMOTICZ DomoticzConfiguration;
  DomoticzConfiguration.protocol = 0;
  DomoticzConfiguration.host[0] = '\0';
  DomoticzConfiguration.user[0] = '\0';
  DomoticzConfiguration.password[0] = '\0';
  DomoticzConfiguration.port = 8080;
  Data->saveConfiguration(DomoticzConfiguration);
}

#ifdef CONFIG_HARDWARE_LED
void AFEDefaults::addLEDConfiguration(uint8_t id, uint8_t gpio) {
  LED LEDConfiguration;
  LEDConfiguration.gpio = gpio;
  LEDConfiguration.changeToOppositeValue = false;
  Data->saveConfiguration(id, LEDConfiguration);
}
#endif

void AFEDefaults::addDeviceID() {
  char id[8];
  uint8_t range;
  for (uint8_t i = 0; i < sizeof(id); i++) {
    range = random(3);
    id[i] = char(range == 0 ? random(48, 57)
                            : range == 1 ? random(65, 90) : random(97, 122));
  }
  Data->saveDeviceID(String(id));
}
void AFEDefaults::eraseConfiguration() { Eeprom.erase(); }

#ifdef CONFIG_HARDWARE_ADC_VCC
void AFEDefaults::addAnalogInputDefaultConfiguration() {
#ifdef DEBUG
  Serial << endl
         << endl
         << "----------------- Writing File -------------------";
  Serial << endl << "Opening file: cfg-analog-input.json : ";
#endif

  File configFile = SPIFFS.open("/cfg-analog-input.json", "w");

  if (configFile) {

    configFile.print("{\"gpio\":17,\"interval\":60,\"numberOfSamples\":1,"
                     "\"idx\":{\"raw\":0,\"percent\":0,\"voltage\":0}}");

#ifdef DEBUG
    Serial << "success" << endl << "Writing JSON : ";
#endif

    configFile.close();

#ifdef DEBUG
    Serial << "success";
#endif

  }
#ifdef DEBUG
  else {
    Serial << endl << "failed to open file for writing";
  }
  Serial << endl << "--------------------------------------------------";
#endif
}

void AFEDefaults::addDeviceDefaultConfiguration() {
#ifdef DEBUG
  Serial << endl
         << endl
         << "----------------- Writing File -------------------";
  Serial << endl << "Opening file: device.json : ";
#endif

  File configFile = SPIFFS.open("/cfg-device.json", "w");

  if (configFile) {
    configFile.print("{\"isAnalogInput\":0}");

#ifdef DEBUG
    Serial << "success" << endl << "Writing JSON : ";
#endif

    configFile.close();

#ifdef DEBUG
    Serial << "success";
#endif
  }
#ifdef DEBUG
  else {
    Serial << endl << "failed to open file for writing";
  }
  Serial << endl << "--------------------------------------------------";
#endif
}
#endif

#ifdef CONFIG_HARDWARE_SPIFFS
void AFEDefaults::formatSPIFFS() {
  if (SPIFFS.format()) {
#ifdef DEBUG
    Serial << endl << "- File system formated";
  } else {
    Serial << endl << "- File system NOT formated";
  }
#endif
}
#endif
