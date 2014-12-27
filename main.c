#include <stm32f10x_lib.h>
#include "sys.h"	
#include "delay.h"	
#include "stdio.h"
#include "init.h"	
 

u8 USART_RX_BUF[256];    //RXT buffer maxium 255bytes
u8 USART_RX_STA=0;       //RX Compleate	 
u8 USART_RX_CNT=0;       //RX Entre Count	
u16 echo=0x7855;
u16 adcx1,adcx2,adcx3,adcx4; 
u8 cntt=1;
char pussy='b';	
int c,d,e,f;
int main(void)
{		
 	Stm32_Clock_Init(9); //system clock init   always init first
	IO_Init();
	GPIOB->CRL&=0XF0FFFFFF;//PB6 Output
	GPIOB->CRL|=0X03000000;//AOUT 
	delay_init(72);	     //delay init
	uart_init(72,9600);  //uart init
	PWM4_Init(150,18000);	 //count to 100, freq/18000
	PWM2_Init(100,1800);	 //count to 100, freq/18000	 //IR aqusition pulse
	Timer3_Init(65535,30);	  //time refrence for echo
	EXTIX_Init();
	Adc_Init();
//	LED0_PWM_VAL=led0pwmval;
	LED0_PWM_VAL2=0x34;
	LED0_PWM_VAL4=0x34;

   	while(1)
	{	
		printf("%u\t%u\t%u\t%u",adcx1,adcx2,adcx3,adcx4);
		printf("\n");
	//	USART1->DR=echo/0xff;
	//	while((USART1->SR&0X40)==0);//wait for transfer to compleate
		LED0=!LED0; 
		delay_us(100000);//wait for the end of transmission	   
	}	 
} 

void EXTI0_IRQHandler(void)
{
	delay_us(10);
	if(PBin(0)==1)
	{
		TIM3->CR1|=0x01;   //start timer
	}
	else
	{
		TIM3->CR1&=0xfe;//stop timer
		echo=TIM3->CNT;	//read value
		TIM3->CNT=0x00; //clear timer
	}
	EXTI->PR=1<<0;  //Clear Interrupt flag 
}

void USART1_IRQHandler(void)
{
	u8 res;	    
	if(USART1->SR&(1<<5))//RXT recieved
	{	 
		res=USART1->DR;	 //read out data and clear interrupt flag
//		LED0_PWM_VAL=res;
		if(!USART_RX_STA)
		{ 	
			USART_RX_BUF[USART_RX_CNT++]=res;
			if(USART_RX_CNT==255)USART_RX_CNT=0;//RXT data over flow,start over	  
		}
	}  											 
}

void TIM2_IRQHandler(void)	 //finish it later when I have access to pots
{ 		    		  			    
	if(TIM2->SR&0X0001)//Overflow Interrupt
	{
		IRO=0;//defult 1
				    				   				     	    	
	}				   
	TIM2->SR&=~(1<<0);//Clear Interrupt flag 	    
}

void ADC1_2_IRQHandler(void)	 //finish it later when I have access to pots
{ 		    		  			    
	switch (cntt){
		case 1:
			adcx1=ADC1->DR;
			break;
		case 2:
			adcx2=ADC1->DR;
			break;
		case 3:
			adcx3=ADC1->DR;
			break;
		case 4:
			adcx4=ADC1->DR;
			break;
	}
	cntt++;
	if(cntt==5)
		cntt=1;
	IRO=!IRO;//defult 1

				    				   				     	    	   
}


