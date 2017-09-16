#include <Arduino.h>
#include <Scheduler.h> 
#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS D2
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
#define FIREBASE_HOST "fisho-7fc6a.firebaseio.com"
#define FIREBASE_AUTH "IQJgmypRZvKw2vnTnbP7WSylVViv75UKfyhCZogT"
#define WIFI_SSID "best"
#define WIFI_PASSWORD "bestbeet2538"

int i = 0, val = 0;
String stat;
float Tempwater;


class Tempwater : public Task { 
protected:
     void setup() {
        pinMode(D5, OUTPUT);
    }
    void loop() {
          float temp;
          temp = sensors.getTempCByIndex(0);
          sensors.requestTemperatures();
          Firebase.setFloat("/WaterTemp/", temp);
          delay(5000);
          if( temp >= 32){
            digitalWrite(D5, 1);
            Firebase.setString("/FiSho/LED/", "Enable");
          }
          else{
            digitalWrite(D5, 0);
            Firebase.setString("/FiSho/LED/", "Disable");
          }
    } 
private:
    uint8_t state;
} tempwater_task;


class PrintTask : public Task {
protected:
    void setup() {
        pinMode(D1, OUTPUT);
    }
    
    void loop()  {
          stat = Firebase.getString("/FiSho/Status/");
          Serial.println(stat);
          if(stat == "Enable"){
          val =  Firebase.getInt("/FiSho/Volume/") *1000;
          Serial.println(val);
          digitalWrite(D1, 1);
          delay(val);
          digitalWrite(D1, 0);
          Firebase.setString("/FiSho/Status/", "Disable");
          }
          else
             digitalWrite(D1, 0);
    }
} print_task;

class MemTask : public Task {
public:
    void loop() {
        Serial.print("Free Heap: ");
        Serial.print(ESP.getFreeHeap());
        Serial.println(" bytes");
        delay(10000);
    }
} mem_task;


void setup() {
  Serial.begin(9600);
  pinMode(D1,OUTPUT);
  pinMode(D5,OUTPUT);
  WiFi.mode(WIFI_STA);
  // connect to wifi.
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("connecting");
  
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("connected: ");
  Serial.println(WiFi.localIP());
  Serial.begin(9600);
  sensors.begin();
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);


  Scheduler.start(&tempwater_task);
  Scheduler.start(&print_task);
  Scheduler.start(&mem_task);
  Scheduler.begin();
} 

void loop() {  
 }
