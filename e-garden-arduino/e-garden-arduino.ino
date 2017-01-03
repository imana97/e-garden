
// ESP8266 LIBRARIES
#include <SoftwareSerial.h>
#include <ESP8266wifi.h>

// SERVO LIBRARIES
#include <Servo.h>

// ADAFRUIT DHT LIBRARIES
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

// WIFI SETTINGS
#define WIFI_SSID       "Oodie"
#define WIFI_PASSWORD   "iloveboodie"

// SERVER SETTINGS
#define EG_SERVER_IP    "10.0.0.12"
#define EG_SERVER_PORT  "5000"
#define EG_USER_ID      "1251"

// DO NOT CHANGE IF USING E-GARDEN SHIELD
// HUMIDITY/TEMPERATURE SENSORS
#define DHTTYPE       DHT11
#define D_HUMID_1     2
#define D_HUMID_2     3

// WATER GAUGE LEVEL
#define D_WATER_GAUGE 4

// 9G SERVO
#define D_SERVO       5

// DIGITAL RELAYS
#define D_RELAY_1     6
#define D_RELAY_2     7
#define D_RELAY_3     8
#define D_RELAY_4     9

// ANALOG FAN CONTROL
#define A_FAN         A5

// ANALOG HEATER CONTROL
#define A_HEATER      A4

// ANALOG PUMP CONTROL
#define A_PUMP        A4

// ANALONG MOISTURE
#define A_MOIST1      A0
#define A_MOIST2      A1
#define A_MOIST3      A2

// WIFI CONTROL
#define sw_serial_rx_pin 11 //  Connect this pin to TX on the esp8266
#define sw_serial_tx_pin 10 //  Connect this pin to RX on the esp8266
#define esp8266_reset_pin 12 // Connect this pin to CH_PD on the esp8266, not reset. (let reset be unconnected)

SoftwareSerial swSerial(sw_serial_rx_pin, sw_serial_tx_pin);

// the last parameter sets the local echo option for the ESP8266 module..
ESP8266wifi wifi(swSerial, swSerial, esp8266_reset_pin);//adding Serial enabled local echo and wifi debug

unsigned long nextPing = 0;

DHT_Unified dht1(D_HUMID_1, DHTTYPE);
DHT_Unified dht2(D_HUMID_2, DHTTYPE);



void setup() {
  swSerial.begin(9600);
  Serial.begin(9600);
  while (!Serial)
    ;
  Serial.println("Starting wifi");


  wifi.begin(); // SETUP WIFI

  dht1.begin(); // start digital humidity sensor 2
  dht2.begin(); // start digital humidity sensor 2

  wifi.connectToAP(WIFI_SSID, WIFI_PASSWORD);
  wifi.connectToServer(EG_SERVER_IP, EG_SERVER_PORT);
  wifi.send(SERVER, "1251:start-connection");
}

void loop() {

  //Make sure the esp8266 is started..
  if (!wifi.isStarted())
    wifi.begin();
 

  //inform the server
  if (millis() > nextPing) {
    wifi.send(SERVER, "1251:isconnected");
    nextPing = millis() + 10000;
  }

  //Listen for incoming messages. It will wait until a message is received, or max 6000ms..
  WifiMessage in = wifi.listenForIncomingMessage(6000);
  if (in.hasData) {
    Serial.println(in.message);
    nextPing = millis() + 10000;
  }
  
  //If you want do disconnect from the server use:
  // wifi.disconnectFromServer();
  
  
} // end loop





































