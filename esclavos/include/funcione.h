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
        cellmax[2]=int(cell_codes[1][i]);
        cellmax[1]=i;

      }
  }
}
//funcion para obtener las celdas que deben estar balanceando
void balanceo(uint16_t cell_codes[1][12],int balancear[11],int diferencialimite, int cellmin[2],float CELL_BALANCE_THRESHOLD_V)
{
int diferencia=0;
    for (int i=0;i<12;i++){
      if(cell_codes[1][i]>CELL_BALANCE_THRESHOLD_V*10000){
    diferencia=cell_codes[1][i]-cellmin[2];
        if(diferencia>diferencialimite){

          balancear[i]=1;
        }else{

          balancear[i]=0;

        }


    }

}


}
