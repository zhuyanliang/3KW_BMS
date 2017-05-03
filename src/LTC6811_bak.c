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

/*!
  6804 conversion command variables.  
*/
uint8_t ADCV[2]; //!< Cell Voltage conversion command.
uint8_t ADAX[2]; //!< GPIO conversion command.


/*!
  \brief This function will initialize all 6811 variables and the SPI port.

  This function will initialize the Linduino to communicate with the LTC6811 with a 1MHz SPI clock.
  The Function also intializes the ADCV and ADAX commands to convert all cell and GPIO voltages in
  the Normal ADC mode.
*/
void LTC6811_initialize(void)
{
	TRISCbits.TRISC2 = 0b0;  // LTC6811µÄCS¹Ü½Å
	
	set_adc(MD_NORMAL,DCP_DISABLED,CELL_CH_ALL,AUX_CH_ALL);
}

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
void set_adc(uint8_t MD, //ADC Mode
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
|ADCV:	   |   0   |   0   |   0   |   0   |   0   |   0   |   1   | MD[1] | MD[2] |   1   |   1   |  DCP  |   0   | CH[2] | CH[1] | CH[0] |  
***********************************************************************************************/
void LTC6811_adcv(void)
{

	uint8_t cmd[4];
	uint16_t cmd_pec;

	cmd[0] = ADCV[0];
	cmd[1] = ADCV[1];

	cmd_pec = pec15_calc(2, ADCV);
	cmd[2] = (uint8_t)(cmd_pec >> 8);
	cmd[3] = (uint8_t)(cmd_pec);

	Set_Ltc6811(0b0);
	spi_write_array(4,cmd);
	Set_Ltc6811(0b1);
}
/*
  LTC6811_adcv Function sequence:
  
  1. Load adcv command into cmd array
  2. Calculate adcv cmd PEC and load pec into cmd array
  3. wakeup isoSPI port, this step can be removed if isoSPI status is previously guaranteed
  4. send broadcast adcv command to LTC6811 daisy chain
*/


/*!******************************************************************************************************
 \brief Start an GPIO Conversion
 
  Starts an ADC conversions of the LTC6811 GPIO inputs.
  The type of ADC conversion executed can be changed by setting the associated global variables:
 |Variable|Function                                      | 
 |--------|----------------------------------------------|
 | MD     | Determines the filter corner of the ADC      |
 | CHG    | Determines which GPIO channels are converted |
 
 
Command Code:
-------------

|CMD[0:1]	|  15   |  14   |  13   |  12   |  11   |  10   |   9   |   8   |   7   |   6   |   5   |   4   |   3   |   2   |   1   |   0   | 
|-----------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|
|ADAX:	    |   0   |   0   |   0   |   0   |   0   |   1   |   0   | MD[1] | MD[2] |   1   |   1   |  DCP  |   0   | CHG[2]| CHG[1]| CHG[0]| 
*********************************************************************************************************/
void LTC6811_adax()
{
  uint8_t cmd[4];
  uint16_t cmd_pec;
 
  cmd[0] = ADAX[0];
  cmd[1] = ADAX[1];
  cmd_pec = pec15_calc(2, ADAX);
  cmd[2] = (uint8_t)(cmd_pec >> 8);
  cmd[3] = (uint8_t)(cmd_pec);
 
  Set_Ltc6811(0b0);
  spi_write_array(4,cmd);
  Set_Ltc6811(0b1);

}
/*
  LTC6811_adax Function sequence:
  
  1. Load adax command into cmd array
  2. Calculate adax cmd PEC and load pec into cmd array
  3. wakeup isoSPI port, this step can be removed if isoSPI status is previously guaranteed
  4. send broadcast adax command to LTC6811 daisy chain
*/


/***********************************************//**
 \brief Reads and parses the LTC6811 cell voltage registers.
 
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
uint8_t LTC6811_rdcv(uint8_t reg, // Controls which cell voltage register is read back.
					 uint8_t total_ic, // the number of ICs in the system
					 uint16_t cell_codes[][12] // Array of the parsed cell codes
					 )
{
  
	const uint8_t NUM_RX_BYT = 8;
	const uint8_t BYT_IN_REG = 6;
	const uint8_t CELL_IN_REG = 3;

	//uint8_t *cell_data;
	uint8_t cell_data[16];
	uint8_t pec_error = 0;
	uint16_t parsed_cell;
	uint16_t received_pec;
	uint16_t data_pec;
	uint8_t data_counter=0; //data counter
	//cell_data = (uint8_t *) malloc((NUM_RX_BYT*total_ic)*sizeof(uint8_t));
	//1.a
	if (reg == 0)
	{
		//a.i
		//executes once for each of the LTC6811 cell voltage registers
		for(uint8_t cell_reg = 1; cell_reg<5; cell_reg++)         			 			
		{
			data_counter = 0;
			//Reads a single Cell voltage register
			LTC6811_rdcv_reg(cell_reg, total_ic,cell_data );								

			// executes for every LTC6811 in the daisy chain
			// current_ic is used as the IC counter
			for (uint8_t current_ic = 0 ; current_ic < total_ic; current_ic++) 			
			{																 	  			

				//a.ii
				// This loop parses the read back data into cell voltages, it 
				// loops once for each of the 3 cell voltage codes in the register 
				for(uint8_t current_cell = 0; current_cell<CELL_IN_REG; current_cell++)	 	
				{														   		  			
					//Each cell code is received as two bytes and is combined to
					// create the parsed cell voltage code
					parsed_cell = cell_data[data_counter] + (cell_data[data_counter + 1] << 8);												 
					cell_codes[current_ic][current_cell  + ((cell_reg - 1) * CELL_IN_REG)] = parsed_cell;
					//Because cell voltage codes are two bytes the data counter
					//must increment by two for each parsed cell code
					data_counter = data_counter + 2;											 																
				}
				//a.iii
				//The received PEC for the current_ic is transmitted as the 7th and 8th
				//after the 6 cell voltage data bytes
		        received_pec = (cell_data[data_counter] << 8) + cell_data[data_counter+1]; 															   
		        data_pec = pec15_calc(BYT_IN_REG, &cell_data[current_ic * NUM_RX_BYT]);
		        if(received_pec != data_pec)
		        {
		        	//The pec_error variable is simply set negative if any PEC errors 
		        	//are detected in the serial data
		          	pec_error = -1;																		
		        }
				//Because the transmitted PEC code is 2 bytes long the data_counter
				//must be incremented by 2 bytes to point to the next ICs cell voltage data
				data_counter=data_counter+2;															
			}
		}
	}
	//1.b
	else
	{
		//b.i
	    LTC6811_rdcv_reg(reg, total_ic,cell_data);
	    // executes for every LTC6811 in the daisy chain
	    // current_ic is used as the IC counter
	    for (uint8_t current_ic = 0 ; current_ic < total_ic; current_ic++) 				
	    {																 	  			
			//b.ii
			// This loop parses the read back data into cell voltages, it 
			// loops once for each of the 3 cell voltage codes in the register 
			for(uint8_t current_cell = 0; current_cell < CELL_IN_REG; current_cell++)   
	        {														   		  			
				//Each cell code is received as two bytes and is combined to
				// create the parsed cell voltage code
				parsed_cell = cell_data[data_counter] + (cell_data[data_counter+1]<<8); 											

				//Because cell voltage codes are two bytes the data counter
				//must increment by two for each parsed cell code
				cell_codes[current_ic][current_cell + ((reg - 1) * CELL_IN_REG)] = 0x0000FFFF & parsed_cell;
				data_counter= data_counter + 2;														
			}
			//b.iii
			//The received PEC for the current_ic is transmitted as the 7th and 8th
			//after the 6 cell voltage data bytes
		    received_pec = (cell_data[data_counter] << 8 )+ cell_data[data_counter + 1]; 					     
	        data_pec = pec15_calc(BYT_IN_REG, &cell_data[current_ic * NUM_RX_BYT]);
	        
			if(received_pec != data_pec)
			{
				pec_error = -1;															
			}
			//Because the transmitted PEC code is 2 bytes long the data_counter
			//must be incremented by 2 bytes to point to the next ICs cell voltage data
			data_counter= data_counter + 2; 																													
		}
  	}

	//free(cell_data);
	return (pec_error);
}
/*
	LTC6811_rdcv Sequence
	
	1. Switch Statement:
		a. Reg = 0
			i. Read cell voltage registers A-D for every IC in the daisy chain
			ii. Parse raw cell voltage data in cell_codes array
			iii. Check the PEC of the data read back vs the calculated PEC for each read register command
		b. Reg != 0 
			i.Read single cell voltage register for all ICs in daisy chain
			ii. Parse raw cell voltage data in cell_codes array
			iii. Check the PEC of the data read back vs the calculated PEC for each read register command
	2. Return pec_error flag
*/


/***********************************************//**
 \brief Read the raw data from the LTC6811 cell voltage register
 
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
-------------

|CMD[0:1]	|  15   |  14   |  13   |  12   |  11   |  10   |   9   |   8   |   7   |   6   |   5   |   4   |   3   |   2   |   1   |   0   | 
|-----------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|
|RDCVA:	    |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   1   |   0   |   0   |
|RDCVB:	    |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   1   |   1   |   0   | 
|RDCVC:	    |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   1   |   0   |   0   |   0   | 
|RDCVD:	    |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   1   |   0   |   1   |   0   |  

 *************************************************/
void LTC6811_rdcv_reg(uint8_t reg, //Determines which cell voltage register is read back
					  uint8_t total_ic, //the number of ICs in the
					  uint8_t *data //An array of the unparsed cell codes
					  )
{
	//number of bytes in each ICs register + 2 bytes for the PEC
	const uint8_t REG_LEN = 8; 
	uint8_t cmd[4];
	uint16_t cmd_pec;

	if (reg == 1)     //1: RDCVA
	{
		cmd[1] = 0x04;
		cmd[0] = 0x00;
	}
	else if(reg == 2) //2: RDCVB
	{
		cmd[1] = 0x06;
		cmd[0] = 0x00;
	} 
	else if(reg == 3) //3: RDCVC
	{
		cmd[1] = 0x08;
		cmd[0] = 0x00;
	} 
	else if(reg == 4) //4: RDCVD
	{
		cmd[1] = 0x0A;
		cmd[0] = 0x00;
	} 

	cmd_pec = pec15_calc(2, cmd);
	cmd[2] = (uint8_t)(cmd_pec >> 8);
	cmd[3] = (uint8_t)(cmd_pec); 
  
	Set_Ltc6811(0b0);
	spi_write_read(cmd,4,data,(REG_LEN*total_ic));
	Set_Ltc6811(0b1);
}
/*
  LTC6811_rdcv_reg Function Process:
  1. Determine Command and initialize command array
  2. Calculate Command PEC
  3. Wake up isoSPI, this step is optional
  4. Send Global Command to LTC6811 daisy chain
*/


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
int8_t LTC6811_rdaux(uint8_t reg, //Determines which GPIO voltage register is read back. 
					 uint8_t total_ic,//the number of ICs in the system
					 uint16_t aux_codes[][6]//A two dimensional array of the gpio voltage codes.
					 )
{
	const uint8_t NUM_RX_BYT = 8;
	const uint8_t BYT_IN_REG = 6;
	const uint8_t GPIO_IN_REG = 3;

	//uint8_t *data;
	uint8_t data[8];
	uint8_t data_counter = 0; 
	int8_t pec_error = 0;
	uint16_t parsed_aux;
	uint16_t received_pec;
	uint16_t data_pec;
	//data = (uint8_t *) malloc((NUM_RX_BYT*total_ic)*sizeof(uint8_t));
	//1.a
	if (reg == 0)
	{
		//a.i
		//executes once for each of the LTC6811 aux voltage registers
		for(uint8_t gpio_reg = 1; gpio_reg<3; gpio_reg++)		 	   		 			
		{
			data_counter = 0;
			//Reads the raw auxiliary register data into the data[] array
			LTC6811_rdaux_reg(gpio_reg, total_ic,data);									

			// executes for every LTC6811 in the daisy chain
			// current_ic is used as the IC counter
			for (uint8_t current_ic = 0 ; current_ic < total_ic; current_ic++) 			
			{																 	  			

				//a.ii
				// This loop parses the read back data into GPIO voltages, it 
				// loops once for each of the 3 gpio voltage codes in the register 
				for(uint8_t current_gpio = 0; current_gpio< GPIO_IN_REG; current_gpio++)	
				{														   		  			
					//Each gpio codes is received as two bytes and is combined to
					// create the parsed gpio voltage code
					parsed_aux = data[data_counter] + (data[data_counter+1]<<8);              					    
					aux_codes[current_ic][current_gpio +((gpio_reg-1)*GPIO_IN_REG)] = parsed_aux;
					//Because gpio voltage codes are two bytes the data counter
					//must increment by two for each parsed gpio voltage code
					data_counter=data_counter+2;												
				}
				//a.iii
				received_pec = (data[data_counter]<<8)+ data[data_counter+1]; 	
				//The received PEC for the current_ic is transmitted as the 7th and 8th
				//after the 6 gpio voltage data bytes												     
				data_pec = pec15_calc(BYT_IN_REG, &data[current_ic*NUM_RX_BYT]);
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
		//b.i
		LTC6811_rdaux_reg(reg, total_ic, data);
		// executes for every LTC6811 in the daisy chain
		// current_ic is used as an IC counter
		for (int current_ic = 0 ; current_ic < total_ic; current_ic++) 			  		
		{							   									          		

			//b.ii
			// This loop parses the read back data. Loops 
			// once for each aux voltage in the register 
			for(int current_gpio = 0; current_gpio<GPIO_IN_REG; current_gpio++)  	 	
			{				
				//Each gpio codes is received as two bytes and is combined to
				// create the parsed gpio voltage code
				parsed_aux = (data[data_counter] + (data[data_counter+1]<<8));    		
														
				aux_codes[current_ic][current_gpio +((reg-1)*GPIO_IN_REG)] = parsed_aux;
				//Because gpio voltage codes are two bytes the data counter
			 	//must increment by two for each parsed gpio voltage code
				data_counter=data_counter+2;									 		
			}
			//b.iii
			//The received PEC for the current_ic is transmitted as the 7th and 8th
			//after the 6 gpio voltage data bytes
			received_pec = (data[data_counter]<<8) + data[data_counter+1]; 				 
												     
			data_pec = pec15_calc(BYT_IN_REG, &data[current_ic*NUM_RX_BYT]);
			if(received_pec != data_pec)
			{
				pec_error = -1;
			}
			//Because the transmitted PEC code is 2 bytes long the data_counter
			//must be incremented by 2 bytes to point to the next ICs gpio voltage data
			data_counter=data_counter+2;												
		}
	}
	//free(data);
	return (pec_error);
}
/*
	LTC6811_rdaux Sequence
	
	1. Switch Statement:
		a. Reg = 0
			i. Read GPIO voltage registers A-D for every IC in the daisy chain
			ii. Parse raw GPIO voltage data in cell_codes array
			iii. Check the PEC of the data read back vs the calculated PEC for each read register command
		b. Reg != 0 
			i.Read single GPIO voltage register for all ICs in daisy chain
			ii. Parse raw GPIO voltage data in cell_codes array
			iii. Check the PEC of the data read back vs the calculated PEC for each read register command
	2. Return pec_error flag
*/


/***********************************************//**
 \brief Read the raw data from the LTC6811 auxiliary register
 
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
|---------------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|
|RDAUXA:	    |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   1   |   1   |   0   |   0   |
|RDAUXB:	    |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   1   |   1   |   1   |   0   | 

 *************************************************/
void LTC6811_rdaux_reg(uint8_t reg, //Determines which GPIO voltage register is read back
					   uint8_t total_ic, //The number of ICs in the system
					   uint8_t *data //Array of the unparsed auxiliary codes 
					   )
{
	const uint8_t REG_LEN = 8; // number of bytes in the register + 2 bytes for the PEC
	uint8_t cmd[4];
	uint16_t cmd_pec;

	if (reg == 1)			//Read back auxiliary group A
	{
		cmd[1] = 0x0C;
		cmd[0] = 0x00;
	}
	else if(reg == 2)		//Read back auxiliary group B 
	{
		cmd[1] = 0x0e;
		cmd[0] = 0x00;
	} 
	else					//Read back auxiliary group A
	{
		cmd[1] = 0x0C;		
		cmd[0] = 0x00;
	}

	cmd_pec = pec15_calc(2, cmd);
	cmd[2] = (uint8_t)(cmd_pec >> 8);
	cmd[3] = (uint8_t)(cmd_pec);
	//4
	Set_Ltc6811(0b0);
	spi_write_read(cmd,4,data,(REG_LEN*total_ic));
	Set_Ltc6811(0b1);

}
/*
  LTC6811_rdaux_reg Function Process:
  1. Determine Command and initialize command array
  2. Calculate Command PEC
  3. Wake up isoSPI, this step is optional
  4. Send Global Command to LTC6811 daisy chain
*/

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
void LTC6811_clrcell()
{
	uint8_t cmd[4];
	uint16_t cmd_pec;

	cmd[0] = 0x07;
	cmd[1] = 0x11;

	cmd_pec = pec15_calc(2, cmd);
	cmd[2] = (uint8_t)(cmd_pec >> 8);
	cmd[3] = (uint8_t)(cmd_pec );
	//4
	Set_Ltc6811(0b0);
	spi_write_read(cmd,4,0,0);
	Set_Ltc6811(0b1);
}
/*
  LTC6811_clrcell Function sequence:
  
  1. Load clrcell command into cmd array
  2. Calculate clrcell cmd PEC and load pec into cmd array
  3. wakeup isoSPI port, this step can be removed if isoSPI status is previously guaranteed
  4. send broadcast clrcell command to LTC6811 daisy chain
*/


/***********************************************************//**
 \brief Clears the LTC6811 Auxiliary registers
 
 The command clears the Auxiliary registers and intiallizes 
 all values to 1. The register will read back hexadecimal 0xFF
 after the command is sent.
 
 
Command Code:
-------------

|CMD[0:1]	    |  15   |  14   |  13   |  12   |  11   |  10   |   9   |   8   |   7   |   6   |   5   |   4   |   3   |   2   |   1   |   0   | 
|---------------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|
|CLRAUX:	    |   0   |   0   |   0   |   0   |   0   |   1   |   1   |   1   |   0   |   0   |   0   |   1   |   0   |   0   |   2   |   0   |
***************************************************************/
void LTC6811_clraux()
{
	uint8_t cmd[4];
	uint16_t cmd_pec;

	cmd[0] = 0x07;
	cmd[1] = 0x12;

	cmd_pec = pec15_calc(2, cmd);
	cmd[2] = (uint8_t)(cmd_pec >> 8);
	cmd[3] = (uint8_t)(cmd_pec);
	//4
	Set_Ltc6811(0b0);
	spi_write_read(cmd,4,0,0);
	Set_Ltc6811(0b1);
}
/*
  LTC6811_clraux Function sequence:
  
  1. Load clraux command into cmd array
  2. Calculate clraux cmd PEC and load pec into cmd array
  3. wakeup isoSPI port, this step can be removed if isoSPI status is previously guaranteed
  4. send broadcast clraux command to LTC6811 daisy chain
*/


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
|             |							CMD[0]                      |                            CMD[1]                             |
|-------------|---------------------------------------------------------------|---------------------------------------------------------------|
|CMD[0:1]	    |  15   |  14   |  13   |  12   |  11   |  10   |   9   |   8   |   7   |   6   |   5   |   4   |   3   |   2   |   1   |   0   | 
|-------------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|
|WRCFG:	    |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   1   |
********************************************************/
void LTC6811_wrcfg(uint8_t total_ic, //The number of ICs being written to
				   uint8_t config[][6] //A two dimensional array of the configuration data that will be written
				   )
{
	const uint8_t CMD_LEN = 4+(8*total_ic);
	uint8_t *cmd;
	uint16_t cfg_pec;
	uint8_t cmd_index; //command counter

	cmd = (uint8_t *)malloc(CMD_LEN*sizeof(uint8_t));

	cmd[0] = 0x00;
	cmd[1] = 0x01;
	cmd[2] = 0x3d;
	cmd[3] = 0x6e;
  
	cmd_index = 4;

	// executes for each LTC6811 in daisy chain, this loops starts with
	// the last IC on the stack. The first configuration written is
	// received by the last IC in the daisy chain			
	for (uint8_t current_ic = total_ic; current_ic > 0; current_ic--)				
	{	
		// executes for each of the 6 bytes in the CFGR register
		// current_byte is the byte counter
		for (uint8_t current_byte = 0; current_byte < 6; current_byte++) 	
		{																				
			//adding the config data to the array to be sent 
			cmd[cmd_index] = config[current_ic-1][current_byte]; 							
			cmd_index = cmd_index + 1;                
		}
		// calculating the PEC for each ICs configuration register data
		cfg_pec = (uint16_t)pec15_calc(6, &config[current_ic-1][0]);		
		cmd[cmd_index] = (uint8_t)(cfg_pec >> 8);
		cmd[cmd_index + 1] = (uint8_t)cfg_pec;
		cmd_index = cmd_index + 2;
	}

	Set_Ltc6811(0b0);
	spi_write_array(CMD_LEN, cmd);
	Set_Ltc6811(0b1);
	free(cmd);
}
/*
	WRCFG Sequence:
	1. Load cmd array with the write configuration command and PEC
	2. Load the cmd with LTC6811 configuration data
	3. Calculate the pec for the LTC6811 configuration data being transmitted
	4. wakeup isoSPI port, this step can be removed if isoSPI status is previously guaranteed
	5. Write configuration data to the LTC6811 daisy chain
	
*/

/*!******************************************************
 \brief Reads configuration registers of a LTC6811 daisy chain
 
@param[in] uint8_t total_ic: number of ICs in the daisy chain

@param[out] uint8_t r_config[][8] is a two dimensional array that the function stores the read configuration data. The configuration data for each IC 
is stored in blocks of 8 bytes with the configuration data of the lowest IC on the stack in the first 8 bytes 
block of the array, the second IC in the second 8 byte etc. Below is an table illustrating the array organization:

|r_config[0][0]|r_config[0][1]|r_config[0][2]|r_config[0][3]|r_config[0][4]|r_config[0][5]|r_config[0][6]  |r_config[0][7] |r_config[1][0]|r_config[1][1]|  .....    |
|--------------|--------------|--------------|--------------|--------------|--------------|----------------|---------------|--------------|--------------|-----------|
|IC1 CFGR0     |IC1 CFGR1     |IC1 CFGR2     |IC1 CFGR3     |IC1 CFGR4     |IC1 CFGR5     |IC1 PEC High    |IC1 PEC Low    |IC2 CFGR0     |IC2 CFGR1     |  .....    |


@return int8_t, PEC Status.
 
	0: Data read back has matching PEC
 
	-1: Data read back has incorrect PEC


Command Code:
-------------

|CMD[0:1]		|  15   |  14   |  13   |  12   |  11   |  10   |   9   |   8   |   7   |   6   |   5   |   4   |   3   |   2   |   1   |   0   | 
|---------------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|-------|
|RDCFG:	        |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   1   |   0   |   0   |   1   |   0   |
********************************************************/
int8_t LTC6811_rdcfg(uint8_t total_ic, //Number of ICs in the system
				     uint8_t r_config[][8] //A two dimensional array that the function stores the read configuration data.
					 )
{
	const uint8_t BYTES_IN_REG = 8;

	uint8_t cmd[4];
	uint8_t *rx_data;
	int8_t pec_error = 0; 
	uint16_t data_pec;
	uint16_t received_pec;

	rx_data = (uint8_t *) malloc((8*total_ic)*sizeof(uint8_t));

	cmd[0] = 0x00;
	cmd[1] = 0x02;
	cmd[2] = 0x2b;
	cmd[3] = 0x0A;
	//3
	Set_Ltc6811(0b0);
	//Read the configuration data of all ICs on the daisy chain into 
	//rx_data[] array
	spi_write_read(cmd, 4, rx_data, (BYTES_IN_REG*total_ic));         
	Set_Ltc6811(0b01);															

	//executes for each LTC6811 in the daisy chain and packs the data
	//into the r_config array as well as check the received Config data
	//for any bit errors
	for (uint8_t current_ic = 0; current_ic < total_ic; current_ic++) 			
	{ 																			
		//4.a																		
		for (uint8_t current_byte = 0; current_byte < BYTES_IN_REG; current_byte++)					
		{
			r_config[current_ic][current_byte] = rx_data[current_byte + (current_ic*BYTES_IN_REG)];
		}
		//4.b
		received_pec = (r_config[current_ic][6]<<8) + r_config[current_ic][7];
		data_pec = pec15_calc(6, &r_config[current_ic][0]);
		if(received_pec != data_pec)
		{
			pec_error = -1;
		}  
	}

	free(rx_data);
	//5
	return(pec_error);
}
/*
	RDCFG Sequence:
	
	1. Load cmd array with the write configuration command and PEC
	2. wakeup isoSPI port, this step can be removed if isoSPI status is previously guaranteed
	3. Send command and read back configuration data
	4. For each LTC6811 in the daisy chain
	  a. load configuration data into r_config array
	  b. calculate PEC of received data and compare against calculated PEC
	5. Return PEC Error
*/

/*!**********************************************************
 \brief calaculates  and returns the CRC15
  
  @param[in] uint8_t len: the length of the data array being passed to the function
               
  @param[in] uint8_t data[] : the array of data that the PEC will be generated from
  

  @returns The calculated pec15 as an unsigned int
***********************************************************/
uint16_t pec15_calc(uint8_t len, //Number of bytes that will be used to calculate a PEC
					uint8_t *data //Array of data that will be used to calculate  a PEC
					)
{
	uint16_t remainder,addr;
	
	remainder = 16;//initialize the PEC
	for(uint8_t i = 0; i<len;i++) // loops for each byte in data array
	{
		addr = ((remainder>>7)^data[i])&0xff;//calculate PEC table address 
		remainder = (remainder<<8)^crc15Table[addr];
	}
	//The CRC15 has a 0 in the LSB so the remainder must be multiplied by 2
	return (remainder*2);
}


/*!
 \brief Writes an array of bytes out of the SPI port
 
 @param[in] uint8_t len length of the data array being written on the SPI port
 @param[in] uint8_t data[] the data array to be written on the SPI port
 
*/
void spi_write_array(uint8_t len, // Option: Number of bytes to be written on the SPI port
					 uint8_t data[] //Array of bytes to be written on the SPI port
					 )
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
void spi_write_read(uint8_t tx_Data[],// array of data to be written on SPI port 
					uint8_t tx_len, 	// length of the tx data arry
					uint8_t *rx_data,	// Input: array that will store the data read by the SPI port
					uint8_t rx_len 		// Option: number of bytes to be read from the SPI port
					)
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



