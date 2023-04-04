// Flame Sensor Module
#include <Servo.h>
#include <SoftwareSerial.h>

//Create software serial object to communicate with SIM800L
SoftwareSerial mySerial(3, 2); //SIM800L Tx & Rx is connected to Arduino #3 & #2
Servo myservo;  // create servo object to control a servo

int tempPin = A0;
int aqpin = A1;
int moistPin = A2; 
int pirPin = 4;                 // PIR Out pin 
int servopin=5;
int buzzer = 6; //buzzer to arduino pin 9
int pumppin = 7;
int ledpin =8;
int isFlamePin = 9;  // This is our input pin
int val;
int aqValue;
int moistValue;  
int isFlame = HIGH;  // HIGH MEANS NO FLAME
int pirStat = 0;                   // PIR status
int limit = 300; 

void setup() {
  pinMode(isFlamePin, INPUT); //flame sensor should be input as it is giving data     
  pinMode(pirPin, INPUT);
  pinMode(buzzer, OUTPUT); // Set buzzer - pin 9 as an output
  pinMode(ledpin, OUTPUT);
  pinMode(pumppin,OUTPUT);
  digitalWrite(pumppin,HIGH);
  Serial.begin(9600); //begin Serial communication
  mySerial.begin(9600);
  Serial.println("Initializing..."); 
  delay(1000);
  readSMS();
  
}

void loop() {
  updateSerial();
  moistValue = analogRead(moistPin); 
  isFlame = digitalRead(isFlamePin);//Readd the data gien by the flame sensor
  pirStat = digitalRead(pirPin);
  aqValue = analogRead(aqpin);       // read analog input pin 0
  val = analogRead(tempPin);
  float mv = ( val/1024.0)*5000;
  float cel = mv/10;
  
  
  if (isFlame== LOW)  //if it is low
  {
    Serial.println("Flame"); //Print Flame Flame
    buzz();
    sendCall();
  }
   
 if (pirStat == HIGH) {            // if motion detected
   Serial.println("Someone is in the bathroom!!");
   buzz();
   SendSMS("Someone is in the bathroom!!");
   delay(1000);
 } 
 if (moistValue>limit) {
   Serial.println("The soil is dry");
   buzz();
   SendSMS("The soil is dry");
 }
 if(aqValue>200){
  Serial.println("The air is toxic");
  buzz();
  sendCall();
 }
 if(cel>50){
  buzz();
  sendCall();
 }
 delay(100);
}
void opendoor(){
  Serial.println("opening");
  
  myservo.attach(servopin);
  myservo.write(100);              // tell servo to go to position in variable 'pos'
  delay(500);
  myservo.detach();
  delay(100);
}
void closedoor(){
  Serial.println("closing");
  myservo.attach(servopin);
  myservo.write(5);              // tell servo to go to position in variable 'pos'
  delay(500);
  myservo.detach();
  delay(100);
}

void buzz(){
  
  tone(buzzer, 1000); // Send 1KHz sound signal...
  delay(1000);        // ...for 1 sec
  noTone(buzzer);     // Stop sound...
  delay(1000);        // ...for 1sec

}

void updateSerial()
{
  delay(500);
  while (Serial.available()) 
  {
    mySerial.write(Serial.read());//Forward what Serial received to Software Serial Port
  }
  while(mySerial.available()) 
  {
    String d=mySerial.readString();
    if(d.indexOf("A1")>=0)
    {
      Serial.println("opening");
      opendoor();    
    }
    
    if(d.indexOf("A2")>=0){
      Serial.println("closing");
      closedoor();
    }
    if(d.indexOf("B1")>=0){
      Serial.println("pump on");
      digitalWrite(pumppin,LOW);
      delay(1000);
      digitalWrite(pumppin,HIGH);
    }
    if(d.indexOf("B2")>=0){
      Serial.println("pump off");
      digitalWrite(pumppin,HIGH);
    }
    if(d.indexOf("C1")>=0){
      Serial.println("led on");
      digitalWrite(ledpin,HIGH);
    }
    if(d.indexOf("C2")>=0){
      Serial.println("led off");
      digitalWrite(ledpin,LOW);
    }
    }
}

void SendSMS(String msg)
{
  mySerial.println("AT"); //Once the handshake test is successful, it will back to OK
  updateSerial();

  mySerial.println("AT+CMGF=1"); // Configuring TEXT mode
  updateSerial();
  mySerial.println("AT+CMGS=\"+9647834202829\"");//change ZZ with country code and xxxxxxxxxxx with phone number to sms
  updateSerial();
  mySerial.print(msg); //text content
  updateSerial();
  mySerial.write(26);
  readSMS();

}
void sendCall()
{
  mySerial.println("AT"); //Once the handshake test is successful, i t will back to OK
  updateSerial();
  
  mySerial.println("ATD+9647834202829;"); //  change ZZ with country code and xxxxxxxxxxx with phone number to dial
  updateSerial();
  delay(20000); // wait for 20 seconds...
  mySerial.println("ATH"); //hang up
  updateSerial();
  readSMS();
}
void readSMS(){
  mySerial.println("AT"); //Once the handshake test is successful, it will back to OK
  updateSerial();

  mySerial.println("AT+CMGF=1"); // Configuring TEXT mode
  updateSerial();
  mySerial.println("AT+CNMI=1,2,0,0,0"); // Decides how newly arrived SMS messages should be handled
  updateSerial();
  delay(500);
}
