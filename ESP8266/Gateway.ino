#include <ESP8266WiFi.h>
#include <PubSubClient.h> 
#include <ArduinoJson.h> 
#include "Credentials.h"
#include <NTPClient.h>
#include <WiFiUdp.h>

#define NTP_OFFSET   0      // In seconds
#define NTP_INTERVAL 60 * 1000    // In miliseconds
#define NTP_ADDRESS  "europe.pool.ntp.org"

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, NTP_ADDRESS, NTP_OFFSET, NTP_INTERVAL);
//String timestampString;
//unsigned long timestamp; 

////final variables 
int eCO2[5]={412,423,441,800,134};
int tVOC[5]={1000,1200,3000,6000,1500};
int lux[5]={20,60,8000,10,2};
int humidity[5]={66,21,11,9,90};
int pressure[5]={1000,1200,1000,1300,1500};
int temperature[5]={23,24,11,67,12};

char buff[300]={0x00}; //5 bytes for each parameter = 30 *5 
int j=0;
int k=0;
int ctr=0;

//IOT functions
WiFiClient wifiClient;
void wifiConnect() ;
void mqttConnect() ;
void callback(char* topic, byte* payload, unsigned int payloadLength);
PubSubClient client(server, 1883, callback, wifiClient);
void publishData();
void publishJson(unsigned char i);


int publishInterval = 5000; // 5 seconds//Send adc every 5sc
long lastPublishMillis;

  
void setup() {
  // put your setup code here, to run once:
  ctr=0;
  Serial.begin(115200); Serial.println();
  wifiConnect();
  //configureTime();
  timeClient.begin();
  mqttConnect();
}

void loop() {
  // put your main code here, to run repeatedly:
readAndSendData();
//    if (!client.loop()) 
//    {
//    mqttConnect();
 // }
  if(ctr==-1)
  {
    delay(500);
    Serial.print("Enter deep sleep");
    ESP.deepSleep(0); //esp_deep_sleep_start();
    Serial.print("You should not be here");
    Serial.print("You should not be here");

 }
}


void readAndSendData()
{
   if(Serial.available()){
    buff[ctr]=Serial.read();
    Serial.print(buff[ctr]);   
    if (buff[ctr] == 'z') {
      Serial.print(buff[ctr]);
      Serial.print("Enter sleep mode z");   
   
      ESP.deepSleep(0);// esp_deep_sleep_start();
    
    }
    ctr++;
   }
   if(ctr==300)
   { 
    processRawSensorValue();
    sendProcessedData();
    ctr=-1;
  
   }

  }

void processRawSensorValue()
{
    j=0;
    k=0;

    
    for(j;j<300;j+=10)
    {
      
      char _buff[11]={0x00};

      for(int l=0; l<10;l++)      
      {
        _buff[l]=buff[j+l];
      }
      _buff[10]='\0';
     
     if(j<=40) temperature[k]= atoi(_buff);
     else if(j<=90) humidity[k]=atoi(_buff);
     else if(j<=140) pressure[k]=atoi(_buff);
     else if(j<=190) lux[k]=atoi(_buff);
     else if(j<=240) eCO2[k]=atoi(_buff);
     else if(j<=290) tVOC[k]=atoi(_buff);

     k++;
     k=k%5;
      }
}


void sendProcessedData(){
    for(unsigned char i =0 ;i<5;i++)  
    {
        timeClient.update();

//        timestampString=  String(timeClient.getEpochTime()-60*(5-i));
//        timestamp=  timeClient.getEpochTime()-60*(5-i);
  
   // timestampString=String(printUnixTime()-60*(5-i));
    publishJson(i);
    }
  }


































/*
 * 
 * Publish Data
 * 
 * 
 */



void publishData()
{
  for(unsigned char i =0 ;i<5;i++)  publishJson(i);
  }


  
void publishJson(unsigned char i)
{
  
StaticJsonBuffer<600> jsonBuffer;
JsonObject& root = jsonBuffer.createObject();

JsonObject& object1 = root.createNestedObject("Sensor1");
object1["t"] = (float)temperature[i]/100; //C .2zecimale
object1["h"] = (float)humidity[i]/100; //% .2zecimale
object1["p"] = (float)pressure[i]/1000; //hPA .3 zecimale
object1["l"] = (float)lux[i]/100; //1ux .2 zecimale
object1["e"] = eCO2[i]; // ppm
object1["T"] = tVOC[i]; //ppm 
String a = "000";
//if((timeClient.getEpochTime()/1000000000)>3)  timeClient.update();


String ab = String(timeClient.getEpochTime()-60*(5-i));
ab.concat(a);
object1["ts"]= ab;


 char buff[600];
 root.printTo(buff, sizeof(buff));
 Serial.println("publishing device metadata:"); Serial.println(buff);
 if (client.publish(eventTopic, buff)) 
 {
   Serial.println("device Publish ok");
 } 
 else 
 {
   Serial.print("device Publish failed:");
 }
}

















/*
 * 
 * Connecting to wifi and mqtt
 * 
 * 
 */




void wifiConnect()
{
  Serial.print("Connecting to "); Serial.print(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.print("nWiFi connected, IP address: "); Serial.println(WiFi.localIP());

}

void mqttConnect() 
{
  if (!!!client.connected()) 
  {
    Serial.print("Reconnecting MQTT client to "); Serial.println(server);
    while (!!!client.connect(clientId, authMethod, token)) 
    {
      Serial.print(".");
      delay(500);
    }
    if (client.subscribe(cmdTopic)) 
    {
      Serial.println("subscribe to responses OK");
    } 
    else 
    {
      Serial.println("subscribe to responses FAILED");
    }
    Serial.println();
  }
}



void callback(char* topic, byte* payload, unsigned int payloadLength) 
{
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < payloadLength; i++) 
  {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Switch on the LED if an 1 was received as first character
  if (payload[0] == '1') 
  {
    digitalWrite(2, LOW);   // Turn the LED on (Note that LOW is the voltage level
    // but actually the LED is on; this is because
    // it is acive low on the ESP-01)
  } 
  else 
  {
    digitalWrite(2, HIGH);  // Turn the LED off by making the voltage HIGH
  }
  
}
