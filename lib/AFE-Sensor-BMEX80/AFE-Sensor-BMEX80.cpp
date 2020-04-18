/* AFE Firmware for smart home devices, Website: https://afe.smartnydom.pl/ */


#include "AFE-Sensor-BMEX80.h"
#ifdef AFE_CONFIG_HARDWARE_BMEX80

AFESensorBMEX80::AFESensorBMEX80(){};

void AFESensorBMEX80::begin(uint8_t id) {
  AFEDataAccess Data;
  configuration = Data.getBMEX80SensorConfiguration(id);
  I2CPORT I2C = Data.getI2CPortConfiguration();

#ifndef AFE_CONFIG_API_DOMOTICZ_ENABLED
  if (strlen(configuration.mqtt.topic) > 0) {
    sprintf(mqttCommandTopic, "%s/cmd", configuration.mqtt.topic);
  } else {
    mqttCommandTopic[0] = '\0';
  }
#endif

#if defined(DEBUG)
  Serial << endl << endl << "-------- BMEX80: Initializing --------";
#endif

  switch (configuration.type) {
  case AFE_BME680_SENSOR:
    _initialized = s6.begin(&configuration, &I2C);
    break;
  case AFE_BME280_SENSOR:
    _initialized = s2.begin(&configuration, &I2C);
    break;
  case AFE_BMP180_SENSOR:
    _initialized = s1.begin(&configuration, &I2C);
  default:
    _initialized = false;
    break;
  }

#ifdef DEBUG
  Serial << endl
         << "Device: " << (_initialized ? "Found" : "Not found: check wiring");
  Serial << endl << "--------------------------------------" << endl;
#endif
}

boolean AFESensorBMEX80::isReady() {
  if (ready) {
    ready = false;
    return true;
  } else {
    return false;
  }
}

void AFESensorBMEX80::listener() {
  if (_initialized) {
    unsigned long time = millis();

    if (startTime == 0) { // starting timer. used for switch sensitiveness
      startTime = time;
    }

    if (time - startTime >= configuration.interval * 1000) {

#if defined(DEBUG)
      Serial << endl
             << endl
             << "--------"
             << " Reading sensor data "
             << "--------";
#endif

      boolean readStatus =
          configuration.type == AFE_BME680_SENSOR
              ? s6.read()
              : configuration.type == AFE_BME280_SENSOR ? s2.read() : s1.read();

      if (readStatus) {
        data =
            configuration.type == AFE_BME680_SENSOR
                ? s6.data
                : configuration.type == AFE_BME280_SENSOR ? s2.data : s1.data;

        applyCorrections();
        ready = true;

#ifdef DEBUG
        Serial << endl
               << "Temperature = " << data.temperature.value << endl
               << "Pressure = " << data.pressure.value
               << ", Relative = " << data.relativePressure.value;
        if (configuration.type != AFE_BMP180_SENSOR) {
          Serial << endl << "Humidity = " << data.humidity.value;
          Serial << endl << "Dew Point = " << data.dewPoint.value;
        }
        if (configuration.type == AFE_BME680_SENSOR) {
          Serial << endl
                 << "Gas Resistance = " << data.gasResistance.value;
          Serial << endl << "IAQ = " << data.iaq.value;
          Serial << endl << "IAQ Accuracy = " << data.iaq.accuracy;
          Serial << endl << "Static IAQ = " << data.staticIaq.value;
          Serial << endl
                 << "Static IAQ Accuracy = " << data.staticIaq.accuracy;
          Serial << endl
                 << "Breath VOC = " << data.breathVocEquivalent.value;
          Serial << endl
                 << "Breath VOC Accuracy = "
                 << data.breathVocEquivalent.accuracy;
          Serial << endl << "CO2 = " << data.co2Equivalent.value;
          Serial << endl
                 << "CO2 Accuracy = " << data.co2Equivalent.accuracy;
        }

      } else {
        Serial << endl << "No data found";
#endif
      }
      startTime = 0;
#ifdef DEBUG
      Serial << endl << "------------------------------------" << endl;
#endif
    }
  }
}

void AFESensorBMEX80::getJSON(char *json) {

  //@TODO Estimate max size of JSON
  StaticJsonBuffer<1000> jsonBuffer;
  JsonObject &root = jsonBuffer.createObject();

  JsonObject &temperature = root.createNestedObject("temperature");
  JsonObject &pressure = root.createNestedObject("pressure");
  JsonObject &relativePressure = root.createNestedObject("relativePressure");

  temperature["value"] = data.temperature.value;
  temperature["unit"] =
      configuration.temperature.unit == AFE_TEMPERATURE_UNIT_CELSIUS ? "C"
                                                                     : "F";
  temperature["correction"] = configuration.temperature.correction;

  pressure["value"] = data.pressure.value;
  pressure["unit"] =
      configuration.pressure.unit == AFE_PRESSURE_UNIT_HPA ? "hPa" : "?";
  pressure["correction"] = configuration.pressure.correction;

  relativePressure["value"] = data.relativePressure.value;
  relativePressure["unit"] = pressure["unit"];

  /* Not applicable for BMP180 Sensor */
  if (configuration.type != AFE_BMP180_SENSOR) {
    JsonObject &dewPoint = root.createNestedObject("dewPoint");
    JsonObject &humidity = root.createNestedObject("humidity");
    JsonObject &heatIndex = root.createNestedObject("heatIndex");

    dewPoint["value"] = data.dewPoint.value;
    dewPoint["unit"] = temperature["unit"];
    humidity["value"] = data.humidity.value;
    humidity["unit"] = "%";
    humidity["correction"] = configuration.humidity.correction;
    humidity["rating"] = data.humidity.rating;

    heatIndex["value"] = data.heatIndex.value;
    heatIndex["unit"] = temperature["unit"];
  }

  /* Only for BME680 Sensor */
  if (configuration.type == AFE_BME680_SENSOR) {
    JsonObject &iaq = root.createNestedObject("iaq");
    JsonObject &staticIaq = root.createNestedObject("staticIaq");
    JsonObject &co2Equivalent = root.createNestedObject("co2Equivalent");
    JsonObject &breathVocEquivalent =
        root.createNestedObject("breathVocEquivalent");
    JsonObject &gasResistance = root.createNestedObject("gasResistance");

    iaq["value"] = data.iaq.value;
    iaq["rating"] = data.iaq.rating;
    iaq["accuracy"] = data.iaq.accuracy;

    staticIaq["value"] = data.staticIaq.value;
    staticIaq["rating"] = data.staticIaq.rating;
    staticIaq["accuracy"] = data.staticIaq.accuracy;

    co2Equivalent["value"] = data.co2Equivalent.value;
    co2Equivalent["unit"] = "ppm";
    co2Equivalent["rating"] = data.co2Equivalent.rating;
    co2Equivalent["accuracy"] = data.co2Equivalent.accuracy;

    breathVocEquivalent["value"] = data.breathVocEquivalent.value;
    breathVocEquivalent["unit"] = "?";
    breathVocEquivalent["accuracy"] = data.breathVocEquivalent.accuracy;

    gasResistance["value"] = data.gasResistance.value;
    gasResistance["unit"] = "kOm";
  }
  //@TODO Estimate max size of JSON
  root.printTo(json, 1000);
}

void AFESensorBMEX80::applyCorrections() {
#ifdef DEBUG
  Serial << endl << "Applying correction to values";
#endif

  AFESensorsCommon calculation;

  if (configuration.temperature.unit == AFE_TEMPERATURE_UNIT_FAHRENHEIT) {
    data.temperature.value =
        calculation.celsiusToFerenheit(data.temperature.value);
  }

  if (configuration.temperature.correction != 0) {
    data.temperature.value += configuration.temperature.correction;
  }

  if (configuration.type != AFE_BMP180_SENSOR) {
    if (configuration.humidity.correction != 0) {
      data.humidity.value += configuration.humidity.correction;
    }

    data.heatIndex.value =
        configuration.temperature.unit == AFE_TEMPERATURE_UNIT_FAHRENHEIT
            ? calculation.heatIndexF((double)data.temperature.value,
                                     (double)data.humidity.value)
            : calculation.heatIndexC((double)data.temperature.value,
                                     (double)data.humidity.value);

    data.dewPoint.value =
        configuration.temperature.unit == AFE_TEMPERATURE_UNIT_FAHRENHEIT
            ? calculation.celsiusToFerenheit(calculation.dewPoint(
                  calculation.ferenheitToCelsius(data.temperature.value),
                  data.humidity.value))
            : calculation.dewPoint(data.temperature.value,
                                   data.humidity.value);

#ifdef AFE_CONFIG_HUMIDITY
    data.humidity.rating =
        calculation.humidityRating(data.humidity.value);
#endif
  }

  if (configuration.pressure.correction != 0) {
    data.pressure.value += configuration.pressure.correction;
  }
  data.relativePressure.value = calculation.relativePressure(
      data.pressure.value, configuration.altitude,
      data.temperature.value);

#ifdef AFE_CONFIG_HARDWARE_BMEX80
  if (configuration.type == AFE_BME680_SENSOR) {
    data.iaq.rating = calculation.iaqRating(data.iaq.value);
    data.staticIaq.rating =
        calculation.iaqRating(data.staticIaq.value);
    data.co2Equivalent.rating =
        calculation.co2Rating(data.co2Equivalent.value);
  }
#endif
}

#if defined(AFE_CONFIG_API_DOMOTICZ_ENABLED) && defined(AFE_CONFIG_HUMIDITY)
afe_humidity_domoticz_state_t AFESensorBMEX80::getDomoticzHumidityState(float value) {
  if (value < 40) {
    return AFE_HUMIDITY_DRY;
  } else if (value >= 40 && value <= 60) {
    return AFE_HUMIDITY_COMFORTABLE;
  } else if (value > 60) {
    return AFE_HUMIDITY_WET;
  } else {
    return AFE_HUMIDITY_NORMAL;
  }
}
#endif // AFE_CONFIG_HARDWARE_BMEX80
#endif
