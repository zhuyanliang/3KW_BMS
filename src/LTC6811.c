/*!
	LTC6811-1 Multicell Battery Monitor
@verbatim	
	The LTC6811 is a 3rd generation multicell battery stack
	monitor that measures up to 12 series connected battery
	cells with a total measurement error of less than 1.2mV. The
	cell measurement range of 0V to 5V makes the LTC6811
	suitable for most battery chemistries. All 12 cell voltages
	can be captured in 290uS, and lower data acquisition rates
	can be selected for high noise reduction.
	
	Using the LTC6811-1, multiple devices are connected in
	a daisy-chain with one host processor connection for all
	devices.
@endverbatim	
REVISION HISTORY
$Revision: 1000 $
$Date: 2013-12-13 

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
//! @defgroup LTC68111 LTC6811-1: Multicell Battery Monitor

/*! @file
    @ingroup LTC68111
    Library for LTC6811-1 Multicell Battery Monitor
*/

#include "include.h"

static const unsigned int crc15Table[256] = 
{
	0x0000, 0xc599, 0xceab, 0x0b32, 0xd8cf, 0x1d56, 0x1664, 0xd3fd, 0xf407, 0x319e, 0x3aac,  //!<precomputed CRC15 Table
	0xff35, 0x2cc8, 0xe951, 0xe263, 0x27fa, 0xad97, 0x680e, 0x633c, 0xa6a5, 0x7558, 0xb0c1, 
	0xbbf3, 0x7e6a, 0x5990, 0x9c09, 0x973b, 0x52a2, 0x815f, 0x44c6, 0x4ff4, 0x8a6d, 0x5b2e,
	0x9eb7, 0x9585, 0x501c, 0x83e1, 0x4678, 0x4d4a, 0x88d3, 0xaf29, 0x6ab0, 0x6182, 0xa41b,
	0x77e6, 0xb27f, 0xb94d, 0x7cd4, 0xf6b9, 0x3320, 0x3812, 0xfd8b, 0x2e76, 0xebef, 0xe0dd, 
	0x2544, 0x02be, 0xc727, 0xcc15, 0x098c, 0xda71, 0x1fe8, 0x14da, 0xd143, 0xf3c5, 0x365c, 
	0x3d6e, 0xf8f7, 0x2b0a, 0xee93, 0xe5a1, 0x2038, 0x07c2, 0xc25b, 0xc969, 0x0cf0, 0xdf0d, 
	0x1a94, 0x11a6, 0xd43f, 0x5e52, 0x9bcb, 0x90f9, 0x5560, 0x869d, 0x4304, 0x4836, 0x8daf,
	0xaa55, 0x6fcc, 0x64fe, 0xa167, 0x729a, 0xb703, 0xbc31, 0x79a8, 0xa8eb, 0x6d72, 0x6640,
	0xa3d9, 0x7024, 0xb5bd, 0xbe8f, 0x7b16, 0x5cec, 0x9975, 0x9247, 0x57de, 0x8423, 0x41ba,
	0x4a88, 0x8f11, 0x057c, 0xc0e5, 0xcbd7, 0x0e4e, 0xddb3, 0x182a, 0x1318, 0xd681, 0xf17b, 
	0x34e2, 0x3fd0, 0xfa49, 0x29b4, 0xec2d, 0xe71f, 0x2286, 0xa213, 0x678a, 0x6cb8, 0xa921, 
	0x7adc, 0xbf45, 0xb477, 0x71ee, 0x5614, 0x938d, 0x98bf, 0x5d26, 0x8edb, 0x4b42, 0x4070, 
	0x85e9, 0x0f84, 0xca1d, 0xc12f, 0x04b6, 0xd74b, 0x12d2, 0x19e0, 0xdc79, 0xfb83, 0x3e1a, 
	0x3528, 0xf0b1, 0x234c, 0xe6d5, 0xede7, 0x287e, 0xf93d, 0x3ca4, 0x3796, 0xf20f, 0x21f2, 
	0xe46b, 0xef59, 0x2ac0, 0x0d3a, 0xc8a3, 0xc391, 0x0608, 0xd5f5, 0x106c, 0x1b5e, 0xdec7, 
	0x54aa, 0x9133, 0x9a01, 0x5f98, 0x8c65, 0x49fc, 0x42ce, 0x8757, 0xa0ad, 0x6534, 0x6e06, 
	0xab9f, 0x7862, 0xbdfb, 0xb6c9, 0x7350, 0x51d6, 0x944f, 0x9f7d, 0x5ae4, 0x8919, 0x4c80, 
	0x47b2, 0x822b, 0xa5d1, 0x6048, 0x6b7a, 0xaee3, 0x7d1e, 0xb887, 0xb3b5, 0x762c, 0xfc41, 
	0x39d8, 0x32ea, 0xf773, 0x248e, 0xe117, 0xea25, 0x2fbc, 0x0846, 0xcddf, 0xc6ed, 0x0374, 
	0xd089, 0x1510, 0x1e22, 0xdbbb, 0x0af8, 0xcf61, 0xc453, 0x01ca, 0xd237, 0x17ae, 0x1c9c, 
	0xd905, 0xfeff, 0x3b66, 0x3054, 0xf5cd, 0x2630, 0xe3a9, 0xe89b, 0x2d02, 0xa76f, 0x62f6, 
	0x69c4, 0xac5d, 0x7fa0, 0xba39, 0xb10b, 0x7492, 0x5368, 0x96f1, 0x9dc3, 0x585a, 0x8ba7, 
	0x4e3e, 0x450c, 0x8095
}; 

Ltc6811_Parameter 	g_ArrayLtc6811Unit[ModuleAmount] = {0};
LTC6811_RegStr		g_Ltc6811Reg[ModuleAmount][6] = {0};
/*!
  6811 conversion command variables.  
*/
uint8_t ADCV[2]; //!< Cell Voltage conversion command.
uint8_t ADAX[2]; //!< GPIO conversion command.


/*!*******************************************************************************************************************
 \brief Maps  global ADC control variables to the appropriate control bytes for each of the different ADC commands
 
@param[in] uint8_t MD The adc conversion mode
@param[in] uint8_t DCP Controls if Discharge is permitted during cell conversions
@param[in] uint8_t CH Determines which cells are measured during an ADC conversion command
@param[in] uint8_t CHG Determines which GPIO channels are measured during Auxiliary conversion command
 
Command Code:
-------------

|command	    |  15   |  14   |  13   |  12   |  11   |  10   |   9   |   8   |   7   |   6   |   5   |   4   |   3   |   2   |   1   |   0   | 
|-----------  |-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|
|ADCV:	    |   0   |   0   |   0   |   0   |   0   |   0   |   1   | MD[1] | MD[2] |   1   |   1   |  DCP  |   0   | CH[2] | CH[1] | CH[0] | 
|ADAX:	    |   0   |   0   |   0   |   0   |   0   |   1   |   0   | MD[1] | MD[2] |   1   |   1   |  DCP  |   0   | CHG[2]| CHG[1]| CHG[0]| 
 ******************************************************************************************************************/
static void Set_Adc(uint8_t MD, //ADC Mode
				 uint8_t DCP, //Discharge Permit
				 uint8_t CH, //Cell Channels to be measured
				 uint8_t CHG //GPIO Channels to be measured
			 )
{
	uint8_t md_bits;

	md_bits = (MD & 0x02) >> 1;
	ADCV[0] = md_bits + 0x02;
	md_bits = (MD & 0x01) << 7;
	ADCV[1] =  md_bits + 0x60 + (DCP<<4) + CH;

	md_bits = (MD & 0x02) >> 1;
	ADAX[0] = md_bits + 0x04;
	md_bits = (MD & 0x01) << 7;
	ADAX[1] = md_bits + 0x60 + CHG;
}

/*!**********************************************************
 \brief calaculates  and returns the CRC15
  @param[in] uint8_t len: the length of the data array being passed to the function     
  @param[in] uint8_t data[] : the array of data that the PEC will be generated from
  @returns The calculated pec15 as an unsigned int
***********************************************************/
uint16_t Pec15_Calc(uint8_t len, uint8_t *data)
{
	uint16_t remainder,addr;
	
	remainder = 16;					//initialize the PEC
	for(uint8_t i = 0; i<len;i++)	// loops for each byte in data array
	{
		addr = ((remainder>>7)^data[i])&0xff;//calculate PEC table address 
		remainder = (remainder<<8)^crc15Table[addr];
	}
	//The CRC15 has a 0 in the LSB so the remainder must be multiplied by 2
	return (remainder*2);
}

/*!
  \brief This function will initialize all 6811 variables and the SPI port.

  This function will initialize the Linduino to communicate with the LTC6811 with a 1MHz SPI clock.
  The Function also intializes the ADCV and ADAX commands to convert all cell and GPIO voltages in
  the Normal ADC mode.
*/
void LTC6811_initialize(void)
{
	TRISCbits.TRISC2 = 0b0;  // LTC6811µÄCS¹Ü½Å
	Set_Adc(MD_NORMAL,DCP_DISABLED,CELL_CH_ALL,AUX_CH_ALL);

	for(uint8_t i=0;i<ModuleAmount;i++)
	{
		g_Ltc6811Reg.cfgr[i][0] = 0x02;
	}
	LTC6811_WriteCfgReg(g_Ltc6811Reg);
}

/*****************************************************//**
 \brief Write the LTC6811 configuration register 
 
 This command will write the configuration registers of the LTC6811-1s 
 connected in a daisy chain stack. The configuration is written in descending 
 order so the last device's configuration is written first.
 
 @param[in] uint8_t total_ic; The number of ICs being written to. 
 
 @param[in] uint8_t config[][6] is a two dimensional array of the configuration data that will be written, the array should contain the 6 bytes for each
 IC in the daisy chain. The lowest IC in the daisy chain should be the first 6 byte block in the array. The array should
 have the following format:
 |  config[0][0]| config[0][1] |  config[0][2]|  config[0][3]|  config[0][4]|  config[0][5]| config[1][0] |  config[1][1]|  config[1][2]|  .....    |
 |--------------|--------------|--------------|--------------|--------------|--------------|--------------|--------------|--------------|-----------|
 |IC1 CFGR0     |IC1 CFGR1     |IC1 CFGR2     |IC1 CFGR3     |IC1 CFGR4     |IC1 CFGR5     |IC2 CFGR0     |IC2 CFGR1     | IC2 CFGR2    |  .....    |
 
 The function will calculate the needed PEC codes for the write data
 and then transmit data to the ICs on a daisy chain.
 
 
Command Code:
-------------
|           |							CMD[0]                              |                            CMD[1]                             |
|-----------|---------------------------------------------------------------|---------------------------------------------------------------|
|CMD[0:1]	|  15   |  14   |  13   |  12   |  11   |  10   |   9   |   8   |   7   |   6   |   5   |   4   |   3   |   2   |   1   |   0   | 
|-----------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|
|WRCFG:	    |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   1   |
********************************************************/
void LTC6811_WriteCfgReg(uint8_t config[][6])
{
	uint8_t 	cmd[4+8*ModuleAmount];
	uint16_t 	pec;
	uint8_t 	cmd_index; //command counter

	cmd[0] = 0x00;
	cmd[1] = 0x01;
	cmd[2] = 0x3d;
	cmd[3] = 0x6e;
	cmd_index = 4;
		
	for (uint8_t i = ModuleAmount; i > 0; i--)				
	{	
		// executes for each of the 6 bytes in the CFGR register
		// current_byte is the byte counter
		for (uint8_t current_byte = 0; current_byte < 6; current_byte++) 	
		{																				
			cmd[cmd_index] = config[i-1][current_byte]; 							
			cmd_index = cmd_index + 1;                
		}
		pec = (uint16_t)Pec15_Calc(6, config[i-1]);		
		cmd[cmd_index] = (uint8_t)(pec >> 8);
		cmd[cmd_index + 1] = (uint8_t)cfg_pec;
		cmd_index = cmd_index + 2;
	}

	Set_Ltc6811(0b0);
	spi_write_array(4+8*ModuleAmount, cmd);
	Set_Ltc6811(0b1);
}
	

