///////////////// code developed by Team !ABHIMANYU ( Purushotam Kumar Agrawal ---> Puru2411 ) ///////////////////////

// till now, send, recive and control bulb  and Fan, (temp. sensor is to be added)
// recive unlock signal, and send security warning
//solar panel in timer inrterupt
// person counting, lcd printing


#include <TimerOne.h>
#include "Servo.h"
#include <LiquidCrystal_I2C.h>
// Set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x27, 16, 2);

const int servoPin = 6; // servor signal pin
int ldr1 = A1; // pins from soler panel
int ldr2 = A2;
int servoAngle =90;
Servo Servo1;

String str = "";
bool StringReady = false;

int id=0;
String value = "";

int light =0, fan =0, lock =1;
int preLight =0, preFan=0, preSecure =0;
String timeDate = "";
int humidity = 40, preHumidity = 0;
int tempareture = 25, preTempareture = 0;
int people =4, prePeople =0 ;


const int relay1 = 4; // pins of relay
const int relay2 = 7;
const int relay3 = 8;
int val[8][3] = {{0, 0, 0},{1, 0, 0},{0, 1, 0},{0, 0, 1},{1, 1, 0},{1, 0, 1},{0, 1, 1}, {1, 1, 1}};

const int led=5; // pin of light bulb
const int ldr=A0; // LDR sensor
const int ledcheck = 13;


const int ir1 = 2; 
const int ir2 = 3;
int i1, i2;

long long int t, t1;

void setup() {    //####################################### setup #####################################################
  // put your setup code here, to run once:

  pinMode(relay1, OUTPUT);
  digitalWrite(relay1, LOW);
  pinMode(relay2, OUTPUT);
  digitalWrite(relay2, LOW);
  pinMode(relay3, OUTPUT);
  digitalWrite(relay3, LOW);

  pinMode(ldr, INPUT);
  pinMode(led, OUTPUT);
  pinMode(ledcheck, OUTPUT);
  
  Servo1.attach(servoPin);
  pinMode(ldr1, INPUT);
  pinMode(ldr2, INPUT);
  pinMode(servoPin, OUTPUT);
  Servo1.write(90);
  Timer1.initialize(10000000); // interrupt time in micro sec.
  Timer1.attachInterrupt(solar); // solar function will be called in every 10 sec.

  pinMode(ir1, INPUT);
  pinMode(ir2, INPUT);

  lcd.begin();  
  lcd.backlight();

  t1 = millis();
  t = millis();
  
  Serial.begin(115200);
  delay(500);
}

void loop() {     //####################################### loop #####################################################
 
 if(Serial.available()){
   str=Serial.readStringUntil('\n');
   StringReady= true;
  }
 
  if (StringReady)
  {
    deCode();
  }
  
  counter(); // count the person if ant one entering.
  if(prePeople>people)
  {
    //print exit
    Serial.println("1/"+String(people));
    prePeople = people;
  }
  else if(prePeople<people)
  {
    //print enter
    Serial.println("1/"+String(people));
    prePeople = people;
  }

  turn_bulb(light);
  turn_fan(fan);

  if(millis()-t1>30000)
  {
    // print locked
    lock =1;
  }

  if(millis()-t>5000)
  {
    // printall the info to lcd
    lcd.setCursor(0,0);
    lcd.print(timeDate + "   ");
    lcd.setCursor(13,0);
    lcd.print(String(tempareture) + "C");
    lcd.setCursor(0,1);
    lcd.print("H"+String(humidity)+"% "+String(people)+"inside   "); 
    lcd.setCursor(13,1);
    lcd.print(String(light) + String(fan));
    if(preSecure)
      lcd.print("!");
     else
      lcd.print(" ");

    t=millis();
  }

  

}


void deCode()    //####################################### deCode #####################################################
{
  StringReady = false;
    //Serial.println(str);

    id = str.substring(0,1).toInt();
    value = str.substring(2);

    if(id == 1)
    {
      // bulb
      light = value.toInt();
      
    }

    else if(id == 2)
    {
      // fan
      fan = value.toInt();
   
    }
    else if(id == 3)
    {
      // password 
      int data = value.toInt();
      if(data)
      {
        lock =0;
        // print on lcd "welcome home"
        lcd.setCursor(0,1);
        lcd.print("  WELCOME HOME  "); 
        t=millis();
        
        t1=millis();
        if(preSecure !=0)
        {
          Serial.println("5/0");
          preSecure =0;
        }
      }
      else
      {
        lock =1;
        // print on lcd "security breach!"
        lcd.setCursor(0,1);
        lcd.print("Security breach!"); 
        t=millis();
        
        if(preSecure !=1)
        {
          Serial.println("5/1");
          preSecure =1;
        }
      }
        
    }
    else if(id == 4)
    {
      // time and date
      //Serial.println("9/" + value);
      timeDate = value;
      // print time and date on lcd
    }
    else if(id == 5)
    {
      // humidity
      humidity = value.toInt();
      Serial.println("6/" + String(humidity));
    }
    
}


void turn_bulb(int key) //####################################### turn_bulb ##############################################
{
  if(!key || !people)
  {
    analogWrite(led, 0);
    digitalWrite(ledcheck, LOW);
    if(preLight != 0)
    {
      Serial.println("3/0");
      preLight =0;

      lcd.setCursor(0,1);
      lcd.print("   Light OFF   "); 
      t=millis();
    }
    return;
  }
  
  int LDR_Data=analogRead(ldr);
  
  int value=map(LDR_Data,0,250,35, 0);
  if(LDR_Data>=250)
  {
    value =0;
  }
  lcd.setCursor(0,1);
  //lcd.print(String(LDR_Data) + "   " + String(value) + "               ");
  delay(100); 
  analogWrite(led, value);
  digitalWrite(ledcheck, HIGH);
  if(preLight != 1)
  {
    Serial.println("3/1");
    preLight =1;

    lcd.setCursor(0,1);
    lcd.print("   Light ON    "); 
    t=millis();
  }

}


void turn_fan(int key)  //####################################### turn_Fan ##############################################
{
  if(!key)
  {
    fan_speed(0);
    if(preFan != 0)
    {
      Serial.println("4/0");
      preFan =0;

      lcd.setCursor(0,1);
      lcd.print("    Fan OFF    "); 
      t=millis();
    }
    return;
  }
  // take input of temp as "tempareture"
  // no. of people as "people"

  // predict stage
  int i = fan_speed_predict(people, tempareture);

  // set speed to i
  fan_speed(i);
  if(preFan != i)
  {
    Serial.println("4/"+String(i));
    preFan =i;

    lcd.setCursor(0,1);
    lcd.print("  Fan ON at " + String(i) + "  "); 
    t=millis();
  }
}
int fan_speed_predict(int num,int temp)
{
  if(temp<=10 || num==0)return 0;
  if(temp>28 && num>5)return 7;
  if(temp>28 && temp<=32 && num==4)return 6;
  if(temp>32)return 7;
  float mul1=5.0*(temp-10),mul2=20.0*num;
  if(mul1>3*mul2)mul2=2*mul2;
  else if(mul2>3*mul1)mul1=2*mul1;
  float mul=(2*mul1+3*mul2)/5;
  float ans=mul*7/100;
  int stage=ans+1;
  if(stage!=1)stage--;
  return stage; 
}
void fan_speed(int i)
{
  digitalWrite(relay1, val[i][0]);
  digitalWrite(relay2, val[i][1]);
  digitalWrite(relay3, val[i][2]);
}

void solar(){       //########################################## solar ##############################################
 
  int ldr1_data = analogRead(ldr1);
  int ldr2_data = analogRead(ldr2);
  
  while(abs(ldr1_data-ldr2_data)>50){
    ldr1_data = analogRead(ldr1);
    ldr2_data = analogRead(ldr2);
    
    if(ldr1_data>ldr2_data)
      servoAngle+=1;
    else if(ldr1_data<ldr2_data)
      servoAngle-=1;
      
    if(servoAngle<=45)
    { 
      servoAngle+=1;
      break;
    }
    else if(servoAngle>=135)
    { 
      servoAngle-=1;
      break;
    }  
    Servo1.write(servoAngle);
    
  }
}


void counter() {    //########################################## person counter ##############################################
  
  i1 = digitalRead(ir1);
  i2 = digitalRead(ir2);
  
  if(!i1 || !i2)
  { 
    if(lock || preSecure)
    {
      // unuthorized access
      if(preSecure !=1)
      {
          Serial.println("5/1");
          preSecure =1;

        lcd.setCursor(0,1);
        lcd.print("Security breach!"); 
        t=millis();
      }
    }
    //Serial.println("counting...");
    if(!i1)
    {
      while(true)
      {
        i1 = digitalRead(ir1);
        i2 = digitalRead(ir2);
        if(!i2)
        {
          people++;
          lcd.setCursor(0,1);
          lcd.print("Person entered "); 
          t=millis();
          break;
        }
        delay(20);
      }
    }
    
    else if(!i2)
    {
      while(true)
      {
        i1 = digitalRead(ir1);
        i2 = digitalRead(ir2);
        if(!i1)
        {
          people--;

          lcd.setCursor(0,1);
          lcd.print("Person leaved  "); 
          t=millis();
          break;
        }
        delay(20);
      }
    }
    i1 = digitalRead(ir1);
    i2 = digitalRead(ir2);
    while(!i1 || !i2)
    {
      delay(20);
      i1 = digitalRead(ir1);
      i2 = digitalRead(ir2);
    }
  }
}


/*
  Serial.println("1/5"); // people
  delay(10000);
  Serial.println("2/24"); // tempareture
  delay(10000);
  Serial.println("3/1"); // bulb
  delay(10000);
  Serial.println("4/1"); // fan
  delay(10000);
  Serial.println("5/0"); // security 
  delay(10000);
  Serial.println("6/45"); // humidity
  delay(10000);
*/
  
