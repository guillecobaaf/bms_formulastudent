#include <Arduino.h>
#include <FlexCan.h>
#include <kinetis_flexcan.h>
#include <SD.H>
#include <SPI.H>
#include <guarda.h>
#include <string.h>

//definimos las variable para can bus

FlexCAN CANbus0(1000000, 0);
FlexCAN CANbus1(1000000, 1);

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


//definimos la variable para escribir en la sd
File myFile;








void setup() {
  // put your setup code here, to run once:
  //init can bus
  CANbus0.begin();
  CANbus1.begin();
  delay(1000);
  Serial.println("Can Receiver Initialized");

  //inicializamos la sd

  if (!SD.begin(BUILTIN_SDCARD)){
    Serial.println("initialization failed");
    while(1);

  }
  Serial.println("initialization done");



}

void loop()
{
  if(CANbus0.available())
  {
    CANbus0.read(msg);
//    Serial.print("CAN bus 0: "); hexDump(8, msg.buf);
  CANbus1.write(msg);
myFile = SD.open("test.txt",FILE_WRITE);
if(myFile){
  Serial.print("Writing to test.txt...");
    uint8_t guarda = msg.buf;
    myFile.println(guarda);
    myFile.println("\n");
    // close the file:
    myFile.close();
    Serial.println("done.");


}
  }

  if(CANbus1.available())
    {
      CANbus1.read(msg);
  //    Serial.print("CAN bus 1: "); hexDump(8, msg.buf);
      CANbus0.write(msg);
      myFile = SD.open("test.txt",FILE_WRITE);
      if(myFile){
        Serial.print("Writing to test.txt...");
          uint8_t guarda = msg.buf;
          myFile.println(guarda);
          myFile.println("\n");
          // close the file:
          myFile.close();
          Serial.println("done.");


      }

    }
  }
