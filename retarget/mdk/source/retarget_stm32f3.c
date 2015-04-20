#include "retarget_stm32f3.h"

void UART2_init(void){
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	
	/*Enable peripherial clock for GPIOA*/
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA,ENABLE);
	/*Enable peipherial clock for UART2*/
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
	
	/*GPIOA Configuration PA2 as TX PA3 as RX*/
	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_2|GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_UP;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	/*Connect USART2 pins to AF2*/
	//TX=PA2
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource2,GPIO_AF_7);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource3,GPIO_AF_7);
	
	USART_StructInit(&USART_InitStructure);
	USART_InitStructure.USART_BaudRate=9600;
	USART_InitStructure.USART_WordLength=USART_WordLength_8b;
	USART_InitStructure.USART_StopBits=USART_StopBits_1;
	USART_InitStructure.USART_Parity=USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl=USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode=USART_Mode_Tx|USART_Mode_Rx;
	USART_Init(USART2,&USART_InitStructure);
	USART_Cmd(USART2,ENABLE);
}



int __getkey(void){
	while(!USART_GetFlagStatus(USART2,USART_FLAG_RXNE));
	return USART_ReceiveData(USART2);
}

int __sendchar(int ch){
	while(!USART_GetFlagStatus(USART2,USART_FLAG_TXE));
  
	USART_SendData(USART2,ch);
	return ch;
}
