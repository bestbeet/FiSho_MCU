#include <Arduino.h>
#include <Scheduler.h> 
#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>
#include <OneWire.h>

// Firebase API
#define FIREBASE_HOST "fisho-7fc6a.firebaseio.com"
#define FIREBASE_AUTH "IQJgmypRZvKw2vnTnbP7WSylVViv75UKfyhCZogT"

// Wifi Setting
#define WIFI_SSID "Best"
#define WIFI_PASSWORD "best2538{}"

///////////////////////////////// Ultrasonic Set //////////////////////////////////
const int pingPin = D1;
int inPin = D2;

////////////////////////////////////////////////////// Calculator Ultrasonic ////////////////////////////////////////////////////////
long microsecondsToCentimeters(long microseconds) 
{
  return microseconds / 29 / 2;
}

//////////////////////////////////// FoodLevel Task ///////////////////////////////////////////
class FoodLevel : public Task { 
protected:
     void setup() {
      Serial.begin (9600);
      pinMode(TRIGGER_PIN, OUTPUT);
      pinMode(ECHO_PIN, INPUT);
    }    
    void loop() {
          long duration, cm;
          pinMode(pingPin, OUTPUT);
          digitalWrite(pingPin, LOW);
          delayMicroseconds(2);
          digitalWrite(pingPin, HIGH);
          delayMicroseconds(5);
          digitalWrite(pingPin, LOW);
          pinMode(inPin, INPUT);
          duration = pulseIn(inPin, HIGH);
          cm = microsecondsToCentimeters(duration);
          Serial.print(cm);
          Serial.println("cm");
          delay(200);
          Firebase.setFloat("/FoodLevel/Temp/", cm);
    } 
private:
    uint8_t state;
} food_task;


/*class PrintTask : public Task {
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
} print_task;*/

/*class MemTask : public Task {
public:
    void loop() {
        Serial.print("Free Heap: ");
        Serial.print(ESP.getFreeHeap());
        Serial.println(" bytes");
        delay(10000);
    }
} mem_task;*/


void setup() {
  Serial.begin(9600);
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


  Scheduler.start(&food_task);
  Scheduler.begin();
} 

void loop() {  
 }
