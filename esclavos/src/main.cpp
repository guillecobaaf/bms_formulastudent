#include <Arduino.h>
#include <stdint.h>
#include <FlexCan.h>
#include <kinetis_flexcan.h>
#include <SoftwareSerial.h>
#include "LT_SPI.h"
#include "LTC68041.h"
#include <linduino.h>
#include "average.h"
#include "funcione.h"

#define ide 0x1


//CONFIGURACION DEL CAN

FlexCAN CANbus(1000000, 0);
;

static CAN_message_t msg;


//CONFIGURACION DEL LTC68041
#define TOTAL_IC  1            // Number of ICs in the isoSPI network LTC6804-2 ICs must be addressed in ascending order starting at 0.

/***** Pack and sensor characteristics *****/
const float CELL_BALANCE_THRESHOLD_V = 3.3;     // Cell balancing occurrs when voltage is above this value

//DEINICION DE PINES
int tempPins[] = {A2};                // Array of arduino pins used for temperature sensor inpout (REVISAR Y PONER)

//VARIABLES FOR TRACKING CELL VOLTAGES
int cellMax_i;                        // Temporary variable for holding index of cell with max voltage
int cellMin_i;                        // Temporary variable for holding index of cell with min voltage
float  cellMin_V;                     // Temporary variable for holding  min measured cell voltage
float  cellMax_V;                     // Temporary variable for holding  max measured cell voltage
float minV1 ;
float maxV1 ;
int cellmax[]={0,0};//en la posicion 1 se almacena el numero de la celda
                    //en la posicion 2 se almacena el valor del voltaje

int cellmin[]={0,100};//en la posicion 1 se almacena el numero de la celda
                    //en la posicion 2 se almacena el valor del voltaje
int diferencialimite=0;//diferencia a la que se empieza a balancear
int diferenciamax=0;//diferenciamax actual
int diferencia=0;//diferencia actual de voltaje
uint16_t balancear=0;// si se debe balancear una celda o no
//si esta a 1 se balancea
//si esta a 0 no
uint16_t vuv=2.6;
uint16_t vov=4.1;
//variables para las temperaturas
float temps[35];
float coefB=3740;
float coefA=0.16655;
float r25=47000;
//nombre de los pines
int analog[]={A0,A1,A2,A3,A4,A5};
int control1[]={17,16,15,14};
int control2[]={18,19,20,21};


/******************************************************
  Global Battery Variables received from 6804 commands
  These variables store the results from the LTC6804
  register reads and the array lengths must be based
  on the number of ICs on the stack
 ******************************************************/
 uint16_t cell_codes[TOTAL_IC][12];
 /*!<
   The cell codes will be stored in the cell_codes[][12] array in the following format:
   |  cell_codes[0][0]| cell_codes[0][1] |  cell_codes[0][2]|    .....     |  cell_codes[0][11]|  cell_codes[1][0] | cell_codes[1][1]|  .....   |
   |------------------|------------------|------------------|--------------|-------------------|-------------------|-----------------|----------|
   |IC1 Cell 1        |IC1 Cell 2        |IC1 Cell 3        |    .....     |  IC1 Cell 12      |IC2 Cell 1         |IC2 Cell 2       | .....    |
 ****/

 uint16_t aux_codes[TOTAL_IC][6];
 /*!<
   The GPIO codes will be stored in the aux_codes[][6] array in the following format:
   |  aux_codes[0][0]| aux_codes[0][1] |  aux_codes[0][2]|  aux_codes[0][3]|  aux_codes[0][4]|  aux_codes[0][5]| aux_codes[1][0] |aux_codes[1][1]|  .....    |
   |-----------------|-----------------|-----------------|-----------------|-----------------|-----------------|-----------------|---------------|-----------|
   |IC1 GPIO1        |IC1 GPIO2        |IC1 GPIO3        |IC1 GPIO4        |IC1 GPIO5        |IC1 Vref2        |IC2 GPIO1        |IC2 GPIO2      |  .....    |
 */

 uint8_t tx_cfg[TOTAL_IC][6];
 /*!<
   The tx_cfg[][6] stores the LTC6804 configuration data that is going to be written
   to the LTC6804 ICs on the daisy chain. The LTC6804 configuration data that will be
   written should be stored in blocks of 6 bytes. The array should have the following format:
   |  tx_cfg[0][0]| tx_cfg[0][1] |  tx_cfg[0][2]|  tx_cfg[0][3]|  tx_cfg[0][4]|  tx_cfg[0][5]| tx_cfg[1][0] |  tx_cfg[1][1]|  tx_cfg[1][2]|  .....    |
   |--------------|--------------|--------------|--------------|--------------|--------------|--------------|--------------|--------------|-----------|
   |IC1 CFGR0     |IC1 CFGR1     |IC1 CFGR2     |IC1 CFGR3     |IC1 CFGR4     |IC1 CFGR5     |IC2 CFGR0     |IC2 CFGR1     | IC2 CFGR2    |  .....    |
 */

 uint8_t rx_cfg[TOTAL_IC][8];
 /*!<
   the rx_cfg[][8] array stores the data that is read back from a LTC6804-1 daisy chain.
   The configuration data for each IC  is stored in blocks of 8 bytes. Below is an table illustrating the array organization:
   |rx_config[0][0]|rx_config[0][1]|rx_config[0][2]|rx_config[0][3]|rx_config[0][4]|rx_config[0][5]|rx_config[0][6]  |rx_config[0][7] |rx_config[1][0]|rx_config[1][1]|  .....    |
   |---------------|---------------|---------------|---------------|---------------|---------------|-----------------|----------------|---------------|---------------|-----------|
   |IC1 CFGR0      |IC1 CFGR1      |IC1 CFGR2      |IC1 CFGR3      |IC1 CFGR4      |IC1 CFGR5      |IC1 PEC High     |IC1 PEC Low     |IC2 CFGR0      |IC2 CFGR1      |  .....    |
 */



 /*!**********************************************************************
   \brief  Inititializes hardware and variables
  ***********************************************************************/

  //inicializamos las funciones para el ltc6804
  void init_cfg();
  void print_cells();
  void print_config();
  void print_rxconfig();
  void serial_print_hex(uint8_t data);

void setup() {
//inicializamos los pines
for(int i=0;i<4;i++){
  pinMode(control1[i],OUTPUT);
  pinMode(control2[i],OUTPUT);
}

 Serial.begin(9600);
 CANbus.begin();
 delay(1000);
 Serial.begin(9600);
 pinMode(13,OUTPUT);
 digitalWrite(13,HIGH);
 Serial.println("Can Receiver Initialized");

 msg.id = ide;
 msg.len = 4;
 msg.buf[0]=01; // 1 SI ES VOLTAJE 2 SI ES TEMPERATURA
 msg.buf[1]=00; // POSICION EN LA QUE SE MIDE
 msg.buf[2]=2.8; //VALOR DEL MENSAJE
 msg.buf[3]=0; //1 SI ESA CELDA ESTA BALANCEANDO 0 SI NO

 //INICIALIZAMOS EL LTC68041
 LTC6804_initialize();
 init_cfg();
 config_vlimites(tx_cfg,vuv,vov);
 delay(1000);
}

void loop() {
  digitalWrite(13,LOW);
  delay(1000);
  digitalWrite(13,HIGH);
  delay(1000);

//LEEMOS LOS VOLTAJES DE LAS CELDAS
wakeup_idle();
LTC6804_adcv();//HACE LA CONVERCION AD Y RELLENA LOS REGITROS DE LAS CELDAS
delay(10);
wakeup_idle();
int error=LTC6804_rdcv(0,TOTAL_IC,cell_codes);//leemos los valores de los registros

if(error == -1)
{
  Serial.println("A PEC error was detected in the received data");
}

//SE MANDAN MENSAJES CON LOS VOLTAJES DE LAS CELDAS Y LA POSICION DE LA MISMA
  msg.buf[0]=1;
for(int i=0;i<=12;i++){

  msg.buf[1]=i;
  msg.buf[2]=int(cell_codes[1][i]);

  if(CANbus.available())
  {
    CANbus.write(msg);
    Serial.println(int(msg.id),HEX);
    Serial.println(int(msg.buf));
    Serial.println("\n");

  }
}
//BALANCEO DE CELDAS
// sacamos las celdas de menor voltaje y mayor voltaje
mayor(cell_codes,cellmax);
menor(cell_codes,cellmin);
diferenciamax=cellmax[2]-cellmin[2];
/*si la diferencia max leida es mayor que la limite se mira si hay mas celdas
que balancear*/
if(diferenciamax>diferencialimite){
balanceo(cell_codes,balancear,diferencialimite,cellmin,CELL_BALANCE_THRESHOLD_V);
  }
//una vez sabido que celdas hay que balancear se carga en los bits de config
control_balanceo(tx_cfg,balancear);
//ESCRIBIMOS TODA LA CONFIGURACION CARGADA
LTC6804_wrcfg(TOTAL_IC, tx_cfg);
//leemos todas las temperaturas
temperaturas(temps,coefA,coefB,r25);
//mandamos por el bus las temperaturas
msg.buf[0]=2;
for(int i=0; i<36;i++){
    msg.buf[1]=i;
    msg.buf[2]=(uint8_t)temps[i];
    if(CANbus.available()){
      CANbus.write(msg);
    }
  }

}




void init_cfg()
{
  for (int i = 0; i<TOTAL_IC; i++)
  {
    tx_cfg[i][0] = 0xFE;
    tx_cfg[i][1] = 0x00 ;
    tx_cfg[i][2] = 0x00 ;
    tx_cfg[i][3] = 0x00 ;
    tx_cfg[i][4] = 0x00 ;
    tx_cfg[i][5] = 0x00 ;
  }
}

void print_cells()
{


  for (int current_ic = 0 ; current_ic < TOTAL_IC; current_ic++)
  {
    Serial.print(" IC ");
    Serial.print(current_ic+1,DEC);
    for (int i=0; i<12; i++)
    {
      Serial.print(" C");
      Serial.print(i+1,DEC);
      Serial.print(":");
      Serial.print(cell_codes[current_ic][i]*0.0001,4);
      Serial.print(",");
    }
    Serial.println();
  }
  Serial.println();
}

void print_config()
{
  int cfg_pec;

  Serial.println("Written Configuration: ");
  for (int current_ic = 0; current_ic<TOTAL_IC; current_ic++)
  {
    Serial.print(" IC ");
    Serial.print(current_ic+1,DEC);
    Serial.print(": ");
    Serial.print("0x");
    serial_print_hex(tx_cfg[current_ic][0]);
    Serial.print(", 0x");
    serial_print_hex(tx_cfg[current_ic][1]);
    Serial.print(", 0x");
    serial_print_hex(tx_cfg[current_ic][2]);
    Serial.print(", 0x");
    serial_print_hex(tx_cfg[current_ic][3]);
    Serial.print(", 0x");
    serial_print_hex(tx_cfg[current_ic][4]);
    Serial.print(", 0x");
    serial_print_hex(tx_cfg[current_ic][5]);
    Serial.print(", Calculated PEC: 0x");
    cfg_pec = pec15_calc(6,&tx_cfg[current_ic][0]);
    serial_print_hex((uint8_t)(cfg_pec>>8));
    Serial.print(", 0x");
    serial_print_hex((uint8_t)(cfg_pec));
    Serial.println();
  }
  Serial.println();
}

void print_rxconfig()
{
  Serial.println("Received Configuration ");
  for (int current_ic=0; current_ic<TOTAL_IC; current_ic++)
  {
    Serial.print(" IC ");
    Serial.print(current_ic+1,DEC);
    Serial.print(": 0x");
    serial_print_hex(rx_cfg[current_ic][0]);
    Serial.print(", 0x");
    serial_print_hex(rx_cfg[current_ic][1]);
    Serial.print(", 0x");
    serial_print_hex(rx_cfg[current_ic][2]);
    Serial.print(", 0x");
    serial_print_hex(rx_cfg[current_ic][3]);
    Serial.print(", 0x");
    serial_print_hex(rx_cfg[current_ic][4]);
    Serial.print(", 0x");
    serial_print_hex(rx_cfg[current_ic][5]);
    Serial.print(", Received PEC: 0x");
    serial_print_hex(rx_cfg[current_ic][6]);
    Serial.print(", 0x");
    serial_print_hex(rx_cfg[current_ic][7]);
    Serial.println();
}
  Serial.println();
}

void serial_print_hex(uint8_t data)
{
  if (data< 16)
  {
    Serial.print("0");
    Serial.print((byte)data,HEX);
  }
  else
    Serial.print((byte)data,HEX);
}
