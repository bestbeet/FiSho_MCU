// DHT Temperature & Humidity Sensor
// Unified Sensor Library Example
// Written by Tony DiCola for Adafruit Industries
// Released under an MIT license.

// Depends on the following Arduino libraries:
// - Adafruit Unified Sensor Library: https://github.com/adafruit/Adafruit_Sensor
// - DHT Sensor Library: https://github.com/adafruit/DHT-sensor-library

#include <ESP8266WiFi.h>
#include <MicroGear.h>
#include <EEPROM.h>

#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

#define DHTPIN            D7         // Pin which is connected to the DHT sensor.

// Uncomment the type of sensor in use:
#define DHTTYPE           DHT11     // DHT 11 
//#define DHTTYPE           DHT22     // DHT 22 (AM2302)
//#define DHTTYPE           DHT21     // DHT 21 (AM2301)

// See guide for details on sensor wiring and usage:
//   https://learn.adafruit.com/dht/overview

DHT_Unified dht(DHTPIN, DHTTYPE);

uint32_t delayMS;

const char* ssid     = "IoT";        // ชื่อ ssid
const char* password = "";    // รหัสผ่าน wifi

#define APPID   "PKPSUNET"          // ให้แทนที่ด้วย AppID
#define KEY     "ixbE6WzAyfcOzhX"            // ให้แทนที่ด้วย Key
#define SECRET  "8DgsUOhQNV59WF7Z9GPee44nu"         // ให้แทนที่ด้วย Secret
#define ALIAS   "FireDetector" // ตั้งชื่อเล่นให้กับอุปกรณ์

WiFiClient client;
int timer = 0;
MicroGear microgear(client);

int buttonpin = D5; // define the flame sensor interface
int analoog = A0; // define the flame sensor interface
int val ;// define numeric variables val
float sensor; //read analoog value
int state = 1;

void onMsghandler(char *topic, uint8_t* msg, unsigned int msglen) { // 
    Serial.print("Incoming message --> ");
    msg[msglen] = '\0';
    Serial.println((char *)msg);
    
    //ถ้าข้อความที่ได้รับ = 1
    if(*(char *)msg == '1'){
        
        digitalWrite(LED_BUILTIN, LOW);   // LED on 

    }else{
        
        digitalWrite(LED_BUILTIN, HIGH);  // LED off
    
    }
}

void onConnected(char *attribute, uint8_t* msg, unsigned int msglen) {
    Serial.println("Connected to NETPIE...");
    microgear.setName(ALIAS);
}

void setup() {
  
  // Listen to message arrived event and connected event
  microgear.on(MESSAGE,onMsghandler);
  microgear.on(CONNECTED,onConnected);
  
  Serial.begin(115200); 
  Serial.println("Starting...");
  
  pinMode (LED_BUILTIN, OUTPUT) ;// define LED as output interface
  pinMode (buttonpin, INPUT) ;// output interface defines the flame sensor
  pinMode (analoog, INPUT) ;// output interface defines the flame sensor
  pinMode (DHTPIN, INPUT);
  delay(10);
  
  if (WiFi.begin(ssid, password)) {
        while (WiFi.status() != WL_CONNECTED) {
            delay(1000);
            Serial.print(".");
      }
  }
    
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
    
  // Init microgeat and connect to NETPIE
  microgear.init(KEY,SECRET,ALIAS);
  microgear.connect(APPID);
  
  // Initialize device.
  dht.begin();
  Serial.println("DHTxx Unified Sensor Example");
  // Print temperature sensor details.
  sensor_t sensor;
  dht.temperature().getSensor(&sensor);
  Serial.println("------------------------------------");
  Serial.println("Temperature");
  Serial.print  ("Sensor:       "); Serial.println(sensor.name);
  Serial.print  ("Driver Ver:   "); Serial.println(sensor.version);
  Serial.print  ("Unique ID:    "); Serial.println(sensor.sensor_id);
  Serial.print  ("Max Value:    "); Serial.print(sensor.max_value); Serial.println(" *C");
  Serial.print  ("Min Value:    "); Serial.print(sensor.min_value); Serial.println(" *C");
  Serial.print  ("Resolution:   "); Serial.print(sensor.resolution); Serial.println(" *C");  
  Serial.println("------------------------------------");
  // Print humidity sensor details.
  dht.humidity().getSensor(&sensor);
  Serial.println("------------------------------------");
  Serial.println("Humidity");
  Serial.print  ("Sensor:       "); Serial.println(sensor.name);
  Serial.print  ("Driver Ver:   "); Serial.println(sensor.version);
  Serial.print  ("Unique ID:    "); Serial.println(sensor.sensor_id);
  Serial.print  ("Max Value:    "); Serial.print(sensor.max_value); Serial.println("%");
  Serial.print  ("Min Value:    "); Serial.print(sensor.min_value); Serial.println("%");
  Serial.print  ("Resolution:   "); Serial.print(sensor.resolution); Serial.println("%");  
  Serial.println("------------------------------------");
  // Set delay between sensor readings based on sensor details.
  delayMS = sensor.min_delay / 1000;
}

void loop() {
  
  sensor = analogRead(analoog);
  Serial.print("Sensor : ");
  Serial.println(sensor);  // display tempature
  
  val = digitalRead (buttonpin) ;// digital interface will be assigned a value of 3 to read val
  Serial.print("Value : ");
  Serial.println(val);
    if (state == 0) // When the flame sensor detects a signal, LED flashes
  {
    digitalWrite (LED_BUILTIN, LOW);
  }
  else
  {
    digitalWrite (LED_BUILTIN, HIGH);
  }
  
  // Delay between measurements.
  delay(delayMS);
  // Get temperature event and print its value.
  sensors_event_t event;  
  dht.temperature().getEvent(&event);
  if (isnan(event.temperature)) {
    Serial.println("Error reading temperature!");
    dht.begin();
  }
  else {
    Serial.print("Temperature: ");
    Serial.print(event.temperature);
    Serial.println(" *C");
  }
  // Get humidity event and print its value.
  dht.humidity().getEvent(&event);
  if (isnan(event.relative_humidity)) {
    Serial.println("Error reading humidity!");
    dht.begin();
  }
  else {
    Serial.print("Humidity: ");
    Serial.print(event.relative_humidity);
    Serial.println("%");
  }
  
//  float humidity = event.relative_humidity;
//  float temperature = event.temperature;

  // Maintain connection to NETPIE
    if (microgear.connected()) {
        Serial.println("..."); 
        microgear.loop();
//        Serial.println(temperature);
        microgear.publish("/fireDetector/flame",sensor);
        microgear.publish("/fireDetector/temp",val);
        Serial.print("published val: ");
        Serial.println(val);
        microgear.publish("/fireDetector/humid",event.relative_humidity);
        microgear.publish("/fireDetector/light",state);
        timer = 0;
    }
    else {
        Serial.println("connection lost, reconnect...");
        if (timer >= 5000) {
            microgear.connect(APPID); 
            timer = 0;
        }
        else timer += 100;
    }
    delay(1000);
  
  Serial.println("");
}
