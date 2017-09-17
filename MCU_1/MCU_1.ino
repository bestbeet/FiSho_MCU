#include <Arduino.h>
#include <Scheduler.h> 
#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS D1 // pin D1 Temprature
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

int Turbiditysensor = A0;// pin A0 Turbidity

// Firebase API
#define FIREBASE_HOST "fisho-7fc6a.firebaseio.com"
#define FIREBASE_AUTH "IQJgmypRZvKw2vnTnbP7WSylVViv75UKfyhCZogT"

// Wifi Setting
#define WIFI_SSID "best"
#define WIFI_PASSWORD "bestbeet2538"

int i = 0, val = 0;
String stat;
float Tempwater;

////////////////////////////////////// Turbidity /////////////////////////////////////////////////
 float Turbidity ()
{
  float val = analogRead(Turbiditysensor);  //create variable to take in analog reading from cell
  float ardval = val*0.00488758553;  //arduino value units 
  float r1 = (50000/ardval)-10000; //R1 value when using Ohm's law
  float I = ardval/r1; //value of I which we are solving for
  float NTUval = I*70000;  //200 = units in NTU
 
  return NTUval;
}


//////////////////////////////////// WaterQulity Task ///////////////////////////////////////////
class WaterQuality : public Task { 
protected:
     void setup() {
    }
    void loop() {
          float temp,turbidity;
          temp = sensors.getTempCByIndex(0);
          sensors.requestTemperatures();
          turbidity = Turbidity();
          Serial.println(temp);
          Serial.println(turbidity);
          Firebase.setFloat("/WaterQuality/Temp/", temp);
          Firebase.setFloat("/WaterQuality/Turbidity/", turbidity);      
    } 
private:
    uint8_t state;
} quality_task;


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
} print_task;

class MemTask : public Task {
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


  Scheduler.start(&quality_task);
/*  Scheduler.start(&print_task);
  Scheduler.start(&mem_task);*/
  Scheduler.begin();
} 

void loop() {  
 }
