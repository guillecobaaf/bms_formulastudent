#include <Arduino.h>
#include <FlexCan.h>
#include <kinetis_flexcan.h>
#include <SD.H>
#include <SPI.H>
#include <guarda.h>
#include <string.h>

//definimos las variable para can bus

FlexCAN CANbus(1000000, 0);
;

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
  CANbus.begin();

  delay(1000);
  Serial.begin(9600);
  pinMode(13,OUTPUT);
  digitalWrite(13,HIGH);
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


    digitalWrite(13,LOW);
    delay(100);
    digitalWrite(13,HIGH);
    if(CANbus.available())
    {
      CANbus.read(msg);
  //    Serial.print("CAN bus 0: "); hexDump(8, msg.buf);

      myFile = SD.open("test.txt",FILE_WRITE);
      if(myFile){
        Serial.print("Writing to test.txt...");
          //imprimo el id del mensaje
          myFile.println(int(msg.id),HEX);
          myFile.println(" ");
          /*imprimo el buf 1 en el que se dira que tipo de dato es
            *01 Datos de voltaje
            *02 Datos de Temperatura
          */
          myFile.println(int(msg.buf[1]));
          /* en el buf 2 ira reflejado la posicion del termistor o de la celda
            en la que se mide la temperatura o el voltaje con el fin de poder
            reflejarla por serial y guardarlo en los datos de la SD

          */

          myFile.println(" ");
          myFile.println(int(msg.buf[2]));

          /* en el buf 3 se guardara los valores mencionados*/

          myFile.println(" ");
          myFile.println(int(msg.buf[3]));

          myFile.println("\n");
          // close the file:
          myFile.close();
          Serial.println("se ha guardado:\n ");
            Serial.println(int(msg.buf[1]));
            Serial.println(" ");
            Serial.println(int(msg.buf[2]));
            Serial.println(" ");
            Serial.println(int(msg.buf[3]));
            Serial.println("\n");
        }
    }


  }
