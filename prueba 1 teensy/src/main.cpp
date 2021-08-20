#include <Arduino.h>
#include <SD.H>
#include <SPI.H>
#include <FlexCan.h>
#include <kinetis_flexcan.h>
//valor del id del dispositivo
#define iden 0x0

File myFile;



//definimos las variable para can bus

FlexCAN CANbus(1000000, 0);


static CAN_message_t msg;
static uint8_t hex[17] = "0123456789abcdef";


static void hexDump(uint8_t dumpLen, uint8_t *bytePtr)
{
  uint8_t working;
  while( dumpLen-- ) {
    working = *bytePtr++;
    Serial.write( hex[ working>>4 ] );
    Serial.write( hex[ working&15 ] );
  }
  Serial.write('\r');
  Serial.write('\n');
}
//definimos la estrutura del mensaje para este dispositivo



void setup() {
  // put your setup code here, to run once:
  pinMode(13,OUTPUT);
  digitalWrite(13,HIGH);
  delay(1000);
  digitalWrite(13,LOW);
  delay(1000);
  digitalWrite(13,HIGH);

  Serial.begin(9600);
  digitalWrite(13,LOW);
  delay(3000);


Serial.println("sip");
  CANbus.begin();

digitalWrite(13,HIGH);

    if (!SD.begin(BUILTIN_SDCARD)){
      Serial.println("initialization failed");
      while(1);

    }else{
    Serial.println("initialization done");
    }
//defino el mensaje

msg.ext = 0;
msg.id = iden;
msg.len = 8;


  }



void loop() {

  digitalWrite(13,HIGH);
  delay(1000);
  digitalWrite(13,LOW);
  delay(1000);
  Serial.printf("sip");
  if(CANbus.available())
   {
     CANbus.read(msg);
 //    Serial.print("CAN bus 0: "); hexDump(8, msg.buf);

   }



  myFile = SD.open("test.txt",FILE_WRITE);

  if(myFile){
    Serial.print("Writing to test.txt...");
      myFile.print(int(msg.buf),HEX);
      Serial.print("\n");
      Serial.print(int(msg.buf),HEX);
      Serial.print("\n");
      Serial.print(int(msg.id),HEX);
      Serial.print("\n");
      myFile.print("\n");
      // close the file:
      myFile.close();
      Serial.println("done.");


  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening test.txt");
  }

  // put your main code here, to run repeatedly:
}
