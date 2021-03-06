/* AFE Firmware for smart home devices, Website: https://afe.smartnydom.pl/ */

#include "AFE-Sites-Generator.h"

AFESitesGenerator::AFESitesGenerator() {}

void AFESitesGenerator::begin(AFEDataAccess *_Data, AFEDevice *_Device,
                              AFEFirmwarePro *_FirmwarePro,
                              AFEJSONRPC *_RestAPI) {
  Device = _Device;
  FirmwarePro = _FirmwarePro;
  Data = _Data;
  RestAPI = _RestAPI;
  Data->getConfiguration(&Firmware);
  Data->getDeviceUID().toCharArray(deviceID, sizeof(deviceID) + 1);
  _HtmlResponse.reserve(AFE_CONFIG_JSONRPC_JSON_RESPONSE_SIZE);
}

void AFESitesGenerator::generateHeader(String &page, uint16_t redirect) {

  page += FPSTR(HTTP_HEADER);

  if (redirect > 0) {
    page.replace("{{s.r}}", "<meta http-equiv=\"refresh\" content=\"" +
                                String(redirect) + "; url=/\">");
  } else {
    page.replace("{{s.r}}", "");
  }

  page.concat(F("<div class=\"c\">"));
}

void AFESitesGenerator::generateEmptyMenu(String &page, uint16_t redirect) {
  generateHeader(page, redirect);
  page.concat(F("<div class=\"l\">{{A}}<small style=\"opacity:.3\">"));
  page.concat(F(L_VERSION));
  page.concat(F(" T{{f.t}}-{{f.v}}</small></div><div class=\"r\">"));
}

void AFESitesGenerator::generateMenu(String &page, uint16_t redirect) {
  Device->begin();

  generateHeader(page, redirect);
  page.concat(F("<div class=\"l\">{{A}}<small style=\"opacity:.3\">"));
  page.concat(F(L_VERSION));
  page.concat(F(" T{{f.t}}-{{f.v}}</small><ul class=\"lst\">"));

  page.concat(FPSTR(HTTP_MENU_HEADER));
  page.replace("{{m.h}}", F("Menu"));

  /* Gnerating Menu */
  addMenuItem(page, F(L_DEVICE), AFE_CONFIG_SITE_DEVICE);

  page.concat(FPSTR(HTTP_MENU_HEADER));
  page.replace("{{m.h}}", F(L_CONNECTIONS));

  addMenuItem(page, F(L_NETWORK), AFE_CONFIG_SITE_NETWORK);

  if (Device->configuration.api.mqtt) {
    addMenuItem(page, F(L_MQTT_BROKER), AFE_CONFIG_SITE_MQTT);
  }

#ifdef AFE_CONFIG_API_DOMOTICZ_ENABLED
  if (Device->configuration.api.domoticz) {
    addMenuItem(page, F(L_DOMOTICZ_SERVER), AFE_CONFIG_SITE_DOMOTICZ);
  }
#endif

/* I2C */
#ifdef AFE_CONFIG_HARDWARE_I2C
  addMenuItem(page, F("I2C"), AFE_CONFIG_SITE_I2C);
#endif // AFE_CONFIG_HARDWARE_I2C

/* UART */
#ifdef AFE_CONFIG_HARDWARE_UART
/* Don't show it if HPMA115SO sensor is not added to the device */
#ifdef AFE_CONFIG_HARDWARE_HPMA115S0
  if (Device->configuration.noOfHPMA115S0s > 0) {
#endif
    addMenuItem(page, F("UART"), AFE_CONFIG_SITE_UART);
#ifdef AFE_CONFIG_HARDWARE_HPMA115S0
  }
#endif // AFE_CONFIG_HARDWARE_HPMA115S0
#endif // AFE_CONFIG_HARDWARE_UART

  page.concat(FPSTR(HTTP_MENU_HEADER));
  page.replace("{{m.h}}", F(L_HARDWARE));

#ifdef AFE_CONFIG_HARDWARE_LED
  if (Device->configuration.noOfLEDs > 0) {
    addMenuHeaderItem(page, F(L_LEDS));
    addMenuSubItem(page, "LED", Device->configuration.noOfLEDs,
                   AFE_CONFIG_SITE_LED);
  }
#endif // AFE_CONFIG_HARDWARE_LED

#ifdef AFE_CONFIG_HARDWARE_GATE
  if (Device->configuration.noOfGates > 0) {
    addMenuHeaderItem(page, F(L_GATE_CONFIGURATION));
    addMenuSubItem(page, L_GATE, Device->configuration.noOfGates,
                   AFE_CONFIG_SITE_GATE);
  }
#endif // AFE_CONFIG_HARDWARE_GATE

/* Relay */
#ifdef AFE_CONFIG_HARDWARE_RELAY
  if (Device->configuration.noOfRelays > 0) {

    addMenuHeaderItem(page, F(L_RELAYS_CONFIGURATION));
    addMenuSubItem(page, L_RELAY, Device->configuration.noOfRelays,
                   AFE_CONFIG_SITE_RELAY);
  }
#endif // AFE_CONFIG_HARDWARE_RELAY

#ifdef AFE_CONFIG_HARDWARE_SWITCH
  if (Device->configuration.noOfSwitches > 0) {
    addMenuHeaderItem(page, F(L_BUTTONS_SWITCHES));
    addMenuSubItem(page, L_SWITCH, Device->configuration.noOfSwitches,
                   AFE_CONFIG_SITE_SWITCH);
  }
#endif // AFE_CONFIG_HARDWARE_SWITCH

#ifdef AFE_CONFIG_HARDWARE_BINARY_SENSOR
  if (Device->configuration.noOfBinarySensors > 0) {
    addMenuHeaderItem(page, F(L_BINARY_SENSORS));
    addMenuSubItem(page, L_SENSOR, Device->configuration.noOfBinarySensors,
                   AFE_CONFIG_SITE_BINARY_SENSOR);
  }
#endif // AFE_CONFIG_HARDWARE_BINARY_SENSOR

/* Contactrons and Gate */
#ifdef AFE_CONFIG_HARDWARE_CONTACTRON
  if (Device->configuration.noOfContactrons > 0) {
    addMenuHeaderItem(page, F(L_CONTACTRONS));
    addMenuSubItem(page, L_SENSOR, Device->configuration.noOfContactrons,
                   AFE_CONFIG_SITE_CONTACTRON);
  }
#endif // AFE_CONFIG_HARDWARE_CONTACTRON

/* Sensor DS18B20 */
#ifdef AFE_CONFIG_HARDWARE_DS18B20
  if (Device->configuration.noOfDS18B20s > 0) {

    addMenuHeaderItem(page, F(L_DS18B20_SENSORS));
    addMenuSubItem(page, L_SENSOR, Device->configuration.noOfDS18B20s,
                   AFE_CONFIG_SITE_DS18B20);
  }
#endif // AFE_CONFIG_HARDWARE_DS18B20

/* Sensor DHT */
#ifdef AFE_CONFIG_HARDWARE_DHT
  if (Device->configuration.noOfDHTs > 0) {
    addMenuHeaderItem(page, F("DHT"));
    addMenuSubItem(page, L_SENSOR, Device->configuration.noOfDHTs,
                   AFE_CONFIG_SITE_DHT);
  }
#endif

#ifdef AFE_CONFIG_HARDWARE_BMEX80
  if (Device->configuration.noOfBMEX80s > 0) {
    addMenuHeaderItem(page, F(L_BMEX80_SENSORS));
    addMenuSubItem(page, L_SENSOR, Device->configuration.noOfBMEX80s,
                   AFE_CONFIG_SITE_BMEX80);
  }
#endif

#ifdef AFE_CONFIG_HARDWARE_BH1750
  if (Device->configuration.noOfBH1750s > 0) {

    addMenuHeaderItem(page, F(L_BH1750_SENSORS));
    addMenuSubItem(page, L_SENSOR, Device->configuration.noOfBH1750s,
                   AFE_CONFIG_SITE_BH1750);
  }
#endif

#ifdef AFE_CONFIG_HARDWARE_HPMA115S0
  /* This is hardcoded for one sensor */
  if (Device->configuration.noOfHPMA115S0s > 0) {
    addMenuHeaderItem(page, F(L_PARTICLE_SENSORS));
    addMenuSubItem(page, L_HPMA115S0_SENSOR,
                   Device->configuration.noOfHPMA115S0s,
                   AFE_CONFIG_SITE_HPMA115S0);
  }
#endif

#ifdef AFE_CONFIG_HARDWARE_ANEMOMETER
  if (Device->configuration.noOfAnemometerSensors > 0) {
    addMenuItem(page, F(L_ANEMOMETER_SENSOR),
                AFE_CONFIG_SITE_ANEMOMETER_SENSOR);
  }
#endif

#ifdef AFE_CONFIG_HARDWARE_RAINMETER
  if (Device->configuration.noOfRainmeterSensors > 0) {
    addMenuItem(page, F(L_RAINMETER), AFE_CONFIG_SITE_RAINMETER_SENSOR);
  }
#endif

#ifdef AFE_CONFIG_HARDWARE_AS3935
  if (Device->configuration.noOfAS3935s > 0) {
    addMenuItem(page, F(L_AS3935_SENSOR), AFE_CONFIG_SITE_AS3935);
  }
#endif

#ifdef AFE_CONFIG_HARDWARE_ADC_VCC
  if (Device->configuration.isAnalogInput) {
    addMenuItem(page, F(L_ANALOG_INPUT), AFE_CONFIG_SITE_ANALOG_INPUT);
  }
#endif

  page.concat(FPSTR(HTTP_MENU_HEADER));
  page.replace("{{m.h}}", F(L_FUNCTIONS));

/* System LED */
#ifdef AFE_CONFIG_HARDWARE_LED
  if (Device->configuration.noOfLEDs > 0) {
    addMenuItem(page, F(L_LED_SYSTEM), AFE_CONFIG_SITE_SYSTEM_LED);
  }
#endif

/* Regulator */
#ifdef AFE_CONFIG_FUNCTIONALITY_REGULATOR
  if (Device->configuration.noOfRegulators > 0) {
    addMenuHeaderItem(page, F(L_REGULATORS));
    addMenuSubItem(page, L_REGULATOR, Device->configuration.noOfRegulators,
                   AFE_CONFIG_SITE_REGULATOR);
  }
#endif

/* Thermal protection */
#ifdef AFE_CONFIG_FUNCTIONALITY_THERMAL_PROTECTOR
  if (Device->configuration.noOfThermalProtectors > 0) {
    addMenuHeaderItem(page, F(L_THERMAL_PROTECTORS));
    addMenuSubItem(page, L_THERMAL_PROTECTOR,
                   Device->configuration.noOfThermalProtectors,
                   AFE_CONFIG_SITE_THERMAL_PROTECTOR);
  }
#endif

  page.concat(FPSTR(HTTP_MENU_HEADER));
  page.replace("{{m.h}}", F(L_FIRMWARE));

  addMenuItem(page, F(L_PASSWORD_SET_PASSWORD), AFE_CONFIG_SITE_PASSWORD);
#ifndef AFE_CONFIG_OTA_NOT_UPGRADABLE
  addMenuItem(page, F(L_FIRMWARE_UPGRADE), AFE_CONFIG_SITE_UPGRADE);
#endif
  addMenuItem(page, F(L_RESET_DEVICE), AFE_CONFIG_SITE_RESET);
  addMenuItem(page, F(L_PRO_VERSION), AFE_CONFIG_SITE_PRO_VERSION);

  addMenuItemExternal(page, F(L_DOCUMENTATION), F(AFE_URL_DOCUMENTATION));
  addMenuItemExternal(page, F(L_HELP), F(AFE_URL_HELP));

  page.concat(F("</ul><h4></h4><ul class=\"lst\">"));

  addMenuItem(page, F(L_FINISH_CONFIGURATION), AFE_CONFIG_SITE_EXIT);

  /* Information section */
  page.concat(F("</ul></div><div class=\"r\">"));
}

void AFESitesGenerator::siteDevice(String &page) {
  boolean _itemDisabled = false;

  Data->getWelcomeMessage(_HtmlResponse);

  if (_HtmlResponse.length() > 0 || RestAPI->accessToWAN()) {
    openMessageSection(page, F("Firmware"), F(""));

    if (_HtmlResponse.length() > 0) {
      page.concat(FPSTR(HTTP_MESSAGE_LINE_ITEM));
      page.replace("{{I}}", _HtmlResponse);
    } else if (RestAPI->accessToWAN()) {

      RestAPI->sent(_HtmlResponse, AFE_CONFIG_JSONRPC_REST_METHOD_WELCOME);
      if (_HtmlResponse.length() > 0) {
        page.concat(FPSTR(HTTP_MESSAGE_LINE_ITEM));
        page.replace("{{I}}", _HtmlResponse);
      }

      RestAPI->sent(_HtmlResponse,
                    AFE_CONFIG_JSONRPC_REST_METHOD_LATEST_VERSION);
      if (_HtmlResponse.length() > 0) {
        page.concat(FPSTR(HTTP_MESSAGE_LINE_ITEM));
        page.replace("{{I}}", _HtmlResponse);
      }

      RestAPI->sent(_HtmlResponse, AFE_CONFIG_JSONRPC_REST_METHOD_CHECK_PRO);
      if (_HtmlResponse.length() > 0) {
        page.concat(FPSTR(HTTP_MESSAGE_LINE_ITEM));
        page.replace("{{I}}", _HtmlResponse);
      }
    }
    closeSection(page);
  }

  /* Section: Device name */
  openSection(page, F(L_DEVICE), F(L_DEVICE_SECTION_INFO));
  addInputFormItem(page, AFE_FORM_ITEM_TYPE_TEXT, "n", L_DEVICE_NAME,
                   Device->configuration.name, "16");
  closeSection(page);

  /* Section: Hardware */
  openSection(page, F(L_DEVICE_HARDWARE_CONFIGURATION),
              F(L_DEVICE_HARDWARE_CONFIGURATION_INFO));

/* LED */
#ifdef AFE_CONFIG_HARDWARE_LED
  addListOfHardwareItem(page, AFE_CONFIG_HARDWARE_NUMBER_OF_LEDS,
                        Device->configuration.noOfLEDs, F("l"),
                        F(L_DEVICE_NUMBER_OF_LEDS));
#endif

/* Contactrons */
#ifdef AFE_CONFIG_HARDWARE_CONTACTRON
  addListOfHardwareItem(page, AFE_CONFIG_HARDWARE_NUMBER_OF_CONTACTRONS,
                        Device->configuration.noOfContactrons, F("co"),
                        F(L_DEVICE_NUMBER_OF_CONTACTRONS));

#endif

#ifdef AFE_CONFIG_HARDWARE_RELAY
  /* Relay */
  addListOfHardwareItem(page, AFE_CONFIG_HARDWARE_NUMBER_OF_RELAYS,
                        Device->configuration.noOfRelays, F("r"),
                        F(L_DEVICE_NUMBER_OF_RELAYS));
#endif

#ifdef AFE_CONFIG_HARDWARE_SWITCH
  /* Switch */
  addListOfHardwareItem(page, AFE_CONFIG_HARDWARE_NUMBER_OF_SWITCHES,
                        Device->configuration.noOfSwitches, F("s"),
                        F(L_DEVICE_NUMBER_OF_SWITCHES));
#endif

#ifdef AFE_CONFIG_HARDWARE_BINARY_SENSOR
  /* Binary sensor */
  addListOfHardwareItem(page, AFE_CONFIG_HARDWARE_NUMBER_OF_BINARY_SENSORS,
                        Device->configuration.noOfBinarySensors, F("b"),
                        F(L_DEVICE_NUMBER_OF_BINARY_SENSORS));
#endif

/* DS18B20 */
#ifdef AFE_CONFIG_HARDWARE_DS18B20

#ifdef T4_CONFIG // Functionality is PRO for T4
  _itemDisabled = !FirmwarePro->Pro.valid;
#else
  _itemDisabled = false;
#endif

  addListOfHardwareItem(page, AFE_CONFIG_HARDWARE_NUMBER_OF_DS18B20,
                        Device->configuration.noOfDS18B20s, F("ds"),
                        F(L_DEVICE_NUMBER_OF_DS18B20_SENSORS), _itemDisabled);
#endif

/* DHT */
#ifdef AFE_CONFIG_HARDWARE_DHT
  addListOfHardwareItem(page, AFE_CONFIG_HARDWARE_NUMBER_OF_DHT,
                        Device->configuration.noOfDHTs, F("dh"),
                        F(L_DHT_SENSORS));
#endif

#ifdef AFE_CONFIG_HARDWARE_DHT
// TODO
#endif

#ifdef AFE_CONFIG_HARDWARE_HPMA115S0
  addListOfHardwareItem(page, AFE_CONFIG_HARDWARE_NUMBER_OF_HPMA115S0,
                        Device->configuration.noOfHPMA115S0s, F("hp"),
                        F(L_DEVICE_NUMBER_OF_HPMA115S0_SENSORS));
#endif

#ifdef AFE_CONFIG_HARDWARE_BH1750

#ifdef T5_CONFIG // Functionality is PRO for T5
  _itemDisabled = !FirmwarePro->Pro.valid;
#else
  _itemDisabled = false;
#endif

  addListOfHardwareItem(page, AFE_CONFIG_HARDWARE_NUMBER_OF_BH1750,
                        Device->configuration.noOfBH1750s, F("bh"),
                        F(L_DEVICE_NUMBER_OF_BH1750_SENSORS), _itemDisabled);
#endif

#ifdef AFE_CONFIG_HARDWARE_BMEX80

#ifdef T5_CONFIG // Functionality is PRO for T5
  _itemDisabled = !FirmwarePro->Pro.valid;
#else
  _itemDisabled = false;
#endif

  addListOfHardwareItem(page, AFE_CONFIG_HARDWARE_NUMBER_OF_BMEX80,
                        Device->configuration.noOfBMEX80s, F("b6"),
                        F(L_DEVICE_NUMBER_OF_BMEX80_SENSORS), _itemDisabled);
#endif

#ifdef AFE_CONFIG_HARDWARE_AS3935
  addListOfHardwareItem(page, AFE_CONFIG_HARDWARE_NUMBER_OF_AS3935,
                        Device->configuration.noOfAS3935s, F("a3"),
                        F(L_DEVICE_NUMBER_OF_AS3935_SENSORS),
                        !FirmwarePro->Pro.valid);
#endif

#ifdef AFE_CONFIG_HARDWARE_ANEMOMETER
  addListOfHardwareItem(page, AFE_CONFIG_HARDWARE_NUMBER_OF_ANEMOMETER_SENSORS,
                        Device->configuration.noOfAnemometerSensors, F("w"),
                        F(L_ANEMOMETER_SENSOR));
#endif

#ifdef AFE_CONFIG_HARDWARE_RAINMETER
  addListOfHardwareItem(page, AFE_CONFIG_HARDWARE_NUMBER_OF_RAINMETER_SENSORS,
                        Device->configuration.noOfRainmeterSensors, F("d"),
                        F(L_RAINMETER));
#endif

#ifdef AFE_CONFIG_HARDWARE_ADC_VCC

  addCheckboxFormItem(
      page, "ad", L_DEVICE_DO_MEASURE_ADC, "1",
      Device->configuration.isAnalogInput,
      (!FirmwarePro->Pro.valid ? L_PRO_VERSION : AFE_FORM_ITEM_SKIP_PROPERTY),
      !FirmwarePro->Pro.valid);
#endif

#if defined(T3_CONFIG)
  itemsNumber = 0;
  for (uint8_t i = 0; i < sizeof(Device->configuration.isPIR); i++) {
    if (Device->configuration.isPIR[i]) {
      itemsNumber++;
    }
  }

  body += addListOfHardwareItem(
      sizeof(Device->configuration.isPIR), itemsNumber, "p",
      language == 0 ? "Ilość czujników PIR" : "Number of PIRs");
#endif

#ifdef AFE_CONFIG_HARDWARE_GATE
  closeSection(page);
  openSection(page, F(L_DEVICE_CONTROLLED_GATES), F(""));
  addListOfHardwareItem(page, AFE_CONFIG_HARDWARE_NUMBER_OF_GATES,
                        Device->configuration.noOfGates, F("g"),
                        F(L_DEVICE_NUMBER_OF_CONTROLLED_GATES));
#endif

  closeSection(page);

#ifdef AFE_CONFIG_HARDWARE_RELAY

  if (Device->configuration.noOfRelays > 0) {

#if defined(AFE_CONFIG_FUNCTIONALITY_REGULATOR) ||                             \
    defined(AFE_CONFIG_FUNCTIONALITY_THERMAL_PROTECTOR)
    /* Additional functionalities */
    openSection(page, F(L_DEVICE_ADDITIONAL_FUNCTIONALITIES), F(""));

#ifdef AFE_CONFIG_FUNCTIONALITY_REGULATOR
    addListOfHardwareItem(page, AFE_CONFIG_HARDWARE_NUMBER_OF_REGULATORS,
                          Device->configuration.noOfRegulators, F("re"),
                          F(L_DEVICE_NUMBER_OF_REGULATORS));
#endif

#ifdef AFE_CONFIG_FUNCTIONALITY_THERMAL_PROTECTOR
    addListOfHardwareItem(page,
                          AFE_CONFIG_HARDWARE_NUMBER_OF_THERMAL_PROTECTORS,
                          Device->configuration.noOfThermalProtectors, F("tp"),
                          F(L_DEVICE_NUMBER_OF_THERMAL_PROTECTORS));
#endif
    closeSection(page);
#endif // defined(AFE_CONFIG_FUNCTIONALITY_REGULATOR) ||
       // defined(AFE_CONFIG_FUNCTIONALITY_THERMAL_PROTECTOR)

  } // noOfRelays > 0

#endif // AFE_CONFIG_HARDWARE_RELAY

  /* Section: APIs */
  openSection(page, F(L_DEVICE_CONTROLLING), F(L_DEVICE_CONTROLLING_INFO));

#ifdef AFE_CONFIG_API_DOMOTICZ_ENABLED
  addSelectFormItemOpen(page, F("v"), F(L_DOMOTICZ_VERSION));
  addSelectOptionFormItem(page, L_DEVICE_DOMOTICZ_VERSION_410, "0",
                          Device->configuration.api.domoticzVersion ==
                              AFE_DOMOTICZ_VERSION_0);
  addSelectOptionFormItem(page, L_DEVICE_DOMOTICZ_VERSION_2020, "1",
                          Device->configuration.api.domoticzVersion ==
                              AFE_DOMOTICZ_VERSION_1);
  addSelectFormItemClose(page);
#endif

  addCheckboxFormItem(page, "h", "HTTP API", "1",
                      Device->configuration.api.http);

#ifdef AFE_CONFIG_API_DOMOTICZ_ENABLED
  addRadioButtonFormItem(page, "m", "Domoticz HTTP API", "1",
                         Device->configuration.api.domoticz);
  addRadioButtonFormItem(page, "m", "Domoticz MQTT API", "2",
                         Device->configuration.api.mqtt);
#else
  addCheckboxFormItem(page, "m", "MQTT API", "1",
                      Device->configuration.api.mqtt);
#endif

  closeSection(page);

  /* Section: auto logout */
  openSection(page, F(L_DEVICE_AUTOLOGOUT_TITLE),
              F(L_DEVICE_AUTOLOGOUT_DESCRIPTION));
  addCheckboxFormItem(page, "al", L_ENABLED, "1",
                      Device->configuration.timeToAutoLogOff > 0);
  closeSection(page);
}

void AFESitesGenerator::siteNetwork(String &page) {
  NETWORK configuration;
  char _ip[18];
  char _int[4];
  Data->getConfiguration(&configuration);

  /* Section: WiFi selection, user and password */
  openSection(page, F(L_NETWORK_CONFIGURATION),
              F(L_NETWORK_CONFIGURATION_INFO));

  addSelectFormItemOpen(page, F("s"), F(L_NETWORK_SSID));

#ifdef DEBUG
  Serial << endl << F("Searching for WiFi networks: ");
#endif
  int numberOfNetworks = WiFi.scanNetworks();
  char _ssid[sizeof(configuration.ssid)];

#ifdef DEBUG
  Serial << endl << F(" - found: ") << numberOfNetworks;
#endif

  for (int i = 0; i < numberOfNetworks; i++) {

#ifdef DEBUG
    Serial << endl << " - " << WiFi.SSID(i);
#endif

    WiFi.SSID(i).toCharArray(_ssid, sizeof(_ssid));
    addSelectOptionFormItem(page, _ssid, _ssid,
                            strcmp(_ssid, configuration.ssid) == 0);
  }
  page.concat(F("</select>"));

  page.concat(F("<input type=\"submit\" class =\"b bc\" value=\""));
  page.concat(F(L_NETWORK_REFRESH));
  page.concat(F("\" formaction=\"/?o="));
  page += AFE_CONFIG_SITE_NETWORK;
  page.concat(F("&c=0\"></div>"));

  addInputFormItem(page, AFE_FORM_ITEM_TYPE_PASSWORD, "p", L_PASSWORD,
                   configuration.password, "32");

  WiFi.macAddress().toCharArray(_ip, 18);

  addInputFormItem(page, AFE_FORM_ITEM_TYPE_TEXT, "m", "MAC", _ip,
                   AFE_FORM_ITEM_SKIP_PROPERTY, AFE_FORM_ITEM_SKIP_PROPERTY,
                   AFE_FORM_ITEM_SKIP_PROPERTY, AFE_FORM_ITEM_SKIP_PROPERTY,
                   AFE_FORM_ITEM_SKIP_PROPERTY, true);
  closeSection(page);

  /* Section: DHCP or Fixed IP */
  openSection(page, F(L_NETWORK_DEVICE_IP), F(L_NETWORK_DEVICE_IP_INFO));
  addCheckboxFormItem(page, "d", L_NETWORK_DHCP_ENABLED, "1",
                      configuration.isDHCP);
  addInputFormItem(page, AFE_FORM_ITEM_TYPE_TEXT, "i1", L_IP_ADDRESS,
                   configuration.ip);
  addInputFormItem(page, AFE_FORM_ITEM_TYPE_TEXT, "i2", L_NETWORK_GATEWAY,
                   configuration.gateway);
  addInputFormItem(page, AFE_FORM_ITEM_TYPE_TEXT, "i3", L_NETWORK_SUBNET,
                   configuration.subnet);
  closeSection(page);

  /* Section: Backup WiFI */
  openSection(page, F(L_NETWORK_BACKUP_CONFIGURATION),
              F(L_NETWORK_BACKUP_CONFIGURATION_HINT));

  addSelectFormItemOpen(page, F("sb"), F(L_NETWORK_SSID));

  addSelectOptionFormItem(page, L_NETWOK_NONE_BACKUP_SSID, L_NONE,
                          strcmp(AFE_CONFIG_NETWORK_DEFAULT_NONE_SSID,
                                 configuration.ssidBackup) == 0);

  for (int i = 0; i < numberOfNetworks; i++) {
    WiFi.SSID(i).toCharArray(_ssid, sizeof(_ssid));
    addSelectOptionFormItem(page, _ssid, _ssid,
                            strcmp(_ssid, configuration.ssidBackup) == 0);
  }
  page.concat(F("</select>"));

  addInputFormItem(page, AFE_FORM_ITEM_TYPE_PASSWORD, "pb", L_PASSWORD,
                   configuration.passwordBackup, "32");

  sprintf(_int, "%d", configuration.noFailuresToSwitchNetwork);
  addInputFormItem(page, AFE_FORM_ITEM_TYPE_NUMBER, "fs",
                   L_NETWORK_SWITCH_TO_BACKUP, _int,
                   AFE_FORM_ITEM_SKIP_PROPERTY, "1", "255", "1");

  closeSection(page);

  /* Section: Advanced settings */
  openSection(page, F(L_NETWORK_ADVANCED), F(""));

  sprintf(_int, "%d", configuration.noConnectionAttempts);

  addInputFormItem(page, AFE_FORM_ITEM_TYPE_NUMBER, "na",
                   L_NETWORK_NUMBER_OF_CONNECTIONS, _int,
                   AFE_FORM_ITEM_SKIP_PROPERTY, "1", "255", "1");

  sprintf(_int, "%d", configuration.waitTimeConnections);
  addInputFormItem(page, AFE_FORM_ITEM_TYPE_NUMBER, "wc",
                   L_NETWORK_TIME_BETWEEN_CONNECTIONS, _int,
                   AFE_FORM_ITEM_SKIP_PROPERTY, "1", "255", "1", L_SECONDS);

  sprintf(_int, "%d", configuration.waitTimeSeries);
  addInputFormItem(page, AFE_FORM_ITEM_TYPE_NUMBER, "ws", L_NETWORK_SLEEP_TIME,
                   _int, AFE_FORM_ITEM_SKIP_PROPERTY, "1", "255", "1",
                   L_SECONDS);

  closeSection(page);
}

void AFESitesGenerator::siteMQTTBroker(String &page) {
  MQTT configuration;
  Data->getConfiguration(&configuration);

  /* Section: MQTT Broker configuration */
  openSection(page, F("MQTT Broker"), F(L_MQTT_CONFIGURATION_INFO));
  addInputFormItem(page, AFE_FORM_ITEM_TYPE_TEXT, "h", "Hostname",
                   configuration.host, "32");
  addInputFormItem(page, AFE_FORM_ITEM_TYPE_TEXT, "a", L_IP_ADDRESS,
                   configuration.ip);
  char _number[7];
  sprintf(_number, "%d", configuration.port);
  addInputFormItem(page, AFE_FORM_ITEM_TYPE_NUMBER, "p", "Port", _number,
                   AFE_FORM_ITEM_SKIP_PROPERTY, "0", "65535", "1");
  addInputFormItem(page, AFE_FORM_ITEM_TYPE_TEXT, "u", L_USERNAME,
                   configuration.user, "32");
  addInputFormItem(page, AFE_FORM_ITEM_TYPE_PASSWORD, "s", L_PASSWORD,
                   configuration.password, "32");

  closeSection(page);

  openSection(page, F(L_MQTT_CONNECTION), F(L_MQTT_CONNECTION_HINT));
  addCheckboxFormItem(page, "ph", L_MQTT_USE_PING, "1",
                      configuration.pingHostBeforeConnection,
                      L_MQTT_USE_PING_HINT);

  sprintf(_number, "%d", configuration.timeout);
  addInputFormItem(page, AFE_FORM_ITEM_TYPE_NUMBER, "t", L_MQTT_TIMEOUT,
                   _number, AFE_FORM_ITEM_SKIP_PROPERTY, "0", "30000", "1",
                   L_MILISECONDS);

  closeSection(page);
/* Section: LWT */
#ifdef AFE_CONFIG_API_DOMOTICZ_ENABLED
  char _idx[7];
  sprintf(_idx, "%d", configuration.lwt.idx);
  openSection(page, F(L_MQTT_IDX_LWT), F(L_DOMOTICZ_NO_IF_IDX_0));
  addInputFormItem(page, AFE_FORM_ITEM_TYPE_NUMBER, "x", "IDX", _idx,
                   AFE_FORM_ITEM_SKIP_PROPERTY,
                   AFE_DOMOTICZ_IDX_MIN_FORM_DEFAULT,
                   AFE_DOMOTICZ_IDX_MAX_FORM_DEFAULT, "1");
  closeSection(page);
#else
  openSection(page, F(L_MQTT_TOPIC_LWT), F(L_MQTT_TOPIC_EMPTY));
  addInputFormItem(page, AFE_FORM_ITEM_TYPE_TEXT, "t0", L_MQTT_TOPIC,
                   configuration.lwt.topic, "64");
  closeSection(page);
#endif

  openSection(page, F(L_MQTT_RETAIN_SECTION), F(L_MQTT_RETAIN_SECTION_INFO));

  addCheckboxFormItem(page, "rl", L_MQTT_RETAIN_LWT, "1",
                      configuration.retainLWT);
  addCheckboxFormItem(page, "ra", L_MQTT_RETAIN_ALL, "1",
                      configuration.retainAll);
  closeSection(page);
}

#ifdef AFE_CONFIG_API_DOMOTICZ_ENABLED
void AFESitesGenerator::siteDomoticzServer(String &page) {
  DOMOTICZ configuration;
  Data->getConfiguration(&configuration);

  openSection(page, F(L_DOMOTICZ_CONFIGURATION),
              F(L_DOMOTICZ_CONFIGURATION_INFO));

  addSelectFormItemOpen(page, F("t"), F(L_DOMOTICZ_PROTOCOL));
  addSelectOptionFormItem(page, "http://", "0", configuration.protocol == 0);
  addSelectOptionFormItem(page, "https://", "1", configuration.protocol == 1);
  addSelectFormItemClose(page);

  addInputFormItem(page, AFE_FORM_ITEM_TYPE_TEXT, "h", "Hostname/IP",
                   configuration.host, "40");

  char _number[6];
  sprintf(_number, "%d", configuration.port);
  addInputFormItem(page, AFE_FORM_ITEM_TYPE_NUMBER, "p", "Port", _number,
                   AFE_FORM_ITEM_SKIP_PROPERTY, "0", "65535", "1");
  addInputFormItem(page, AFE_FORM_ITEM_TYPE_TEXT, "u", L_USERNAME,
                   configuration.user, "32");
  addInputFormItem(page, AFE_FORM_ITEM_TYPE_PASSWORD, "s", L_PASSWORD,
                   configuration.password, "32");

  closeSection(page);
}
#endif // AFE_CONFIG_API_DOMOTICZ_ENABLED

void AFESitesGenerator::sitePassword(String &page) {
  PASSWORD configuration;
  Data->getConfiguration(&configuration);
  openSection(page, F(L_PASSWORD_SET_PASSWORD_TO_PANEL), F(""));
  addCheckboxFormItem(page, "r", L_PASSWORD_PROTECT_BY_PASSWORD, "1",
                      configuration.protect);
  if (configuration.protect) {
    addInputFormItem(page, AFE_FORM_ITEM_TYPE_TEXT, "p", L_PASSWORD,
                     configuration.password, "8");
  }
  closeSection(page);
}

#ifdef AFE_CONFIG_HARDWARE_LED
void AFESitesGenerator::siteLED(String &page, uint8_t id) {
  char title[8];
  LED configuration;
  Data->getConfiguration(id, &configuration);
  sprintf(title, "LED: #%d", id + 1);
#ifdef AFE_CONFIG_HARDWARE_MCP23017
  openSection(page, title, F(L_MCP23017_LED_CONNECTION));
#else
  openSection(page, title, F(""));
#endif

  addListOfGPIOs(page, F("g"), configuration.gpio);

#ifdef AFE_CONFIG_HARDWARE_MCP23017
  page.concat(FPSTR(HTTP_INFO_TEXT));
  page.replace("{{i.v}}", F(L_MCP23017_CONNECTION_VIA_MCP));
  addDeviceI2CAddressSelectionItem(page, configuration.mcp23017.address);
  addListOfMCP23017GPIOs(page, "mg", configuration.mcp23017.gpio);
  closeSection(page);
  openSection(page, F(L_MCP23107_LED_ADDITIONAL_SETTINGS), F(""));
#endif

  addCheckboxFormItem(page, "w", L_LED_CHANGE_INDICATION, "1",
                      configuration.changeToOppositeValue);
  closeSection(page);
}

void AFESitesGenerator::siteSystemLED(String &page) {
  uint8_t configuration = Data->getSystemLedID();
  openSection(page, F(L_LED_SYSTEM), F(L_LED_SYSTEM_INFO));
  addLEDSelectionItem(page, configuration);
  closeSection(page);
}
#endif // AFE_CONFIG_HARDWARE_LED

#ifdef AFE_CONFIG_HARDWARE_RELAY
void AFESitesGenerator::siteRelay(String &page, uint8_t id) {
  RELAY configuration;
  Data->getConfiguration(id, &configuration);

  char _number[9];
  char _text[23];
  sprintf(_text, "%s #%d", L_RELAY, id + 1);
  openSection(page, _text, F(""));

#ifdef AFE_CONFIG_HARDWARE_GATE
  /* Reading gate configuration */
  GATE gateConfiguration;
  boolean isGateRelay = false;
  for (uint8_t i = 0; i < AFE_CONFIG_HARDWARE_NUMBER_OF_GATES; i++) {
    Data->getConfiguration(i, &gateConfiguration);
    if (gateConfiguration.relayId != AFE_HARDWARE_ITEM_NOT_EXIST &&
        gateConfiguration.relayId == id) {
      isGateRelay = true;
      break;
    }
  }
#endif

#ifdef AFE_CONFIG_HARDWARE_GATE
  /* Below code is conditioned for the Gate functionality only. It's not
   * shown if the relay is assigned to the Gate */
  if (!isGateRelay) {
#endif

    addInputFormItem(page, AFE_FORM_ITEM_TYPE_TEXT, "n", L_NAME,
                     configuration.name, "16");
#ifdef AFE_CONFIG_HARDWARE_GATE
  }
#endif

#ifdef AFE_CONFIG_HARDWARE_MCP23017
  closeSection(page);
  openSection(page, F(L_MCP23017_CONNECTION), F(L_MCP23017_RELAY_CONNECTION));
#endif // AFE_CONFIG_HARDWARE_MCP23017

  addListOfGPIOs(page, F("g"), configuration.gpio);

#ifdef AFE_CONFIG_HARDWARE_MCP23017
  page.concat(FPSTR(HTTP_INFO_TEXT));
  page.replace("{{i.v}}", F(L_MCP23017_CONNECTION_VIA_MCP));
  addDeviceI2CAddressSelectionItem(page, configuration.mcp23017.address);
  addListOfMCP23017GPIOs(page, "mg", configuration.mcp23017.gpio);

  closeSection(page);
  openSection(page, F(L_MCP23017_RELAY_TRIGGERED), F(""));

#endif // AFE_CONFIG_HARDWARE_MCP23017

  addSelectFormItemOpen(page, F("ts"), F(L_RELAY_TRIGGERED));
  addSelectOptionFormItem(page, L_RELAY_TRIGGERED_HIGH_SIGNAL, "1",
                          configuration.triggerSignal ==
                              AFE_RELAY_SIGNAL_TRIGGER_HIGH);
  addSelectOptionFormItem(page, L_RELAY_TRIGGERED_LOW_SIGNAL, "0",
                          configuration.triggerSignal ==
                              AFE_RELAY_SIGNAL_TRIGGER_LOW);
  addSelectFormItemClose(page);

#ifdef AFE_CONFIG_HARDWARE_GATE
  /* Below code is conditioned for the Gate functionality only. It's not
   * shown if the relay is assigned to the Gate */
  if (!isGateRelay) {
#endif

    closeSection(page);

    openSection(page, F(L_RELAY_DEFAULT_VALUES), F(""));
    addSelectFormItemOpen(page, F("pr"), F(L_RELAY_DEFAULT_POWER_RESTORED));
    addSelectOptionFormItem(page, L_RELAY_NO_ACTION, "0",
                            configuration.state.powerOn == 0);
    addSelectOptionFormItem(page, L_RELAY_OFF, "1",
                            configuration.state.powerOn == 1);
    addSelectOptionFormItem(page, L_RELAY_ON, "2",
                            configuration.state.powerOn == 2);
    addSelectOptionFormItem(page, L_RELAY_LAST_KNOWN_STATE, "3",
                            configuration.state.powerOn == 3);
    addSelectOptionFormItem(page, L_RELAY_OPPOSITE_TO_LAST_KNOWN_STATE, "4",
                            configuration.state.powerOn == 4);
    addSelectFormItemClose(page);

    if (Device->configuration.api.mqtt) {

      addSelectFormItemOpen(page, F("mc"), F(L_RELAY_DEFAULT_MQTT_CONNECTED));
      addSelectOptionFormItem(page, L_RELAY_NO_ACTION, "0",
                              configuration.state.MQTTConnected == 0);
      addSelectOptionFormItem(page, L_RELAY_OFF, "1",
                              configuration.state.MQTTConnected == 1);
      addSelectOptionFormItem(page, L_RELAY_ON, "2",
                              configuration.state.MQTTConnected == 2);
      addSelectOptionFormItem(page, L_RELAY_LAST_KNOWN_STATE, "3",
                              configuration.state.MQTTConnected == 3);
      addSelectOptionFormItem(page, L_RELAY_OPPOSITE_TO_LAST_KNOWN_STATE, "4",
                              configuration.state.MQTTConnected == 4);

#ifndef AFE_CONFIG_API_DOMOTICZ_ENABLED
      addSelectOptionFormItem(page, L_RELAY_DEFAULT_GET_FROM_MQTT, "5",
                              configuration.state.MQTTConnected == 5);
#endif
      addSelectFormItemClose(page);
    }

    closeSection(page);
#ifdef AFE_CONFIG_HARDWARE_GATE
  }
#endif

#ifdef AFE_CONFIG_HARDWARE_GATE
  /* Below code is conditioned for the Gate functionality only. It's not
   * shown if the relay is assigned to the Gate */
  if (!isGateRelay) {
#endif

    openSection(page, F(L_RELAY_AUTOMATIC_SWITCHING_OFF), F(""));

#ifdef AFE_CONFIG_HARDWARE_GATE
  }
#endif

  dtostrf(configuration.timeToOff, 1, 1, _number);

#ifdef AFE_CONFIG_HARDWARE_GATE
  if (isGateRelay) {
    addInputFormItem(
        page, AFE_FORM_ITEM_TYPE_NUMBER, "ot", L_RELAY_IMPULSE_DURATION,
        _number, AFE_FORM_ITEM_SKIP_PROPERTY, "1", "99999", "1", L_MILISECONDS);
  } else {
    addInputFormItem(
        page, AFE_FORM_ITEM_TYPE_NUMBER, "ot", L_RELAY_SWITCH_OFF_AFTER,
        _number, AFE_FORM_ITEM_SKIP_PROPERTY, "0", "86400", "0.1", L_SECONDS);
  }
#else // Not a GATE
  addInputFormItem(page, AFE_FORM_ITEM_TYPE_NUMBER, "ot",
                   L_RELAY_SWITCH_OFF_AFTER, _number,
                   AFE_FORM_ITEM_SKIP_PROPERTY, "0", "86400", "0.1", L_SECONDS);

#endif

  closeSection(page);

#ifdef AFE_CONFIG_HARDWARE_GATE
  /* Excluded code below for Gate functionality and the relay assigned to
   * the gate */
  if (!isGateRelay) {
#endif

#ifdef AFE_CONFIG_HARDWARE_LED
    openSection(page, F(L_RELAY_SELECT_LED_4_RELAY), F(""));
    addLEDSelectionItem(page, configuration.ledID);
    closeSection(page);
#endif

#ifdef AFE_CONFIG_HARDWARE_GATE
    /* LED Exclusion for a relay assigned to ta gate */
  }
#endif

#ifdef AFE_CONFIG_HARDWARE_GATE
  /* Excluded code below for Gate functionality and the relay assigned to
   * the gate */
  if (!isGateRelay) {
#endif

#ifdef AFE_CONFIG_API_DOMOTICZ_ENABLED
    if (Device->configuration.api.domoticz || Device->configuration.api.mqtt) {
      openSection(page, F("Domoticz"), F(L_DOMOTICZ_NO_IF_IDX_0));
      char _idx[7];
      sprintf(_idx, "%d", configuration.domoticz.idx);
      addInputFormItem(page, AFE_FORM_ITEM_TYPE_NUMBER, "x", "IDX", _idx,
                       AFE_FORM_ITEM_SKIP_PROPERTY,
                       AFE_DOMOTICZ_IDX_MIN_FORM_DEFAULT,
                       AFE_DOMOTICZ_IDX_MAX_FORM_DEFAULT, "1");
      closeSection(page);
    }
#else
  if (Device->configuration.api.mqtt) {
    openSection(page, F(L_RELAY_MQTT_TOPIC), F(L_MQTT_TOPIC_EMPTY));
    addInputFormItem(page, AFE_FORM_ITEM_TYPE_TEXT, "t", L_MQTT_TOPIC,
                     configuration.mqtt.topic, "64");
    closeSection(page);
  }
#endif

#ifdef AFE_CONFIG_HARDWARE_GATE
  }
#endif
}
#endif // AFE_CONFIG_HARDWARE_RELAY

#ifdef AFE_CONFIG_FUNCTIONALITY_REGULATOR

void AFESitesGenerator::siteRegulator(String &page, uint8_t id) {
  REGULATOR configuration;
  RELAY relayConfiguration;
  char text[30];
  char value[4];
  Data->getConfiguration(id, &configuration);

  openSection(page, F(L_REGULATOR), F(""));

  /* Item: name */
  addInputFormItem(page, AFE_FORM_ITEM_TYPE_TEXT, "n", L_NAME,
                   configuration.name, "16");

  /* Item: relay */
  addSelectFormItemOpen(page, F("r"), F(L_RELAY));
  sprintf(value, "%d", AFE_HARDWARE_ITEM_NOT_EXIST);
  addSelectOptionFormItem(
      page, L_NONE, value,
      configuration.relayId == AFE_HARDWARE_ITEM_NOT_EXIST ? true : false);

  for (uint8_t i = 0; i < Device->configuration.noOfRelays; i++) {
    Data->getConfiguration(i, &relayConfiguration);
    sprintf(text, "%d: %s", i + 1, relayConfiguration.name);
    sprintf(value, "%d", i);
    addSelectOptionFormItem(page, text, value,
                            configuration.relayId == i ? true : false);
  }
  addSelectFormItemClose(page);

  /* Item: sensor */
  addSelectFormItemOpen(page, F("s"), F(L_SENSOR));
  sprintf(value, "%d", AFE_HARDWARE_ITEM_NOT_EXIST);
  addSelectOptionFormItem(
      page, L_NONE, value,
      configuration.sensorId == AFE_HARDWARE_ITEM_NOT_EXIST ? true : false);

#ifdef AFE_CONFIG_HARDWARE_DS18B20
  DS18B20 ds18b20Configuration;
  for (uint8_t i = 0; i < Device->configuration.noOfDS18B20s; i++) {
    Data->getConfiguration(i, &ds18b20Configuration);
    sprintf(text, "DS18B20 %d: %s", i + 1, ds18b20Configuration.name);
    sprintf(value, "%d", i);
    addSelectOptionFormItem(page, text, value,
                            configuration.sensorId == i ? true : false);
  }
#endif // AFE_CONFIG_HARDWARE_DS18B20

#ifdef AFE_CONFIG_HARDWARE_DHT
  DHT DHTConfiguration;
  for (uint8_t i = 0; i < Device->configuration.noOfDHTs; i++) {
    Data->getConfiguration(i, &DHTConfiguration);
    sprintf(text, "DHT %d: %s", i + 1, DHTConfiguration.name);
    sprintf(value, "%d", i);
    addSelectOptionFormItem(page, text, value,
                            configuration.sensorId == i ? true : false);
  }
  addSelectFormItemClose(page);

  /* Item: controlling parameter */
  addSelectFormItemOpen(page, F("cp"), F(L_DHT_CONTROLLING_PARAMETER));
  sprintf(value, "%d", AFE_HARDWARE_ITEM_NOT_EXIST);
  addSelectOptionFormItem(page, L_NONE, value,
                          configuration.controllingParameter ==
                                  AFE_HARDWARE_ITEM_NOT_EXIST
                              ? true
                              : false);

  sprintf(value, "%d", AFE_FUNCTIONALITY_REGULATOR_CP_TEMPERATURE);
  addSelectOptionFormItem(page, L_REGULATOR_CP_TEMPERATURE, value,
                          configuration.controllingParameter ==
                                  AFE_FUNCTIONALITY_REGULATOR_CP_TEMPERATURE
                              ? true
                              : false);

  sprintf(value, "%d", AFE_FUNCTIONALITY_REGULATOR_CP_HEAT_INDEX);
  addSelectOptionFormItem(page, L_REGULATOR_CP_HEAT_INDEX, value,
                          configuration.controllingParameter ==
                                  AFE_FUNCTIONALITY_REGULATOR_CP_HEAT_INDEX
                              ? true
                              : false);

  sprintf(value, "%d", AFE_FUNCTIONALITY_REGULATOR_CP_HUMIDITY);
  addSelectOptionFormItem(page, L_REGULATOR_CP_HUMIDITY, value,
                          configuration.controllingParameter ==
                                  AFE_FUNCTIONALITY_REGULATOR_CP_HUMIDITY
                              ? true
                              : false);

  sprintf(value, "%d", AFE_FUNCTIONALITY_REGULATOR_CP_ABSOLOUTE_HUMIDITY);
  addSelectOptionFormItem(
      page, L_REGULATOR_CP_ABSOLOUTE_HUMIDITY, value,
      configuration.controllingParameter ==
              AFE_FUNCTIONALITY_REGULATOR_CP_ABSOLOUTE_HUMIDITY
          ? true
          : false);

  sprintf(value, "%d", AFE_FUNCTIONALITY_REGULATOR_CP_DEW_POINT);
  addSelectOptionFormItem(page, L_REGULATOR_CP_DEW_POINT, value,
                          configuration.controllingParameter ==
                                  AFE_FUNCTIONALITY_REGULATOR_CP_DEW_POINT
                              ? true
                              : false);

#endif // AFE_CONFIG_HARDWARE_DHT

  addSelectFormItemClose(page);

  /* Item: regulator enabled? */
  addCheckboxFormItem(page, "e", L_REGULATOR_ENABLED, "1",
                      configuration.enabled);

  closeSection(page);

  addRegulatorControllerItem(page, &configuration);

#ifdef AFE_CONFIG_API_DOMOTICZ_ENABLED
  if (Device->configuration.api.domoticz || Device->configuration.api.mqtt) {
    openSection(page, F("Domoticz"), F(L_DOMOTICZ_NO_IF_IDX_0));
    char _idx[7];
    sprintf(_idx, "%d", configuration.domoticz.idx);
    addInputFormItem(page, AFE_FORM_ITEM_TYPE_NUMBER, "x", "IDX", _idx,
                     AFE_FORM_ITEM_SKIP_PROPERTY,
                     AFE_DOMOTICZ_IDX_MIN_FORM_DEFAULT,
                     AFE_DOMOTICZ_IDX_MAX_FORM_DEFAULT, "1");
    closeSection(page);
  }
#else
  if (Device->configuration.api.mqtt) {
    openSection(page, F(L_REGULATOR_MQTT_TOPIC), F(L_MQTT_TOPIC_EMPTY));
    addInputFormItem(page, AFE_FORM_ITEM_TYPE_TEXT, "t", L_MQTT_TOPIC,
                     configuration.mqtt.topic, "64");
    closeSection(page);
  }
#endif
}
#endif // AFE_CONFIG_FUNCTIONALITY_REGULATOR

#ifdef AFE_CONFIG_FUNCTIONALITY_THERMAL_PROTECTOR
void AFESitesGenerator::siteThermalProtector(String &page, uint8_t id) {
  THERMAL_PROTECTOR configuration;
  RELAY relayConfiguration;
  char text[30];
  char value[4];
  Data->getConfiguration(id, &configuration);

  openSection(page, F(L_THERMAL_PROTECTORS),
              F(L_THERMAL_PROTECTOR_AUTOMATIC_SWITCHING_OFF));

  addInputFormItem(page, AFE_FORM_ITEM_TYPE_TEXT, "n", L_NAME,
                   configuration.name, "16");

  addSelectFormItemOpen(page, F("r"), F(L_RELAY));
  sprintf(value, "%d", AFE_HARDWARE_ITEM_NOT_EXIST);
  addSelectOptionFormItem(
      page, L_NONE, value,
      configuration.relayId == AFE_HARDWARE_ITEM_NOT_EXIST ? true : false);

  for (uint8_t i = 0; i < Device->configuration.noOfRelays; i++) {
    Data->getConfiguration(i, &relayConfiguration);
    sprintf(text, "%d: %s", i + 1, relayConfiguration.name);
    sprintf(value, "%d", i);
    addSelectOptionFormItem(page, text, value,
                            configuration.relayId == i ? true : false);
  }
  addSelectFormItemClose(page);

  addSelectFormItemOpen(page, F("s"), F(L_SENSOR));
  sprintf(value, "%d", AFE_HARDWARE_ITEM_NOT_EXIST);
  addSelectOptionFormItem(
      page, L_NONE, value,
      configuration.sensorId == AFE_HARDWARE_ITEM_NOT_EXIST ? true : false);

#ifdef AFE_CONFIG_HARDWARE_DS18B20
  DS18B20 ds18b20Configuration;
  for (uint8_t i = 0; i < Device->configuration.noOfDS18B20s; i++) {
    Data->getConfiguration(i, &ds18b20Configuration);
    sprintf(text, "DS18B20 %d: %s", i + 1, ds18b20Configuration.name);
    sprintf(value, "%d", i);
    addSelectOptionFormItem(page, text, value,
                            configuration.sensorId == i ? true : false);
  }
#endif

#ifdef AFE_CONFIG_HARDWARE_DHT
  DHT DHTConfiguration;
  for (uint8_t i = 0; i < Device->configuration.noOfDHTs; i++) {
    Data->getConfiguration(i, &DHTConfiguration);
    sprintf(text, "DHT %d: %s", i + 1, DHTConfiguration.name);
    sprintf(value, "%d", i);
    addSelectOptionFormItem(page, text, value,
                            configuration.sensorId == i ? true : false);
  }
#endif

  addSelectFormItemClose(page);

  addCheckboxFormItem(page, "e", L_THERMAL_PROTECTOR_ENABLED, "1",
                      configuration.enabled);
  sprintf(text, "%-.3f", configuration.temperature);
  addInputFormItem(page, AFE_FORM_ITEM_TYPE_NUMBER, "m",
                   L_RELAY_SWITCH_OFF_ABOVE, text, AFE_FORM_ITEM_SKIP_PROPERTY,
                   "-999", "999", "any");

  closeSection(page);

#ifdef AFE_CONFIG_API_DOMOTICZ_ENABLED
  if (Device->configuration.api.domoticz || Device->configuration.api.mqtt) {
    openSection(page, F("Domoticz"), F(L_DOMOTICZ_NO_IF_IDX_0));
    char _idx[7];
    sprintf(_idx, "%d", configuration.domoticz.idx);
    addInputFormItem(page, AFE_FORM_ITEM_TYPE_NUMBER, "x", "IDX", _idx,
                     AFE_FORM_ITEM_SKIP_PROPERTY,
                     AFE_DOMOTICZ_IDX_MIN_FORM_DEFAULT,
                     AFE_DOMOTICZ_IDX_MAX_FORM_DEFAULT, "1");
    closeSection(page);
  }
#else
  if (Device->configuration.api.mqtt) {
    openSection(page, F(L_THERMAL_PROTECTOR_MQTT_TOPIC), F(L_MQTT_TOPIC_EMPTY));
    addInputFormItem(page, AFE_FORM_ITEM_TYPE_TEXT, "t", L_MQTT_TOPIC,
                     configuration.mqtt.topic, "64");
    closeSection(page);
  }
#endif
}
#endif // AFE_CONFIG_FUNCTIONALITY_THERMAL_PROTECTOR

#ifdef AFE_CONFIG_HARDWARE_SWITCH
void AFESitesGenerator::siteSwitch(String &page, uint8_t id) {
  SWITCH configuration;
  Data->getConfiguration(id, &configuration);
  char text[25];

#ifdef AFE_CONFIG_HARDWARE_GATE
  GATE gateConfiguration;
#endif

  sprintf(text, "%s #%d", L_SWITCH_BUTTON, id + 1);

  openSection(page, text, F(""));

#ifndef AFE_CONFIG_HARDWARE_MCP23017
  addListOfGPIOs(page, F("g"), configuration.gpio);
#endif

  addSelectFormItemOpen(page, F("f"), F(L_SWITCH_FUNCTIONALITY));
  addSelectOptionFormItem(page, L_NONE, "0", configuration.functionality ==
                                                 AFE_SWITCH_FUNCTIONALITY_NONE);
  addSelectOptionFormItem(page, L_SWITCH_SYSTEM_BUTTON, "1",
                          configuration.functionality ==
                              AFE_SWITCH_FUNCTIONALITY_MULTI);

#if defined(AFE_CONFIG_HARDWARE_RELAY) || defined(AFE_CONFIG_HARDWARE_GATE)

  addSelectOptionFormItem(page, L_SWITCH_CONTROL_RELAY, "2",
                          configuration.functionality ==
                              AFE_SWITCH_FUNCTIONALITY_RELAY);
#endif // defined(AFE_CONFIG_HARDWARE_RELAY) ||
       // defined(AFE_CONFIG_HARDWARE_GATE)

  addSelectFormItemClose(page);

#if defined(AFE_CONFIG_HARDWARE_RELAY) || defined(AFE_CONFIG_HARDWARE_GATE)

  addSelectFormItemOpen(page, F("r"), F(L_SWITCH_RELAY_CONTROLLED));
  addSelectOptionFormItem(page, L_NONE, "255",
                          configuration.relayID == AFE_HARDWARE_ITEM_NOT_EXIST);

#ifdef AFE_CONFIG_HARDWARE_GATE
  uint8_t relayIsForGate;
#endif
  RELAY relayConfiguration;
  for (uint8_t i = 0; i < Device->configuration.noOfRelays; i++) {
    page += "<option value=\"";
    page += i;
    page += "\"";
    page += configuration.relayID == i ? " selected=\"selected\"" : "";
    page += ">";
#ifdef AFE_CONFIG_HARDWARE_GATE
    relayIsForGate = false;
    for (uint8_t j = 0; j < Device->configuration.noOfGates; j++) {
      Data->getConfiguration(j, &gateConfiguration);
      if (i == gateConfiguration.relayId) {
        page += F(L_GATE);
        page += ": ";
        page += gateConfiguration.name;
        relayIsForGate = true;
        break;
      }
    }
    if (!relayIsForGate) {
      Data->getConfiguration(i, &relayConfiguration);
      sprintf(text, "%d: %s", i + 1, relayConfiguration.name);
      page.concat(text);
    }
#else
    Data->getConfiguration(i, &relayConfiguration);
    sprintf(text, "%d: %s", i + 1, relayConfiguration.name);
    page.concat(text);
#endif
    page += "</option>";
  }
  addSelectFormItemClose(page);

#endif // // defined(AFE_CONFIG_HARDWARE_RELAY) ||
       // defined(AFE_CONFIG_HARDWARE_GATE)

  addSelectFormItemOpen(page, F("m"), F(L_SWITCH_TYPE));
  addSelectOptionFormItem(page, L_SWITCH_MONOSTABLE, "0",
                          configuration.type == 0);
  addSelectOptionFormItem(page, L_SWITCH_BISTABLE, "1",
                          configuration.type == 1);
  addSelectFormItemClose(page);

  page.concat(FPSTR(HTTP_INFO_TEXT));
  page.replace("{{i.v}}", F(L_SWITCH_SENSITIVENESS_HINT));

  char _number[4];
  sprintf(_number, "%d", configuration.sensitiveness);

  addInputFormItem(page, AFE_FORM_ITEM_TYPE_NUMBER, "s", L_SENSITIVENESS,
                   _number, AFE_FORM_ITEM_SKIP_PROPERTY, "0", "999", "1",
                   L_MILISECONDS);
  closeSection(page);

#ifdef AFE_CONFIG_HARDWARE_MCP23017
  openSection(page, F(L_MCP23017_CONNECTION), F(L_MCP23017_SWITCH_CONNECTION));
  addListOfGPIOs(page, F("g"), configuration.gpio);
  page.concat(FPSTR(HTTP_INFO_TEXT));
  page.replace("{{i.v}}", F(L_MCP23017_CONNECTION_VIA_MCP));
  addDeviceI2CAddressSelectionItem(page, configuration.mcp23017.address);
  addListOfMCP23017GPIOs(page, "mg", configuration.mcp23017.gpio);

  closeSection(page);
#endif // AFE_CONFIG_HARDWARE_MCP23017

#ifdef AFE_CONFIG_API_DOMOTICZ_ENABLED
  if (Device->configuration.api.domoticz || Device->configuration.api.mqtt) {
    openSection(page, F("Domoticz"), F(L_DOMOTICZ_NO_IF_IDX_0));
    char _idx[7];
    sprintf(_idx, "%d", configuration.domoticz.idx);
    addInputFormItem(page, AFE_FORM_ITEM_TYPE_NUMBER, "x", "IDX", _idx,
                     AFE_FORM_ITEM_SKIP_PROPERTY,
                     AFE_DOMOTICZ_IDX_MIN_FORM_DEFAULT,
                     AFE_DOMOTICZ_IDX_MAX_FORM_DEFAULT, "1");
    closeSection(page);
  }
#else
  if (Device->configuration.api.mqtt) {
    openSection(page, F(L_SWITCH_MQTT_TOPIC), F(L_MQTT_TOPIC_EMPTY));
    addInputFormItem(page, AFE_FORM_ITEM_TYPE_TEXT, "t", L_MQTT_TOPIC,
                     configuration.mqtt.topic, "64");
    closeSection(page);
  }
#endif
}
#endif // AFE_CONFIG_HARDWARE_SWITCH

#ifdef AFE_CONFIG_HARDWARE_DS18B20
void AFESitesGenerator::siteDS18B20Sensor(String &page, uint8_t id) {

  AFESensorDS18B20 _Sensor;
  DS18B20Addresses _addresses;
  uint8_t numberOfFoundSensors;
  DS18B20 configuration;
  Data->getConfiguration(id, &configuration);
  char _number[13];

  openSection(page, F(L_DS18B20_SENSOR), F(""));

  /* Item: GPIO */
  addListOfGPIOs(page, F("g"), configuration.gpio, "GPIO");

  /* Item: Sensor address */

  numberOfFoundSensors = _Sensor.scan(configuration.gpio, _addresses);

  if (numberOfFoundSensors > 0) {
    addSelectFormItemOpen(page, F("a"), F(L_ADDRESS));
    char _scannedAddressText[17];
    char _configAddressText[17];
    for (uint8_t i = 0; i < numberOfFoundSensors; i++) {
      _Sensor.addressToChar(_addresses[i], _scannedAddressText);
      _Sensor.addressToChar(configuration.address, _configAddressText);
      addSelectOptionFormItem(page, _scannedAddressText, _scannedAddressText,
                              memcmp(_addresses[i], configuration.address,
                                     sizeof(_addresses[i])) == 0);
    }
    page.concat(F("</select><input type=\"submit\" class =\"b bc\" "
            "value=\"" L_DS18B20_SEARCH "\"></div>"));
  } else {
    page.concat(F("<p class=\"cm\">" L_DS18B20_NO_SENSOR "</p>"));
    page.concat(F("<input type=\"submit\" class =\"b bc\" value=\"" L_DS18B20_SEARCH
            "\"><br /><br />"));
  }

  /* Item: Name */
  addInputFormItem(page, AFE_FORM_ITEM_TYPE_TEXT, "n", L_NAME,
                   configuration.name, "16");

  /* Item: Interval */
  sprintf(_number, "%d", configuration.interval);
  addInputFormItem(page, AFE_FORM_ITEM_TYPE_NUMBER, "f", L_MEASURMENTS_INTERVAL,
                   _number, AFE_FORM_ITEM_SKIP_PROPERTY, "2", "86400", "1",
                   L_SECONDS);

  /* Item: Send only changes */
  addCheckboxFormItem(page, "s", L_DS18B20_SENT_ONLY_CHANGES, "1",
                      configuration.sendOnlyChanges);

  /* Item: Correction */
  sprintf(_number, "%-.3f", configuration.correction);
  addInputFormItem(page, AFE_FORM_ITEM_TYPE_NUMBER, "k",
                   L_DS18B20_TEMPERATURE_CORRECTION, _number,
                   AFE_FORM_ITEM_SKIP_PROPERTY, "-99.999", "99.999", "0.001");

  /* Item: Unit */
  addSelectFormItemOpen(page, F("u"), F(L_UNITS));
  addSelectOptionFormItem(page, "C", "1",
                          configuration.unit == AFE_TEMPERATURE_UNIT_CELSIUS);
  addSelectOptionFormItem(page, "F", "2", configuration.unit ==
                                              AFE_TEMPERATURE_UNIT_FAHRENHEIT);
  addSelectFormItemClose(page);

  /* Item: Resolution */
  addSelectFormItemOpen(page, F("r"), F(L_DS18B20_RESOLUTION));
  addSelectOptionFormItem(page, L_DS18B20_RESOLUTION_9B, "9",
                          configuration.resolution == 9);
  addSelectOptionFormItem(page, L_DS18B20_RESOLUTION_10B, "10",
                          configuration.resolution == 10);
  addSelectOptionFormItem(page, L_DS18B20_RESOLUTION_11B, "11",
                          configuration.resolution == 11);
  addSelectOptionFormItem(page, L_DS18B20_RESOLUTION_12B, "12",
                          configuration.resolution == 12);

  addSelectFormItemClose(page);

  closeSection(page);

#ifdef AFE_CONFIG_API_DOMOTICZ_ENABLED
  if (Device->configuration.api.domoticz || Device->configuration.api.mqtt) {
    openSection(page, F("Domoticz"), F(L_DOMOTICZ_NO_IF_IDX_0));
    sprintf(_number, "%d", configuration.domoticz.idx);
    addInputFormItem(page, AFE_FORM_ITEM_TYPE_NUMBER, "x", "IDX", _number,
                     AFE_FORM_ITEM_SKIP_PROPERTY,
                     AFE_DOMOTICZ_IDX_MIN_FORM_DEFAULT,
                     AFE_DOMOTICZ_IDX_MAX_FORM_DEFAULT, "1");

    closeSection(page);
  }
#else
  if (Device->configuration.api.mqtt) {
    openSection(page, F(L_DS18B20_MQTT_TOPIC), F(L_MQTT_TOPIC_EMPTY));
    addInputFormItem(page, AFE_FORM_ITEM_TYPE_TEXT, "t", L_MQTT_TOPIC,
                     configuration.mqtt.topic, "64");

    closeSection(page);
  }
#endif
}
#endif // AFE_CONFIG_HARDWARE_DS18B20

#ifdef AFE_CONFIG_HARDWARE_DHT
void AFESitesGenerator::siteDHTSensor(String &page, uint8_t id) {

  AFESensorDHT _Sensor;
  DHT configuration;
  Data->getConfiguration(id, &configuration);
  char _number[13];
  char _text[13];

  sprintf(_text, "DHT: #%d", id + 1);
  openSection(page, _text, F(""));

  /* Item: GPIO */
  addListOfGPIOs(page, F("g"), configuration.gpio, "GPIO");

  /* Item: Name */
  addInputFormItem(page, AFE_FORM_ITEM_TYPE_TEXT, "n", L_NAME,
                   configuration.name, "16");

  /* Item: type of the sensor */
  addSelectFormItemOpen(page, F("t"), F(L_DHT_SENSOR_TYPE));
  addSelectOptionFormItem(page, L_NONE, "255",
                          configuration.type == AFE_HARDWARE_ITEM_NOT_EXIST);
  addSelectOptionFormItem(page, L_DHT_AUTO_DETECT, "0",
                          configuration.type ==
                              AFE_CONFIG_HARDWARE_DHT_TYPE_AUTO);
  addSelectOptionFormItem(page, "DHT11", "1",
                          configuration.type ==
                              AFE_CONFIG_HARDWARE_DHT_TYPE_DHT11);
  addSelectOptionFormItem(page, "DHT22", "2",
                          configuration.type ==
                              AFE_CONFIG_HARDWARE_DHT_TYPE_DHT22);
  addSelectOptionFormItem(page, "AM2302", "3",
                          configuration.type ==
                              AFE_CONFIG_HARDWARE_DHT_TYPE_AM2302);
  addSelectOptionFormItem(page, "RHT03", "4",
                          configuration.type ==
                              AFE_CONFIG_HARDWARE_DHT_TYPE_RHT03);
  addSelectFormItemClose(page);

  /* Item: Interval */
  sprintf(_number, "%d", configuration.interval);
  addInputFormItem(page, AFE_FORM_ITEM_TYPE_NUMBER, "f", L_MEASURMENTS_INTERVAL,
                   _number, AFE_FORM_ITEM_SKIP_PROPERTY, "2", "86400", "1",
                   L_SECONDS);

  /* Item: Send only changes */

  addCheckboxFormItem(page, "s", L_DHT_SENT_ONLY_CHANGES, "1",
                      configuration.sendOnlyChanges);

  closeSection(page);

  /* Item: Unit */
  openSection(page, F(L_UNITS), F(""));
  addSelectFormItemOpen(page, F("tu"), F(L_TEMPERATURE));
  addSelectOptionFormItem(page, "C", "1", configuration.temperature.unit ==
                                              AFE_TEMPERATURE_UNIT_CELSIUS);
  addSelectOptionFormItem(page, "F", "2", configuration.temperature.unit ==
                                              AFE_TEMPERATURE_UNIT_FAHRENHEIT);
  addSelectFormItemClose(page);
  closeSection(page);

  /* Item: Corrections of sensor values */
  openSection(page, F(L_CORRECTIONS), F(""));
  sprintf(_number, "%-.3f", configuration.temperature.correction);
  addInputFormItem(page, AFE_FORM_ITEM_TYPE_NUMBER, "tc", L_TEMPERATURE,
                   _number, AFE_FORM_ITEM_SKIP_PROPERTY, "-99.999", "99.999",
                   "0.001");

  /* Item: humidity correction */
  sprintf(_number, "%-.3f", configuration.humidity.correction);
  addInputFormItem(page, AFE_FORM_ITEM_TYPE_NUMBER, "hc", L_HUMIDITY, _number,
                   AFE_FORM_ITEM_SKIP_PROPERTY, "-99.999", "99.999", "0.001");

  closeSection(page);

#ifdef AFE_CONFIG_API_DOMOTICZ_ENABLED
  if (Device->configuration.api.domoticz || Device->configuration.api.mqtt) {
    openSection(page, F("Domoticz"), F(L_DOMOTICZ_NO_IF_IDX_0));

    sprintf(_number, "%d", configuration.domoticz.temperature.idx);
    addInputFormItem(page, AFE_FORM_ITEM_TYPE_NUMBER, "i1", L_TEMPERATURE_IDX,
                     _number, AFE_FORM_ITEM_SKIP_PROPERTY,
                     AFE_DOMOTICZ_IDX_MIN_FORM_DEFAULT,
                     AFE_DOMOTICZ_IDX_MAX_FORM_DEFAULT, "1");

    sprintf(_number, "%d", configuration.domoticz.humidity.idx);
    addInputFormItem(page, AFE_FORM_ITEM_TYPE_NUMBER, "i2", L_HUMIDITY_IDX,
                     _number, AFE_FORM_ITEM_SKIP_PROPERTY,
                     AFE_DOMOTICZ_IDX_MIN_FORM_DEFAULT,
                     AFE_DOMOTICZ_IDX_MAX_FORM_DEFAULT, "1");

    sprintf(_number, "%d", configuration.domoticz.temperatureHumidity.idx);
    addInputFormItem(
        page, AFE_FORM_ITEM_TYPE_NUMBER, "i5", L_HUMIDITY_IDX_TEMP_HUM, _number,
        AFE_FORM_ITEM_SKIP_PROPERTY, AFE_DOMOTICZ_IDX_MIN_FORM_DEFAULT,
        AFE_DOMOTICZ_IDX_MAX_FORM_DEFAULT, "1");

    sprintf(_number, "%d", configuration.domoticz.absoluteHumidity.idx);
    addInputFormItem(
        page, AFE_FORM_ITEM_TYPE_NUMBER, "i8", L_HUMIDITY_IDX_ABSOLUTE, _number,
        AFE_FORM_ITEM_SKIP_PROPERTY, AFE_DOMOTICZ_IDX_MIN_FORM_DEFAULT,
        AFE_DOMOTICZ_IDX_MAX_FORM_DEFAULT, "1");

    sprintf(_number, "%d", configuration.domoticz.dewPoint.idx);
    addInputFormItem(
        page, AFE_FORM_ITEM_TYPE_NUMBER, "i3", L_HUMIDITY_IDX_DEW_POINT,
        _number, AFE_FORM_ITEM_SKIP_PROPERTY, AFE_DOMOTICZ_IDX_MIN_FORM_DEFAULT,
        AFE_DOMOTICZ_IDX_MAX_FORM_DEFAULT, "1");

    sprintf(_number, "%d", configuration.domoticz.heatIndex.idx);
    addInputFormItem(
        page, AFE_FORM_ITEM_TYPE_NUMBER, "i4", L_HUMIDITY_IDX_HEAT_INDEX,
        _number, AFE_FORM_ITEM_SKIP_PROPERTY, AFE_DOMOTICZ_IDX_MIN_FORM_DEFAULT,
        AFE_DOMOTICZ_IDX_MAX_FORM_DEFAULT, "1");

    sprintf(_number, "%d", configuration.domoticz.perception.idx);
    addInputFormItem(page, AFE_FORM_ITEM_TYPE_NUMBER, "i6", L_PERCEPTION_IDX,
                     _number, AFE_FORM_ITEM_SKIP_PROPERTY,
                     AFE_DOMOTICZ_IDX_MIN_FORM_DEFAULT,
                     AFE_DOMOTICZ_IDX_MAX_FORM_DEFAULT, "1");

    sprintf(_number, "%d", configuration.domoticz.comfort.idx);
    addInputFormItem(page, AFE_FORM_ITEM_TYPE_NUMBER, "i7", L_COMFORT_IDX,
                     _number, AFE_FORM_ITEM_SKIP_PROPERTY,
                     AFE_DOMOTICZ_IDX_MIN_FORM_DEFAULT,
                     AFE_DOMOTICZ_IDX_MAX_FORM_DEFAULT, "1");

    closeSection(page);
  }
#else
  if (Device->configuration.api.mqtt) {
    openSection(page, F(L_DHT_MQTT_TOPIC), F(L_MQTT_TOPIC_EMPTY));
    addInputFormItem(page, AFE_FORM_ITEM_TYPE_TEXT, "m", L_MQTT_TOPIC,
                     configuration.mqtt.topic, "64");

    closeSection(page);
  }
#endif
}
#endif // AFE_CONFIG_HARDWARE_DHT

#if defined(T3_CONFIG)
String AFESitesGenerator::sitePIR(uint8_t id) {
  PIR configuration = Data->getPIRConfiguration(id);
  DEVICE device = Data->getConfiguration();

  String body = "<fieldset>";
  char field[13];
  sprintf(field, "g%d", id);
  body += "<div class=\"cf\">";
  body += addListOfGPIOs(field, configuration.gpio);
  body += "</div>";

  body += "<div class=\"cf\">";
  body += "<label>";
  body += language == 0 ? "Nazwa" : "Name";
  body += "</label>";
  body += "<input name=\"n" + String(id) +
          "\" type=\"text\" maxlength=\"16\" value=\"";
  body += configuration.name;
  body += "\">";
  body += "<span class=\"hint\">Max 16 ";
  body += language == 0 ? "znaków" : "chars";
  body += "</span>";
  body += "</div>";

  body += "<div class=\"cf\"><label>";
  body += language == 0 ? "Typ" : "Type";
  body += "</label><select name=\"o" + String(id) + "\"><option value=\"0\"";
  body += (configuration.type == 0 ? " selected=\"selected\"" : "");
  body += ">";
  body += language == 0 ? "NO" : "NO";
  body += "</option><option value=\"1\"";
  body += (configuration.type == 1 ? " selected=\"selected\"" : "");
  body += ">";
  body += language == 0 ? "NC" : "NC";
  body += "</option></select></div>";

  body += "<div class=\"cf\">";
  body += "<label>";
  body += language == 0 ? "LED" : "LED";
  body += "</label>";
  body += "<select name=\"l" + String(id) + "\">";
  body += "<option value=\"9\" ";
  body += (configuration.ledId == 9 ? "selected=\"selected\"" : "");
  body += ">Brak</option>";
  for (uint8_t i = 0; i < 5; i++) {
    if (Device->configuration.isLED[i]) {
      body += "<option value=\"";
      body += i;
      body += "\" ";
      body += (configuration.ledId == i ? "selected=\"selected\"" : "");
      body += ">";
      body += i + 1;
      body += "</option>";
    }
  }
  body += "</select>";
  body += "<span class=\"hint\">";
  body += language == 0 ? "Dioda LED sygnalizuje wykryty ruch przez czujnik"
                        : "LED indicates motion detected by the sensor";
  body += "</span>";
  body += "</div>";
  body += "<br><p class=\"cm\">";
  body += language == 0
              ? "Czujnik PIR może bezpośrednio sterować jednym przekaźnikiem. "
                "Poniżej możesz wybrać którym oraz ustawić dodatkowe "
                "parametry sterowania"
              : "Motion detection sensor can control a relay assigned to it. "
                "Below you can choose a one and set additional parameters";
  body += "</p>";

  body += "<div class=\"cf\">";
  body += "<label>";
  body += language == 0 ? "Wybierz przekaźnik" : "Select relay";
  body += "</label>";
  body += "<select name=\"r" + String(id) + "\">";
  body += "<option value=\"9\" ";
  body += (configuration.relayId == 9 ? "selected=\"selected\"" : "");
  body += ">Brak</option>";
  for (uint8_t i = 0; i < 4; i++) {
    if (Device->configuration.isRelay[i]) {
      body += "<option value=\"";
      body += i;
      body += "\" ";
      body += (configuration.relayId == i ? "selected=\"selected\"" : "");
      body += ">";
      body += i + 1;
      body += "</option>";
    }
  }
  body += "</select>";
  body += "</div>";

  body += "<div class=\"cf\"><label>";
  body += language == 0 ? "Czas uruchomienia" : "How long active";
  body += "</label>";
  body += "<input name=\"d" + String(id) +
          "\" type=\"number\" max=\"86400\" min=\"0.01\" step=\"0.01\" "
          "value=\"";
  body += configuration.howLongKeepRelayOn;
  body += "\">";
  body += "<span class=\"hint\">0.01 - 86400 (24h) se";
  body += language == 0 ? "kund" : "conds";
  body += "</span>";
  body += "</div>";

  body += "<div class=\"cc\">";
  body += "<label>";
  body += "<input name=\"i";
  body += id;
  body += "\" type=\"checkbox\" value=\"1\"";
  body += configuration.invertRelayState ? " checked=\"checked\"" : "";
  body += ">";
  body += language == 0 ? "Wyłącz przekaźnik, gdy PIR wykryje ruch"
                        : "Switch off relay if PIR detects move";

  body += "</label></div></fieldset>";
  char title[25];
  language == 0 ? sprintf(title, "Czujnik ruchu (PIR) #%d", id + 1)
                : sprintf(title, "Motion detection sesnor (PIR) #%d", id + 1);

  String page = openSection(title, "", body);

  if (Device->configuration.api.domoticz) {
    body = "<fieldset><div class=\"cf\"><label>IDX</label><input name=\"x";
    body += id;
    body += "\" type=\"number\" step=\"1\" min=\"0\" max=\"999999\"  value=\"";
    body += configuration.idx;
    body += "\"><span class=\"hint\">";
    body += language == 0 ? "Zakres: " : "Range: ";
    body += "0 - 999999</span></div></fieldset>";
    page += openSection(
        "Domoticz",
        language == 0
            ? "Jeśli IDX jest 0 to wartośc nie będzie wysyłana do Domoticz"
            : "If IDX is set to 0 then a value won't be sent to Domoticz",
        body);
  }

  return page;
}
#endif

#ifdef AFE_CONFIG_HARDWARE_CONTACTRON
void AFESitesGenerator::siteContactron(String &page, uint8_t id) {
  CONTACTRON configuration;
  Data->getConfiguration(id, &configuration);
  char title[23];
  sprintf(title, "%s #%d", L_CONTACTRON, id + 1);
  openSection(page, title, F(""));

  /* Item: GPIO */
  addListOfGPIOs(page, F("g"), configuration.gpio, "GPIO");

  /* Item: name */
  addInputFormItem(page, AFE_FORM_ITEM_TYPE_TEXT, "n", L_NAME,
                   configuration.name, "16");

  /* Item: type */
  addSelectFormItemOpen(page, F("y"), F(L_SWITCH_TYPE));
  addSelectOptionFormItem(page, "NO", "0",
                          configuration.type == AFE_CONTACTRON_NO);
  addSelectOptionFormItem(page, "NC", "1",
                          configuration.type == AFE_CONTACTRON_NC);

  addSelectFormItemClose(page);

  page.concat(FPSTR(HTTP_ITEM_HINT));
  page.replace("{{i.h}}", F(L_CONTACTRON_SET_SENSITIVENESS));

  /* Item: bouncing */
  char _number[5];
  sprintf(_number, "%d", configuration.bouncing);
  addInputFormItem(page, AFE_FORM_ITEM_TYPE_NUMBER, "b", L_SENSITIVENESS,
                   _number, AFE_FORM_ITEM_SKIP_PROPERTY, "0", "2000", "1",
                   L_MILISECONDS);
  closeSection(page);

  /* Item: LED */
  openSection(page, F(L_CONTACTRON_LED_ASSIGNED_TO_SENSOR), F(""));
  addLEDSelectionItem(page, configuration.ledID);
  closeSection(page);

#ifdef AFE_CONFIG_API_DOMOTICZ_ENABLED
  if (Device->configuration.api.domoticz || Device->configuration.api.mqtt) {
    openSection(page, F("Domoticz"), F(L_DOMOTICZ_NO_IF_IDX_0));

    char _idx[7];
    sprintf(_idx, "%d", configuration.domoticz.idx);
    addInputFormItem(page, AFE_FORM_ITEM_TYPE_NUMBER, "x", "IDX", _idx,
                     AFE_FORM_ITEM_SKIP_PROPERTY,
                     AFE_DOMOTICZ_IDX_MIN_FORM_DEFAULT,
                     AFE_DOMOTICZ_IDX_MAX_FORM_DEFAULT, "1");

    closeSection(page);
  }
#else
  if (Device->configuration.api.mqtt) {
    openSection(page, F(L_CONTACTRON_MQTT_TOPIC), F(L_MQTT_TOPIC_EMPTY));
    addInputFormItem(page, AFE_FORM_ITEM_TYPE_TEXT, "t", L_MQTT_TOPIC,
                     configuration.mqtt.topic, "64");
    closeSection(page);
  }
#endif
}
#endif // AFE_CONFIG_HARDWARE_CONTACTRON

#ifdef AFE_CONFIG_HARDWARE_GATE
void AFESitesGenerator::siteGate(String &page, uint8_t id) {
  GATE gateConfiguration;
  Data->getConfiguration(id, &gateConfiguration);
  CONTACTRON contactronConfiguration[2];

  openSection(page, F(L_GATE_CONFIGURATION), F(""));

  /* Item: Gate name */
  addInputFormItem(page, AFE_FORM_ITEM_TYPE_TEXT, "n", L_NAME,
                   gateConfiguration.name, "16");

  /* Item: relay */
  char _relayName[20];
  char _relayId[4];
  RELAY relayConfiguration;
  addSelectFormItemOpen(page, F("r"), F(L_GATE_RELAY_ID_CONTROLLING_GATE));
  sprintf(_relayId, "%d", AFE_HARDWARE_ITEM_NOT_EXIST);

  addSelectOptionFormItem(page, L_NONE, _relayId,
                          gateConfiguration.relayId ==
                              AFE_HARDWARE_ITEM_NOT_EXIST);

  for (uint8_t i = 0; i < Device->configuration.noOfRelays; i++) {
    Data->getConfiguration(i, &relayConfiguration);

    if (relayConfiguration.name[0] == '\0') {
      sprintf(_relayName, "%d: %s", i + 1, L_RELAY);
    } else {
      sprintf(_relayName, "%d: %s", i + 1, relayConfiguration.name);
    }
    sprintf(_relayId, "%d", i);
    addSelectOptionFormItem(page, _relayName, _relayId,
                            gateConfiguration.relayId == i);
  }
  relayConfiguration = {0};
  addSelectFormItemClose(page);

  /* Item: contactron 1 */
  if (Device->configuration.noOfContactrons > 0) {
    addGateContactronsListItem(page, F("c1"),
                               gateConfiguration.contactron.id[0]);
  }

  /* If there is more than a one contactron connected, add option to assigne
   * it to the gate */

  if (Device->configuration.noOfContactrons > 1) {
    /* Item: contactron 2 */
    addGateContactronsListItem(page, F("c2"),
                               gateConfiguration.contactron.id[1]);
  }

  closeSection(page);

  /* Add section of Gate states configuration is there is at least one
   * contactron connected to the device and assigned to the Gate */
  if (Device->configuration.noOfContactrons > 0) {

    uint8_t numberOfContractons = 0;
    if (gateConfiguration.contactron.id[1] != AFE_HARDWARE_ITEM_NOT_EXIST) {
      numberOfContractons = 2;
    } else if (gateConfiguration.contactron.id[0] !=
               AFE_HARDWARE_ITEM_NOT_EXIST) {
      numberOfContractons = 1;
    }

#ifdef DEBUG
    Serial << endl
           << F("Number of contactros set for the gate: ")
           << numberOfContractons;
#endif

    if (numberOfContractons > 0) {

      openSection(page, F(L_GATE_STATES_CONFIGURATION), F(""));

      for (uint8_t i = 0; i < numberOfContractons; i++) {

        Data->getConfiguration(gateConfiguration.contactron.id[i],
                               &contactronConfiguration[i]);
      }

      page.concat(F("<fieldset>"));

      page.concat(F("<p class=\"cm\">"));
      page.concat(F(L_GATE_IF_MAGNETIC_SENSOR));
      page.concat(F(": <strong>"));
      page += contactronConfiguration[0].name;

      if (numberOfContractons == 2) {
        page.concat(F("</strong> "));
        page.concat(F(L_GATE_AND_SENSOR));
        page.concat(F(": <strong>"));
        page += contactronConfiguration[1].name;
        page.concat(F("</strong> "));
        page.concat(F(L_GATE_ARE_OPEN));
      } else {
        page.concat(F("</strong> "));
        page.concat(F(L_GATE_IS_OPEN));
      }
      page.concat(F(" "));
      page.concat(F(L_GATE_THEN));
      page.concat(F(":</p>"));
      addGateStatesListItem(page, F("s0"), gateConfiguration.states.state[0]);
      if (numberOfContractons == 2) {
        page.concat(F("<br><br><p class=\"cm\">"));
        page.concat(F(L_GATE_IF_MAGNETIC_SENSOR));
        page.concat(F(": <strong>"));
        page += contactronConfiguration[0].name;
        page.concat(F("</strong> "));
        page.concat(F(L_GATE_IS_OPEN));
        page.concat(F(" "));
        page.concat(F(L_GATE_AND_SENSOR));
        page.concat(F(": <strong>"));
        page += contactronConfiguration[1].name;
        page.concat(F("</strong> "));
        page.concat(F(L_GATE_IS_CLOSED));
        page.concat(F(" "));
        page.concat(F(L_GATE_THEN));
        page.concat(F(":"));
        page.concat(F("</p>"));
        addGateStatesListItem(page, F("s1"), gateConfiguration.states.state[1]);

        page.concat(F("<br><br><p class=\"cm\">"));
        page.concat(F(L_GATE_IF_MAGNETIC_SENSOR));
        page.concat(F(": <strong>"));
        page += contactronConfiguration[0].name;
        page.concat(F("</strong> "));
        page.concat(F(L_GATE_IS_CLOSED));
        page.concat(F(" "));
        page.concat(F(L_GATE_AND_SENSOR));
        page.concat(F(": <strong>"));
        page += contactronConfiguration[1].name;
        page.concat(F("</strong> "));
        page.concat(F(L_GATE_IS_OPEN));
        page.concat(F(" "));
        page.concat(F(L_GATE_THEN));
        page.concat(F(":"));
        page.concat(F("</p>"));

        addGateStatesListItem(page, F("s2"), gateConfiguration.states.state[2]);
      }

      page.concat(F("<br><br><p class=\"cm\">"));

      page.concat(F(L_GATE_IF_MAGNETIC_SENSOR));
      page.concat(F(": <strong>"));
      page += contactronConfiguration[0].name;
      if (numberOfContractons == 2) {
        page.concat(F("</strong> "));
        page.concat(F(L_GATE_AND_SENSOR));
        page.concat(F(": <strong>"));
        page += contactronConfiguration[1].name;
        page.concat(F("</strong> "));
        page.concat(F(L_GATE_ARE_CLOSED));
      } else {
        page.concat(F("</strong> "));
        page.concat(F(L_GATE_IS_CLOSED));
      }
      page.concat(F(" "));
      page.concat(F(L_GATE_THEN));
      page.concat(F(":</p>"));
      addGateStatesListItem(page, F("s3"), gateConfiguration.states.state[3]);
      closeSection(page);
    }
  }

#ifdef AFE_CONFIG_API_DOMOTICZ_ENABLED
  if (Device->configuration.api.domoticz || Device->configuration.api.mqtt) {
    openSection(page, F("Domoticz"), F(L_DOMOTICZ_NO_IF_IDX_0));
    char _idx[7];

    if (Device->configuration.api.mqtt) {
      sprintf(_idx, "%d", gateConfiguration.domoticzControl.idx);
      addInputFormItem(page, AFE_FORM_ITEM_TYPE_NUMBER, "z", "IDX Start/Stop",
                       _idx, AFE_FORM_ITEM_SKIP_PROPERTY,
                       AFE_DOMOTICZ_IDX_MIN_FORM_DEFAULT,
                       AFE_DOMOTICZ_IDX_MAX_FORM_DEFAULT, "1");
    }

    sprintf(_idx, "%d", gateConfiguration.domoticz.idx);
    addInputFormItem(page, AFE_FORM_ITEM_TYPE_NUMBER, "x", L_GATE_IDX_STATE,
                     _idx, AFE_FORM_ITEM_SKIP_PROPERTY,
                     AFE_DOMOTICZ_IDX_MIN_FORM_DEFAULT,
                     AFE_DOMOTICZ_IDX_MAX_FORM_DEFAULT, "1");

    closeSection(page);
  }
#else
  if (Device->configuration.api.mqtt) {
    openSection(page, F(L_GATE_MQTT_TOPIC), F(L_MQTT_TOPIC_EMPTY));
    addInputFormItem(page, AFE_FORM_ITEM_TYPE_TEXT, "t", L_MQTT_TOPIC,
                     gateConfiguration.mqtt.topic, "64");
    closeSection(page);
  }
#endif
}
#endif // AFE_CONFIG_HARDWARE_GATE

#ifdef AFE_CONFIG_HARDWARE_HPMA115S0
void AFESitesGenerator::siteHPMA115S0Sensor(String &page, uint8_t id) {
  HPMA115S0 configuration;
  Data->getConfiguration(id, &configuration);
  openSection(page, F(L_HPMA115S0_SENSOR), F(""));
  addInputFormItem(page, AFE_FORM_ITEM_TYPE_TEXT, "n", L_NAME,
                   configuration.name, "16");

  char _number[7];
  sprintf(_number, "%d", configuration.interval);
  addInputFormItem(page, AFE_FORM_ITEM_TYPE_NUMBER, "f", L_MEASURMENTS_INTERVAL,
                   _number, AFE_FORM_ITEM_SKIP_PROPERTY, "5", "86400", "1",
                   L_SECONDS);

  page.concat(FPSTR(HTTP_INFO_TEXT));
  page.replace("{{i.v}}", F(L_HPMA115S0_POST_SLEEP_INTERVAL));

  sprintf(_number, "%d", configuration.timeToMeasure);
  addInputFormItem(page, AFE_FORM_ITEM_TYPE_NUMBER, "m",
                   L_HPMA115S0_MEASURE_AFTER, _number,
                   AFE_FORM_ITEM_SKIP_PROPERTY, "0", "999", "1", L_SECONDS);
  closeSection(page);

  openSection(page, F(L_HPMA115S0_WHO_NORMS), F(L_HPMA115S0_WHO_NORMS_HINT));

  sprintf(_number, "%-.2f", configuration.whoPM10Norm);
  addInputFormItem(page, AFE_FORM_ITEM_TYPE_NUMBER, "n1", "PM10", _number,
                   AFE_FORM_ITEM_SKIP_PROPERTY, "0", "999.99", "0.01",
                   L_HPMA115S0_WHO_NORM_UNIT);

  sprintf(_number, "%-.2f", configuration.whoPM25Norm);
  addInputFormItem(page, AFE_FORM_ITEM_TYPE_NUMBER, "n2", "PM25", _number,
                   AFE_FORM_ITEM_SKIP_PROPERTY, "0", "999.99", "0.01",
                   L_HPMA115S0_WHO_NORM_UNIT);

  closeSection(page);

#ifdef AFE_CONFIG_API_DOMOTICZ_ENABLED
  if (Device->configuration.api.domoticz || Device->configuration.api.mqtt) {
    openSection(page, F("Domoticz"), F(L_DOMOTICZ_NO_IF_IDX_0));
    sprintf(_number, "%d", configuration.domoticz.pm10.idx);
    addInputFormItem(page, AFE_FORM_ITEM_TYPE_NUMBER, "x1", "IDX PM10", _number,
                     AFE_FORM_ITEM_SKIP_PROPERTY,
                     AFE_DOMOTICZ_IDX_MIN_FORM_DEFAULT,
                     AFE_DOMOTICZ_IDX_MAX_FORM_DEFAULT, "1");
    sprintf(_number, "%d", configuration.domoticz.pm25.idx);
    addInputFormItem(page, AFE_FORM_ITEM_TYPE_NUMBER, "x2", "IDX PM2.5",
                     _number, AFE_FORM_ITEM_SKIP_PROPERTY,
                     AFE_DOMOTICZ_IDX_MIN_FORM_DEFAULT,
                     AFE_DOMOTICZ_IDX_MAX_FORM_DEFAULT, "1");
    sprintf(_number, "%d", configuration.domoticz.whoPM10Norm.idx);
    addInputFormItem(page, AFE_FORM_ITEM_TYPE_NUMBER, "x3", "IDX WHO PM10",
                     _number, AFE_FORM_ITEM_SKIP_PROPERTY,
                     AFE_DOMOTICZ_IDX_MIN_FORM_DEFAULT,
                     AFE_DOMOTICZ_IDX_MAX_FORM_DEFAULT, "1");
    sprintf(_number, "%d", configuration.domoticz.whoPM25Norm.idx);
    addInputFormItem(page, AFE_FORM_ITEM_TYPE_NUMBER, "x4", "IDX WHO PM2.5",
                     _number, AFE_FORM_ITEM_SKIP_PROPERTY,
                     AFE_DOMOTICZ_IDX_MIN_FORM_DEFAULT,
                     AFE_DOMOTICZ_IDX_MAX_FORM_DEFAULT, "1");
    closeSection(page);
  }
#else
  if (Device->configuration.api.mqtt) {
    openSection(page, F(L_HPMA115S0_MQTT_TOPIC), F(L_MQTT_TOPIC_EMPTY));
    addInputFormItem(page, AFE_FORM_ITEM_TYPE_TEXT, "t", L_MQTT_TOPIC,
                     configuration.mqtt.topic, "64");
    closeSection(page);
  }
#endif // AFE_CONFIG_API_DOMOTICZ_ENABLED
}
#endif // AFE_CONFIG_HARDWARE_HPMA115S0

#ifdef AFE_CONFIG_HARDWARE_BMEX80
void AFESitesGenerator::siteBMEX80Sensor(String &page, uint8_t id) {
  char _number[7];
  BMEX80 configuration;
  Data->getConfiguration(id, &configuration);

  openSection(page, F(L_BMEX80_SENSOR), F(""));

  /* Item: IIC address selection */
  addDeviceI2CAddressSelectionItem(page, configuration.i2cAddress);

  /* Item: type of the sensor */
  addSelectFormItemOpen(page, F("b"), F(L_BMEX80_SENSOR_TYPE));
  addSelectOptionFormItem(page, L_NONE, "255",
                          configuration.type == AFE_BMX_UNKNOWN_SENSOR);
  addSelectOptionFormItem(page, "BMx085/BMx180", "1",
                          configuration.type == AFE_BMP180_SENSOR);
  addSelectOptionFormItem(page, "BMx280", "2",
                          configuration.type == AFE_BME280_SENSOR);
  addSelectOptionFormItem(page, "BMx680", "6",
                          configuration.type == AFE_BME680_SENSOR);
  addSelectFormItemClose(page);

  page.concat(
      "<input type=\"submit\" class=\"b bw\" value=\"" L_BMEX80_REFRESH_SETTINGS
      "\"><br><br>");

  /* Item: name */
  addInputFormItem(page, AFE_FORM_ITEM_TYPE_TEXT, "n", L_NAME,
                   configuration.name, "16");

  /* Item: interval */
  sprintf(_number, "%d", configuration.interval);
  addInputFormItem(page, AFE_FORM_ITEM_TYPE_NUMBER, "f", L_MEASURMENTS_INTERVAL,
                   _number, AFE_FORM_ITEM_SKIP_PROPERTY, "5", "86400", "1",
                   L_SECONDS);

  closeSection(page);

  if (configuration.type != AFE_BMX_UNKNOWN_SENSOR) {

    /* Item: Unit */
    openSection(page, F(L_UNITS), F(""));
    addSelectFormItemOpen(page, F("tu"), F(L_TEMPERATURE));
    addSelectOptionFormItem(page, "C", "1", configuration.temperature.unit ==
                                                AFE_TEMPERATURE_UNIT_CELSIUS);
    addSelectOptionFormItem(page, "F", "2",
                            configuration.temperature.unit ==
                                AFE_TEMPERATURE_UNIT_FAHRENHEIT);
    addSelectFormItemClose(page);
    closeSection(page);

    /* Item: Corrections of sensor values */
    openSection(page, F(L_CORRECTIONS), F(""));
    sprintf(_number, "%-.3f", configuration.temperature.correction);
    addInputFormItem(page, AFE_FORM_ITEM_TYPE_NUMBER, "tc", L_TEMPERATURE,
                     _number, AFE_FORM_ITEM_SKIP_PROPERTY, "-99.999", "99.999",
                     "0.001");

    if (configuration.type != AFE_BMP180_SENSOR) {
      /* Item: humidity correction */
      sprintf(_number, "%-.3f", configuration.humidity.correction);
      addInputFormItem(page, AFE_FORM_ITEM_TYPE_NUMBER, "hc", L_HUMIDITY,
                       _number, AFE_FORM_ITEM_SKIP_PROPERTY, "-99.999",
                       "99.999", "0.001");
    }

    /* Item: pressure correction */
    sprintf(_number, "%-.3f", configuration.pressure.correction);
    addInputFormItem(page, AFE_FORM_ITEM_TYPE_NUMBER, "pc", L_PRESSURE, _number,
                     AFE_FORM_ITEM_SKIP_PROPERTY, "-999.999", "999.999",
                     "0.001");

    /* Item: altitude */
    sprintf(_number, "%d", configuration.altitude);
    addInputFormItem(page, AFE_FORM_ITEM_TYPE_NUMBER, "hi", L_BMEX80_ALTITIDE,
                     _number, AFE_FORM_ITEM_SKIP_PROPERTY, "-431", "8850", "1",
                     L_BMEX80_METERS);

    closeSection(page);

#ifdef AFE_CONFIG_API_DOMOTICZ_ENABLED
    if (Device->configuration.api.domoticz || Device->configuration.api.mqtt) {
      openSection(page, F("Domoticz"), F(L_DOMOTICZ_NO_IF_IDX_0));
      sprintf(_number, "%d", configuration.domoticz.temperature.idx);
      addInputFormItem(page, AFE_FORM_ITEM_TYPE_NUMBER, "i1", L_TEMPERATURE_IDX,
                       _number, AFE_FORM_ITEM_SKIP_PROPERTY,
                       AFE_DOMOTICZ_IDX_MIN_FORM_DEFAULT,
                       AFE_DOMOTICZ_IDX_MAX_FORM_DEFAULT, "1");

      if (configuration.type != AFE_BMP180_SENSOR) {
        sprintf(_number, "%d", configuration.domoticz.humidity.idx);
        addInputFormItem(page, AFE_FORM_ITEM_TYPE_NUMBER, "i2", L_HUMIDITY_IDX,
                         _number, AFE_FORM_ITEM_SKIP_PROPERTY,
                         AFE_DOMOTICZ_IDX_MIN_FORM_DEFAULT,
                         AFE_DOMOTICZ_IDX_MAX_FORM_DEFAULT, "1");

        sprintf(_number, "%d", configuration.domoticz.absoluteHumidity.idx);
        addInputFormItem(page, AFE_FORM_ITEM_TYPE_NUMBER, "i15",
                         L_HUMIDITY_IDX_ABSOLUTE, _number,
                         AFE_FORM_ITEM_SKIP_PROPERTY,
                         AFE_DOMOTICZ_IDX_MIN_FORM_DEFAULT,
                         AFE_DOMOTICZ_IDX_MAX_FORM_DEFAULT, "1");

        sprintf(_number, "%d", configuration.domoticz.dewPoint.idx);
        addInputFormItem(page, AFE_FORM_ITEM_TYPE_NUMBER, "i3",
                         L_HUMIDITY_IDX_DEW_POINT, _number,
                         AFE_FORM_ITEM_SKIP_PROPERTY,
                         AFE_DOMOTICZ_IDX_MIN_FORM_DEFAULT,
                         AFE_DOMOTICZ_IDX_MAX_FORM_DEFAULT, "1");

        sprintf(_number, "%d", configuration.domoticz.heatIndex.idx);
        addInputFormItem(page, AFE_FORM_ITEM_TYPE_NUMBER, "i4",
                         L_HUMIDITY_IDX_HEAT_INDEX, _number,
                         AFE_FORM_ITEM_SKIP_PROPERTY,
                         AFE_DOMOTICZ_IDX_MIN_FORM_DEFAULT,
                         AFE_DOMOTICZ_IDX_MAX_FORM_DEFAULT, "1");

        sprintf(_number, "%d", configuration.domoticz.perception.idx);
        addInputFormItem(page, AFE_FORM_ITEM_TYPE_NUMBER, "i13",
                         L_PERCEPTION_IDX, _number, AFE_FORM_ITEM_SKIP_PROPERTY,
                         AFE_DOMOTICZ_IDX_MIN_FORM_DEFAULT,
                         AFE_DOMOTICZ_IDX_MAX_FORM_DEFAULT, "1");

        sprintf(_number, "%d", configuration.domoticz.comfort.idx);
        addInputFormItem(page, AFE_FORM_ITEM_TYPE_NUMBER, "i14", L_COMFORT_IDX,
                         _number, AFE_FORM_ITEM_SKIP_PROPERTY,
                         AFE_DOMOTICZ_IDX_MIN_FORM_DEFAULT,
                         AFE_DOMOTICZ_IDX_MAX_FORM_DEFAULT, "1");

        sprintf(_number, "%d",
                configuration.domoticz.temperatureHumidityPressure.idx);
        addInputFormItem(page, AFE_FORM_ITEM_TYPE_NUMBER, "i0",
                         L_HUMIDITY_IDX_TEMP_HUM_BAR, _number,
                         AFE_FORM_ITEM_SKIP_PROPERTY,
                         AFE_DOMOTICZ_IDX_MIN_FORM_DEFAULT,
                         AFE_DOMOTICZ_IDX_MAX_FORM_DEFAULT, "1");

        sprintf(_number, "%d", configuration.domoticz.temperatureHumidity.idx);
        addInputFormItem(page, AFE_FORM_ITEM_TYPE_NUMBER, "i12",
                         L_HUMIDITY_IDX_TEMP_HUM, _number,
                         AFE_FORM_ITEM_SKIP_PROPERTY,
                         AFE_DOMOTICZ_IDX_MIN_FORM_DEFAULT,
                         AFE_DOMOTICZ_IDX_MAX_FORM_DEFAULT, "1");
      }

      sprintf(_number, "%d", configuration.domoticz.pressure.idx);
      addInputFormItem(page, AFE_FORM_ITEM_TYPE_NUMBER, "i5", L_PRESSURE_IDX,
                       _number, AFE_FORM_ITEM_SKIP_PROPERTY,
                       AFE_DOMOTICZ_IDX_MIN_FORM_DEFAULT,
                       AFE_DOMOTICZ_IDX_MAX_FORM_DEFAULT, "1");

      sprintf(_number, "%d", configuration.domoticz.relativePressure.idx);
      addInputFormItem(page, AFE_FORM_ITEM_TYPE_NUMBER, "i6",
                       L_PRESSURE_IDX_RELATIVE_PRESSURE, _number,
                       AFE_FORM_ITEM_SKIP_PROPERTY,
                       AFE_DOMOTICZ_IDX_MIN_FORM_DEFAULT,
                       AFE_DOMOTICZ_IDX_MAX_FORM_DEFAULT, "1");

      if (configuration.type == AFE_BME680_SENSOR) {
        sprintf(_number, "%d", configuration.domoticz.iaq.idx);
        addInputFormItem(page, AFE_FORM_ITEM_TYPE_NUMBER, "i7",
                         L_BMEX80_IDX_IQA, _number, AFE_FORM_ITEM_SKIP_PROPERTY,
                         AFE_DOMOTICZ_IDX_MIN_FORM_DEFAULT,
                         AFE_DOMOTICZ_IDX_MAX_FORM_DEFAULT, "1");

        sprintf(_number, "%d", configuration.domoticz.staticIaq.idx);
        addInputFormItem(page, AFE_FORM_ITEM_TYPE_NUMBER, "i8",
                         L_BMEX80_IDX_STATIC_IAQ, _number,
                         AFE_FORM_ITEM_SKIP_PROPERTY,
                         AFE_DOMOTICZ_IDX_MIN_FORM_DEFAULT,
                         AFE_DOMOTICZ_IDX_MAX_FORM_DEFAULT, "1");

        sprintf(_number, "%d", configuration.domoticz.co2Equivalent.idx);
        addInputFormItem(page, AFE_FORM_ITEM_TYPE_NUMBER, "i9",
                         L_BMEX80_IDX_CO2_EQUVALENT, _number,
                         AFE_FORM_ITEM_SKIP_PROPERTY,
                         AFE_DOMOTICZ_IDX_MIN_FORM_DEFAULT,
                         AFE_DOMOTICZ_IDX_MAX_FORM_DEFAULT, "1");

        sprintf(_number, "%d", configuration.domoticz.breathVocEquivalent.idx);
        addInputFormItem(page, AFE_FORM_ITEM_TYPE_NUMBER, "i10",
                         L_BMEX80_IDX_BVOC_EQUIVALENT, _number,
                         AFE_FORM_ITEM_SKIP_PROPERTY,
                         AFE_DOMOTICZ_IDX_MIN_FORM_DEFAULT,
                         AFE_DOMOTICZ_IDX_MAX_FORM_DEFAULT, "1");

        sprintf(_number, "%d", configuration.domoticz.gasResistance.idx);
        addInputFormItem(page, AFE_FORM_ITEM_TYPE_NUMBER, "i11",
                         L_BMEX80_IDX_GAS_SENSOR, _number,
                         AFE_FORM_ITEM_SKIP_PROPERTY,
                         AFE_DOMOTICZ_IDX_MIN_FORM_DEFAULT,
                         AFE_DOMOTICZ_IDX_MAX_FORM_DEFAULT, "1");
      }
      closeSection(page);
    }
#else
    if (Device->configuration.api.mqtt) {
      openSection(page, F(L_BMEX80_MQTT_TOPIC), F(L_MQTT_TOPIC_EMPTY));
      addInputFormItem(page, AFE_FORM_ITEM_TYPE_TEXT, "t", L_MQTT_TOPIC,
                       configuration.mqtt.topic, "64");
      closeSection(page);
    }
#endif // AFE_CONFIG_API_DOMOTICZ_ENABLED
  }
}
#endif // AFE_CONFIG_HARDWARE_BMEX80

#ifdef AFE_CONFIG_HARDWARE_BH1750
void AFESitesGenerator::siteBH1750Sensor(String &page, uint8_t id) {

  BH1750 configuration;
  Data->getConfiguration(id, &configuration);
  openSection(page, F(L_BH1750_SENSOR), F(""));

  /* Item: I2C Address selection */
  addDeviceI2CAddressSelectionItem(page, configuration.i2cAddress);

  /* Item: name of the sensor */
  addInputFormItem(page, AFE_FORM_ITEM_TYPE_TEXT, "n", L_NAME,
                   configuration.name, "16");

  /* Item: interval */
  char _number[7];
  sprintf(_number, "%d", configuration.interval);
  addInputFormItem(page, AFE_FORM_ITEM_TYPE_NUMBER, "f", L_MEASURMENTS_INTERVAL,
                   _number, AFE_FORM_ITEM_SKIP_PROPERTY, "5", "86400", "1",
                   L_SECONDS);

  /* Item: sensitivness, not possiblity to change
  sprintf(_number, "%d", configuration.mode);
  addInputFormItem(page, AFE_FORM_ITEM_TYPE_NUMBER, "m", L_SENSITIVENESS,
                   _number, AFE_FORM_ITEM_SKIP_PROPERTY,
                   AFE_FORM_ITEM_SKIP_PROPERTY, AFE_FORM_ITEM_SKIP_PROPERTY,
                   AFE_FORM_ITEM_SKIP_PROPERTY, L_ADC_CANT_CHANGE, true);
*/
  closeSection(page);

#ifdef AFE_CONFIG_API_DOMOTICZ_ENABLED
  if (Device->configuration.api.domoticz || Device->configuration.api.mqtt) {
    openSection(page, F("Domoticz"), F(L_DOMOTICZ_NO_IF_IDX_0));
    sprintf(_number, "%d", configuration.domoticz.idx);
    addInputFormItem(page, AFE_FORM_ITEM_TYPE_NUMBER, "d", "IDX", _number,
                     AFE_FORM_ITEM_SKIP_PROPERTY,
                     AFE_DOMOTICZ_IDX_MIN_FORM_DEFAULT,
                     AFE_DOMOTICZ_IDX_MAX_FORM_DEFAULT, "1");
    closeSection(page);
  }
#else
  if (Device->configuration.api.mqtt) {
    openSection(page, F(L_BH1750_MQTT_TOPIC), F(L_MQTT_TOPIC_EMPTY));
    addInputFormItem(page, AFE_FORM_ITEM_TYPE_TEXT, "t", L_MQTT_TOPIC,
                     configuration.mqtt.topic, "64");
    closeSection(page);
  }
#endif // AFE_CONFIG_API_DOMOTICZ_ENABLED
}
#endif // AFE_CONFIG_HARDWARE_BH1750

#ifdef AFE_CONFIG_HARDWARE_AS3935
// String AFESitesGenerator::siteAS3935Sensor(uint8_t id) {
void AFESitesGenerator::siteAS3935Sensor(String &page, uint8_t id) {
  AS3935 configuration;
  Data->getConfiguration(id, &configuration);
  char _number[2];

  openSection(page, F(L_AS3935_SENSOR), F(""));
  addDeviceI2CAddressSelectionItem(page, configuration.i2cAddress);
  addInputFormItem(page, AFE_FORM_ITEM_TYPE_TEXT, "n", L_NAME,
                   configuration.name, "16");
  addListOfGPIOs(page, "g", configuration.irqGPIO);

  page += "<div class=\"cf\"><label>";
  page += F(L_DISTANCE_UNIT);
  page += "</label><select name=\"u\"><option value=\"";
  page += AFE_DISTANCE_KM;
  page += "\"";
  page +=
      (configuration.unit == AFE_DISTANCE_KM ? " selected=\"selected\"" : "");
  page += ">";
  page += F(L_KM);
  page += "</option><option value=\"";
  page += AFE_DISTANCE_MIL;
  page += "\"";
  page +=
      (configuration.unit == AFE_DISTANCE_MIL ? " selected=\"selected\"" : "");
  page += ">";
  page += F(L_MILES);
  page += "</option></select></div>";

  sprintf(_number, "%d", configuration.watchdogThreshold);
  addInputFormItem(page, AFE_FORM_ITEM_TYPE_NUMBER, "e",
                   L_AS3935_WATCHDOG_THRESHOLD, _number,
                   AFE_FORM_ITEM_SKIP_PROPERTY, "1", "10", "1",
                   L_AS3935_WATCHDOG_THRESHOLD_HINT);

  sprintf(_number, "%d", configuration.spikesRejectionLevel);
  addInputFormItem(page, AFE_FORM_ITEM_TYPE_NUMBER, "s",
                   L_AS3935_SPIKES_REJECTION, _number,
                   AFE_FORM_ITEM_SKIP_PROPERTY, "1", "11", "1",
                   L_AS3935_SPIKES_REJECTION_HINT);

  page += "<div class=\"cf\"><label>";
  page += F(L_AS3935_MIN_SPIKES);
  page += "</label><select name=\"m\"><option value=\"1\"";
  page += (configuration.minimumNumberOfLightningSpikes == 1
               ? " selected=\"selected\""
               : "");
  page += ">1</option><option value=\"5\"";
  page += (configuration.minimumNumberOfLightningSpikes == 5
               ? " selected=\"selected\""
               : "");
  page += ">5</option><option value=\"9\"";
  page += (configuration.minimumNumberOfLightningSpikes == 9
               ? " selected=\"selected\""
               : "");
  page += ">9</option><option value=\"16\"";
  page += (configuration.minimumNumberOfLightningSpikes == 16
               ? " selected=\"selected\""
               : "");
  page += ">16</option></select></div>";

  page += "<div class=\"cc\"><label><input name =\"f\" type=\"checkbox\" "
          "value=\"1\"";
  page +=
      configuration.setNoiseFloorAutomatically ? " checked=\"checked\">" : ">";
  page += F(L_AS3935_AUTOMATIC_NOISE_FLOOR_CONTROL);
  page += "</label></div><p class=\"cm\">";

  page += F(L_AS3935_SET_LEVEL_OF_NOISE_FLOOR);
  sprintf(_number, "%d", configuration.noiseFloor);
  addInputFormItem(page, AFE_FORM_ITEM_TYPE_NUMBER, "nf", L_AS3935_NOISE_FLOOR,
                   _number, AFE_FORM_ITEM_SKIP_PROPERTY, "1", "7", "1",
                   L_AS3935_NOISE_FLOOR_HINT);

  page += "</p><div class=\"cf\"><label>";
  page += F(L_AS3935_SENSOR_INDOOR_OUTDOOR);
  page += "</label><select name=\"w\"><option value=\"1\"";
  page += (configuration.indoor == true ? " selected=\"selected\"" : "");
  page += ">";
  page += F(L_AS3935_INDOOR);
  page += "</option><option value=\"0\"";
  page += (configuration.indoor == false ? " selected=\"selected\"" : "");
  page += ">";
  page += F(L_AS3935_OUTDOOR);
  page += "</option></select></div>";

  closeSection(page);

#ifdef AFE_CONFIG_API_DOMOTICZ_ENABLED
  if (Device->configuration.api.domoticz || Device->configuration.api.mqtt) {
    openSection(page, F("Domoticz"), F(L_DOMOTICZ_NO_IF_IDX_0));
    sprintf(_number, "%d", configuration.domoticz.idx);
    addInputFormItem(page, AFE_FORM_ITEM_TYPE_NUMBER, "d", "IDX", _number,
                     AFE_FORM_ITEM_SKIP_PROPERTY,
                     AFE_DOMOTICZ_IDX_MIN_FORM_DEFAULT,
                     AFE_DOMOTICZ_IDX_MAX_FORM_DEFAULT, "1");
    closeSection(page);
  }
#else
  if (Device->configuration.api.mqtt) {
    openSection(page, F(L_AS3935_MQTT_TOPIC), F(L_MQTT_TOPIC_EMPTY));
    addInputFormItem(page, AFE_FORM_ITEM_TYPE_TEXT, "t", L_MQTT_TOPIC,
                     configuration.mqtt.topic, "64");
    closeSection(page);
  }
#endif // AFE_CONFIG_API_DOMOTICZ_ENABLED
}
#endif // AFE_CONFIG_HARDWARE_AS3935

#ifdef AFE_CONFIG_HARDWARE_ANEMOMETER
void AFESitesGenerator::siteAnemometerSensor(String &page) {

  openSection(page, F(L_ANEMOMETER_SENSOR), F(""));
  ANEMOMETER configuration;
  Data->getConfiguration(&configuration);

  /* Item: name */
  addInputFormItem(page, AFE_FORM_ITEM_TYPE_TEXT, "n", L_NAME,
                   configuration.name, "16");

  /* Item: GPIO */
  addListOfGPIOs(page, F("g"), configuration.gpio);

  /* Item: interval */
  char _number[7];
  sprintf(_number, "%d", configuration.interval);
  addInputFormItem(page, AFE_FORM_ITEM_TYPE_NUMBER, "f", L_MEASURMENTS_INTERVAL,
                   _number, AFE_FORM_ITEM_SKIP_PROPERTY, "5", "86400", "1",
                   L_SECONDS);
  closeSection(page);

  openSection(page, F(L_ANEMOMETER_CALIBRATION),
              F(L_ANEMOMETER_IMPULSE_DISTANCE_HINT));

  /* Item: Distance */
  sprintf(_number, "%-.2f", configuration.impulseDistance);
  addInputFormItem(page, AFE_FORM_ITEM_TYPE_NUMBER, "l",
                   L_ANEMOMETER_IMPULSE_DISTANCE, _number,
                   AFE_FORM_ITEM_SKIP_PROPERTY, "0", "999.99", "0.01");

  /* Item: Distance unit */

  addSelectFormItemOpen(page, F("u"), F(L_DISTANCE_UNIT));
  addSelectOptionFormItem(page, L_CM, "0", configuration.impulseDistanceUnit ==
                                               AFE_DISTANCE_CENTIMETER);
  addSelectOptionFormItem(page, L_M, "1", configuration.impulseDistanceUnit ==
                                              AFE_DISTANCE_METER);
  addSelectOptionFormItem(page, L_KM, "2", configuration.impulseDistanceUnit ==
                                               AFE_DISTANCE_KILOMETER);
  addSelectFormItemClose(page);

  page.concat(FPSTR(HTTP_INFO_TEXT));
  page.replace("{{i.v}}", F(L_ANEMOMETER_SENSITIVENESS_HINT));

  sprintf(_number, "%d", configuration.sensitiveness);
  addInputFormItem(page, AFE_FORM_ITEM_TYPE_NUMBER, "s", L_SENSITIVENESS,
                   _number, AFE_FORM_ITEM_SKIP_PROPERTY, "0", "255", "1",
                   L_MILISECONDS);

  closeSection(page);

#ifdef AFE_CONFIG_API_DOMOTICZ_ENABLED
  if (Device->configuration.api.domoticz || Device->configuration.api.mqtt) {
    openSection(page, F("Domoticz"), F(L_DOMOTICZ_NO_IF_IDX_0));
    char _idx[7];
    sprintf(_idx, "%d", configuration.domoticz.idx);
    addInputFormItem(page, AFE_FORM_ITEM_TYPE_NUMBER, "x", "IDX", _idx,
                     AFE_FORM_ITEM_SKIP_PROPERTY,
                     AFE_DOMOTICZ_IDX_MIN_FORM_DEFAULT,
                     AFE_DOMOTICZ_IDX_MAX_FORM_DEFAULT, "1");
    closeSection(page);
  }
#else
  if (Device->configuration.api.mqtt) {
    openSection(page, F(L_SWITCH_MQTT_TOPIC), F(L_MQTT_TOPIC_EMPTY));
    addInputFormItem(page, AFE_FORM_ITEM_TYPE_TEXT, "t", L_MQTT_TOPIC,
                     configuration.mqtt.topic, "64");
    closeSection(page);
  }
#endif
}
#endif // AFE_CONFIG_HARDWARE_ANEMOMETER

#ifdef AFE_CONFIG_HARDWARE_RAINMETER
void AFESitesGenerator::siteRainmeterSensor(String &page) {

  openSection(page, F(L_RAINMETER), F(""));
  RAINMETER configuration;
  Data->getConfiguration(&configuration);

  addInputFormItem(page, AFE_FORM_ITEM_TYPE_TEXT, "n", L_NAME,
                   configuration.name, "16");

  addListOfGPIOs(page, F("g"), configuration.gpio);

  char _number[8];
  sprintf(_number, "%d", configuration.interval);
  addInputFormItem(page, AFE_FORM_ITEM_TYPE_NUMBER, "f", L_MEASURMENTS_INTERVAL,
                   _number, AFE_FORM_ITEM_SKIP_PROPERTY, "5", "86400", "1",
                   L_SECONDS);
  closeSection(page);
  openSection(page, F(L_RAINMETER_CALIBRATION), F(""));

  sprintf(_number, "%-.2f", configuration.resolution);
  addInputFormItem(page, AFE_FORM_ITEM_TYPE_NUMBER, "r", L_RAINMETER_RESOLUTION,
                   _number, AFE_FORM_ITEM_SKIP_PROPERTY, "0", "9999.99", "0.01",
                   "ml/m2");
  closeSection(page);
#ifdef AFE_CONFIG_API_DOMOTICZ_ENABLED
  if (Device->configuration.api.domoticz || Device->configuration.api.mqtt) {
    openSection(page, F("Domoticz"), F(L_DOMOTICZ_NO_IF_IDX_0));
    char _idx[7];
    sprintf(_idx, "%d", configuration.domoticz.idx);
    addInputFormItem(page, AFE_FORM_ITEM_TYPE_NUMBER, "x", "IDX", _idx,
                     AFE_FORM_ITEM_SKIP_PROPERTY,
                     AFE_DOMOTICZ_IDX_MIN_FORM_DEFAULT,
                     AFE_DOMOTICZ_IDX_MAX_FORM_DEFAULT, "1");
  }
#else
  if (Device->configuration.api.mqtt) {
    openSection(page, F(L_SWITCH_MQTT_TOPIC), F(L_MQTT_TOPIC_EMPTY));
    addInputFormItem(page, AFE_FORM_ITEM_TYPE_TEXT, "t", L_MQTT_TOPIC,
                     configuration.mqtt.topic, "64");
  }
#endif
  closeSection(page);
}
#endif // AFE_CONFIG_HARDWARE_RAINMETER

#ifdef AFE_CONFIG_HARDWARE_ADC_VCC
void AFESitesGenerator::siteADCInput(String &page) {
  ADCINPUT configuration;
  Data->getConfiguration(&configuration);

  openSection(page, F(L_ANALOG_INPUT), F(""));
  char _number[13];

  sprintf(_number, "%d", configuration.gpio);
  addInputFormItem(page, AFE_FORM_ITEM_TYPE_NUMBER, "g", "GPIO", _number,
                   AFE_FORM_ITEM_SKIP_PROPERTY AFE_FORM_ITEM_SKIP_PROPERTY
                       AFE_FORM_ITEM_SKIP_PROPERTY,
                   "?");

  sprintf(_number, "%d", configuration.interval);
  addInputFormItem(page, AFE_FORM_ITEM_TYPE_NUMBER, "v", L_MEASURMENTS_INTERVAL,
                   _number, AFE_FORM_ITEM_SKIP_PROPERTY, "1", "86400", "1",
                   L_SECONDS);

  sprintf(_number, "%d", configuration.numberOfSamples);
  addInputFormItem(page, AFE_FORM_ITEM_TYPE_NUMBER, "n",
                   L_ADC_NUMBER_OF_SAMPLES, _number,
                   AFE_FORM_ITEM_SKIP_PROPERTY, "1", "999", "1");

  closeSection(page);

  openSection(page, F(L_ADC_VOLTAGE_DIVIDER), F(""));

  sprintf(_number, "%-.2f", (float)configuration.maxVCC);
  addInputFormItem(page, AFE_FORM_ITEM_TYPE_NUMBER, "m", L_ADC_MEASURED_VOLTAGE,
                   _number, AFE_FORM_ITEM_SKIP_PROPERTY, "0", "1000", "0.01",
                   "V");

  sprintf(_number, "%d", (uint32_t)configuration.divider.Ra);
  addInputFormItem(page, AFE_FORM_ITEM_TYPE_NUMBER, "ra", "R[a]", _number,
                   AFE_FORM_ITEM_SKIP_PROPERTY, "0", "90000000", "1", "Om");

  sprintf(_number, "%d", (uint32_t)configuration.divider.Rb);
  addInputFormItem(page, AFE_FORM_ITEM_TYPE_NUMBER, "rb", "R[b]", _number,
                   AFE_FORM_ITEM_SKIP_PROPERTY, "0", "90000000", "1", "Om");
  closeSection(page);

#ifdef AFE_CONFIG_API_DOMOTICZ_ENABLED
  char _idx[7];
  if (Device->configuration.api.domoticz || Device->configuration.api.mqtt) {
    openSection(page, F("Domoticz"), F(L_DOMOTICZ_NO_IF_IDX_0));
    sprintf(_idx, "%d", configuration.domoticz.raw);
    addInputFormItem(page, AFE_FORM_ITEM_TYPE_NUMBER, "x0", L_ADC_IDX_RAW_DATA,
                     _idx, AFE_FORM_ITEM_SKIP_PROPERTY,
                     AFE_DOMOTICZ_IDX_MIN_FORM_DEFAULT,
                     AFE_DOMOTICZ_IDX_MAX_FORM_DEFAULT, "1");
    sprintf(_idx, "%d", configuration.domoticz.percent);
    addInputFormItem(page, AFE_FORM_ITEM_TYPE_NUMBER, "x1", L_ADC_IDX_PERCENT,
                     _idx, AFE_FORM_ITEM_SKIP_PROPERTY,
                     AFE_DOMOTICZ_IDX_MIN_FORM_DEFAULT,
                     AFE_DOMOTICZ_IDX_MAX_FORM_DEFAULT, "1");
    sprintf(_idx, "%d", configuration.domoticz.voltage);
    addInputFormItem(page, AFE_FORM_ITEM_TYPE_NUMBER, "x2", L_ADC_IDX_VOLTAGE,
                     _idx, AFE_FORM_ITEM_SKIP_PROPERTY,
                     AFE_DOMOTICZ_IDX_MIN_FORM_DEFAULT,
                     AFE_DOMOTICZ_IDX_MAX_FORM_DEFAULT, "1");
    sprintf(_idx, "%d", configuration.domoticz.voltageCalculated);
    addInputFormItem(
        page, AFE_FORM_ITEM_TYPE_NUMBER, "x3", L_ADC_IDX_VOLTAGE_CALCULATED,
        _idx, AFE_FORM_ITEM_SKIP_PROPERTY, AFE_DOMOTICZ_IDX_MIN_FORM_DEFAULT,
        AFE_DOMOTICZ_IDX_MAX_FORM_DEFAULT, "1");
    closeSection(page);
  }
#else
  if (Device->configuration.api.mqtt) {
    openSection(page, F(L_ADC_MQTT_TOPIC), F(L_MQTT_TOPIC_EMPTY));
    addInputFormItem(page, AFE_FORM_ITEM_TYPE_TEXT, "t", L_MQTT_TOPIC,
                     configuration.mqtt.topic, "64");
    closeSection(page);
  }
#endif // AFE_CONFIG_API_DOMOTICZ_ENABLED

#ifdef AFE_CONFIG_FUNCTIONALITY_BATTERYMETER
  openSection(page, F(L_BATTERY_METER), F(""));

  sprintf(_number, "%-.3f", configuration.battery.minVoltage);
  addInputFormItem(page, AFE_FORM_ITEM_TYPE_NUMBER, "lv", L_BATTERY_MIN_LEVEL,
                   _number, AFE_FORM_ITEM_SKIP_PROPERTY, "0", "1000", "0.001");

  sprintf(_number, "%-.3f", configuration.battery.maxVoltage);
  addInputFormItem(page, AFE_FORM_ITEM_TYPE_NUMBER, "hv", L_BATTERY_MAX_LEVEL,
                   _number, AFE_FORM_ITEM_SKIP_PROPERTY, "0", "1000", "0.001");

  closeSection(page);

#ifdef AFE_CONFIG_API_DOMOTICZ_ENABLED
  if (Device->configuration.api.domoticz || Device->configuration.api.mqtt) {
    openSection(page, F("Domoticz"), F(L_DOMOTICZ_NO_IF_IDX_0));
    sprintf(_idx, "%d", configuration.battery.domoticz.idx);
    addInputFormItem(page, AFE_FORM_ITEM_TYPE_NUMBER, "bx", "IDX", _idx,
                     AFE_FORM_ITEM_SKIP_PROPERTY,
                     AFE_DOMOTICZ_IDX_MIN_FORM_DEFAULT,
                     AFE_DOMOTICZ_IDX_MAX_FORM_DEFAULT, "1");
    closeSection(page);
  }
#else
  if (Device->configuration.api.mqtt) {
    openSection(page, F(L_BATTERY_MQTT_TOPIC), F(L_MQTT_TOPIC_EMPTY));
    addInputFormItem(page, AFE_FORM_ITEM_TYPE_TEXT, "bt", L_MQTT_TOPIC,
                     configuration.battery.mqtt.topic, "64");
    closeSection(page);
  }
#endif // AFE_CONFIG_API_DOMOTICZ_ENABLED

#endif // AFE_CONFIG_FUNCTIONALITY_BATTERYMETER
}
#endif // AFE_CONFIG_HARDWARE_ADC_VCC

#ifdef AFE_CONFIG_HARDWARE_UART
void AFESitesGenerator::siteUARTBUS(String &page) {
  SERIALPORT configuration;
  Data->getConfiguration(&configuration);
  openSection(page, F("UART"), F(""));
  addListOfGPIOs(page, F("r"), configuration.RXD, "GPIO RXD");
  addListOfGPIOs(page, F("t"), configuration.TXD, "GPIO TXD");
  closeSection(page);
}
#endif // AFE_CONFIG_HARDWARE_UART

#ifdef AFE_CONFIG_HARDWARE_I2C
void AFESitesGenerator::siteI2CBUS(String &page) {
  I2CPORT configuration;
  Data->getConfiguration(&configuration);
  openSection(page, F("I2C"), F(""));
  addListOfGPIOs(page, F("a"), configuration.SDA, "GPIO SDA");
  addListOfGPIOs(page, F("l"), configuration.SCL, "GPIO SCL");
  closeSection(page);
}
#endif // AFE_CONFIG_HARDWARE_I2C

#ifndef AFE_CONFIG_OTA_NOT_UPGRADABLE
void AFESitesGenerator::siteUpgrade(String &page) {

  siteWANUpgrade(page, F(L_UPGRADE_READ_BEFORE));

  openSection(page, F(L_UPGRADE_FROM_FILE), F(""));

  page.concat(
      F("<form method=\"post\" action=\"upgrade?o=21\" "
        "enctype=\"multipart/form-data\"><div "
        "class=\"cf\"><label>{{L1}}</label><input "
        "class=\"bs\" "
        "name=\"update\" type=\"file\" accept=\".bin\"></div><input "
        "style=\"margin-top:1.5em\" type=\"submit\" value=\"{{L2}}\" class=\"b "
        "be\"></form>"));

  page.replace("{{L1}}", F(L_UPGRADE_SELECT_FIRMWARE));
  page.replace("{{L2}}", F(L_UPGRADE));
  closeSection(page);

  if (RestAPI->accessToWAN()) {

    RestAPI->sent(_HtmlResponse,
                  AFE_CONFIG_JSONRPC_REST_METHOD_GET_LATEST_FIRMWARE_ID);

    if (_HtmlResponse.length() > 0) {
      openSection(page, F(L_UPGRADE_VIA_WAN), F(L_UPGRADE_VIA_WAN_HINT));

      page.concat(
          F("<form method=\"post\" action=\"/?o=35\"><input type=\"hidden\" "
            "name=\"f\" value=\"{{V}}\"><input type=\"submit\" "
            "class=\"b bw\" value=\"{{L}}\"></form>"));
      page.replace("{{L}}", F(L_UPGRADE_VIA_WAN));
      page.replace("{{V}}", _HtmlResponse);
      closeSection(page);
    }
  }

  openMessageSection(page, F(L_UPGRADE_FIRMWAR_YOUR_CURRENT_FIRMWARE), F(""));
  page.concat(FPSTR(HTTP_MESSAGE_LINE_ITEM));
  page.replace("{{I}}", F(L_UPGRADE_FIRMWARE_VERSION));
  page.concat(FPSTR(HTTP_MESSAGE_LINE_ITEM));
  page.replace("{{I}}", F(L_UPGRADE_FIRMWARE_DEVICE_NAME));
  page.concat(FPSTR(HTTP_MESSAGE_LINE_ITEM));
  page.replace("{{I}}", F(L_UPGRADE_FIRMWARE_API));
  page.concat(FPSTR(HTTP_MESSAGE_LINE_ITEM));
  page.replace("{{I}}", F(L_UPGRADE_FIRMWARE_DEVICE_ID));
  closeMessageSection(page);
}

void AFESitesGenerator::sitePostUpgrade(String &page, boolean status) {
  openMessageSection(page, F(L_FIRMWARE_UPGRADE), F(""));
  page.concat(FPSTR(HTTP_MESSAGE_LINE_ITEM));
  if (!status) {
    page.replace("{{I}}", F(L_UPGRADE_FAILED));
  } else {
    page.replace("{{I}}", F(L_UPGRADE_SUCCESSFUL));
  }
  page.concat(FPSTR(HTTP_MESSAGE_LINE_ITEM));
  page.replace("{{I}}", F(L_UPGRADE_DEVICE_WILL_BE_REBOOTED));
  closeMessageSection(page);
}

void AFESitesGenerator::siteWANUpgrade(String &page,
                                       const __FlashStringHelper *title) {
  openMessageSection(page, title, F(""));
  page.concat(FPSTR(HTTP_MESSAGE_LINE_ITEM));
  page.replace("{{I}}", F(L_UPGRADE_INTERUPTED));
  page.concat(FPSTR(HTTP_MESSAGE_LINE_ITEM));
  page.replace("{{I}}", F(L_UPGRADE_DONT_PLUG_OFF));
  page.concat(FPSTR(HTTP_MESSAGE_LINE_ITEM));
  page.replace("{{I}}", F(L_UPGRADE_TIME));
  page.concat(FPSTR(HTTP_MESSAGE_LINE_ITEM));
  page.replace("{{I}}", F(L_UPGRADE_AUTO_REBOOT));
  closeMessageSection(page);
}

#endif

void AFESitesGenerator::siteReset(String &page) {
  openMessageSection(page, F(L_UPGRADE_RESTORING_DEFAULT_SETTING), F(""));
  page.concat(FPSTR(HTTP_MESSAGE_LINE_ITEM));
  page.replace("{{I}}", F(L_RESTORE_CONFIGURATION_WILL_BE_REMOVED));
  page.concat(F("</ul>"));
  page.concat(
      F("<input style=\"margin-top:1.5em\" type=\"submit\" class=\"b be\" "
        "value=\"{{L}}\">"));
  page.replace("{{L}}", F(L_UPGRADE_RESTORE_DEFAULT_SETTINGS));
  closeSection(page);
}

void AFESitesGenerator::sitePostReset(String &page) {
  openMessageSection(page, F(L_UPGRADE_RESTORING_DEFAULT_SETTING), F(""));
  page.concat(FPSTR(HTTP_MESSAGE_LINE_ITEM));
  page.replace("{{I}}", F(L_RESTORE_IN_PROGRESS));
  page.concat(FPSTR(HTTP_MESSAGE_LINE_ITEM));
  page.replace("{{I}}",
               F(L_RESTORE_NETWORK_CONNECT_TO_HOTSPOT_AFTER_UPGRADE_AFE));
  page.concat(FPSTR(HTTP_MESSAGE_LINE_ITEM));
  page.replace("{{I}}", F(L_RESTORE_NETWORK_OPEN_PANEL));
  closeMessageSection(page);
}

void AFESitesGenerator::siteExit(String &page, uint8_t command) {
  openMessageSection(page, F(L_UPGRADE_REBOOT), F(""));

  page.concat(FPSTR(HTTP_MESSAGE_LINE_ITEM));
  page.replace("{{I}}", F(L_UPGRADE_REBOOT_IN_PROGRESS));

  page.concat(FPSTR(HTTP_MESSAGE_LINE_ITEM));
  if (command != AFE_MODE_ACCESS_POINT) {
    page.replace("{{I}}", F(L_UPGRADE_SITE_WILL_BE_RELOADED));
  } else {
    page.replace("{{I}}",
                 F(L_RESTORE_NETWORK_CONNECT_TO_HOTSPOT_AFTER_UPGRADE));
    page.concat(FPSTR(HTTP_MESSAGE_LINE_ITEM));
    page.replace("{{I}}", F(L_RESTORE_NETWORK_OPEN_PANEL));
  }

  closeMessageSection(page);
}

void AFESitesGenerator::siteConnecting(String &page) {
  NETWORK configuration;
  Data->getConfiguration(&configuration);
  openMessageSection(page, F(L_NETWORK_ALMOST), F(""));
  page.concat(FPSTR(HTTP_MESSAGE_LINE_ITEM));
  page.replace("{{I}}", F(L_NETWORK_DEVICE_CONNECTS));
  page.concat(FPSTR(HTTP_MESSAGE_LINE_ITEM));
  page.replace("{{I}}", F(L_NETWORK_CONNECT_TO));
  page.concat(FPSTR(HTTP_MESSAGE_LINE_ITEM));
  page.replace("{{I}}", F(L_NETWORK_SEARCH_FOR_IP_ADDRESS));
  page.replace("{{M}}", WiFi.macAddress());
  page.replace("{{S}}", configuration.ssid);
  closeMessageSection(page);
}

void AFESitesGenerator::siteIndex(String &page, boolean authorized) {
  DEVICE configuration;
  configuration = Device->configuration;

  char _text[42];
  sprintf(_text, "%s: %s", L_DEVICE, configuration.name);
  openMessageSection(page, _text, F(""));

  if (RestAPI->accessToWAN()) {

    RestAPI->sent(_HtmlResponse, AFE_CONFIG_JSONRPC_REST_METHOD_WELCOME);
    if (_HtmlResponse.length() > 0) {
      page.concat(FPSTR(HTTP_MESSAGE_LINE_ITEM));
      page.replace("{{I}}", _HtmlResponse);
    }

    RestAPI->sent(_HtmlResponse, AFE_CONFIG_JSONRPC_REST_METHOD_LATEST_VERSION);
    if (_HtmlResponse.length() > 0) {
      page.concat(FPSTR(HTTP_MESSAGE_LINE_ITEM));
      page.replace("{{I}}", _HtmlResponse);
    }

    RestAPI->sent(_HtmlResponse, AFE_CONFIG_JSONRPC_REST_METHOD_CHECK_PRO);
    if (_HtmlResponse.length() > 0) {
      page.concat(FPSTR(HTTP_MESSAGE_LINE_ITEM));
      page.replace("{{I}}", _HtmlResponse);
    }

    page.concat(F("</ul>"));
  }

  closeMessageSection(page);

  openSection(page, F(L_INDEX_LAUNCH_CONFIGURATION_PANEL),
              F(L_INDEX_LAUNCH_CONFIGURATION_PANEL_HINT));

  if (!authorized) {
    page.concat(F("<h3>"));
    page.concat(F(L_INDEX_WRONG_PASSWORD));
    page.concat(F("</h3>"));
  }
  page.concat(F("<form method=\"post\"><div class=\"cf\"><input name=\"p\" "
                "type=\"password\" "
                "placeholder=\""));
  page.concat(F(L_PASSWORD));
  page.concat(F("\"> <input type=\"submit\" class=\"b bs\" "
                "value=\""));
  page.concat(F(L_INDEX_NORMAL_MODE));
  page.concat(F("\" formaction=\"/?o=0&i="));
  page += AFE_MODE_CONFIGURATION;
  page.concat(F("\" /> <input type=\"submit\" class=\"b be\" value=\""));
  page.concat(F(L_INDEX_HOTSPOT_MODE));
  page.concat(F("\" formaction=\"/?o=0&i="));
  page += AFE_MODE_ACCESS_POINT;
  page.concat(F("\" /></div></form>"));
  closeSection(page);
}

void AFESitesGenerator::siteProKey(String &page) {
  PRO_VERSION configuration;
  Data->getConfiguration(&configuration);
  openSection(page, F(L_PRO_VERSION), F(""));

  if (RestAPI->accessToWAN()) {
    addInputFormItem(page, AFE_FORM_ITEM_TYPE_TEXT, "k", L_PRO_KEY,
                     configuration.serial, "18");

    page.concat(F("<ul class=\"lst\">"));

    RestAPI->sent(_HtmlResponse, AFE_CONFIG_JSONRPC_REST_METHOD_CHECK_PRO);
    if (_HtmlResponse.length() > 0) {
      page.concat(FPSTR(HTTP_MESSAGE_LINE_ITEM));
      page.replace("{{I}}", _HtmlResponse);
    }
    page.concat(F("</ul>"));

  } else {
    page.concat(F("<h3>"));
    page.concat(F(L_PRO_CANNOT_BE_CONFIGURED));
    page.concat(F("</h3>"));
  }
  closeSection(page);
}

#ifdef AFE_CONFIG_HARDWARE_BINARY_SENSOR
void AFESitesGenerator::siteBinarySensor(String &page, uint8_t id) {
  BINARY_SENSOR configuration;
  Data->getConfiguration(id, &configuration);
  char text[19];

  sprintf(text, "%s #%d", L_BINARY_SENSOR, id + 1);

  openSection(page, text, F(""));

  /* Item: name of the sensor */
  addInputFormItem(page, AFE_FORM_ITEM_TYPE_TEXT, "n", L_NAME,
                   configuration.name, "16");

  /* Item: send as switch*/
  addCheckboxFormItem(page, "ss", L_BINARY_SEND_AS_SWITCH, "1",
                      configuration.sendAsSwitch, L_BINARY_SEND_AS_SWITCH_HINT);

  closeSection(page);

  openSection(page, F(L_MCP23017_CONNECTION),
              F(L_BINARY_SENSOR_MCP23017_CONNECTION));

  /* Item: GPIO */
  addListOfGPIOs(page, F("g"), configuration.gpio);

#ifdef AFE_CONFIG_HARDWARE_MCP23017
  /* Item: GPIO from expander */
  page.concat(FPSTR(HTTP_INFO_TEXT));
  page.replace("{{i.v}}", F(L_MCP23017_CONNECTION_VIA_MCP));
  addDeviceI2CAddressSelectionItem(page, configuration.mcp23017.address);
  addListOfMCP23017GPIOs(page, "mg", configuration.mcp23017.gpio);
#endif // AFE_CONFIG_HARDWARE_MCP23017

  closeSection(page);

  openSection(page, F(L_NETWORK_ADVANCED), F(""));

  /* Item: internal pull-up */
  addCheckboxFormItem(page, "pr", L_BINARY_PULLUP_RESISTOR, "1",
                      configuration.internalPullUp);

  /* Item: revert signal */
  addCheckboxFormItem(page, "rs", L_BINARY_SENSOR_SENT_REVERTED_STATE, "1",
                      configuration.revertSignal);

  /* Item: Bouncing */
  page.concat(FPSTR(HTTP_INFO_TEXT));
  page.replace("{{i.v}}", F(L_SWITCH_SENSITIVENESS_HINT));

  char _number[4];
  sprintf(_number, "%d", configuration.bouncing);

  addInputFormItem(page, AFE_FORM_ITEM_TYPE_NUMBER, "b", L_SENSITIVENESS,
                   _number, AFE_FORM_ITEM_SKIP_PROPERTY, "0", "999", "1",
                   L_MILISECONDS);

  closeSection(page);

#ifdef AFE_CONFIG_API_DOMOTICZ_ENABLED
  if (Device->configuration.api.domoticz || Device->configuration.api.mqtt) {
    openSection(page, F("Domoticz"), F(L_DOMOTICZ_NO_IF_IDX_0));
    char _idx[7];
    sprintf(_idx, "%d", configuration.domoticz.idx);
    addInputFormItem(page, AFE_FORM_ITEM_TYPE_NUMBER, "x", "IDX", _idx,
                     AFE_FORM_ITEM_SKIP_PROPERTY,
                     AFE_DOMOTICZ_IDX_MIN_FORM_DEFAULT,
                     AFE_DOMOTICZ_IDX_MAX_FORM_DEFAULT, "1");
    closeSection(page);
  }
#else
  if (Device->configuration.api.mqtt) {
    openSection(page, F(L_BINARY_SENSOR_MQTT_TOPIC), F(L_MQTT_TOPIC_EMPTY));
    addInputFormItem(page, AFE_FORM_ITEM_TYPE_TEXT, "t", L_MQTT_TOPIC,
                     configuration.mqtt.topic, "64");
    closeSection(page);
  }
#endif
}
#endif // AFE_CONFIG_HARDWARE_BINARY_SENSOR

void AFESitesGenerator::generateFooter(String &page, boolean extended) {
  if (Device->getMode() == AFE_MODE_NORMAL && RestAPI->accessToWAN()) {
    RestAPI->sent(_HtmlResponse, AFE_CONFIG_JSONRPC_REST_METHOD_BOTTOM_TEXT);
    if (_HtmlResponse.length() > 0) {
      page.concat(_HtmlResponse);
    }
  }

  page.concat(F("</div></div>"));
  page.replace("{{e.f}}", String(system_get_free_heap_size() / 1024));

  page.replace("{{A}}",
               RestAPI->accessToWAN()
                   ? F("<img style=\"opacity:.4\" "
                       "src=\"http://api.smartnydom.pl/images/"
                       "afe-logo.png\"><br>")
                   : F("<h1 style=\"margin-bottom:0\">AFE Firmware</h1>"));

#ifdef AFE_CONFIG_API_DOMOTICZ_ENABLED
  page.replace("{{f.a}}", F("Domoticz"));
#else
  page.replace("{{f.a}}", F("Standard"));
#endif

#if defined(ESP_4MB)
  page.replace("{{f.s}}", F("4Mb"));
#elif defined(ESP_2MB)
  page.replace("{{f.s}}", F("2Mb"));
#else
  page.replace("{{f.s}}", F("1Mb"));
#endif

#if defined(ESP8285)
  page.replace("{{f.e}}", F("8285"));
#elif defined(ESP32)
  page.replace("{{f.e}}", F("32"));
#else
  page.replace("{{f.e}}", F("8266"));
#endif

  FirmwarePro->Pro.valid ? page.replace("{{f.p}}", F(L_YES))
                         : page.replace("{{f.p}}", F(L_NO));

#ifdef AFE_CONFIG_T1_USE_MAX_HARDWARE
  char _version[sizeof(Firmware.version) + 6];
  sprintf(_version, "%s %s", Firmware.version, "MEGA");
  page.replace("{{f.v}}", _version);
#else
  page.replace("{{f.v}}", Firmware.version);
#endif

  page.replace("{{f.t}}", String(Firmware.type));
  page.replace("{{f.d}}", F(AFE_DEVICE_TYPE_NAME));
  page.replace("{{f.n}}", deviceID);
  page.replace("{{f.l}}", L_LANGUAGE_SHORT);
  page.replace("{{f.h}}", String(AFE_DEVICE_TYPE_ID));

  page.concat(F("</body></html>"));
}

void AFESitesGenerator::openSection(String &page, const char *title,
                                    const __FlashStringHelper *description) {
  page.concat(FPSTR(HTTP_FORM_BLOCK_HEADER));
  page.replace("{{T}}", title);
  page.replace("{{D}}", description);
}

void AFESitesGenerator::openSection(String &page,
                                    const __FlashStringHelper *title,
                                    const __FlashStringHelper *description) {
  page.concat(FPSTR(HTTP_FORM_BLOCK_HEADER));
  page.replace("{{T}}", title);
  page.replace("{{D}}", description);
}

void AFESitesGenerator::openMessageSection(
    String &page, const char *title, const __FlashStringHelper *description) {
  openSection(page, title, description);
  page.concat(F("<ul class=\"lst\">"));
}

void AFESitesGenerator::openMessageSection(
    String &page, const __FlashStringHelper *title,
    const __FlashStringHelper *description) {
  openSection(page, title, description);
  page.concat(F("<ul class=\"lst\">"));
}

void AFESitesGenerator::closeSection(String &page) {
  page.concat(FPSTR(HTTP_FORM_BLOCK_CLOSURE));
}

void AFESitesGenerator::closeMessageSection(String &page) {
  page.concat(F("</ul>"));
  page.concat(FPSTR(HTTP_FORM_BLOCK_CLOSURE));
}

void AFESitesGenerator::addListOfHardwareItem(String &page, uint8_t noOfItems,
                                              uint8_t noOffConnected,
                                              const __FlashStringHelper *field,
                                              const __FlashStringHelper *label,
                                              boolean disabled) {
  page.concat(F("<div class=\"cf\"><label>"));
  page.concat(label);
  page.concat(F("</label><select name=\""));
  page.concat(field);
  page.concat(F("\""));
  if (disabled) {
    page.concat(F(" disabled=\"disabled\""));
  }
  page.concat(F("><option value=\"0\""));
  page.concat((noOffConnected == 0 ? F(" selected=\"selected\"") : F("")));
  page.concat(F(">"));
  page.concat(F(L_NONE));
  page.concat(F("</option>"));
  if (!disabled) {
    for (uint8_t i = 1; i < (noOfItems + 1); i++) {
      page.concat(F("<option value=\""));
      page.concat(i);
      page.concat(F("\""));
      page.concat(noOffConnected == i ? F(" selected=\"selected\"") : F(""));
      page.concat(F(">"));
      page.concat(i);
      page.concat(F("</option>"));
    }
  }
  page.concat(F("</select>"));
  if (disabled) {
    page.concat(F("<span class=\"hint\">("));
    page.concat(F(L_PRO_VERSION));
    page.concat(F(")</span>"));
  }
  page.concat(F("</div>"));
}

#ifdef AFE_CONFIG_FUNCTIONALITY_REGULATOR
void AFESitesGenerator::addRegulatorControllerItem(String &page,
                                                   REGULATOR *configuration) {

  char _value[20];

  openSection(page, F(L_REGULATOR_REGULATION), F(""));
  page.concat(FPSTR(HTTP_ITEM_REGULATOR));
  page.replace("{{L.R}}", F(L_REGULATOR_TURN_ON_IF));
  page.replace("{{i.s0}}",
               configuration->turnOnAbove == 0 ? " selected=\"selected\"" : "");
  page.replace("{{i.s1}}",
               configuration->turnOnAbove == 1 ? " selected=\"selected\"" : "");
  sprintf(_value, "%-.4f", configuration->turnOn);
  page.replace("{{i.v}}", _value);
  page.replace("{{i.n}}", "ta");
  page.replace("{{i.in}}", "on");

  page.concat(FPSTR(HTTP_ITEM_REGULATOR));
  page.replace("{{L.R}}", F(L_REGULATOR_TURN_OFF_IF));
  page.replace("{{i.s0}}", configuration->turnOffAbove == 0
                               ? " selected=\"selected\""
                               : "");
  page.replace("{{i.s1}}", configuration->turnOffAbove == 1
                               ? " selected=\"selected\""
                               : "");
  sprintf(_value, "%-.4f", configuration->turnOff);
  page.replace("{{i.v}}", _value);
  page.replace("{{i.n}}", "tb");
  page.replace("{{i.in}}", "off");

  page.replace("{{LRL}}", F(L_REGULATOR_LOWER));
  page.replace("{{LRH}}", F(L_REGULATOR_HIGHER));

  closeSection(page);
}
#endif // AFE_CONFIG_FUNCTIONALITY_REGULATOR

void AFESitesGenerator::addListOfGPIOs(String &item,
                                       const __FlashStringHelper *field,
                                       uint8_t selected, const char *title) {

  item.concat(FPSTR(HTTP_ITEM_SELECT_OPEN));
  item.replace("{{i.n}}", field);
  item.replace("{{i.l}}", title);
  item.concat(FPSTR(HTTP_ITEM_SELECT_OPTION));
  item.replace("{{i.v}}", String(AFE_HARDWARE_ITEM_NOT_EXIST));
  item.replace("{{i.l}}", F(L_NONE));
  item.replace("{{i.s}}", selected == AFE_HARDWARE_ITEM_NOT_EXIST
                              ? F(" selected=\"selected\"")
                              : F(""));

  for (uint8_t i = 0; i < AFE_NUMBER_OF_GPIOS; i++) {
    item.concat(FPSTR(HTTP_ITEM_SELECT_OPTION));

    item.replace("{{i.v}}", String(pgm_read_byte(GPIOS + i)));
    item.replace("{{i.l}}", String(pgm_read_byte(GPIOS + i)));
    item.replace("{{i.s}}", selected == pgm_read_byte(GPIOS + i)
                                ? F(" selected=\"selected\"")
                                : F(""));
  }
  item.concat(FPSTR(HTTP_ITEM_SELECT_CLOSE));
}

#ifdef AFE_CONFIG_HARDWARE_MCP23017
void AFESitesGenerator::addListOfMCP23017GPIOs(String &item, const char *field,
                                               uint8_t selected,
                                               const char *title) {

  item.concat(FPSTR(HTTP_ITEM_SELECT_OPEN));
  item.replace("{{i.n}}", field);
  item.replace("{{i.l}}", title);
  item.concat(FPSTR(HTTP_ITEM_SELECT_OPTION));
  item.replace("{{i.v}}", String(AFE_HARDWARE_ITEM_NOT_EXIST));
  item.replace("{{i.l}}", F(L_NONE));
  item.replace("{{i.s}}", selected == AFE_HARDWARE_ITEM_NOT_EXIST
                              ? F(" selected=\"selected\"")
                              : F(""));
  char gpioName[3];

  for (uint8_t i = 0; i < AFE_NUMBER_OF_MCP23017_GPIOS; i++) {

    sprintf(gpioName, "%s%d", i < 8 ? "A" : "B",
            pgm_read_byte(MCP23017_GPIOS_ID + i) - (i < 8 ? 0 : 8));
    item.concat(FPSTR(HTTP_ITEM_SELECT_OPTION));
    item.replace("{{i.v}}", String(pgm_read_byte(MCP23017_GPIOS_ID + i)));
    item.replace("{{i.l}}", gpioName);
    item.replace("{{i.s}}", selected == pgm_read_byte(MCP23017_GPIOS_ID + i)
                                ? F(" selected=\"selected\"")
                                : F(""));
  }
  item.concat(FPSTR(HTTP_ITEM_SELECT_CLOSE));
}
#endif

void AFESitesGenerator::addInputFormItem(String &item, const char *type,
                                         const char *name, const char *label,
                                         const char *value, const char *size,
                                         const char *min, const char *max,
                                         const char *step, const char *hint,
                                         boolean readonly) {
  item.concat(F("<div class=\"cf\"><label>"));
  item.concat(label);
  item.concat(F("</label><input name=\""));
  item.concat(name);
  item.concat(F("\" type=\""));
  item.concat(type);
  item.concat(F("\" "));
  if (readonly) {
    item.concat(F("readonly=\"readonly\" "));
  }
  if (strcmp(size, "?") != 0) {
    item.concat(F("maxlength=\""));
    item.concat(size);
    item.concat(F("\" "));
  }
  if (strcmp(type, "number") == 0) {
    if (strcmp(min, "?") != 0) {
      item.concat(F("min=\""));
      item.concat(min);
      item.concat(F("\" "));
    }
    if (strcmp(max, "?") != 0) {
      item.concat(F("max=\""));
      item.concat(max);
      item.concat(F("\" "));
    }
    if (strcmp(step, "?") != 0) {
      item.concat(F("step=\""));
      item.concat(step);
      item.concat(F("\" "));
    }
  }
  item.concat(F("value=\""));
  item.concat(value);
  item.concat(F("\">"));
  if (strcmp(size, "?") != 0) {
    item.concat(F("<span class=\"hint\">Max "));
    item.concat(size);
    item.concat(F(" "));
    item.concat(F(L_NUMBER_OF_CHARS));
    item.concat(F("</span>"));
  }
  if (strcmp(type, "number") == 0) {
    if (strcmp(min, "?") != 0 && strcmp(max, "?") != 0) {
      item.concat(F("<span class=\"hint\">"));
      item.concat(F(L_RANGE));
      item.concat(F(" "));
      item.concat(min);
      item.concat(F(" - "));
      item.concat(max);
      if (strcmp(hint, "?") != 0) {
        item.concat(F(" "));
        item.concat(hint);
      }
      item.concat(F("</span>"));
    } else if (strcmp(hint, "?") != 0) {
      item.concat(F("<span class=\"hint\">"));
      item.concat(hint);
      item.concat(F("</span>"));
    }
  }
  item.concat(F("</div>"));
}

void AFESitesGenerator::_addSelectionFormItem(
    String &item, boolean type, const char *name, const char *label,
    const char *value, boolean checked, const char *hint, boolean disabled) {
  item.concat(FPSTR(HTTP_ITEM_CHECKBOX));
  item.replace("{{i.t}}", type ? "checkbox" : "radio");
  item.replace("{{i.n}}", name);
  item.replace("{{i.l}}", label);
  item.replace("{{i.v}}", value);
  item.replace("{{i.c}}", checked ? " checked=\"checked\"" : "");
  item.replace("{{i.d}}", disabled ? " disabled=\"disabled\"" : "");
  item.replace("{{i.h}}",
               strcmp(hint, AFE_FORM_ITEM_SKIP_PROPERTY) != 0
                   ? "<span class=\"hint\">(" + String(hint) + ")</span>"
                   : "");
}

void AFESitesGenerator::addCheckboxFormItem(String &item, const char *name,
                                            const char *label,
                                            const char *value, boolean checked,
                                            const char *hint,
                                            boolean disabled) {
  _addSelectionFormItem(item, true, name, label, value, checked, hint,
                        disabled);
}

void AFESitesGenerator::addRadioButtonFormItem(
    String &item, const char *name, const char *label, const char *value,
    boolean checked, const char *hint, boolean disabled) {
  _addSelectionFormItem(item, false, name, label, value, checked, hint,
                        disabled);
}

void AFESitesGenerator::addMenuItem(String &item,
                                    const __FlashStringHelper *title,
                                    uint8_t siteId) {
  item.concat(FPSTR(HTTP_MENU_ITEM));
  item.replace("{{i.i}}", FPSTR(title));
  char _number[3];
  sprintf(_number, "%d", siteId);
  item.replace("{{s.i}}", _number);
}

void AFESitesGenerator::addMenuItemExternal(String &item,
                                            const __FlashStringHelper *title,
                                            const __FlashStringHelper *url) {
  item.concat(FPSTR(HTTP_MENU_ITEM_EXTERNAL));
  item.replace("{{i.i}}", title);
  item.replace("{{s.u}}", url);
}

void AFESitesGenerator::addMenuHeaderItem(String &item,
                                          const __FlashStringHelper *title) {
  item.concat(FPSTR(HTTP_MENU_SUBITEMS_HEADER));
  item.replace("{{i.i}}", FPSTR(title));
}

void AFESitesGenerator::addMenuSubItem(String &item, const char *title,
                                       uint8_t numberOfItems, uint8_t siteId) {
  for (uint8_t i = 0; i < numberOfItems; i++) {
    item.concat(FPSTR(HTTP_MENU_SUBITEM));
    item.replace("{{i.i}}", title);
    item.replace("{{i.D}}", String(i));
    item.replace("{{i.y}}", String(i + 1));
    item.replace("{{s.i}}", String(siteId));
  }
}

void AFESitesGenerator::addSelectFormItemOpen(
    String &item, const __FlashStringHelper *name,
    const __FlashStringHelper *label) {
  item.concat(FPSTR(HTTP_ITEM_SELECT_OPEN));
  item.replace("{{i.l}}", label);
  item.replace("{{i.n}}", name);
}

void AFESitesGenerator::addSelectOptionFormItem(String &item, const char *label,
                                                const char *value,
                                                boolean selected) {
  item.concat(FPSTR(HTTP_ITEM_SELECT_OPTION));
  item.replace("{{i.v}}", value);
  item.replace("{{i.l}}", label);
  item.replace("{{i.s}}", selected ? " selected=\"selected\"" : "");
}

void AFESitesGenerator::addSelectFormItemClose(String &item) {
  item.concat(FPSTR(HTTP_ITEM_SELECT_CLOSE));
}

#ifdef AFE_CONFIG_HARDWARE_I2C
void AFESitesGenerator::addDeviceI2CAddressSelectionItem(String &item,
                                                         uint8_t address) {
  AFEI2CScanner I2CScanner;
  I2CScanner.begin();

  addSelectFormItemOpen(item, F("a"), F("I2C " L_ADDRESS));
  addSelectOptionFormItem(item, L_NONE, "0", address == 0);

  char _i2cItemName[90];
  char _number[4];
  for (uint8_t addressToScan = 1; addressToScan < 127; addressToScan++) {
    if (I2CScanner.scan(addressToScan)) {
      sprintf(_i2cItemName, "[0x%X] : %s", addressToScan,
              I2CScanner.getName(addressToScan));
      sprintf(_number, "%d", addressToScan);
      addSelectOptionFormItem(item, _i2cItemName, _number,
                              address == addressToScan);
    }
  }
  addSelectFormItemClose(item);
}
#endif // AFE_CONFIG_HARDWARE_I2C

#ifdef AFE_CONFIG_HARDWARE_GATE
void AFESitesGenerator::addGateStatesListItem(String &item,
                                              const __FlashStringHelper *name,
                                              byte state) {

  addSelectFormItemOpen(item, name, F(L_GATE_SET_STATE));
  addSelectOptionFormItem(item, L_GATE_OPENED, "0", state == AFE_GATE_OPEN);
  addSelectOptionFormItem(item, L_GATE_PARTIALLY_OPENED, "2",
                          state == AFE_GATE_PARTIALLY_OPEN);
  addSelectOptionFormItem(item, L_GATE_CLOSED, "1", state == AFE_GATE_CLOSED);
  addSelectOptionFormItem(item, L_GATE_UNKNOWN_STATE, "9",
                          state == AFE_GATE_UNKNOWN);
  addSelectFormItemClose(item);
}
#endif // AFE_CONFIG_HARDWARE_GATE

#ifdef AFE_CONFIG_HARDWARE_CONTACTRON
void AFESitesGenerator::addGateContactronsListItem(
    String &item, const __FlashStringHelper *name, uint8_t contactronId) {
  char _name[23];
  char _id[4];
  CONTACTRON configuration;

  addSelectFormItemOpen(item, name, F(L_CONTACTRON));
  sprintf(_id, "%d", AFE_HARDWARE_ITEM_NOT_EXIST);
  addSelectOptionFormItem(item, L_NONE, _id,
                          contactronId == AFE_HARDWARE_ITEM_NOT_EXIST);

  for (uint8_t i = 0; i < Device->configuration.noOfContactrons; i++) {
    Data->getConfiguration(i, &configuration);

    if (configuration.name[0] == '\0') {
      sprintf(_name, "%d: %s", i + 1, L_CONTACTRON);
    } else {
      sprintf(_name, "%d: %s", i + 1, configuration.name);
    }
    sprintf(_id, "%d", i);
    addSelectOptionFormItem(item, _name, _id, contactronId == i);
  }
  addSelectFormItemClose(item);
}
#endif // AFE_CONFIG_HARDWARE_CONTACTRON

#ifdef AFE_CONFIG_HARDWARE_LED
void AFESitesGenerator::addLEDSelectionItem(String &item, uint8_t id) {
  char _id[4];
  char _label[4];
  addSelectFormItemOpen(item, F("l"), F("LED"));
  sprintf(_id, "%d", AFE_HARDWARE_ITEM_NOT_EXIST);
  addSelectOptionFormItem(item, L_NONE, _id, id == AFE_HARDWARE_ITEM_NOT_EXIST);

  for (uint8_t i = 0; i < Device->configuration.noOfLEDs; i++) {
    sprintf(_label, "%d", i + 1);
    sprintf(_id, "%d", i);
    addSelectOptionFormItem(item, _label, _id, id == i);
  }
  addSelectFormItemClose(item);
}
#endif // AFE_CONFIG_HARDWARE_LED
