#include <RF22.h>
#include <RF22Router.h>
#include <dht.h>

#define EXHIBIT_ADDRESS 5
#define CENTRAL_ADDRESS 4

#define MilliSecToSec 1000

#define Touch_pin 3
#define DHT11_PIN 7

RF22Router rf22(EXHIBIT_ADDRESS);
dht DHT;
int sensorVal = 23;
long randNumber;
int max_delay=3000;
boolean t=false; //Variable for help us in time when Dht take measurement
unsigned long startTime=0;
unsigned long currentTime;
void setup() {
   pinMode(Touch_pin,INPUT);
   Serial.begin(9600);
  if (!rf22.init())
    Serial.println("RF22 init failed");
  // Defaults after init are 434.0MHz, 0.05MHz AFC pull-in, modulation FSK_Rb2_4Fd36
  if (!rf22.setFrequency(431.0))
    Serial.println("setFrequency Fail");
  rf22.setTxPower(RF22_TXPOW_20DBM);
  //1,2,5,8,11,14,17,20 DBM
  //rf22.setModemConfig(RF22::OOK_Rb40Bw335  );
  rf22.setModemConfig(RF22::GFSK_Rb125Fd125);
  // Manually define the routes for this network
  rf22.addRouteTo(CENTRAL_ADDRESS, CENTRAL_ADDRESS);
  sensorVal = analogRead(A0);
  randomSeed(sensorVal);// (μία μόνο φορά μέσα στην setup)
  
  
  

}

void loop() {
   //Read Touch Sensor Value
int   digitalValue = digitalRead (Touch_pin);
  //If TouchSensor True then send Data to Central_Node
  if(digitalValue==1)
  {
  char data_read[RF22_ROUTER_MAX_MESSAGE_LEN];
  uint8_t data_send[RF22_ROUTER_MAX_MESSAGE_LEN];
  memset(data_read, '\0', RF22_ROUTER_MAX_MESSAGE_LEN);
  memset(data_send, '\0', RF22_ROUTER_MAX_MESSAGE_LEN);
  sprintf(data_read, "%d",digitalValue);
  data_read[RF22_ROUTER_MAX_MESSAGE_LEN - 1] = '\0';
  memcpy(data_send, data_read, RF22_ROUTER_MAX_MESSAGE_LEN);
  boolean Aloha_flag = false;
  while (!Aloha_flag)
  {
    if (rf22.sendtoWait(data_send, sizeof(data_send), CENTRAL_ADDRESS) != RF22_ROUTER_ERROR_NONE)
    {
      Serial.println("sendtoWait failed");
      randNumber=random(200,max_delay);
      Serial.println(randNumber);
      delay(randNumber);
    }
    else
    {
      Aloha_flag = true;
      Serial.println("sendtoWait Succesful");
    }

  }
  }
  //Send Humidity and Temp every 5 minutes
  if(t==true)
  {
   startTime=millis()*pow(10.0,-3.0)/60.0;    //Convert to Minutes
   t=false;
  }
  currentTime=millis()*pow(10.0,-3.0)/60.0-startTime;  //Convert to Minutes
  
  if(currentTime>5.0)
  {
    int chk = DHT.read11(DHT11_PIN); //take measurement
    Serial.println(DHT.temperature);
    Serial.println(DHT.humidity);
    char data_read[RF22_ROUTER_MAX_MESSAGE_LEN];
    uint8_t data_send[RF22_ROUTER_MAX_MESSAGE_LEN];
    memset(data_read, '\0', RF22_ROUTER_MAX_MESSAGE_LEN);
    memset(data_send, '\0', RF22_ROUTER_MAX_MESSAGE_LEN);
    float temperature=DHT.temperature*100;
    temperature=(int)temperature;
    float humidity=DHT.humidity*100; 
    humidity=(int)humidity;

    sprintf(data_read, "%d%d",(int)temperature,(int)humidity);
    data_read[RF22_ROUTER_MAX_MESSAGE_LEN - 1] = '\0';
    memcpy(data_send, data_read, RF22_ROUTER_MAX_MESSAGE_LEN);
    boolean Aloha_flag = false;
    while (!Aloha_flag)
    {
      if (rf22.sendtoWait(data_send, sizeof(data_send), CENTRAL_ADDRESS) != RF22_ROUTER_ERROR_NONE)
      {
          //Serial.println("sendtoWait failed");
          randNumber=random(200,max_delay);
          //Serial.println(randNumber);
          delay(randNumber);
      }
      else
      {
      Aloha_flag = true;
      t=true;
      //Serial.println("sendtoWait Succesful");
      }

  }
  }
    
}
