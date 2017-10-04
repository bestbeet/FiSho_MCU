#include <Arduino.h>
#include <Scheduler.h> 
#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <MCP3008.h>
#define CS_PIN D8
#define CLOCK_PIN D5
#define MOSI_PIN D7
#define MISO_PIN D6
MCP3008 adc(CLOCK_PIN, MOSI_PIN, MISO_PIN, CS_PIN);
////////////////////////////// Deep Sleep ////////////////////////////////////////
#define SECONDS_DS(seconds)  ((seconds)*1000000UL)

 ////////////////////// Water Temprature Setting /////////////////////////////////
#define ONE_WIRE_BUS D1 // pin D1 Temprature
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

////////////////////////// Turbidity Setting ///////////////////////////////////////
int Turbiditysensor;// pin Turbidity

////////////////////////// pH Setting ///////////////////////////////////////
//#define SensorPin A0           // pH meter Analog output to Arduino Analog Input A1
#define Offset 0.00            // deviation compensate
unsigned long int avgValue;     // Store the average value of the sensor feedback

////////////////////////////// Water Level Setting ////////////////////////////////
int W0=0,W1,W2,W3;  // normal
///////////////////////////////// Firebase Setting /////////////////////////////////////
#define FIREBASE_HOST "fisho-7fc6a.firebaseio.com"
#define FIREBASE_AUTH "IQJgmypRZvKw2vnTnbP7WSylVViv75UKfyhCZogT"

//////////////////////////////// Wifi Setting /////////////////////////////////////////////
#define WIFI_SSID "Best"
#define WIFI_PASSWORD "best2538{}"

////////////////////////////////////// Turbidity Functino /////////////////////////////////////////////////
 float Turbidity ()
{
  float val = adc.readADC(1);  //create variable to take in analog reading from cell
  float ardval = val*0.00488758553;  //arduino value units 
  float r1 = (50000/ardval)-10000; //R1 value when using Ohm's law
  float I = ardval/r1; //value of I which we are solving for
  float NTUval = I*70000;  //200 = units in NTU
 
  return NTUval;
}

/////////////////////////////////////////// pH Function ////////////////////////////////////////////
float pHsensor()
{
  int buf[10];                //buffer for read analog
  for(int i=0;i<10;i++)       //Get 10 sample value from the sensor for smooth the value
  {
    buf[i]=adc.readADC(0);
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

///////////////////////////////////////// Water Level Function //////////////////////////////////////////////
int Waterlevel()
{
  int level=0;

  W1 = digitalRead(D2);
  W2 = digitalRead(D3);
  W3 = digitalRead(D4);
  if ( W1 == 0 )
  {
    level = 1;
  }
  if ( W2 == 0 )
  {
    level = 2;
  }
  if ( W3 == 0 )
  {
    level = 3;
  }
  if ( (W1 == 1 && W2 == 1) && W3 == 1 )
  {
    level = 0;
  }
  
  return level;
}

//////////////////////////////////// WaterQulity Task ///////////////////////////////////////////
class WaterQuality : public Task { 
protected:
     void setup() {
    }
    void loop() {
          float temp,turbidity,pH;
          temp = sensors.getTempCByIndex(0);
          sensors.requestTemperatures();
          turbidity = Turbidity();
          pH = pHsensor();
          
          Serial.println(temp);
          Serial.println(turbidity);
          Serial.println(pH);
          
          Firebase.setFloat("/WaterQuality/Temp/", temp);
          Firebase.setFloat("/WaterQuality/Turbidity/", turbidity);
          Firebase.setFloat("/WaterQuality/pH/", pH);     
    } 
private:
    uint8_t state;
} quality_task;


////////////////////////////////// Water Level Task ////////////////////////////////////////////
class WaterLevel : public Task {
protected:
    void setup() {
    }
    
    void loop()  {
      int waterlevel;


      waterlevel = Waterlevel();
      if( waterlevel == 0 )
      {
        Firebase.setString("/Tank/WaterLevel/","Low Level");
      }
      else if ( waterlevel == 1 )
      {
        Firebase.setString("/Tank/WaterLevel/","Normal");
      }
      else if ( waterlevel == 2 )
      {
        Firebase.setString("/Tank/WaterLevel/","Abnormal");
      }
      else if ( waterlevel == 3 )
      {
        Firebase.setString("/Tank/WaterLevel/","Dangerous");
      }
    }
} waterlevel_task;

/*class DeepSleep : public Task {
public:
    void loop() {
        ESP.deepSleep(SECONDS_DS(5));
    }
} sleep_task;*/


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
  Scheduler.start(&waterlevel_task);
  //Scheduler.start(&sleep_task);
  Scheduler.begin();
} 

void loop() {  
 }
