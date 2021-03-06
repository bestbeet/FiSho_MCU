#include <Arduino.h>
#include <Servo.h>
#include <Scheduler.h>
#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>
#include <Servo.h>

// Firebase API
#define FIREBASE_HOST "fisho-7fc6a.firebaseio.com"
#define FIREBASE_AUTH "IQJgmypRZvKw2vnTnbP7WSylVViv75UKfyhCZogT"

// Wifi Setting
#define WIFI_SSID "Best"
#define WIFI_PASSWORD "best2538{}"

///////////////////////////////// Ultrasonic Set //////////////////////////////////
const int pingPin = D1;
int inPin = D2;
/////////////////////////////// Servo feed /////////////////////////////
Servo myservo;
//////////////////////////////// PunpIn Set ////////////////////////
int pumpin = D3;
/////////////////////////////// PumpOut Set ////////////////////////
int pumpout = D4;
////////////////////////////// Oxygen Set /////////////////////////////
int oxygen = D5;

////////////////////////////////////////////////////// Calculator Ultrasonic ////////////////////////////////////////////////////////
long microsecondsToCentimeters(long microseconds)
{
  return microseconds / 29 / 2;
}
/////////////////////////////////////////// Calculator Food Level /////////////////////////////////////
float foodpercent(float cm){
  float foodlevelset,foodlevel;
  foodlevelset = Firebase.getFloat("/FoodLevel/LevelSet/");
  foodlevel = ((foodlevelset-cm)/foodlevelset)*100;
  return foodlevel;
}
////////////////////////////////////////////// PumpIn On //////////////////////////////////////////
void pumpinon (){
  digitalWrite(pumpin, HIGH);
  Firebase.setString("/Tank/Pump/","Enable");
}
////////////////////////////////////////////// PumpIn Off ////////////////////////////////////////
void pumpinoff(){
  digitalWrite(pumpin, LOW);
  Firebase.setString("/Tank/Pump/","Disable");
}
////////////////////////////////////////////// PumpOut On /////////////////////////////////////////
void pumpouton(){
  digitalWrite(pumpout, HIGH);
  Firebase.setString("/Tank/PumpOut/","Enable");
}
///////////////////////////////////////////// PumpOut Off ///////////////////////////////////////
void pumpoutoff(){
  digitalWrite(pumpout, LOW);
  Firebase.setString("/Tank/PumpOut/","Disable");
}
///////////////////////////// Feed in Servo Moter /////////////////////////////////////
void feed(int val)
{
  if ( val == 1 )
  {     
    myservo.write(0); 
  }
  else{
    myservo.write(90);
  }
                           
}
//////////////////////////////////// FoodLevel Task ///////////////////////////////////////////
class FoodLevel : public Task {
protected:
     void setup() {
      pinMode(pingPin, OUTPUT);
      pinMode(inPin, INPUT);
      //pinMode(D6, OUTPUT);
      myservo.attach(D6); 
      myservo.write(90);
      Serial.begin (9600);

    }
    void loop() {
          long duration, cm;
          float food,foodlevelset;
          int minute,i,n = 500;
          
          foodlevelset = Firebase.getFloat("/FoodLevel/LevelSet/");
          String Status = Firebase.getString("/FeedSet/Status/");
          minute = Firebase.getInt("/FeedSet/Minute/");
          digitalWrite(pingPin, LOW);
          delayMicroseconds(2);
          
          digitalWrite(pingPin, HIGH);
          delayMicroseconds(5);
          
          digitalWrite(pingPin, LOW);
          duration = pulseIn(inPin, HIGH);
          
          cm = microsecondsToCentimeters(duration);
          Serial.print("CM : ");
          Serial.println(cm);
          
          food = foodpercent(cm);
          
          if ( cm >= 0 && cm <= foodlevelset){
             Firebase.setFloat("/FoodLevel/Level/", food);
          }
          else if (cm <= foodlevelset+10){
             Firebase.setFloat("/FoodLevel/Level/", 0);
          }
          else{
             Firebase.setString("/FoodLevel/Level/", "Error");
          }
          
          if (Status == "Enable"){
            for(i = minute*60*1000;i>=0;i = i-(n*4)){
              //digitalWrite(D6, HIGH);
              myservo.write(0); 
              delay(n);
              //digitalWrite(D6, LOW);      
              myservo.write(90); 
              delay(n);    
            }
            Firebase.setString("/FeedSet/Notification/", "Enable");
            Firebase.setString("/FeedSet/Status/", "Disable");
            delay(500);
            Firebase.setString("/FeedSet/Notification/", "Disable");
          
          }
          else{
            myservo.write(90);
          }
          

          
          
    }
private:
    uint8_t state;
} food_task;

///////////////////////////// PumpIn Task ////////////////////////////////////////////////////////
class Pump : public Task {
protected:
    void setup() {
      pinMode(pumpin, OUTPUT);
      pinMode(pumpout, OUTPUT);
      Serial.begin (9600);
    }

    void loop()  {
          String PumpStatus = Firebase.getString("/Tank/WaterLevel/");

          if( PumpStatus == "Low Level"){
            pumpinon();
            pumpoutoff();
          }
          else if ( PumpStatus == "Dangerous"){
            pumpouton();
            pumpinoff();
          }
          else if ( PumpStatus == "Normal") {
            pumpinoff();
            pumpoutoff();
          }
          else{
            pumpinoff();
            pumpoutoff();
          }
    }
} pump_task;

///////////////////////////////// Oxygen Task //////////////////////////////////////////////
class Oxygen : public Task {
  protected:
    void setup(){
          pinMode(oxygen, OUTPUT);
    }
    void loop(){
          float minutes,timedelay = 0;
          minutes = Firebase.getFloat("/TankSet/TimeOxygen/");
          timedelay = minutes*60*1000;
          digitalWrite(oxygen, HIGH);
          Firebase.setString("/Tank/Oxygen/","Enable");
          delay(timedelay);
          digitalWrite(oxygen, LOW);
          Firebase.setString("/Tank/Oxygen/","Disable");
          delay(timedelay);

    }
} oxygen_task;

///////////////////////////////// Main //////////////////////////////////////////////
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
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Scheduler.start(&food_task);
  Scheduler.start(&pump_task);
  Scheduler.start(&oxygen_task);
  Scheduler.begin();
}

void loop() {
 }
