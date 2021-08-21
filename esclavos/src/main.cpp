#include <Arduino.h>
#include <stdint.h>
#include <FlexCan.h>
#include <kinetis_flexcan.h>
//#include <SoftwareSerial.h>
//#include "LT_SPI.h"
//#include "LTC68041.h"
//#include "funcione.h"
#define ide 0x1


//configuracion del can

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


void setup() {

 Serial.begin(9600);
 CANbus.begin();
 delay(1000);
 Serial.begin(9600);
 pinMode(13,OUTPUT);
 digitalWrite(13,HIGH);
 Serial.println("Can Receiver Initialized");

 msg.id = ide;
 msg.len = 3;
 msg.buf[0]=01;
 msg.buf[1]=00;
 msg.buf[2]=2.8;
}

void loop() {
  digitalWrite(13,LOW);
  delay(1000);
  digitalWrite(13,HIGH);
  delay(1000);
CANbus.write(msg);
Serial.println(int(msg.id),HEX);
Serial.println(int(msg.buf));
Serial.println("\n");
delay(1000);

}
