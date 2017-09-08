#include <SoftwareSerial.h>
#include <OneWire.h>
#include <DallasTemperature.h>
// Config Value
#define ONE_WIRE_BUS 4 // pin 4 Temprature
int Turbiditysensor = A1;// pin A1 Turbidity
const int pingPin = 5; // Trig pin 5
int inPin = 6; // Echo pin 6
#define SensorPin 2           //pH meter Analog output to Arduino Analog Input 2
#define Offset 0.00            //deviation compensate
unsigned long int avgValue;     //Store the average value of the sensor feedback

SoftwareSerial ArduinoSerial(3, 2); // RX 3 , TX 2
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);


void setup() {
  Serial.begin(115200);
  ArduinoSerial.begin(4800);

  pinMode(13, OUTPUT);
}
void loop() {
  float Tempwater, Turbidity, pH;
  long Waterlevel;
  Tempwater = tempwater();
  Turbidity = turbidity();
  Waterlevel = waterlevel();
  pH = pHsensor();
   
  Serial.print("Temp :");
  Serial.print(Tempwater);
  Serial.println(" Celsius");
  Serial.print("Turbididty :");
  Serial.print(Turbidity);
  Serial.println(" NTU");
  Serial.print("WaterLevel :");
  Serial.print(Waterlevel);
  Serial.println(" cm");
  Serial.print("pH :");
  Serial.println(pH);
  ArduinoSerial.print(Tempwater); ArduinoSerial.print(" ");
  ArduinoSerial.print(Turbidity); ArduinoSerial.print(" ");
  ArduinoSerial.print(Waterlevel); ArduinoSerial.print(" ");
  ArduinoSerial.print(pH); ArduinoSerial.print("\n");
  
  
  delay(2000);
}
float tempwater()
{
  float temp;
  temp = sensors.getTempCByIndex(0);
  sensors.requestTemperatures(); 
  return temp;
}
///////////////////////////////////////////////////// Function Waterlevel //////////////////////////////////////////////////////
long waterlevel()
{
  long duration,cm;
  pinMode(pingPin, OUTPUT);
  digitalWrite(pingPin, LOW);
  delayMicroseconds(2);
  digitalWrite(pingPin, HIGH);
  delayMicroseconds(5);
  digitalWrite(pingPin, LOW);
  pinMode(inPin, INPUT);
  duration = pulseIn(inPin, HIGH);
  cm = microsecondsToCentimeters(duration);
  
  return cm;
}
////////////////////////////////////////////////////// Function Calculator Ultrasonic ////////////////////////////////////////////////////////
long microsecondsToCentimeters(long microseconds) 
{
  return microseconds / 29 / 2;
}
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
////////////////////////////////////// Turbidity /////////////////////////////////////////////////
float turbidity ()
{
  float val = analogRead(Turbiditysensor);  //create variable to take in analog reading from cell
  float ardval = val*0.00488758553;  //arduino value units 
  float r1 = (50000/ardval)-10000; //R1 value when using Ohm's law
  float I = ardval/r1; //value of I which we are solving for
  float NTUval = I*70000;  //200 = units in NTU
 
  return NTUval;
}

