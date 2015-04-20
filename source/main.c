#include "stm32f30x.h"                  // Device header
/*Led PB13, Button PC13*/
#include "retarget_stm32f3.h"
#include <stdio.h>
void delay_ms(int delay_time);
void led_init(void);
void lin_init(void);

volatile int BREAK_RECIVED=0;
char cBuffer[80];
char indexHead=0;
short dBuffer[80];
int irqCoutner=0;
int main(){
	
	UART2_init();
	led_init();
	lin_init();
	printf("\nsystem ready\n");
	while(1){
		gets(cBuffer);
		for(int i=0; i<indexHead;i++){
			if(dBuffer[i]<0){
				printf("break\n");
			}else{
				printf("0x%02x\n",dBuffer[i]);
			}
		}
		indexHead=0;
	}
}

void lin_init(void){
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	
	/*Enable peripherial clock for GPIOC*/
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC,ENABLE);
	/*Enable peipherial clock for UART1*/
	RCC_APB2PeriphClockCmd(RCC_APB2ENR_USART1EN,ENABLE);
	
	/*GPIOA Configuration PC4 as TX PC5 as RX*/
	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_4|GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_UP;
	GPIO_Init(GPIOC,&GPIO_InitStructure);
	
	/*Connect USART1 to PC4 and PC5, alternative fucntion 7*/
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource4,GPIO_AF_7);
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource5,GPIO_AF_7);
	/*Regular USART confguration*/
	USART_StructInit(&USART_InitStructure);
	USART_InitStructure.USART_BaudRate=9600;
	USART_InitStructure.USART_WordLength=USART_WordLength_8b;
	USART_InitStructure.USART_StopBits=USART_StopBits_1;
	USART_InitStructure.USART_Parity=USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl=USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode=USART_Mode_Tx|USART_Mode_Rx;
	USART_Init(USART1,&USART_InitStructure);
	/*Lin specific configuration*/
	USART_LINBreakDetectLengthConfig(USART1,USART_LINBreakDetectLength_10b);
	USART_LINCmd(USART1,ENABLE);
	
	/*Unmask Interrupts*/
	USART_ITConfig(USART1,USART_IT_LBD,ENABLE);
	USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);
	
	USART_Cmd(USART1,ENABLE);
	
	NVIC_EnableIRQ(USART1_IRQn);
}

void led_init(void){
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB,ENABLE);
	GPIO_InitTypeDef myGPIO;
	GPIO_StructInit(&myGPIO);
	myGPIO.GPIO_Pin=GPIO_Pin_13;
	myGPIO.GPIO_Mode=GPIO_Mode_OUT;
	myGPIO.GPIO_OType=GPIO_OType_PP;
	myGPIO.GPIO_PuPd=GPIO_PuPd_NOPULL;
	myGPIO.GPIO_Speed=GPIO_Speed_10MHz;
	GPIO_Init(GPIOB,&myGPIO);
	GPIO_WriteBit(GPIOB,GPIO_Pin_13,Bit_RESET);
}

void delay_ms(int delay_time){
	for(int i=0; i<delay_time; i++);
}

void USART1_IRQHandler(void){
	if(USART_GetFlagStatus(USART1,USART_FLAG_FE)){
		USART_ClearFlag(USART1,USART_FLAG_FE);
		USART_ClearFlag(USART1,USART_FLAG_RXNE);
		USART_ReceiveData(USART1);
	}else if(USART_GetFlagStatus(USART1,USART_FLAG_LBD)){
		USART_ClearFlag(USART1,USART_FLAG_LBD);
		GPIO_WriteBit(GPIOB,GPIO_Pin_13,!GPIO_ReadOutputDataBit(GPIOB,GPIO_Pin_13));
		dBuffer[indexHead]=-1;
		indexHead++;
	}else if(USART_GetFlagStatus(USART1,USART_FLAG_RXNE)){
		dBuffer[indexHead]=USART_ReceiveData(USART1);
		indexHead++;
		USART_ClearFlag(USART1,USART_FLAG_RXNE);
	}
	irqCoutner++;
}
