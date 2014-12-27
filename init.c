#include "init.h"
void SystemInit(void);
void IO_Init()
{
	RCC->APB2ENR|=1<<3;    //Enable PortB Clock	  
	RCC->APB2ENR|=1<<2;    //Enable PortA Clock	  	 
}
void EXTIX_Init(void)
{
	RCC->APB2ENR|=1<<2;     //ʹ��PORTAʱ��
	JTAG_Set(JTAG_SWD_DISABLE);//�ر�JTAG��SWD   

	GPIOB->CRL&=0XFF0FFFF0;//PA0���ó�����	  
	GPIOB->CRL|=0X00300008;   
				   	
	Ex_NVIC_Config(GPIO_B,0,0x03); //�ش���

	MY_NVIC_Init(1,1,EXTI0_IRQChannel,2);   
	   
}


void PWM4_Init(u16 arr,u16 psc)
{		 					 
	RCC->APB1ENR|=1<<2;    //TIM4 Clock Enable    

	GPIOA->CRH&=0XFFFFFFF0;//PA8 Input
	GPIOA->CRH|=0X00000004;//float HR
	  	
	GPIOB->CRL&=0X0FFFFFFF;//PB7 Output
	GPIOB->CRL|=0XB0000000;// 	  
	GPIOB->ODR|=1<<7;//PB7 Pull up	

	TIM4->ARR=arr;//Auto Reload Value 
	TIM4->PSC=psc;//Freq Devider
	
	TIM4->CCMR1|=7<<12;  //CH2 PWM2 mode		 
	TIM4->CCMR1|=1<<11; //CH2preload enable   

	TIM4->CCER|=1<<4;   //OC2 output enable	   

	TIM4->CR1=0x80;     //ARPE enable
	TIM4->CR1|=0x01;    //enable timer3 									  
}  
void PWM2_Init(u16 arr,u16 psc)
{		 					 
	RCC->APB1ENR|=1<<0;    //TIM2 Clock Enable    

	  	
	GPIOA->CRL&=0XFFFFFF0F;//PA1 Output
	GPIOA->CRL|=0X000000B0;//AOUT 	  
	GPIOA->ODR|=1<<1;//PA1 Pull up	

	TIM2->ARR=arr;//Auto Reload Value 
	TIM2->PSC=psc;//Freq Devider
	
	TIM2->CCMR1|=7<<12;  //CH2 PWM2 mode		 
	TIM2->CCMR1|=1<<11; //CH2preload enable   

	TIM2->CCER|=1<<4;   //OC2 output enable	   

	TIM2->CR1=0x80;     //ARPE enable
	TIM2->CR1|=0x01;    //enable timer3 									  
} 	 
void Timer4_Init(u16 arr,u16 psc)
{
	RCC->APB1ENR|=1<<2;//TIM4 clock enable    
 	TIM4->ARR=arr;  //auto reload value 
	TIM4->PSC=psc;  //freq devider

	TIM4->DIER|=1<<0;   //allow update interrupt				
	TIM4->DIER|=1<<6;   //allow trigger interrupt	   
	TIM4->CR1|=0x01;    //enable Timer 4
  	MY_NVIC_Init(2,3,TIM4_IRQChannel,2);//Req1��SubPrioity3��Group2									 
}
void Timer3_Init(u16 arr,u16 psc)	   //time measuring pulse
{
	RCC->APB1ENR|=1<<1;//TIM4 clock enable    
 	TIM3->ARR=arr;  //auto reload value 
	TIM3->PSC=psc;  //freq devider

//	TIM3->DIER|=1<<0;   //allow update interrupt				
//	TIM3->DIER|=1<<6;   //allow trigger interrupt	   

  	MY_NVIC_Init(3,4,TIM3_IRQChannel,2);//Req1��SubPrioity3��Group2									 
}
void Timer2_Init(u16 arr,u16 psc)	 //IR sample pulse
{
	RCC->APB1ENR|=1<<0;//TIM4 clock enable    
 	TIM2->ARR=arr;  //auto reload value 
	TIM2->PSC=psc;  //freq devider

	TIM2->DIER|=1<<0;   //allow update interrupt				
	TIM2->DIER|=1<<6;   //allow trigger interrupt	   
	TIM2->CR1|=0x01;    //enable Timer 4 

  	MY_NVIC_Init(2,4,TIM3_IRQChannel,2);//Req1��SubPrioity3��Group2									 
}
void uart_init(u32 pclk2,u32 bound)
{  	 
	float temp;
	u16 mantissa;
	u16 fraction;	   
	temp=(float)(pclk2*1000000)/(bound*16);//get USARTDIV
	mantissa=temp;				 //get natrual part
	fraction=(temp-mantissa)*16; //get decimal part
    mantissa<<=4;
	mantissa+=fraction; 
	RCC->APB2ENR|=1<<2;   //enable PortA clock  
	RCC->APB2ENR|=1<<14;  //enable USART clock 
	GPIOA->CRH&=0XFFFFF00F; 
	GPIOA->CRH|=0X000008B0;//IO status
		  
	RCC->APB2RSTR|=1<<14;   //reset usart1
	RCC->APB2RSTR&=~(1<<14);//stop reset	   	   

 	USART1->BRR=mantissa; // Baud Rate config	 
	USART1->CR1|=0X200C;  //1 stop bit, no check.


	USART1->CR1|=1<<8;    //PE interrupt enable
	USART1->CR1|=1<<5;    //RXT buffer non-empty interrupt enable	    	
	MY_NVIC_Init(3,3,USART1_IRQChannel,2);//group2��low priority 

}

			 
void  Adc_Init(void)
{    
	//�ȳ�ʼ��IO��
 	RCC->APB2ENR|=1<<2;    //ʹ��PORTA��ʱ�� 
	GPIOA->CRH=0x444448B4;
	GPIOA->CRL=0x444000B0;//PA0  2 3 4anolog���� 1 out
	//ͨ��10/11����			 
	RCC->APB2ENR|=1<<9;    //ADC1ʱ��ʹ��	  
	RCC->APB2RSTR|=1<<9;   //ADC1��λ
	RCC->APB2RSTR&=~(1<<9);//��λ����	    
	RCC->CFGR&=~(3<<14);   //��Ƶ��������	
	//SYSCLK/DIV2=12M ADCʱ������Ϊ12M,ADC���ʱ�Ӳ��ܳ���14M!
	//���򽫵���ADC׼ȷ���½�! 
	RCC->CFGR|=2<<14;      	 

	ADC1->CR1|=0x6820<<0;   
	ADC1->CR2|=0x160000<<0;      //ʹ�����ⲿ����(SWSTART)!!!	����ʹ��һ���¼�������

	ADC1->SQR1|=0x00300000;     //1��ת���ڹ��������� Ҳ����ֻת����������1
	ADC1->SQR3|=0x00020C40;		   
	//����ͨ��0~3�Ĳ���ʱ��
	ADC1->SMPR2&=0XFFFFF000;//ͨ��0,1,2,3����ʱ�����	  
	ADC1->SMPR2|=6<<9;      //ͨ��3  239.5����,��߲���ʱ�������߾�ȷ��	 
	ADC1->SMPR2|=6<<6;      //ͨ��2  239.5����,��߲���ʱ�������߾�ȷ��	 
	ADC1->SMPR2|=6<<12;      //ͨ��1  239.5����,��߲���ʱ�������߾�ȷ��	 
	ADC1->SMPR2|=6<<0;      //ͨ��0  239.5����,��߲���ʱ�������߾�ȷ��	 
									 //cr2  0x00160000
									 //cr1	0x00006820
									 //sqr1	0x00300000
						
	ADC1->CR2|=1<<0;	    //����ADת����	 
	ADC1->CR2|=1<<3;        //ʹ�ܸ�λУ׼  
	while(ADC1->CR2&1<<3);  //�ȴ�У׼���� 			 
    //��λ��������ò���Ӳ���������У׼�Ĵ�������ʼ�����λ��������� 		 
	ADC1->CR2|=1<<2;        //����ADУ׼	   
	while(ADC1->CR2&1<<2);  //�ȴ�У׼����
	//��λ����������Կ�ʼУ׼������У׼����ʱ��Ӳ�����  
	MY_NVIC_Init(4,3,ADC1_2_IRQChannel,2);
}				  
