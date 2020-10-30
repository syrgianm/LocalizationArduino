#include <RF22.h>
#include <RF22Router.h>
#include<math.h>
#define BASE_ADDRESS0 0
#define BASE_ADDRESS1 1
#define BASE_ADDRESS2 2
#define BASE_ADDRESS3 3
#define VISITOR_ADDRESS 6
#define CENTRAL_ADDRESS 4
#define NumberOfBases 4
#define NumberOfDimensions 2
#define PacketsToBeSent 15

   
RF22Router rf22(VISITOR_ADDRESS);
int sensorVal = 23;
long randNumber;
int max_delay=3000;
  //Initialize Base Coordinations
int BaseCoords[NumberOfBases][NumberOfDimensions]= { 
{10,10},
{20,10},
{10,20},
{20,20} 
};  //Position 0 (line) is for Base1 , Pos 1(line)is for Base2 etc. COlumn 0 for x pos and column 1 for y pos
 

  //Initialize Matrix for Powers from Base Stations

//double PowerMatrix[NumberOfBases][PacketsToBeSent]; // Lines like BaseCoords , Columns For every Power we recieve from Linth Base Station

void setup() {
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
  sensorVal = analogRead(A0);
  randomSeed(sensorVal);// (μία μόνο φορά μέσα στην setup)
  rf22.addRouteTo(CENTRAL_ADDRESS,CENTRAL_ADDRESS);

  // Manually define the routes for this network
  rf22.addRouteTo(BASE_ADDRESS0, BASE_ADDRESS0);
  rf22.addRouteTo(BASE_ADDRESS1, BASE_ADDRESS1);
  rf22.addRouteTo(BASE_ADDRESS2, BASE_ADDRESS2);
  rf22.addRouteTo(BASE_ADDRESS3, BASE_ADDRESS3);



 

  
  

      }

void loop() {


  boolean Flag=false; //flag to wait recieve from every base station all packets
  int Counter[NumberOfBases]={0,0,0,0}; //Counter to calculate when we recieve 15 Packets from evey Base Station
  float MaxPower[NumberOfBases]={-100,-100,-100,-100}; //Initiallization matrix to have the max power from 15 packets from every Base station
  while(Flag==false)
  {
        uint8_t buf[RF22_ROUTER_MAX_MESSAGE_LEN];
        char incoming[RF22_ROUTER_MAX_MESSAGE_LEN];
        memset(buf, '\0', RF22_ROUTER_MAX_MESSAGE_LEN);
        memset(incoming, '\0', RF22_ROUTER_MAX_MESSAGE_LEN);
        uint8_t len = sizeof(buf);
        uint8_t from;
        int received_value = 0;
        if (rf22.recvfromAck(buf, &len, &from))
            { 
                /* Take Data But Uneccessary
                buf[RF22_ROUTER_MAX_MESSAGE_LEN - 1] = '\0';
                memcpy(incoming, buf, RF22_ROUTER_MAX_MESSAGE_LEN);
                Serial.print("got request from : ");
                Serial.println(from, DEC);
                received_value = atoi((char*)incoming);
                Serial.println(received_value);
                */
                
                 uint8_t Tx=from;                               //Transmmiter of the packet
                 //Serial.println(Tx);
                if(Counter[Tx]<14)    //Check for take only 14 packets from every Base
                {
                 float rssi = rf22.rssiRead();              //Calculate Power from the packet we recieved
                 double Pr = ((double)rssi - 230.0) / 1.8;   //Calculate in Power in dbm
                 //PowerMatrix[Tx][Counter[Tx]]=Pr;
                 Counter[Tx]=Counter[Tx]+1;
                 if(Pr>MaxPower[Tx]) {
                  MaxPower[Tx]=Pr;
                                     }
            
                } 
                
                
                if((Counter[0]==(PacketsToBeSent-1)) && (Counter[1]==(PacketsToBeSent-1)) && (Counter[2]==(PacketsToBeSent-1)) && (Counter[3]==(PacketsToBeSent-1))) //Break if we recieve All 15 packets from Every Base 0 to 14
                {
                   
                    Flag=true;  
                }
                
    
            }  
  
  }
  //float Mean_Values[NumberOfBases];   //Initialize Mean Values from Power We recieved
  //float Sum_Mean=0;              //Initialize Sum Value Of All Mean
  
  /*for(int i=0;i<NumberOfBases;i++)
    {
      for(int j=0;j<PacketsToBeSent;j++)
      {
        Mean_Values[i]=Mean_Values[i]+PowerMatrix[i][j];
      }
    Mean_Values[i]=Mean_Values[i]/PacketsToBeSent;  //Calculate Mean Value from every for every Base Station
    Serial.println(Mean_Values[i]);
    Sum_Mean=Mean_Values[i]+Sum_Mean; 
    }
  */
float  SumMax=0; //To Sum all the Max Poewers
float PowerinmW[PacketsToBeSent];
 for(int i=0;i<NumberOfBases;i++) {
        PowerinmW[i]=pow(10.0,(MaxPower[i])/10.0);
        SumMax=(PowerinmW[i])+SumMax;
                          
        Serial.println(MaxPower[i]);
        Serial.println(PowerinmW[i]);
                             }


//float Mean_Max=SumMax/(float)NumberOfBases;
float Weight[NumberOfBases]; //Initialize Weights for Localization 
for(int i=0;i<NumberOfBases;i++)
{
           
        Weight[i]=(PowerinmW[i])/(SumMax);   //Calculate Weight for every Base_Station
                          


}
//Initialize Coordinates of Visitor
float x=0;
float y=0;
for(int i=0;i<NumberOfBases;i++)
{

  x=x+Weight[i]*BaseCoords[i][0];
  y=y+Weight[i]*BaseCoords[i][1];
}
Serial.print("The coordinates of visitor :(");
Serial.print(x);
Serial.print(",");
Serial.print(y);
Serial.println(")");

 //Send Coordinates To Central Node
  
  char data_read[RF22_ROUTER_MAX_MESSAGE_LEN];
  uint8_t data_send[RF22_ROUTER_MAX_MESSAGE_LEN];
  memset(data_read, '\0', RF22_ROUTER_MAX_MESSAGE_LEN);
  memset(data_send, '\0', RF22_ROUTER_MAX_MESSAGE_LEN);
  x=x*100;
  y=y*100;
  sprintf(data_read, "%d%d",(int)x,(int)y);
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
