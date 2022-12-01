#include <Wire.h>       //For communicate
#include <MPU6050.h>    //The main library of the MPU6050
//#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#define BLYNK_PRINT Serial

#define RightMotorSpeed       D5   //14 
#define RightMotorDir         D6   //12 
#define LeftMotorSpeed        D7   //13 
#define LeftMotorDir          D8   //15 

MPU6050 mpu;
int16_t ax, ay, az;
int16_t gx, gy, gz;
int m_ax, m_ay;
int data[2];

const uint64_t pipe = 0xE8E8F0F0E1LL;
char auth[] = "jlpmeBbUI95r1BKvrhg83XRFB8EzG63p";  //Blynk Authentication Token
char ssid[] = "Ivory";           //WIFI Name
char pass[] = "kendangsari"; 

 boolean fall = false; 
 boolean trigger1 = false; 
 boolean trigger2 = false; 
 boolean trigger3 = false; 
 byte trigger1count = 0; 
 byte trigger2count = 0; 
 byte trigger3count = 0; 
 int angleChange = 0;

 int minRange = 312;
 int maxRange = 712;

 int minspeed = 450;
 int maxspeed = 1020;
 int nospeed = 0;

 void moveControl(int x, int y)
{
//Move Forward
    if(y >= maxRange && x >= minRange && x<= maxRange)
    {
        digitalWrite( RightMotorDir,HIGH);
        digitalWrite(LeftMotorDir,HIGH);
        analogWrite(RightMotorSpeed, maxspeed);
        analogWrite(LeftMotorSpeed , maxspeed);
    }


//Move Forward Right
    else if(x >= maxRange && y >= maxRange)
    {
        digitalWrite( RightMotorDir, HIGH);
        digitalWrite(LeftMotorDir,HIGH);
        analogWrite(RightMotorSpeed,minspeed);
        analogWrite(LeftMotorSpeed ,maxspeed);
    }

//Move Forward Left
    else if(x <= minRange && y >= maxRange)
    {
        digitalWrite( RightMotorDir,HIGH);
        digitalWrite(LeftMotorDir,HIGH);
        analogWrite(RightMotorSpeed,maxspeed);
        analogWrite(LeftMotorSpeed ,minspeed);
    }

//No Move
    else if(y < maxRange && y > minRange && x < maxRange && x > minRange)
    {
        analogWrite(RightMotorSpeed,nospeed);
        analogWrite(LeftMotorSpeed , nospeed);
    }

//Move Backward
    else if(y <= minRange && x >= minRange && x <= maxRange)
    {
        digitalWrite( RightMotorDir,LOW);
        digitalWrite(LeftMotorDir,LOW);
        analogWrite(RightMotorSpeed,maxspeed);
        analogWrite(LeftMotorSpeed ,maxspeed);
    }

//Move Backward Right
    else if(y <= minRange && x <= minRange)
    {
        digitalWrite( RightMotorDir,LOW);
        digitalWrite(LeftMotorDir,LOW);
        analogWrite(RightMotorSpeed,minspeed);
        analogWrite(LeftMotorSpeed ,maxspeed);
    }

//Move Backward Left
    else if(y <= minRange && x >= maxRange)
    {
        digitalWrite( RightMotorDir,LOW);
        digitalWrite(LeftMotorDir,LOW);
        analogWrite(RightMotorSpeed,maxspeed);
        analogWrite(LeftMotorSpeed ,minspeed);
    }
}


void setup(void){
  Serial.begin(9600);
  Wire.begin();
  pinMode(D4, OUTPUT);
  Blynk.begin(auth, ssid, pass);
  pinMode(RightMotorSpeed, OUTPUT);
  pinMode(LeftMotorSpeed , OUTPUT);
  pinMode( RightMotorDir, OUTPUT);
  pinMode(LeftMotorDir, OUTPUT);

  digitalWrite(RightMotorSpeed, LOW);
  digitalWrite(LeftMotorSpeed , LOW);
  digitalWrite( RightMotorDir, HIGH);
  digitalWrite(LeftMotorDir, HIGH);
}
BLYNK_WRITE(V1)
{
    int x = param[0].asInt();
    int y = param[1].asInt();
    moveControl(x,y);
}
BLYNK_WRITE(V2)
{
   int pinvalue = param.asInt();
   Serial.println(pinvalue);
     if(pinvalue == 1)
  {
  digitalWrite(D4, HIGH);
  }
  else
  {
  digitalWrite(D4, LOW);  
    }
}
void loop(void){
  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
  Blynk.run();

 
  data[0] = map(ax, -17000, 17000, 300, 400 ); //Send X axis data
  data[1] = map(ay, -17000, 17000, 100, 200);  //Send Y axis data
  float Raw_Amp = pow(pow(ax, 2) + pow(ay, 2) + pow(az, 2), 0.5);
  int Amp = Raw_Amp/160;  
 Serial.print("Amp :"); Serial.println(Amp);
  if (Amp <= 2 && trigger2 == false) { //if AM breaks lower threshold (0.4g)     
  trigger1 = true;     
  Serial.println("TRIGGER 1 ACTIVATED");   
  }   
  if (trigger1 == true) {     
  trigger1count++;     
  if (Amp >= 12) { //if AM breaks upper threshold (3g)
       trigger2 = true;
       Serial.println("TRIGGER 2 ACTIVATED");
       trigger1 = false; trigger1count = 0;
     }
   }
   if (trigger2 == true) {
     trigger2count++;
     angleChange = pow(pow(gx, 2) + pow(gy, 2) + pow(gz, 2), 0.5); Serial.println(angleChange);
     if (angleChange >= 30 && angleChange <= 400) { //if orientation changes by between 80-100 degrees       
  trigger3 = true; trigger2 = false; trigger2count = 0;       
  Serial.println(angleChange);       
  Serial.println("TRIGGER 3 ACTIVATED");     
  }   
}   
  if (trigger3 == true) {     
    trigger3count++;     
  if (trigger3count >= 10) {
       angleChange = pow(pow(gx, 2) + pow(gy, 2) + pow(gz, 2), 0.5);
       //delay(10);
       Serial.println(angleChange);
       if ((angleChange >= 0) && (angleChange <= 10)) { //if orientation changes remains between 0-10 degrees         
  fall = true; trigger3 = false; trigger3count = 0;         
  Serial.println(angleChange);       }       
  else {
    trigger3 = false; 
    trigger3count = 0;         
    Serial.println("TRIGGER 3 DEACTIVATED");       
  }//user regained normal orientation         
  }
  }
 
  delay(1000);
  //radio.write(data, sizeof(data));
  if(m_ax > 360){
      //forward            
      //analogWrite(enbA, RightSpd);
      //analogWrite(enbB, LeftSpd);
      digitalWrite(RightMotorSpeed, HIGH);
      digitalWrite(RightMotorDir, LOW);
      digitalWrite(LeftMotorSpeed, HIGH);
      digitalWrite(LeftMotorDir, LOW);
    }
      if(m_ax < 320){
      //backward              
     // analogWrite(enbA, RightSpd);
      //analogWrite(enbB, LeftSpd);
      digitalWrite(RightMotorSpeed, LOW);
      digitalWrite(RightMotorDir, HIGH);
      digitalWrite(LeftMotorSpeed, LOW);
      digitalWrite(LeftMotorDir, HIGH);
    }
    if(m_ay > 180){
      //left
      //analogWrite(enbA, RightSpd);
      //analogWrite(enbB, LeftSpd);
      digitalWrite(RightMotorSpeed, HIGH);
      digitalWrite(RightMotorDir, LOW);
      digitalWrite(LeftMotorSpeed, LOW);
      digitalWrite(LeftMotorDir, HIGH);
    }
    if(m_ay < 110){
      //right
      //analogWrite(enbA, RightSpd);
      //analogWrite(enbB, LeftSpd);
      digitalWrite(RightMotorSpeed, LOW);
      digitalWrite(RightMotorDir, HIGH);
      digitalWrite(LeftMotorSpeed, HIGH);
      digitalWrite(LeftMotorDir, LOW);
    }
     if(m_ax > 330 && m_ax < 360 && m_ay > 130 && m_ay < 160){
      //stop car
      //analogWrite(enbA, 0);
      //analogWrite(enbB, 0);
      digitalWrite(RightMotorSpeed, LOW);
      digitalWrite(RightMotorDir, LOW);
      digitalWrite(LeftMotorSpeed, LOW);
      digitalWrite(LeftMotorDir, LOW);
    }
  
}