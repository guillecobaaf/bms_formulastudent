#include <Arduino.h>
#include <FlexCan.h>
#include <kinetis_flexcan.h>
#include <SD.H>
#include <SPI.H>
#include <string.h>
#include <funciones.h>
//definimos la funciones
void casos (int buf0,float buf2,int tvent1, int tvent2,int tcort,int pinpwm,int chenable,int disenable,float vmin,float vmax);
//definimos las variable para can bus

FlexCAN CANbus(1000000, 0);
;

static CAN_message_t msg;
static uint8_t hex[17] = "0123456789abcdef";






//definimos la variable para escribir en la sd
File myFile;

//definimos las variables de pines para el funcionamiento del maestro
int pinpwm= 23; //pin al que van conectados los ventiladores
int chenable= 36; //pin al que va conectado el charge enable
int disenable= 37; //pin al que va conectado el discharge enable
int curr1= A8;
int curr2= A7;
//definimos las variables de control del sistema
int tvent1 = 30; //temperatura a la que empiezan los ventiladores a v lenta
int tvent2 = 40; //temperatura a la ventiladores 100%
int tcort = 60; //temperatura maxima por normativa
float corrientech = 5; //corriente maxima de carga
float corrientedch = 10;//corriente maxima de descarga
float vmin = 2.5; //voltaje minimo para la celda
float vmax = 4.1; //voltaje maximo para la celda
int vvent=3;
//varibles generales

int sip=0;



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
    delay(1000);
    digitalWrite(13,HIGH);
    delay(1000);
    if(CANbus.available())
    {
      CANbus.read(msg);
  //    Serial.print("CAN bus 0: "); hexDump(8, msg.buf);
  myFile = SD.open("test.txt",FILE_WRITE);

  if(myFile){
    Serial.print("Writing to test.txt...");
      //imprimo el id del mensaje
      myFile.printf("%d ",int(msg.id),HEX);


      /*imprimo el buf 0 en el que se dira que tipo de dato es
        *01 Datos de voltaje
        *02 Datos de Temperatura
      */
      myFile.printf("%f ",float(msg.buf[0]));


      /* en el buf 1 ira reflejado la posicion del termistor o de la celda
        en la que se mide la temperatura o el voltaje con el fin de poder
        reflejarla por serial y guardarlo en los datos de la SD
      */

      myFile.printf("%f ",float(msg.buf[1]));

      /* en el buf 2 se guardara los valores mencionados*/


      myFile.printf("%f",float(msg.buf[2]));

      myFile.printf("\n");
      // close the file:
      myFile.close();
      Serial.println("se ha guardado:\n ");
        Serial.printf("%d ",int(msg.id),HEX);

        Serial.printf("%f ",float(msg.buf[0]));

        Serial.printf("%f ",float(msg.buf[1]));

        Serial.printf("%f \n",float(msg.buf[2]));

    }

  }else if(!CANbus.available()){

    digitalWrite(chenable,HIGH);
    digitalWrite(disenable,HIGH);
  }

    casos(int(msg.buf[0]),float(msg.buf[2]),tvent1,tvent2,tcort,pinpwm,chenable,disenable, vmin, vmax,vvent);



  }
