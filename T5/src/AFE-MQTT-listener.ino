/* AFE Firmware for smart home devices
  LICENSE: https://github.com/tschaban/AFE-Firmware/blob/master/LICENSE
  DOC: http://smart-house.adrian.czabanowski.com/afe-firmware-pl/ */

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

/* Method is launched after MQTT Message is received */
void MQTTMessagesListener(char *topic, byte *payload, unsigned int length) {

  char _mqttTopic[70];
  Led.on();
  if (length >= 1) { // command arrived
    /* Checking if Relay related message has been received  */
    for (uint8_t i = 0; i < sizeof(Device.configuration.isRelay); i++) {
      if (Device.configuration.isRelay[i]) {
        sprintf(_mqttTopic, "%scmd", Relay[i].getMQTTTopic());

        if (strcmp(topic, _mqttTopic) == 0) {
          if ((char)payload[1] == 'n') {
            Relay[i].on();
            Mqtt.publish(Relay[i].getMQTTTopic(), "state", "on");
          } else if ((char)payload[1] == 'f') {
            Relay[i].off();
            Mqtt.publish(Relay[i].getMQTTTopic(), "state", "off");
          } else if ((char)payload[1] == 'e') {
            MQTTPublishRelayState(i);
          } else if ((char)payload[1] == 'o') { // toggle
            Relay[i].get() == RELAY_ON ? Relay[i].off() : Relay[i].on();
            MQTTPublishRelayState(i);
          }
        }
      } else {
        break;
      }
    }

    sprintf(_mqttTopic, "%scmd", MQTTConfiguration.topic);
    if (strcmp(topic, _mqttTopic) == 0) {
      if ((char)payload[2] == 'b' && length == 6) { // reboot
        Device.reboot(MODE_NORMAL);
      } else if ((char)payload[2] == 'n' && length == 17) { // configurationMode
        Device.reboot(MODE_CONFIGURATION);
      } else if ((char)payload[2] == 't' && length == 14) { // getTemperature
        char temperatureString[6];
        dtostrf(SensorDHT.getTemperature(), 2, 2, temperatureString);
        Mqtt.publish("temperature", temperatureString);
      } else if ((char)payload[2] == 't' && length == 11) { // getHumidity
        char humidityString[6];
        dtostrf(SensorDHT.getHumidity(), 2, 2, humidityString);
        Mqtt.publish("humidity", humidityString);
      } else if ((char)payload[2] == 't' && length == 12) { // getHeatIndex
        char heatIndex[6];
        dtostrf(SensorDHT.getHeatIndex(), 2, 2, heatIndex);
        Mqtt.publish("heatIndex", heatIndex);
      }
    }
  }

  Led.off();
}

/* Metod publishes Relay state (used eg by HTTP API) */
void MQTTPublishRelayState(uint8_t id) {
  if (Device.configuration.mqttAPI) {
    Mqtt.publish(Relay[id].getMQTTTopic(), "state",
                 Relay[id].get() == RELAY_ON ? "on" : "off");
  }
}
