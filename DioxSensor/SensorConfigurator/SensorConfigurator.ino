#include <SoftwareSerial.h>

SoftwareSerial modulePort(7,8);
char scratchpad[20];

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
  Serial.begin(9600);
  modulePort.begin(9600);
}

void loop() {
  int series, group, sensor;
  Serial.print(F("CO2 sensor configurator\r\n"));
  Serial.print(F("Sensor series number to write:"));
  while(Serial.available() == 0);
  series = Serial.parseInt();
  Serial.print(F("Sensor group number to write (0-65536):"));
  while(Serial.available() == 0);
  group = Serial.parseInt();
  Serial.print(F("Sensor ID (1-255):"));
  while(Serial.available() == 0);
  sensor = Serial.parseInt();
  Serial.print(F("Programming sensor configuration"));
  
  modulePort.print(F("AT"));
  modulePort.readBytes(scratchpad, 2);//OK
  modulePort.print(F("AT+IBEA0"));
  modulePort.readBytes(scratchpad,8);//OK+Set:0
  modulePort.print(F("AT+IBE042454143"));
  modulePort.readBytes(scratchpad,17);//OK+Set:0x42454143
  modulePort.print(F("AT+IBE14F4E4154"));
  modulePort.readBytes(scratchpad,17);//OK+Set:0x4F4E4154
  modulePort.print(F("AT+IBE24F520000"));
  modulePort.readBytes(scratchpad,17);//OK+Set:0x4F520000
  strncpy(scratchpad,"AT+IBE300000000",15);
  byte2ASCII0x((unsigned char)((series >> 24) & 0xFF), scratchpad+7);
  byte2ASCII0x((unsigned char)((series >> 16) & 0xFF), scratchpad+9);
  byte2ASCII0x((unsigned char)((series >> 8) & 0xFF), scratchpad+11);
  byte2ASCII0x((unsigned char)(series & 0xFF), scratchpad+13);
  modulePort.print(scratchpad);
  modulePort.readBytes(scratchpad,17);//OK+Set:0x********
  strncpy(scratchpad,"AT+MARJ0x0000",13);
  byte2ASCII0x((unsigned char)((group >> 8) & 0xFF), scratchpad+9);
  byte2ASCII0x((unsigned char)(group & 0xFF), scratchpad+11);
  modulePort.print(scratchpad);
  modulePort.readBytes(scratchpad, 13);//OK+Set:0x****
  strncpy(scratchpad,"AT+MINO0x0000",13);
  byte2ASCII0x((unsigned char)(sensor & 0xFF), scratchpad+9);
  modulePort.print(scratchpad);
  modulePort.readBytes(scratchpad, 13);//OK+Set:0x****
  modulePort.print(F("AT+IBEA1"));
  modulePort.readBytes(scratchpad, 8);//OK+Set:1
  modulePort.print(F("AT+RESET"));
  modulePort.readBytes(scratchpad, 8);//OK+RESET
  
  Serial.print(F("done\r\n"));
}
