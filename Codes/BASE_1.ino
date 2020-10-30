#include <RF22.h>
#include <RF22Router.h>
#define BASE_ADDRESS 1
#define VISITOR_ADDRESS 6

RF22Router rf22(BASE_ADDRESS);


int sensorVal =1;
boolean successful_packet = false;
long randNumber;
int max_delay=3000;

void setup() {
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
  rf22.addRouteTo(VISITOR_ADDRESS, VISITOR_ADDRESS);
  sensorVal = analogRead(A0);
  randomSeed(sensorVal);// (μία μόνο φορά μέσα στην setup)

}

void loop() 
{
  int Visitor=1;
  int Counter=5;
  int PacketsToSent=0;
  
  while(Counter < Visitor + 5)
  {
   boolean Aloha_Flag=false ;

    while(Aloha_Flag==false)
    {
       // Get Measurement
      sensorVal=10;
      Serial.print("sensor Value: ");
      Serial.println(sensorVal);
  
      char data_read[RF22_ROUTER_MAX_MESSAGE_LEN];
      uint8_t data_send[RF22_ROUTER_MAX_MESSAGE_LEN];
      memset(data_read, '\0', RF22_ROUTER_MAX_MESSAGE_LEN);
      memset(data_send, '\0', RF22_ROUTER_MAX_MESSAGE_LEN);
      sprintf(data_read, "%d", sensorVal);
      data_read[RF22_ROUTER_MAX_MESSAGE_LEN - 1] = '\0';
      memcpy(data_send, data_read, RF22_ROUTER_MAX_MESSAGE_LEN); 
      
      if (rf22.sendtoWait(data_send, sizeof(data_send), VISITOR_ADDRESS) != RF22_ROUTER_ERROR_NONE)
       {
      Serial.println("sendtoWait failed");
      randNumber=random(200,max_delay);
      Serial.println(randNumber);
      delay(randNumber);
       }
      else
      {
      Aloha_Flag=true;
      Serial.println("sendtoWait Succesful");
      PacketsToSent++ ;
      if ( PacketsToSent==15)
      {
         Counter++ ;
         PacketsToSent=0 ;
       }
      }
    
      

    }
 }
  
  
  

}
