
#include <ArduinoMqttClient.h>
#include "LiquidCrystal_I2C.h"
#include <WiFi.h>
#include <string> 
#include <SparkFunTMP102.h> // Used to send and recieve specific information from tmp102 sensor
#include "secret.h"

// To connect with SSL/TLS:
// 1) Change WiFiClient to WiFiSSLClient.
// 2) Change port value from 1883 to 8883.
// 3) Change broker value to a server with a known SSL/TLS root certificate 
//    flashed in the WiFi module.

WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);

// Set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x27, 20, 4);

const char broker[] = "broker.hivemq.com";
int        port     = 1883;
const char topic[]  = "arduino/simple";

const long interval = 10000;
unsigned long previousMillis = 0;

char countString[3];

TMP102 tmpSensor;

void setup() {

  Wire1.setSDA(14);
  Wire1.setSCL(15);
  Wire1.begin();
  Wire.setSDA(20);
  Wire.setSCL(21);
  Wire.begin();
 	lcd.begin();

	// Turn on the blacklight and print a message.
	lcd.backlight();
	lcd.print("Trasmissione dati");
  lcd.setCursor(0,2);
  lcd.print("5A Elettronica 2023");
  delay(4000);
  lcd.clear();
  lcd.setCursor(0,0);

   /*It will return true on success or false on failure to communicate. */
  if(!tmpSensor.begin(0x48,Wire1))
  { 
     lcd.print("Cannot connect " );
     lcd.setCursor(0,1);
     lcd.print("to tmp102 sensor!");
     lcd.setCursor(0,2);
     lcd.print("Stop");
    while(1);
  }

  // attempt to connect to WiFi network:
   lcd.print("Connecting to " );
   lcd.setCursor(0,1);
   lcd.print(ssid);

  while (WiFi.begin(ssid, pass) != WL_CONNECTED) {
    // failed, retry
    lcd.print("." );
    delay(5000);
  }

 lcd.clear();
 lcd.setCursor(0,0);
 lcd.print("Connected!" );
 delay(2000);

  // You can provide a unique client ID, if not set the library uses Arduino-millis()
  // Each client must have a unique client ID
  // mqttClient.setId("clientId");

  // You can provide a username and password for authentication
  // mqttClient.setUsernamePassword("username", "password");
 lcd.clear();
 lcd.setCursor(0,0);
 lcd.print("Connecting to " );
 lcd.setCursor(0,1);
 lcd.print(broker);
 delay(2000);
  
  //Serial.println(broker);

  if (!mqttClient.connect(broker, port)) {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Connection failed " );
    lcd.setCursor(0,1);
    lcd.print("Stop!" );

    while (1);
  }

 lcd. clear();
 lcd.setCursor(0,0);
 lcd.print("Connected to hivemq" );
 delay(2000);
}

void loop() {
  // call poll() regularly to allow the library to send MQTT keep alives which
  // avoids being disconnected by the broker
  mqttClient.poll();

  // to avoid having delays in loop, we'll use the strategy from BlinkWithoutDelay
  // see: File -> Examples -> 02.Digital -> BlinkWithoutDelay for more info
  unsigned long currentMillis = millis();

  float temp = tmpSensor.readTempC();
  
  if (currentMillis - previousMillis >= interval) {
    // save the last time a message was sent
    previousMillis = currentMillis;
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("temperatura: " );
    lcd.print(temp);
    mqttClient.beginMessage(topic);
    //mqttClient.print("weather,location=us-midwest temperature=82 1465839830100400200"); //dataformat influx in telegraf
    mqttClient.print("misure,location=camera temperature="); //dataformat senza timestamp
    mqttClient.print(temp);
    mqttClient.endMessage();

    
  }
}
