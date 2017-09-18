#include <Arduino.h>
#include <Scheduler.h> 
#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>
#include <OneWire.h>

// Firebase API
#define FIREBASE_HOST "fisho-7fc6a.firebaseio.com"
#define FIREBASE_AUTH "IQJgmypRZvKw2vnTnbP7WSylVViv75UKfyhCZogT"

// Wifi Setting
#define WIFI_SSID "best"
#define WIFI_PASSWORD "bestbeet2538"

int pH = A0;// pin A0 pH


int i = 0, val = 0;
String stat;
float Tempwater;

/////////////////////////////////////////// pH ////////////////////////////////////////////
float pHsensor()
{
  int buf[10];                //buffer for read analog
  for(int i=0;i<10;i++)       //Get 10 sample value from the sensor for smooth the value
  {
    buf[i]=analogRead(SensorPin);
    delay(10);
  }
  for(int i=0;i<9;i++)        //sort the analog from small to large
  {
    for(int j=i+1;j<10;j++)
    {
      if(buf[i]>buf[j])
      {
        int temp=buf[i];
        buf[i]=buf[j];
        buf[j]=temp;
      }
    }
  }
  avgValue=0;
  for(int i=2;i<8;i++)                      //take the average value of 6 center sample
    avgValue+=buf[i];
  float phValue=(float)avgValue*5.0/1024/6; //convert the analog into millivolt
  phValue=3.5*phValue+Offset;                      //convert the millivolt into pH value    
 
  delay(800);
  return phValue;
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


  Scheduler.start(&tempwater_task);
  Scheduler.begin();
} 

void loop() {  
 }
