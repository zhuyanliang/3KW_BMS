/* 
 * File:   newfile.h
 * Author: Administrator
 *
 * Created on 2016��6��21��, ����11:35
 */

#ifndef INCLUDE_H
#define	INCLUDE_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <pic18.h>
#include <xc.h>
#include <stdlib.h>
#include "common.h"
#include "gpio.h"
#include "isr.h"
#include "timer.h"
#include "flash.h"
#include "eeprom.h"
#include "adc.h"
#include "crc8.h"
#include "spi.h"
#include "ecan.h"
#include "LTC6811.h"
#include "config.h"
#include "detect.h"
#include "manage.h"
#include "soc.h"
#include "soh.h"   
#include "selftest.h"
#include "fault.h"  
#include "canbus.h"
#include "lcd.h"
#include "sci.h"
#include "tsksci.h"

#ifdef	__cplusplus
}
#endif

#endif	/* INCLUDE_H */

