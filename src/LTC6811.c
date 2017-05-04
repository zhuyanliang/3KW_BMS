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

uint8_t				g_Ltc6811CfgReg[ModuleAmount][6];
/*!
  6811 conversion command variables.  
*/
uint8_t g_ADCV[2]; //!< Cell Voltage conversion command.
uint8_t g_ADAX[2]; //!< GPIO conversion command.


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
|g_ADCV:	    |   0   |   0   |   0   |   0   |   0   |   0   |   1   | MD[1] | MD[2] |   1   |   1   |  DCP  |   0   | CH[2] | CH[1] | CH[0] | 
|g_ADAX:	    |   0   |   0   |   0   |   0   |   0   |   1   |   0   | MD[1] | MD[2] |   1   |   1   |  DCP  |   0   | CHG[2]| CHG[1]| CHG[0]| 
 ******************************************************************************************************************/
static void Set_Adc(uint8_t MD,uint8_t DCP, uint8_t CH, uint8_t CHG)
{
	uint8_t md_bits;

	md_bits = (MD & 0x02) >> 1;
	g_ADCV[0] = md_bits + 0x02;
	md_bits = (MD & 0x01) << 7;
	g_ADCV[1] =  md_bits + 0x60 + (DCP<<4) + CH;

	md_bits = (MD & 0x02) >> 1;
	g_ADAX[0] = md_bits + 0x04;
	md_bits = (MD & 0x01) << 7;
	g_ADAX[1] = md_bits + 0x60 + CHG;
}

static void SPI_Write_Array(uint8_t len, uint8_t data[])
{
	for(uint8_t i = 0; i < len; i++)
	{
		SPI_SendByte((int8_t)data[i]);
	}
}

/*!
 \brief Writes and read a set number of bytes using the SPI port.

@param[in] uint8_t tx_data[] array of data to be written on the SPI port
@param[in] uint8_t tx_len length of the tx_data array
@param[out] uint8_t rx_data array that read data will be written too. 
@param[in] uint8_t rx_len number of bytes to be read from the SPI port.
*/
static void SPI_Write_Read(uint8_t tx_Data[],uint8_t tx_len,uint8_t *rx_data,uint8_t rx_len)
{
	for(uint8_t i = 0; i < tx_len; i++)
	{
		SPI_SendByte(tx_Data[i]);
	}

	for(uint8_t i = 0; i < rx_len; i++)
	{
		rx_data[i] = (uint8_t)SPI_ReceiveByte();
	}

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

	return (remainder<<1);
}

/*!
  \brief This function will initialize all 6811 variables and the SPI port.

  This function will initialize the Linduino to communicate with the LTC6811 with a 1MHz SPI clock.
  The Function also intializes the g_ADCV and g_ADAX commands to convert all cell and GPIO voltages in
  the Normal ADC mode.
*/
void LTC6811_Initialize(void)
{
	TRISCbits.TRISC2 = 0b0;  // LTC6811µÄCS¹Ü½Å
	Set_Adc(MD_NORMAL,DCP_ENABLED,CELL_CH_ALL,AUX_CH_ALL);

	g_Ltc6811CfgReg[0][0] = 0x02;
    g_Ltc6811CfgReg[1][0] = 0x02;
	LTC6811_WriteCfgReg(g_Ltc6811CfgReg);
}

/*****************************************************//**
 brief Write the LTC6811 configuration register 
 
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
|         |							CMD[0]                       |                            CMD[1]                             |
|---------|---------------------------------------------------------------|---------------------------------------------------------------|
|CMD[0:1]	|  15   |  14   |  13   |  12   |  11   |  10   |   9   |   8   |   7   |   6   |   5   |   4   |   3   |   2   |   1   |   0   | 
|---------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|
|WRCFG:	|   0   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   1   |
********************************************************/
void LTC6811_WriteCfgReg(uint8_t config[ModuleAmount][6])
{
	uint8_t 	cmd[4+8*ModuleAmount];
	uint16_t 	pec;
	uint8_t 	cmd_index; //command counter

	cmd[0] = 0x00;
	cmd[1] = 0x01;
	cmd[2] = 0x3d;
	cmd[3] = 0x6e;
	cmd_index = 4;
		
	for (uint8_t i = 0; i < ModuleAmount; i++)				
	{	
		for (uint8_t j = 0; j < 6; j++) 	
		{																				
			cmd[cmd_index] = config[i][j]; 							
			cmd_index ++;                
		}
		pec = (uint16_t)Pec15_Calc(6, config[i]);		
		cmd[cmd_index] = (uint8_t)(pec >> 8);
		cmd[cmd_index + 1] = (uint8_t)pec;
		cmd_index = cmd_index + 2;
	}

	Set_Ltc6811(0b0);
	SPI_Write_Array(4+8*ModuleAmount,cmd);
	Set_Ltc6811(0b1);
}

/*!*********************************************************************************************
  \brief Starts cell voltage conversion
  Starts ADC conversions of the LTC6811 Cpin inputs.
  The type of ADC conversion executed can be changed by setting the associated global variables:
 |Variable|Function                                      | 
 |--------|----------------------------------------------|
 | MD     | Determines the filter corner of the ADC      |
 | CH     | Determines which cell channels are converted |
 | DCP    | Determines if Discharge is Permitted	     |
Command Code:
-------------
|CMD[0:1]  |  15   |  14   |  13   |  12   |  11   |  10   |   9   |   8   |   7   |   6   |   5   |   4   |   3   |   2   |   1   |   0   | 
|--------- |-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|
|g_ADCV:	 |   0   |   0   |   0   |   0   |   0   |   0   |   1   | MD[1] | MD[2] |   1   |   1   |  DCP  |   0   | CH[2] | CH[1] | CH[0] |  
***********************************************************************************************/
void LTC6811_Adcv(void)
{

	uint8_t cmd[4];
	uint16_t cmd_pec;

	cmd[0] = g_ADCV[0];
	cmd[1] = g_ADCV[1];

	cmd_pec = Pec15_Calc(2, g_ADCV);
	cmd[2] = (uint8_t)(cmd_pec >> 8);
	cmd[3] = (uint8_t)(cmd_pec);

	Set_Ltc6811(0b0);
	SPI_Write_Array(4,cmd);
	Set_Ltc6811(0b1);
}



/***********************************************
 brief Reads and parses the LTC6811 cell voltage registers.
 The function is used to read the cell codes of the LTC6811.
 This function will send the requested read commands parse the data
 and store the cell voltages in cell_codes variable. 
 @param[in] uint8_t reg; This controls which cell voltage register is read back. 
          0: Read back all Cell registers 	  
          1: Read back cell group A 	  
          2: Read back cell group B   
          3: Read back cell group C   
          4: Read back cell group D 
 @param[in] uint8_t total_ic; This is the number of ICs in the daisy chain(-1 only) 
 @param[out] uint16_t cell_codes[]; An array of the parsed cell codes from lowest to highest. The cell codes will
  be stored in the cell_codes[] array in the following format:
  |  cell_codes[0][0]| cell_codes[0][1] |  cell_codes[0][2]|    .....     |  cell_codes[0][11]|  cell_codes[1][0] | cell_codes[1][1]|  .....   |
  |------------------|------------------|------------------|--------------|-------------------|-------------------|-----------------|----------|
  |IC1 Cell 1        |IC1 Cell 2        |IC1 Cell 3        |    .....     |  IC1 Cell 12      |IC2 Cell 1         |IC2 Cell 2       | .....    |
  @return int8_t, PEC Status.
 
		0: No PEC error detected
  
		-1: PEC error detected, retry read
 
 *************************************************/
uint8_t LTC6811_ReadCellVolt(uint8_t reg,uint16_t cell_codes[2][12])
{
	uint8_t 	cell_data[16];
	uint8_t 	pec_error = 0;
	uint16_t 	parsed_cell;
	uint16_t 	received_pec;
	uint16_t 	data_pec;
	uint8_t 	data_counter = 0; //data counter
	
	if (reg == 0)
	{
		for(uint8_t cell_reg = 1; cell_reg<5; cell_reg++)         			 			
		{
			data_counter = 0;
			LTC6811_Rdcv_Reg(cell_reg,cell_data);								

			for (uint8_t current_ic = 0 ; current_ic < ModuleAmount; current_ic++) 			
			{																 	  			
				for(uint8_t current_cell = 0; current_cell<3; current_cell++)	 	
				{														   		  			
					parsed_cell = cell_data[data_counter] + (cell_data[data_counter + 1] << 8);												 
					cell_codes[current_ic][current_cell  + ((cell_reg - 1) * 3)] = parsed_cell;
					data_counter = data_counter + 2;											 																
				}

		        received_pec = (cell_data[data_counter] << 8) + cell_data[data_counter+1]; 															   
		        data_pec = Pec15_Calc(6, &cell_data[current_ic * LTC6811_REG_LEN]);
		        if(received_pec != data_pec)
		        {
		          	pec_error = -1;																		
		        }
				data_counter=data_counter+2;															
			}
		}
	}
	else
	{
	    LTC6811_Rdcv_Reg(reg,cell_data);

	    for (uint8_t current_ic = 0 ; current_ic < ModuleAmount; current_ic++) 				
	    {
			for(uint8_t current_cell = 0; current_cell < 3; current_cell++)   
	        {														   		  			
				parsed_cell = cell_data[data_counter] + (cell_data[data_counter+1]<<8); 											
				cell_codes[current_ic][current_cell + ((reg - 1) * 3)] = 0x0000FFFF & parsed_cell;
				data_counter= data_counter + 2;														
			}

		    received_pec = (cell_data[data_counter] << 8 )+ cell_data[data_counter + 1]; 					     
	        data_pec = Pec15_Calc(6, &cell_data[current_ic * LTC6811_REG_LEN]);
	        
			if(received_pec != data_pec)
			{
				pec_error = -1;															
			}
			data_counter= data_counter + 2; 																													
		}
  	}
    
	return (pec_error);
}


/***********************************************//**
 brief Read the raw data from the LTC6811 cell voltage register
 
 The function reads a single cell voltage register and stores the read data
 in the *data point as a byte array. This function is rarely used outside of 
 the LTC6811_rdcv() command. 
 @param[in] uint8_t reg; This controls which cell voltage register is read back.    
          1: Read back cell group A 
          2: Read back cell group B  
          3: Read back cell group C 
          4: Read back cell group D   		  		  
 @param[in] uint8_t total_ic; This is the number of ICs in the daisy chain(-1 only)
 @param[out] uint8_t *data; An array of the unparsed cell codes 
Command Code:
------------
|CMD[0:1]	|  15   |  14   |  13   |  12   |  11   |  10   |   9   |   8   |   7   |   6   |   5   |   4   |   3   |   2   |   1   |   0   | 
|---------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|
|RDCVA:	|   0   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   1   |   0   |   0   |
|RDCVB:	|   0   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   1   |   1   |   0   | 
|RDCVC:	|   0   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   1   |   0   |   0   |   0   | 
|RDCVD:	|   0   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   1   |   0   |   1   |   0   |  

 *************************************************/
void LTC6811_Rdcv_Reg(uint8_t reg,uint8_t *data)
{
	//number of bytes in each ICs register + 2 bytes for the PEC
	uint8_t cmd[4];
	uint16_t pec;

	if (1 == reg)     //1: RDCVA
	{
		cmd[1] = 0x04;
		cmd[0] = 0x00;
	}
	else if(2 == reg) //2: RDCVB
	{
		cmd[1] = 0x06;
		cmd[0] = 0x00;
	} 
	else if(3 == reg) //3: RDCVC
	{
		cmd[1] = 0x08;
		cmd[0] = 0x00;
	} 
	else if(4 == reg) //4: RDCVD
	{
		cmd[1] = 0x0A;
		cmd[0] = 0x00;
	} 

	pec = Pec15_Calc(2, cmd);
	cmd[2] = (uint8_t)(pec >> 8);
	cmd[3] = (uint8_t)(pec); 
  
	Set_Ltc6811(0b0);
	SPI_Write_Read(cmd,4,data,LTC6811_REG_LEN*ModuleAmount);
	
	Set_Ltc6811(0b1);
}

/*!******************************************************************************************************
 brief Start an GPIO Conversion
  Starts an ADC conversions of the LTC6811 GPIO inputs.
  The type of ADC conversion executed can be changed by setting the associated global variables:
 |Variable|Function                                      | 
 |--------|----------------------------------------------|
 | MD     | Determines the filter corner of the ADC      |
 | CHG    | Determines which GPIO channels are converted |
 
Command Code:
-------------

|CMD[0:1]	|  15   |  14   |  13   |  12   |  11   |  10   |   9   |   8   |   7   |   6   |   5   |   4   |   3   |   2   |   1   |   0   | 
|---------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|
|g_ADAX:	|   0   |   0   |   0   |   0   |   0   |   1   |   0   | MD[1] | MD[2] |   1   |   1   |  DCP  |   0   | CHG[2]| CHG[1]| CHG[0]| 
*********************************************************************************************************/
void LTC6811_Adax(void)
{
  uint8_t cmd[4];
  uint16_t cmd_pec;
 
  cmd[0] = g_ADAX[0];
  cmd[1] = g_ADAX[1];
  cmd_pec = Pec15_Calc(2, g_ADAX);
  cmd[2] = (uint8_t)(cmd_pec >> 8);
  cmd[3] = (uint8_t)(cmd_pec);
 
  Set_Ltc6811(0b0);
  SPI_Write_Array(4,cmd);
  Set_Ltc6811(0b1);
}

/***********************************************************************************//**
 \brief Reads and parses the LTC6811 auxiliary registers.
 The function is used
 to read the  parsed GPIO codes of the LTC6811. This function will send the requested 
 read commands parse the data and store the gpio voltages in aux_codes variable 
@param[in] uint8_t reg; This controls which GPIO voltage register is read back. 	  
          0: Read back all auxiliary registers   
          1: Read back auxiliary group A   
          2: Read back auxiliary group B 
@param[in] uint8_t total_ic; This is the number of ICs in the daisy chain(-1 only) 
@param[out] uint16_t aux_codes[][6]; A two dimensional array of the gpio voltage codes. The GPIO codes will
 be stored in the aux_codes[][6] array in the following format:
 |  aux_codes[0][0]| aux_codes[0][1] |  aux_codes[0][2]|  aux_codes[0][3]|  aux_codes[0][4]|  aux_codes[0][5]| aux_codes[1][0] |aux_codes[1][1]|  .....    |
 |-----------------|-----------------|-----------------|-----------------|-----------------|-----------------|-----------------|---------------|-----------|
 |IC1 GPIO1        |IC1 GPIO2        |IC1 GPIO3        |IC1 GPIO4        |IC1 GPIO5        |IC1 Vref2        |IC2 GPIO1        |IC2 GPIO2      |  .....    |
 
@return  int8_t, PEC Status
 
  0: No PEC error detected
  
 -1: PEC error detected, retry read
 *************************************************/
int8_t LTC6811_ReadAux(uint8_t reg,uint16_t aux_codes[ModuleAmount][6])
{
	const uint8_t NUM_RX_BYT = 8;
	const uint8_t BYT_IN_REG = 6;
	const uint8_t GPIO_IN_REG = 3;

	uint8_t 	data[8*ModuleAmount];
	uint8_t 	data_counter = 0; 
	int8_t 		pec_error = 0;
	uint16_t 	parsed_aux;
	uint16_t 	received_pec;
	uint16_t 	data_pec;

	if (reg == 0)
	{
		for(uint8_t reg = 1; reg<3; reg++)		 	   		 			
		{
			data_counter = 0;
			LTC6811_Rdaux_Reg(reg,data);									
			for (uint8_t current_ic = 0 ; current_ic < ModuleAmount; current_ic++) 			
			{																 	  			
				for(uint8_t current_gpio = 0; current_gpio< GPIO_IN_REG; current_gpio++)	
				{														   		  			
					parsed_aux = data[data_counter] + (data[data_counter+1]<<8);              					    
					aux_codes[current_ic][current_gpio +((reg-1)*GPIO_IN_REG)] = parsed_aux;
					data_counter=data_counter+2;												
				}
				received_pec = (data[data_counter]<<8)+ data[data_counter+1]; 											     
				data_pec = Pec15_Calc(BYT_IN_REG, &data[current_ic*NUM_RX_BYT]);
				if(received_pec != data_pec)
				{
					pec_error = -1;
				}
				data_counter=data_counter+2;																							
			}
		}

	}
	else
	{
		LTC6811_Rdaux_Reg(reg, data);
		for (int current_ic = 0 ; current_ic < ModuleAmount; current_ic++) 			  		
		{							   									          		
			for(int current_gpio = 0; current_gpio<GPIO_IN_REG; current_gpio++)  	 	
			{				
				parsed_aux = (data[data_counter] + (data[data_counter+1]<<8));    		
														
				aux_codes[current_ic][current_gpio +((reg-1)*GPIO_IN_REG)] = parsed_aux;
				data_counter=data_counter+2;									 		
			}

			received_pec = (data[data_counter]<<8) + data[data_counter+1]; 				 
												     
			data_pec = Pec15_Calc(BYT_IN_REG, &data[current_ic*NUM_RX_BYT]);
			if(received_pec != data_pec)
			{
				pec_error = -1;
			}
			data_counter=data_counter+2;												
		}
	}

	return (pec_error);
}


/***********************************************//**
 brief Read the raw data from the LTC6811 auxiliary register
 The function reads a single GPIO voltage register and stores thre read data
 in the *data point as a byte array. This function is rarely used outside of 
 the LTC6811_rdaux() command. 
 @param[in] uint8_t reg; This controls which GPIO voltage register is read back. 	  
          1: Read back auxiliary group A  
          2: Read back auxiliary group B  
@param[in] uint8_t total_ic; This is the number of ICs in the daisy chain
@param[out] uint8_t *data; An array of the unparsed aux codes 
Command Code:
-------------

|CMD[0:1]	    |  15   |  14   |  13   |  12   |  11   |  10   |   9   |   8   |   7   |   6   |   5   |   4   |   3   |   2   |   1   |   0   | 
|-------------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|
|RDAUXA:	    |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   1   |   1   |   0   |   0   |
|RDAUXB:	    |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   1   |   1   |   1   |   0   | 

 *************************************************/
void LTC6811_Rdaux_Reg(uint8_t reg,uint8_t *data)
{
	const uint8_t REG_LEN = 8; // number of bytes in the register + 2 bytes for the PEC
	uint8_t cmd[4];
	uint16_t cmd_pec;

	if (1 == reg)			//Read back auxiliary group A
	{
		cmd[1] = 0x0C;
		cmd[0] = 0x00;
	}
	else if(2 == reg)		//Read back auxiliary group B 
	{
		cmd[1] = 0x0e;
		cmd[0] = 0x00;
	} 
	else					//Read back auxiliary group A
	{
		cmd[1] = 0x0C;		
		cmd[0] = 0x00;
	}

	cmd_pec = Pec15_Calc(2, cmd);
	cmd[2] = (uint8_t)(cmd_pec >> 8);
	cmd[3] = (uint8_t)(cmd_pec);
	//4
	Set_Ltc6811(0b0);
	SPI_Write_Read(cmd,4,data,(REG_LEN*ModuleAmount));
	Set_Ltc6811(0b1);
}


/********************************************************//**
 \brief Clears the LTC6811 cell voltage registers
 The command clears the cell voltage registers and intiallizes 
 all values to 1. The register will read back hexadecimal 0xFF
 after the command is sent.
Command Code:
------------- 
|CMD[0:1]	    |  15   |  14   |  13   |  12   |  11   |  10   |   9   |   8   |   7   |   6   |   5   |   4   |   3   |   2   |   1   |   0   | 
|---------------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|
|CLRCELL:	    |   0   |   0   |   0   |   0   |   0   |   1   |   1   |   1   |   0   |   0   |   0   |   1   |   0   |   0   |   0   |   1   |
************************************************************/
void LTC6811_ClrCell(void)
{
	uint8_t cmd[4];
	uint16_t cmd_pec;

	cmd[0] = 0x07;
	cmd[1] = 0x11;

	cmd_pec = Pec15_Calc(2, cmd);
	cmd[2] = (uint8_t)(cmd_pec >> 8);
	cmd[3] = (uint8_t)(cmd_pec );
	//4
	Set_Ltc6811(0b0);
	SPI_Write_Read(cmd,4,0,0);
	Set_Ltc6811(0b1);
}

/***********************************************************//**
 \brief Clears the LTC6811 Auxiliary registers
 The command clears the Auxiliary registers and intiallizes 
 all values to 1. The register will read back hexadecimal 0xFF
 after the command is sent.
Command Code:
-------------
|CMD[0:1]	    |  15   |  14   |  13   |  12   |  11   |  10   |   9   |   8   |   7   |   6   |   5   |   4   |   3   |   2   |   1   |   0   | 
|-------------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|
|CLRAUX:	    |   0   |   0   |   0   |   0   |   0   |   1   |   1   |   1   |   0   |   0   |   0   |   1   |   0   |   0   |   2   |   0   |
***************************************************************/
void LTC6811_ClrAux(void)
{
	uint8_t cmd[4];
	uint16_t cmd_pec;

	cmd[0] = 0x07;
	cmd[1] = 0x12;

	cmd_pec = Pec15_Calc(2, cmd);
	cmd[2] = (uint8_t)(cmd_pec >> 8);
	cmd[3] = (uint8_t)(cmd_pec);
	//4
	Set_Ltc6811(0b0);
	SPI_Write_Read(cmd,4,0,0);
	Set_Ltc6811(0b1);
}



