///////////////// code developed by Team !ABHIMANYU ( Purushotam Kumar Agrawal ---> Puru2411 ) ///////////////////////

// importing libraey
#include<ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include<ThingSpeak.h>
#include <TimeLib.h>
#include <WidgetRTC.h> 


// You should get Auth Token in the Blynk App. this token is used to send data from right server
char auth[] = "fksGd6nME8iXfIEqYXZFvNLpEY9Z-sVo";  // if you change token, please update it on IFTTT site also

char ip[] = "184.106.153.149";         // ip of thinkSpeak server
long channelId= 1193703;               // Channel id
const char* api= "YDPQCJIAPFSPUIKT";   // Api key to write on the server

// Your WiFi credentials.
char ssid[] = "purushotamhonor9n";
char pass[] = "Pka@1234";


// decleration of pins and variables
int ledPin = 2;
long long int t =0, t1 =0;
String str = "";
bool StringReady = false;
int id=0, value=0;

String correctPass = "Abhi@123";
int wrongCounter =0;

WidgetLCD lcd(V0);
WiFiClient client;

// for time and date
BlynkTimer timer;
WidgetRTC rtc;


// Digital clock display of the time
void clockDisplay()
{
  // You can call hour(), minute(), ... at any time
  // Please see Time library examples for details

  String currentTime = String(hour()) + ":" + minute();
  String currentDate = String(day()) + "-" + month(); // + "-" + year(); add this for year
  String str = "4/"+currentTime+"||"+currentDate;
  Serial.println(str);
  
}

BLYNK_CONNECTED() {
  // Synchronize time on connection
  rtc.begin();
}


// This function will be called every time Slider Widget
// in Blynk app writes values to the Virtual Pin 1
BLYNK_WRITE(V1)
{
  int pinValue = param.asInt(); // assigning incoming value from pin V1 to a variable
  // You can also use:
  // String i = param.asStr();
  // double d = param.asDouble();
  
  if(pinValue)
  {
    Serial.println("1/1");
    digitalWrite(ledPin,HIGH);
    lcd.print(0, 1, "     LED ON     ");
    t = millis();
  }
  if(!pinValue)
  {
    Serial.println("1/0");
    digitalWrite(ledPin,LOW);
    lcd.print(0, 1, "     LED OFF    ");
    t = millis();
  }
}

BLYNK_WRITE(V2)
{
  int pinValue = param.asInt();
  
  if(pinValue)
  {
    Serial.println("2/1");
    lcd.print(0, 1, "     FAN ON     ");
    t = millis();
  }
  if(!pinValue)
  {
    Serial.println("2/0");
    lcd.print(0, 1, "     FAN OFF    ");
    t = millis();
  }
}

BLYNK_WRITE(V3)
{
  String password = param.asStr(); 

    if(correctPass == password)
    {
      lcd.print(0, 1, "  WELCOME HOME  ");
      t = millis();
      Serial.println("3/1");
      wrongCounter =0;
    }
    else
    {
      lcd.print(0, 1, "WRONG PASSWORD!!");
      wrongCounter += 1;
    }
    
    if(wrongCounter>=4)
    {
      lcd.print(0, 1, "HOME IS LOCKED!!");
      t = millis();
      Serial.println("3/0");
    }
}


void setup()
{
  // Debug console
  Serial.begin(115200);

  Blynk.begin(auth, ssid, pass);
  // You can also specify server:
  //Blynk.begin(auth, ssid, pass, "blynk-cloud.com", 80);
  //Blynk.begin(auth, ssid, pass, IPAddress(192,168,1,100), 8080);

  lcd.clear(); //Use it to clear the LCD Widget
  lcd.print(0, 0, "ABHIMANYU's HOME"); // use: (position X: 0-15, position Y: 0-1, "Message you want to print")
  t = millis();
  
  pinMode(ledPin,OUTPUT);
  digitalWrite(ledPin,LOW);

  setSyncInterval(10 * 60); // Sync interval in seconds (10 minutes)
  timer.setInterval(60000L, clockDisplay); // Display digital clock every 60 seconds

  ThingSpeak.begin(client);

  t1 = millis();
  
}

void loop()
{
  Blynk.run();
  timer.run();

  if(millis() - t > 5000)
  {
    lcd.print(0, 0, "ABHIMANYU's HOME");
    lcd.print(0, 1, "                ");
    t = millis();
  }
  if(Serial.available()){
   str=Serial.readStringUntil('\n');
   StringReady= true;
  }
 
  if (StringReady)
  {
    StringReady = false;
    
    id = str.substring(0,1).toInt();
    value = str.substring(2).toInt();

    if(client.connect(ip,80)){
     
      if(id == 1)
      {
        // people
        ThingSpeak.setField(1,value);
        lcd.print(0, 1, String(value) + " PEOPLE INSIDE");
        t = millis();
      }
  
      else if(id == 2)
      {
        // tempareture
        ThingSpeak.setField(2,value);
        lcd.print(0, 1, String(value) + "C tempareture ");
        t = millis();
      }
      else if(id == 3)
      {
        // bulb
        ThingSpeak.setField(3,value);
      }
      else if(id == 4)
      {
        // fan
        ThingSpeak.setField(4,value);
      }
      else if(id == 5)
      {
        // security warning
        if(value)
          Blynk.notify("Warning!! unathorized access to your home.");
          Blynk.email("teamabhimanyu.eclub@gmail.com", "your home is in danger", "Warning!! someone is trying to enter in house with wrong password.");
        ThingSpeak.setField(5,value);
      }
      else if(id == 6)
      {
        // humidity
        ThingSpeak.setField(6,value);
        lcd.print(0, 1, String(value) + " % HUMIDITY");
        t = millis();
      }
      
    }
  }

  if(millis() - t1 > 60000)
  {
    ThingSpeak.writeFields(channelId,api);
    //Serial.println("sending data");
    t1 = millis();
  }

}
