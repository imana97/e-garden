
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
#define EG_SERVER_IP    "198.211.99.105"
#define EG_SERVER_PORT  "5000"
#define EG_USER_ID      "1251"

// DO NOT CHANGE IF USING E-GARDEN SHIELD
// HUMIDITY/TEMPERATURE SENSORS
#define DHTTYPE         DHT11
#define EG_HUMID1       4

// 9G SERVO
#define EG_SERVO        6

// LED
#define EG_LED          13

// buzzer/ alert
#define EG_ALERT        5

// I2C
#define EG_SDA          A4
#define EG_SCL          A5

// DIGITAL RELAYS
#define EG_RELAY1       A3
#define EG_RELAY2       A2
#define EG_RELAY3       2
#define EG_RELAY4       3

// PWM FAN CONTROL
#define EG_FAN_PWM      9

// HEATER CONTROL
#define EG_VENTILATOR   8

// WATER GAUGE LEVEL
#define EG_WATER_GAUGE  A1

// ANALOG PUMP CONTROL
#define EG_PUMP         7

// ANALONG MOISTURE
#define EG_MOIST1       A0


// WIFI CONTROL
#define sw_serial_rx_pin 11 //  Connect this pin to TX on the esp8266
#define sw_serial_tx_pin 10 //  Connect this pin to RX on the esp8266
#define esp8266_reset_pin 12 // Connect this pin to CH_PD on the esp8266, not reset. (let reset be unconnected)

// LOOP INTERVAL
#define EG_LOOP         10000 // in ms

SoftwareSerial swSerial(sw_serial_rx_pin, sw_serial_tx_pin);

// GLOBAL VARIABLES ########################################

// the last parameter sets the local echo option for the ESP8266 module..
ESP8266wifi wifi(swSerial, swSerial, esp8266_reset_pin); //adding Serial enabled local echo and wifi debug

// allocate a time keeper
unsigned long nextPing = 0;

// create an instance of the humidity/temperature sensor
DHT_Unified dht1(EG_HUMID1, DHTTYPE);



//################ FUNCTIONS ###############################################

// turn on led for 1 second
void update_connection_status() {

  digitalWrite(EG_LED, HIGH);
  delay(1000);
  digitalWrite(EG_LED, LOW);

}

// set the alarm
void alarm_alert() {
  tone(5, 2000, 100);
  delay(150);
  tone(5, 2000, 100);
  delay(150);
  tone(5, 2000, 100);
}

// return the moisture level
int get_moisture_level() {
  return analogRead(EG_MOIST1);
}

//safe pump watering
void watering(int t) {
  digitalWrite(EG_PUMP, HIGH); // turn on the pump
  // wait 2 seconds
  delay(t);
  // turn off pump
  digitalWrite(EG_PUMP, LOW);
};

// return the temperature level.
int get_temp() {
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
int get_humidty() {
  sensors_event_t event;
  dht1.humidity().getEvent(&event);
  if (!isnan(event.relative_humidity)) {
    return event.relative_humidity;
  } else {
    return 18;
  }
}




// water plants untill a certain moisture level
int water_plant(int moist) {
  delay(2000);

  Serial.println("water plants starts");
  // first check if there is water in the tank
  if (digitalRead(EG_WATER_GAUGE) == 0) {
    Serial.println("there is water");
    // there is water in the tank
    if (get_moisture_level() < moist) {
      // water for 2 seconds
      watering(2000);
      return water_plant(moist); // call water function untill the water level is reached.
    } else {
      Serial.println("moisture level reached: " + get_moisture_level());
      // moisture level is reached.
      return moist;
    }
  } else {
    Serial.println("NO WATER");
    // buzz the alert
    alarm_alert();
    // water gauge is empty
    return -1;
  }
}

int set_temp(int temp) {
  if (get_temp() > temp) {
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

  update_connection_status();
  //Serial.println("Service Received: " + service);
  //Serial.println("Value of the Service: " + value);


  /*
     Service commands

     1- water plants until it reaches the value
     2- set temperature to a certail level
  */

  // run the service
  if (service == 1) {
    Serial.println("service 1 received");
    //int m = water_plant(value);
    //wifi.send(SERVER, response + ':' + m);
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


  // RELAY SETUP
  pinMode(EG_RELAY1, OUTPUT);
  digitalWrite(EG_RELAY1, HIGH); // OFF
  pinMode(EG_RELAY2, OUTPUT);
  digitalWrite(EG_RELAY2, HIGH); // OFF
  pinMode(EG_RELAY3, OUTPUT);
  digitalWrite(EG_RELAY3, HIGH); // OFF
  pinMode(EG_RELAY4, OUTPUT);
  digitalWrite(EG_RELAY4, HIGH); // OFF

  // MOTOR SETUP
  pinMode(EG_FAN_PWM, OUTPUT);
  pinMode(EG_VENTILATOR, OUTPUT);
  pinMode(EG_PUMP, OUTPUT);

  // WATER TANK
  pinMode(EG_WATER_GAUGE, INPUT);

  // alert
  pinMode(EG_ALERT, OUTPUT);

  // STATUS LED
  pinMode(EG_LED, OUTPUT);




}

void loop() {

  //Make sure the esp8266 is started..
  if (!wifi.isStarted())
    wifi.begin();

  //###########TESTS################################

  //water_plant(2000);
  digitalWrite(EG_VENTILATOR,HIGH);

  //########## END TESTS ###########################


  //SEND PACKAGE TO SERVER
  if (millis() > nextPing) {
    wifi.send(SERVER, "connected:" + millis());
    nextPing = millis() + EG_LOOP;
  }

  //RECEIVE PACKAGE FROM SERVER It will wait until a message is received, or max 6000ms..
  WifiMessage in = wifi.listenForIncomingMessage(6000);
  if (in.hasData) {
    nextPing = millis() + EG_LOOP;

    //Serial.println(in.message);// PACKAGE MESSAGE.
    // process the incoming message
    process_server_command(in.message);
  }

  //If you want do disconnect from the server use:
  // wifi.disconnectFromServer();


} // end loop







