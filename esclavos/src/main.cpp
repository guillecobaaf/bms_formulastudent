#include <Arduino.h>
#include <stdint.h>
#include <SoftwareSerial.h>
#include "LT_SPI.h"
#include "LTC68041.h"
#include "funcione.h"

const uint8_t TOTAL_IC = 1;//!<number of ICs in the daisy chain
const uint8_t debug_mode = 1;//para imprimir por serial tengase a 1

/******************************************************
 *** Global Battery Variables received from 6804 commands
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

 /*!***********************************
 \brief Initializes the configuration array
 **************************************/




void setup() {
 Serial.begin(115200);
 LTC6804_initialize();  //Initialize LTC6804 hardware
 init_cfg();        //initialize the 6804 configuration array to be written

}

void loop() {
  // put your main code here, to run repeatedly:
}
