#include <MQ135.h>//GEORG K github library
#include <DHT11.h>//library found in github
#include <SoftwareSerial.h> //we will need it for communication between application and ibeacon

DHT11 Dht11(4); //it is assigned to d4 of arduino, check circuit(1) of documentation
SoftwareSerial mySerial(7,8);

char bytebucket[16];

void byte2ASCII0x(unsigned char value, char * location){
  int v0 = value / 16;
  int v1 = value % 16;
  if(v0 >= 10){
    location[0] = v0 - 10 + 'A';
  }else{
    location[0] = v0 + '0';
  }
  if(v1 >= 10){
    location[1] = v1 - 10 + 'A';
  }else{
    location[1] = v1 + '0';
  }
}

void setup() {
  
  pinMode(A0,INPUT);//setting the co2 sensor in pin a0 as input

  pinMode(4, INPUT);//setting the temp/hum sensor in d4 as input
  
  Serial.begin(9600); //baud rate of 9600
  mySerial.begin(9600);

  Serial.println("SYSTEM RESET");
}

void loop() {
   long sumCo2 = 0.0;
  char readingAt[14] = "AT+MINO0x00FF";
   
  MQ135 g0 = MQ135(A0); //initializing the sensor with the correct pin

  float rzero = g0.getRZero(); /*needed to calculate rzero */

  Serial.print("R0: ");
  Serial.print(rzero,DEC); //displaying sensor to update code witg new RZERO
  Serial.println(" ");
  
  float temp;
  float hum;
  
  int x = Dht11.read(hum,temp); // finding our the temperature and humidity values and assigning constantly
    
  Serial.print("Humidity: ");
  Serial.print(hum);
  Serial.println("%");
  
  Serial.print("Temperature: ");
  Serial.print(temp);
  Serial.println(" ");
  
  for(int i = 0; i < 16; i++){
    sumCo2 += (long) g0.getCorrectedPPM(hum, temp); // we are using the corrected ppm values which means that we are including temperature and humidity
    delay(100);
  }

  
  unsigned int ppm = (unsigned int) sumCo2/16;
  
  Serial.print("Corrected PPM: ");
  Serial.print(ppm); // displaying ppm in the terminal
  Serial.println(" ");
  
  mySerial.print(F("AT+IBEA0"));
  mySerial.readBytes(bytebucket,8);
  mySerial.print(F("AT+MINO?"));
  mySerial.readBytes(bytebucket,13);//OK+Get:0x0100 
  strncpy(readingAt+9,bytebucket+9,2);
  byte2ASCII0x((unsigned char)(ppm / 100), readingAt+11);
  
  mySerial.print(readingAt);
  mySerial.readBytes(bytebucket,13);
  mySerial.print(F("AT+IBEA1"));
  mySerial.readBytes(bytebucket,8);
  
  mySerial.print(F("AT+RESET"));
  mySerial.readBytes(bytebucket,8);

  //this conditions are only to display on the applications
  if(ppm > 5000)
  {  
    Serial.println("Critical CO2");
  }
  else if(ppm >= 2000 && ppm < 5000)
  {
    Serial.println("Severe CO2");
  }
  else if(ppm >= 1000 && ppm < 2000)
  {
    Serial.println("Substantial CO2");
  }
  else if(ppm >= 250 && ppm < 1000)
  {
    Serial.println("Clean Air ");
  }
  else if( ppm < 250)
  {
    Serial.println("Low CO2");
  }
  delay(30000);
  Serial.println(" ");                       
}
