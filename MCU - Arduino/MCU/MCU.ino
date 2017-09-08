#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <SoftwareSerial.h>
#include <Servo.h>
#include "ThingSpeak.h"

//Code By ArduinoALL
WiFiClient client;
unsigned long myChannelNumber = 240769;
const char * myWriteAPIKey = "W986B2XBZA8GEZFQ";
const char* ssid = "best";
const char* password = "bestbeet2538";
WiFiServer server(80);

float Tempwater, Waterlevel, pH;
float Turbidity;
int sw = D1; // Switch control Servo feed
int ledPin = D5;
Servo myservo; // Servo feed

SoftwareSerial NodeSerial(D2, D3); // RX | TX
void setup() {
  Serial.begin(115200);

  pinMode(D2, INPUT);
  pinMode(D3, OUTPUT);
  pinMode(sw, INPUT); 
  myservo.attach(D4); 
  myservo.write(90);
  WiFi.begin(ssid, password);
  ///// set moblie /////
  IPAddress local_ip = {192, 168, 137, 45}; //ฟิกค่า IP
  IPAddress gateway = {192, 168, 1, 1}; //ตั้งค่า IP Gateway
  IPAddress subnet = {255, 255, 255, 0}; //ตั้งค่า Subnet
  WiFi.config(local_ip, gateway, subnet); //setค่าไปยังโมดูล
  ThingSpeak.begin(client);
 
  NodeSerial.begin(4800);
}
void loop() {
   
   int val = digitalRead(sw);
   long duration, cm;
   feed(val);
   while (NodeSerial.available() > 0) {
    Tempwater = NodeSerial.parseFloat();
    Turbidity = NodeSerial.parseFloat();
    Waterlevel = NodeSerial.parseFloat();
    
    pH = NodeSerial.parseFloat();
    if (NodeSerial.read() == '\n') {

          Serial.print("NodeMCU or ESP8266"); Serial.print(" : "); 
          Serial.print(Tempwater);Serial.print(" : ");
          Serial.println(Turbidity);Serial.print(" : ");
          Serial.println(Waterlevel);Serial.print(" : ");
          Serial.println(pH);

    }
  }
   /* ThingSpeak.setField(1, Waterlevel);
    ThingSpeak.setField(2, Tempwater);
    ThingSpeak.setField(3, Turbidity);
    ThingSpeak.setField(4, pH);
    ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);*/


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

