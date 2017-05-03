/************************************
REVISION HISTORY
$Revision: 1000 $
$Date: 2013-07-15 

Copyright (c) 2013, Linear Technology Corp.(LTC)
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The views and conclusions contained in the software and documentation are those
of the authors and should not be interpreted as representing official policies,
either expressed or implied, of Linear Technology Corp.

The Linear Technology Linduino is not affiliated with the official Arduino team.
However, the Linduino is only possible because of the Arduino team's commitment
to the open-source community.  Please, visit http://www.arduino.cc and
http://store.arduino.cc , and consider a purchase that will help fund their
ongoing work.

Copyright 2013 Linear Technology Corp. (LTC)
***********************************************************/

/*! @file
    @ingroup LTC68111
    Header for LTC6811-1 Multicell Battery Monitor
*/
 
#ifndef LTC6811_H
#define LTC6811_H

#define LTC6811_CS LATCbits.LATC2 

#define Set_Ltc6811(x) LTC6811_CS = x

/*
	Pre computed crc15 table used for the LTC6811 PEC calculation
	
	The code used to generate the crc15 table is:
	
void generate_crc15_table()
{
  int remainder;
	for(int i = 0; i<256;i++)
	{	
		remainder =  i<< 7;
		for (int bit = 8; bit > 0; --bit)
  		  {
     		
     			 if ((remainder & 0x4000) > 0)//equivalent to remainder & 2^14 simply check for MSB
    			  {
        				remainder = ((remainder << 1)) ;
        				remainder = (remainder ^ 0x4599);
     			 }
     			 else
      			{
       				 remainder = ((remainder << 1));
      			}
   		 }
	
		crc15Table[i] = remainder&0xFFFF;
	
	}
}
*/



 /*! 
 
  |MD| Dec  | ADC Conversion Model|
  |--|------|---------------------|
  |01| 1    | Fast 			   	  |
  |10| 2    | Normal 			  |
  |11| 3    | Filtered 		   	  |
*/
#define MD_FAST 	1
#define MD_NORMAL 	2
#define MD_FILTERED 3


 /*! 
 |CH | Dec  | Channels to convert |
 |---|------|---------------------|
 |000| 0    | All Cells  		  |
 |001| 1    | Cell 1 and Cell 7   |
 |010| 2    | Cell 2 and Cell 8   |
 |011| 3    | Cell 3 and Cell 9   |
 |100| 4    | Cell 4 and Cell 10  |
 |101| 5    | Cell 5 and Cell 11  |
 |110| 6    | Cell 6 and Cell 12  |
*/

#define CELL_CH_ALL 	0
#define CELL_CH_1and7 	1
#define CELL_CH_2and8 	2
#define CELL_CH_3and9 	3
#define CELL_CH_4and10 	4
#define CELL_CH_5and11 	5
#define CELL_CH_6and12 	6


/*!

  |CHG | Dec  |Channels to convert   | 
  |----|------|----------------------|
  |000 | 0    | All GPIOS and 2nd Ref| 
  |001 | 1    | GPIO 1 			     |
  |010 | 2    | GPIO 2               |
  |011 | 3    | GPIO 3 			  	 |
  |100 | 4    | GPIO 4 			  	 |
  |101 | 5    | GPIO 5 			 	 |
  |110 | 6    | Vref2  			  	 |
*/

#define AUX_CH_ALL 		0
#define AUX_CH_GPIO1 	1
#define AUX_CH_GPIO2 	2
#define AUX_CH_GPIO3 	3
#define AUX_CH_GPIO4 	4
#define AUX_CH_GPIO5 	5
#define AUX_CH_VREF2 	6

//uint8_t CHG = 0; //!< aux channels to be converted
 /*!****************************************************
  \brief Controls if Discharging transitors are enabled
  or disabled during Cell conversions.
  
 |DCP | Discharge Permitted During conversion |
 |----|----------------------------------------|
 |0   | No - discharge is not permitted         |
 |1   | Yes - discharge is permitted           |
 
********************************************************/
#define DCP_DISABLED	0
#define DCP_ENABLED 	1

void LTC6811_initialize(void);

void set_adc(uint8_t MD, uint8_t DCP, uint8_t CH, uint8_t CHG);

void LTC6811_adcv(void); 

void LTC6811_adax(void);

uint8_t LTC6811_rdcv(uint8_t reg, uint8_t total_ic, uint16_t cell_codes[][12]);

void LTC6811_rdcv_reg(uint8_t reg, uint8_t nIC, uint8_t *data);

int8_t LTC6811_rdaux(uint8_t reg, uint8_t nIC, uint16_t aux_codes[][6]);

void LTC6811_rdaux_reg(uint8_t reg, uint8_t nIC,uint8_t *data);

void LTC6811_clrcell(void);

void LTC6811_clraux(void);

void LTC6811_wrcfg(uint8_t nIC,uint8_t config[][6]);

int8_t LTC6811_rdcfg(uint8_t nIC, uint8_t r_config[][8]);

uint16_t pec15_calc(uint8_t len, uint8_t *data);

void spi_write_array(uint8_t len, uint8_t data[]);
void spi_write_read(uint8_t tx_Data[],uint8_t tx_len, uint8_t *rx_data,uint8_t rx_len);

#endif
