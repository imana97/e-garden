
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
#define A_FAN_REVERSE A4

// ANALOG PUMP CONTROL
#define A_PUMP        A3

// ANALONG MOISTURE
#define A_MOIST1      A0
#define A_MOIST2      A1
#define A_MOIST3      A2

// WIFI CONTROL
#define sw_serial_rx_pin 11 //  Connect this pin to TX on the esp8266
#define sw_serial_tx_pin 10 //  Connect this pin to RX on the esp8266
#define esp8266_reset_pin 12 // Connect this pin to CH_PD on the esp8266, not reset. (let reset be unconnected)

// LOOP INTERVAL
#define E_GARDEN_LOOP 10000

SoftwareSerial swSerial(sw_serial_rx_pin, sw_serial_tx_pin);

// GLOBAL VARIABLES ########################################

// the last parameter sets the local echo option for the ESP8266 module..
ESP8266wifi wifi(swSerial, swSerial, esp8266_reset_pin);//adding Serial enabled local echo and wifi debug

unsigned long nextPing = 0;

DHT_Unified dht1(D_HUMID_1, DHTTYPE);
DHT_Unified dht2(D_HUMID_2, DHTTYPE);



//################ FUNCTIONS ###############################################

// return the moisture level
int moisture_level() {
  return (analogRead(A_MOIST1) + analogRead(A_MOIST2) + analogRead(A_MOIST3)) / 3;
}

//safe pump watering
void watering(int t) {
  digitalWrite(A_PUMP, HIGH); // turn on the pump
  // wait 2 seconds
  delay(t);
  // turn off pump
  digitalWrite(A_PUMP, LOW);
};

// return the temperature level. only works with 1 temperature for now.
int temp1() {
  sensors_event_t event;
  dht1.temperature().getEvent(&event);
  if (!isnan(event.temperature)) {
    // temperature available
    return event.temperature;
  } else {
    return 25;
  }
}

// return the humidity level
int humid1() {
  sensors_event_t event;
  dht1.humidity().getEvent(&event);
  if (!isnan(event.relative_humidity)) {
    return event.relative_humidity;
  } else {
    return 50;
  }
}




// water plants untill a certain moisture level
int water_plant(int moist) {

  // first check if there is water in the tank
  if (digitalRead(D_WATER_GAUGE) == 1) {
    // there is water in the tank
    if (moisture_level() < moist) {
      // water for 2 seconds
      watering(2000);
      return water_plant(moist); // call water function untill the water level is reached.
    } else {
      // moisture level is reached.
      return moist;
    }
  } else {
    // water gauge is empty
    return -1;
  }
}

int set_temp(int temp) {
  if (temp1() > temp) {
    // the temperature in the panel is higher than what we want. so we need to cool it down.
    
    
  } else {
    // the temperature in the panel is lower than what we want, so heat it up.

    
  }
} // end function

// get incoming package and interperet it
String get_value(String data, char separator, int index)
{
  int found = 0;
  int strIndex[] = { 0, -1 };
  int maxIndex = data.length() - 1;

  for (int i = 0; i <= maxIndex && found <= index; i++) {
    if (data.charAt(i) == separator || i == maxIndex) {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i + 1 : i;
    }
  }
  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}


// this function takes the server commnad and run the appropriate service and then send a command to server upon successful execution of the service.
void process_server_command(String command) {

  String response = "resp:" + get_value(command, ':', 0).toInt();
  int service = get_value(command, ':', 1).toInt();
  int value = get_value(command, ':', 2).toInt();


  /*
     Service commands

     1- water plants until it reaches the value
     2- set temperature to a certail level
  */

  // run the service
  if (service == 1) {
    int m = water_plant(value);
    wifi.send(SERVER, response + ':' + m);
  }

  if (service == 2) {

  }
};



void setup() {
  swSerial.begin(9600);
  Serial.begin(9600);
  while (!Serial)
    ;
  Serial.println("Starting wifi");


  // WIFI SETUP
  wifi.begin(); // SETUP WIFI
  wifi.connectToAP(WIFI_SSID, WIFI_PASSWORD);
  wifi.connectToServer(EG_SERVER_IP, EG_SERVER_PORT);
  wifi.send(SERVER, "start-connection");


  // HUMIDITY, TEMPERATURE SENSOR SETUP
  dht1.begin(); // start digital humidity sensor 2
  dht2.begin(); // start digital humidity sensor 2


  // RELAY SETUP
  pinMode(D_RELAY_1, OUTPUT);
  digitalWrite(D_RELAY_1, HIGH); // OFF
  pinMode(D_RELAY_2, OUTPUT);
  digitalWrite(D_RELAY_2, HIGH); // OFF
  pinMode(D_RELAY_3, OUTPUT);
  digitalWrite(D_RELAY_3, HIGH); // OFF
  pinMode(D_RELAY_4, OUTPUT);
  digitalWrite(D_RELAY_4, HIGH); // OFF

  // MOTOR SETUP
  pinMode(A_FAN, OUTPUT);
  pinMode(A_FAN_REVERSE, OUTPUT);
  pinMode(A_PUMP, OUTPUT);

  // WATER TANK
  pinMode(D_WATER_GAUGE, INPUT);


}

void loop() {

  //Make sure the esp8266 is started..
  if (!wifi.isStarted())
    wifi.begin();


  //SEND PACKAGE TO SERVER
  if (millis() > nextPing) {
    wifi.send(SERVER, "connected:" + millis());
    nextPing = millis() + E_GARDEN_LOOP;
  }

  //RECEIVE PACKAGE FROM SERVER It will wait until a message is received, or max 6000ms..
  WifiMessage in = wifi.listenForIncomingMessage(6000);
  if (in.hasData) {
    Serial.println(in.message);// PACKAGE MESSAGE.
    nextPing = millis() + E_GARDEN_LOOP;

    // process the incoming message
    process_server_command(in.message);
  }

  //If you want do disconnect from the server use:
  // wifi.disconnectFromServer();


} // end loop







