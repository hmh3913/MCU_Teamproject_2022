/*3번 키트*/

#include "stm32f767xx.h"
#include "OK-STM767.h"
#include "OK-STM767_large.h"

/*인터럽트 핸들러*/
void EXTI0_IRQHandler(void);			/* EXTI0 interrupt function  버튼1*/
void EXTI1_IRQHandler(void);			/* EXTI1 interrupt function  버튼 2*/
void EXTI2_IRQHandler(void);			/* EXTI2 interrupt function  버튼 3*/
void EXTI3_IRQHandler(void);			/* EXTI3 interrupt function  버튼 4*/
/*레지스터 설정*/
void Key_enable(void);				//키 작동
void SET_USART3(void);				//USART3통신
void TX3_char(U08 data);			/* transmit a character by USART3 */
void TX3_string(U08 *string);			/* transmit a string by USART3 */
void SET_UART7(void);
void TX7_char(U08 data);			/* transmit a character by UART7 */
void TX7_string(U08 *string);			/* transmit a string by UART7 */
unsigned char RX3_char(void);		/* receive a character by USART3 */
unsigned char RX7_char(void);		/* receive a character by UART7 */
/*변수 선언*/
volatile unsigned char RXD1, count1;	//USART 동작을 위한 변수
volatile unsigned char RXD2, count2;	//USART 동작을 위한 변수
volatile unsigned short color;


void EXTI0_IRQHandler(void)			/* EXTI0 interrupt function */
{
    /*여기에 key 1 기능을 적어주세요
	YES 송신*/
     TX7_char(0x0D); TX7_char(0x0A);   
    TX7_string("Transmit :  "); 
    TX7_string("  YES!!  ");
    TX7_char(0x0D); TX7_char(0x0A);
    
    TX3_char(0x0D); TX3_char(0x0A);
    TX3_string("Transmit :  ");   
    TX3_string(" YES!! ");
    TX3_char(0x0D); TX3_char(0x0A);
    
    while ((GPIOC->IDR & 0x00000001) != 0x00000001); // debouncing
    Delay_ms(20);
    EXTI->PR = 0x00000001;			// clear pending bit of EXTI0
    NVIC->ICPR[0] = 0x00000040;			// clear pending bit of (6)EXTI0
}

void EXTI1_IRQHandler(void)			/* EXTI1 interrupt function */
{
    /*여기에 key 2 기능을 적어주세요
	NO 송신*/
     TX7_char(0x0D); TX7_char(0x0A);   
    TX7_string("Transmit :  "); 
     TX7_string(" NO ");
     TX7_char(0x0D); TX7_char(0x0A);
     
    TX3_char(0x0D); TX3_char(0x0A);
    TX3_string("Transmit :  ");
     TX3_string(" NO ");
     TX3_char(0x0D); TX3_char(0x0A);
    while ((GPIOC->IDR & 0x00000002) != 0x00000002); // debouncing
    Delay_ms(20);
    EXTI->PR = 0x00000002;			// clear pending bit of EXTI1
    NVIC->ICPR[0] = 0x00000080;			// clear pending bit of (7)EXTI1
}

void EXTI2_IRQHandler(void)			/* EXTI2 interrupt function */
{
    /*여기에 key 3기능을 적어주세요
	테스트 버튼*/
    TX7_char(0x0D); TX7_char(0x0A);
    TX7_string("Transmit :  ");
    TX7_string("  system test!!  ");
    TX7_char(0x0D); TX7_char(0x0A);
    
    TX3_char(0x0D); TX3_char(0x0A);
    TX3_string("Transmit :  ");
    TX3_string("  system test!! ");
    TX3_char(0x0D); TX3_char(0x0A);

    while ((GPIOC->IDR & 0x00000004) != 0x00000004); // debouncing
    Delay_ms(20);
    EXTI->PR = 0x00000004;			// clear pending bit of EXTI2
    NVIC->ICPR[0] = 0x00000100;			// clear pending bit of (8)EXTI2
}

void EXTI3_IRQHandler(void)			/* EXTI3 interrupt function */
{
    /*여기에 key 4기능을 적어주세요
        화재감지 동작 확인*/
     ADC1->CR2 = 0x00000000;
     TX3_char(0x0D); TX3_char(0x0A);
     TX3_string("Transmit :  ");
     TX3_string("  FIRE !  ");
     TX3_char(0x0D); TX3_char(0x0A);
     TX3_string(" location : yonginsi 351-00 ");
     TX3_char(0x0D); TX3_char(0x0A);
     TX3_string(" employee : 300 ");
     TX3_char(0x0D); TX3_char(0x0A);
     TX3_string(" type : ware house ");
     TX3_char(0x0D); TX3_char(0x0A);
     
    TX7_char(0x0D); TX7_char(0x0A);   
    TX7_string("Transmit :  "); 
     TX7_string("  FIRE !  ");
     TX7_char(0x0D); TX7_char(0x0A);
     TX7_string(" location : yonginsi 351-00 ");
     TX7_char(0x0D); TX7_char(0x0A);
     TX7_string(" employee : 300 ");
     TX7_char(0x0D); TX7_char(0x0A);
     TX7_string(" type : ware house ");
     TX7_char(0x0D); TX7_char(0x0A);
     
    while ((GPIOC->IDR & 0x00000008) != 0x00000008); // debouncing
    Delay_ms(20);
    EXTI->PR = 0x00000008;			// clear pending bit of EXTI3
    NVIC->ICPR[0] = 0x00000200;			// clear pending bit of (9)EXTI3
}


int main(void) {
    Initialize_MCU();
    Delay_ms(50);
    Initialize_LCD();
    Initialize_TFT_LCD();

    Key_enable();
    SET_USART3();
    SET_UART7();
    
    TFT_string(17, 8,White,Blue," 송신 ");
    TFT_string( 0,11,White,Black,"1 : Yes / 2 : No / 3 : Test / 4 : FIRE ");
    TFT_string(17,16,White,Blue," 수신 ");
    TFT_xy(0,20);
    color ^= 0xFFFF;
    TFT_color(color,Black);
    Line(0,156,319,156,White);
    Line(0,212,319,212,White);

    while (1) {
      RXD2 = RX7_char();				// receive data if any
      if((RXD2 >= 0x20) && (RXD2 <= 0x7F))
        { TFT_English(RXD2);			// display a character
          count2++;				// 120 character OK ?
          if(count2 >= 120)			// if yes, go home
            { count2 = 0;
	      TFT_xy(0,20);
              color ^= 0xFFFF;
              TFT_color(color,Black);
            }
	  TX3_char(RXD2);
        }
    }
    
}

void SET_USART3(void)
{
  GPIOB->MODER &= 0xFF0FFFFF;			// PB10 = USART3_TX, PB11 = USART3_RX
  GPIOB->MODER |= 0x00A00000;  
  GPIOB->AFR[1] &= 0xFFFF00FF;
  GPIOB->AFR[1] |= 0x00007700;

  RCC->APB1ENR |= 0x00040000;			// enable USART3 clock

  USART3->CR1 = 0x0000000D;			// TE=RE=UE = 1, 8 data bit, oversampling by 16
  USART3->CR2 = 0x00000000;			// asynchronous mode, 1 stop bit
  USART3->CR3 = 0x00000000;			// 3 sampling bit
  USART3->BRR = 5624;				// 9600 bps 
  Delay_ms(1);
  RXD1 = USART3->RDR;				// dummy read
}

void SET_UART7(void)
{
  GPIOA->MODER &= 0xFFFCFFFF;			// PA8 = UART7_RX
  GPIOA->MODER |= 0x00020000;
  GPIOA->AFR[1] = 0x0000000C;                  // AFIO 12
  
  GPIOE->MODER &= 0xFFFCFFFF;			// PE8 = UART78_TX
  GPIOE->MODER |= 0x00020000;
  GPIOE->AFR[1] &= 0xFFFFFFF0;                  // AFIO 8
  GPIOE->AFR[1] |= 0x00000008;
  
  RCC->APB1ENR |= 0x40000000;			// enable UART7 clock

  UART7->CR1 = 0x0000002D;			// TE=RE=UE = 1, 8 data bit, oversampling by 16
  UART7->CR2 = 0x00000000;			// asynchronous mode, 1 stop bit
  UART7->CR3 = 0x00000000;			// 3 sampling bit
  UART7->BRR = 5625;				// 9600 bps = 54MHz/5625
  Delay_ms(1);
  RXD2 = UART7->RDR;				// dummy read

}
unsigned char RX3_char(void)			/* receive a character by USART3 */
{
  if(USART3->ISR & 0x00000020)   		// if RXNE=1, return with a received character
    return USART3->RDR;
  else						// if RXNE=0, return with 0x00
    return 0x00;
}

void TX3_char(U08 data)				/* transmit a character by USART3 */
{
  while(!(USART3->ISR & 0x00000080));		// wait until TXE = 1
  USART3->TDR = data;
}

void TX3_string(U08 *string)			/* transmit a string by USART3 */
{
  while(*string != '\0')
    { TX3_char(*string);
       string++;
    }
}
unsigned char RX7_char(void)			/* receive a character by UART7 */
{
  if(UART7->ISR & 0x00000020)   		// if RXNE=1, return with a received character
    return UART7->RDR;
  else						// if RXNE=0, return with 0x00
    return 0x00;
}

void TX7_char(U08 data)				/* transmit a character by UART7 */
{
  while(!(UART7->ISR & 0x00000080));		// wait until TXE = 1
  UART7->TDR = data;
}

void TX7_string(U08 *string)			/* transmit a string by UART7 */
{
  while(*string != '\0')
    { TX7_char(*string);
       string++;
    }
}

/*키 인터럽트 */
void Key_enable(void) {
    GPIOC->MODER &= 0xFFFFFF00;			// PC3,2,1,0 = input mode
    RCC->APB2ENR |= 0x00000001;			// enable SYSCFG
    SYSCFG->EXTICR[0] = 0x00002222;		// EXTI3,2,1,0 = PC3,2,1,0(KEY4,3,2,1)
    EXTI->IMR = 0x0000000F;			// enable EXTI3,2,1,0 interrupt
    EXTI->RTSR = 0x00000000;			// disable EXTI3,2,1,0 rising edge
    EXTI->FTSR = 0x0000000F;			// enable EXTI3,2,1,0 falling edge
    NVIC->ISER[0] = 0x000003C0;			// enable (9)EXTI3 ~ (6)EXTI0 interrupt
}

