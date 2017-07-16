#include "AFE-Defaults.h"                                                                                                                                       

AFEDefaults::AFEDefaults () {}

void AFEDefaults::set() {

    AFEDataAccess Data;

    NETWORK networkConfiguration;
    FIRMWARE firmwareConfiguration;
    MQTT MQTTConfiguration;
    DOMOTICZ DomoticzConfiguration;
    RELAY RelayConfiguration;
    SWITCH SwitchConfiguration;
    DS18B20 DS18B20Configuration;



    sprintf(networkConfiguration.ssid,"Za Strugiem");
    sprintf(networkConfiguration.password,"julia2105");
    sprintf(networkConfiguration.host,"AFE-Device");
    networkConfiguration.dhcp = true;
    networkConfiguration.ip = IPAddress(0,0,0,0);
    networkConfiguration.gateway = IPAddress(0,0,0,0);
    networkConfiguration.subnet = IPAddress(255,255,255,0);
    networkConfiguration.noConnectionAttempts = 20;
    networkConfiguration.waitTimeConnections = 1;
    networkConfiguration.waitTimeSeries = 60;

    sprintf(firmwareConfiguration.version,"1.0.0");
    firmwareConfiguration.type = 0;
    firmwareConfiguration.autoUpgrade = 0;
    sprintf(firmwareConfiguration.upgradeURL,"http://api.czabanowski.com");

    /* @TODO Only for MQTT */
    sprintf(MQTTConfiguration.host,"");
    MQTTConfiguration.ip = IPAddress(0,0,0,0);
    sprintf(MQTTConfiguration.user,"");
    sprintf(MQTTConfiguration.password,"");
    MQTTConfiguration.port = 1833;
    sprintf(MQTTConfiguration.topic,"/afedevice/");

    /* @TODO Only for Domoticz */
    sprintf(DomoticzConfiguration.host,"");
    DomoticzConfiguration.ip = IPAddress(0,0,0,0);
    sprintf(DomoticzConfiguration.user,"");
    sprintf(DomoticzConfiguration.password,"");
    DomoticzConfiguration.port = 8080;

    RelayConfiguration.present = false;
    RelayConfiguration.timeToOff = 0;
    RelayConfiguration.statePowerOn = 2;
    /* @TODO Only for MQTT */
    RelayConfiguration.stateMQTTConnected = 0;
    sprintf(RelayConfiguration.name,"relay");
    /* @TODO Only for Domoticz */
    RelayConfiguration.idx = 0;
    RelayConfiguration.publishToDomoticz = false;


    SwitchConfiguration.present = false;
    SwitchConfiguration.gpio = 0;
    SwitchConfiguration.type = 0;
    SwitchConfiguration.sensitiveness = 0;
    SwitchConfiguration.functionality = 0;

    DS18B20Configuration.present = false;
    DS18B20Configuration.gpio = 0;
    DS18B20Configuration.correction = 0;
    DS18B20Configuration.interval = 60;
    DS18B20Configuration.unit = 0;

     /* Saving data */
    Data.saveConfiguration(networkConfiguration);
    Data.saveConfiguration(firmwareConfiguration);
    /* @TODO Only for MQTT */
    Data.saveConfiguration(MQTTConfiguration);
    /* @TODO Only for Domoticz */
    Data.saveConfiguration(DomoticzConfiguration);
    Data.saveConfiguration(0,RelayConfiguration);
    Data.saveConfiguration(0,SwitchConfiguration);
    Data.saveConfiguration(DS18B20Configuration);

    Data.saveDeviceMode(2);
    Data.saveRelayState(0,false);
    Data.saveLanguage(0);

}
