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

#define Set_Ltc6811(x) {Nop();Nop();Nop();Nop();LTC6811_CS = x; Nop();Nop();Nop();Nop();}

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


#define  WRCFG_CMD             ((uint8_t)0x01)               // Write config registers
#define  RDCFG_CMD             ((uint8_t)0x02)               // Read config registers


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

typedef struct 
{
	int16_t  CellVolt[12]; 
	uint16_t Temp1;
	uint16_t Temp2;
	uint16_t Temp3;
	uint16_t Temp4;
}Ltc6811_Parameter;

typedef struct
{
	uint8_t cfgr[6];
	uint8_t cvregA[6];
	uint8_t cvregB[6];
	uint8_t cvregC[6];
	uint8_t cvregD[6];
	uint8_t axregA[6];
	uint8_t axregB[6];
	uint8_t stregA[6];
	uint8_t stregB[6];
	uint8_t commreg[6];
	uint8_t contreg[6];
	uint8_t pwmreg[6];
}LTC6811_RegStr;

extern Ltc6811_Parameter 	g_ArrayLtc6811Unit[ModuleAmount];
extern LTC6811_RegStr		g_Ltc6811Reg;

void LTC6811_initialize(void);

void LTC6811_Adcv(void); 

#endif
