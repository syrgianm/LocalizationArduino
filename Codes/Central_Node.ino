#include <RF22.h>
#include <RF22Router.h>
#include <string.h>
#define EXHIBIT_ADDRESS 5
#define VISITOR_ADDRESS 6 
#define CENTRAL_ADDRESS 4

#define led 7
#define buzzer 6

RF22Router rf22(CENTRAL_ADDRESS);

 float temperature=0.0;
 float humidity=0.0;
 float x=0.0;
 float y=0.0;
 char buf1[4];
 char buf2[4];
 int received_value;
void setup() {
  pinMode(led,OUTPUT);
  pinMode(buzzer,OUTPUT);
  Serial.begin(9600);
  if (!rf22.init())
    Serial.println("RF22 init failed");
  // Defaults after init are 434.0MHz, 0.05MHz AFC pull-in, modulation FSK_Rb2_4Fd36
  if (!rf22.setFrequency(431.0))
    Serial.println("setFrequency Fail");
  rf22.setTxPower(RF22_TXPOW_20DBM);
  //1,2,5,8,11,14,17,20 DBM
  rf22.setModemConfig(RF22::GFSK_Rb125Fd125);
  //modulation

  // Manually define the routes for this network
  rf22.addRouteTo(EXHIBIT_ADDRESS,EXHIBIT_ADDRESS);
  rf22.addRouteTo(VISITOR_ADDRESS,VISITOR_ADDRESS);

}

void loop() {
  
  uint8_t buf[RF22_ROUTER_MAX_MESSAGE_LEN];
  char incoming[RF22_ROUTER_MAX_MESSAGE_LEN];
  memset(buf, '\0', RF22_ROUTER_MAX_MESSAGE_LEN);
  memset(incoming, '\0', RF22_ROUTER_MAX_MESSAGE_LEN);
  uint8_t len = sizeof(buf);
  uint8_t from;
  if (rf22.recvfromAck(buf, &len, &from))
  {
     buf[RF22_ROUTER_MAX_MESSAGE_LEN - 1] = '\0';
     memcpy(incoming, buf, RF22_ROUTER_MAX_MESSAGE_LEN);
     //Serial.print("got request from : ");
     //Serial.println(from, DEC);
     //received_value = atoi((char*)incoming);
     //Serial.println(received_value);

    if(from == 5)
    {
      
      received_value = atoi(incoming);
      if (received_value==1)
       {
        digitalWrite(led,HIGH);
        digitalWrite(buzzer,HIGH);
        Serial.println("Exhibit in danger");
        delay(1000);
        digitalWrite(led,LOW);
        digitalWrite(buzzer,LOW);
       }
       else
       {
        memcpy(buf1,incoming,4);
        temperature=atoi(buf1);
        temperature=(float)temperature/100.00;
        memcpy(buf2,incoming+4,4);
        humidity=atoi(buf2);
        humidity=(float)humidity/100.00;
        Serial.print("Temperature:");
        Serial.print(temperature);
        Serial.print("   Humidity:");
        Serial.println(humidity);

          if ( temperature>30.0||temperature<10.0||humidity<30.0||humidity>70.0 )
          {
           digitalWrite(led,HIGH);
           digitalWrite(buzzer,HIGH);
           Serial.println("Bad Conditions"); 
           delay(1000);
           digitalWrite(led,LOW);
           digitalWrite(buzzer,LOW);
          }
      
       }
    }
   else 
   {
    memcpy(buf1,incoming,4);
    x=atoi(buf1);
    x=(float)x/100.00;
    memcpy(buf2,incoming+4,4);
    y=atoi(buf2);
    y=(float)y/100.00;
    Serial.print("The coordinates of visitor :(");
    Serial.print(x);
    Serial.print(",");
    Serial.print(y);
    Serial.println(")");
   }
     
       

    
      

    }

  }
