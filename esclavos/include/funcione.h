#include <Arduino.h>
#include <stdint.h>
#include <SoftwareSerial.h>
#include "LT_SPI.h"
#include "LTC68041.h"



//funcion para obtener la celda de mayor voltaje

void mayor(uint16_t cell_codes[1][12],int cellmax[2]){

  for (int i=0;i<12;i++){
      if(int(cell_codes[1][i])>cellmax[2]){
        cellmax[2]=int(cell_codes[1][i]);
        cellmax[1]=i;

      }

  }


}
//funcion para obtener la celda de menor voltaje
void menor(uint16_t cell_codes[1][12],int cellmin[2]){

  for (int i=0;i<12;i++){
      if(int(cell_codes[1][i])<cellmin[2]){
        cellmin[2]=int(cell_codes[1][i]);
        cellmin[1]=i;

      }
  }
}
//funcion para obtener las celdas que deben estar balanceando
void balanceo(uint16_t cell_codes[1][12],uint16_t balancear,int diferencialimite, int cellmin[2], float CELL_BALANCE_THRESHOLD_V)
{
int diferencia=0;
    for (int i=0;i<12;i++){
      if(cell_codes[1][i]>CELL_BALANCE_THRESHOLD_V*10000){
    diferencia=cell_codes[1][i]-cellmin[2];
        if(diferencia>diferencialimite){

          balancear=(balancear & 0b0000000000000001)<<1;
        }else{

          balancear= balancear << 1;

        }


    }

}


}


void config_vlimites ( uint8_t tx_cfg[][6], uint16_t vuv,uint16_t vov){
//calcula el vuv y vov
/*
vuv value under voltage = limite minimo de voltaje
vov value over voltage = limite maximo de voltaje
*/
vuv=(vuv+1)*0.0016;
vov=vov*0.0016;
tx_cfg[1][0]=0b0000100;
tx_cfg[1][1]=vuv&(0b11111111);
tx_cfg[1][2]=(((vuv>>8) & (0b00001111))|((vov & 0b00001111<<4)));
tx_cfg[1][3]=((vov >> 4) & (0b11111111));
tx_cfg[1][5]=0b00000000;

}

void control_balanceo (uint8_t tx_cfg[][6],uint16_t balancear){

uint8_t cfg4 = balancear & 0b11111111;
uint8_t cfg5 = balancear >> 8;
 cfg5 = cfg5 & 0b00001111;

//rellenamos tx_cfg de forma que balanceen las celdas que se han calculado
tx_cfg[1][4] = (uint8_t)cfg4;
tx_cfg[1][5] = (uint8_t)cfg5;
/*con la ultima linea rellenamos nada mas los 4 primeros bits y dejamos los ultimos
tal y como estaban*/


}

void temperaturas (float temps[35],float coefA,float coefB, float r25 ){
int analog[]={A0,A1,A2,A3,A4,A5};
int control1[]={17,16,15,14};
int control2[]={18,19,20,21};
//leemos los termistores no multiplexados
temps[0]=analogRead(analog[2])/1024 *3.3;
temps[1]=analogRead(analog[3])/1024 *3.3;
temps[2]=analogRead(analog[4])/1024 *3.3;
temps[3]=analogRead(analog[5])/1024 *3.3;
//leemos los termistores multiplexados del multiplexor 1
for (int i=4;i<20;i++){
  for(int j=0;j<2;j++){
    digitalWrite(control1[0],j) ;
      for (int k=0;k<2;k++){
        digitalWrite(control1[1],k);
          for(int l=0;l<2;l++){
            digitalWrite(control1[2],l);
              for(int m=0;m<2;m++){
                  digitalWrite(control1[3],m);
                  
                  temps[i]=analogRead(analog[0])/1024 * 3.3;
              }
          }
      }
  }
}
//leemos los termistores multiplexados del mux 2
for (int i=20;i<36;i++){
  for(int j=0;j<2;j++){
    digitalWrite(control2[0],j) ;
      for (int k=0;k<2;k++){
        digitalWrite(control2[1],k);
          for(int l=0;l<2;l++){
            digitalWrite(control2[2],l);
              for(int m=0;m<2;m++){
                  digitalWrite(control2[3],m);

                  temps[i]=analogRead(analog[1])/1024 * 3.3;
              }
          }
      }
  }
}
//ahora transformamos el valor de voltaje en temperatura que podamos tratar
float Rt;
for(int i=0; i<36;i++){
  Rt=(temps[i]*7680)/(3.3-temps[i]);
  temps[i]=(coefB/(log(Rt/coefA)))-273.15;
}

}
