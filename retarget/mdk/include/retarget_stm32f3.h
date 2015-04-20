#ifndef __RETARGET_TM4C_H
#define __RETARGET_TM4C_H
#include "stm32f30x.h"                  // Device header
#include "retarget.h"
	// U0Rx (VCP receive) connected to PA2
	// U0Tx (VCP transmit) connected to PA3
	void UART2_init(void);
#endif
