// PA8 12 RX, PE8 8 TX : UART7

#include "stm32f767xx.h"
#include "OK-STM767.h"

unsigned char RX7_char(void);			/* receive a character by UART7 */
void TX7_char(U08 data);			/* transmit a character by UART7 */
void TX7_string(U08 *string);			/* transmit a string by UART7 */

int main(void)
{
  unsigned char RXD, count;
  unsigned short color;
  
  Initialize_MCU();				// initialize MCU and kit
  Delay_ms(50);					// wait for system stabilization
  Initialize_LCD();				// initialize text LCD module
  Initialize_TFT_LCD();				// initialize TFT-LCD module

  LCD_string(0x80," OK-STM767 V1.0 ");		// display title
  LCD_string(0xC0," Test Module.c  ");

  TFT_string( 2, 4,White,Magenta," 블루투스 모듈 직렬 통신 (폴링 방식 송수신) ");
  TFT_string(17, 8,White,Blue," 송신 ");
  TFT_string( 9,11,White,Black,"KEY1~KEY4를 누르시오 !");
  TFT_string(17,16,White,Blue," 수신 ");
  Line(0,156,319,156,White);
  Line(0,212,319,212,White);
  Delay_ms(1000);

  GPIOC->BSRR = 0x00000010;                     // LED = VCC of HM-10
  
  GPIOA->MODER &= 0xFFFCFFFF;			// PA8 = UART7_RX
  GPIOA->MODER |= 0x00020000;
  GPIOA->AFR[1] = 0x0000000C;                  // AFIO 12
  
  GPIOE->MODER &= 0xFFFCFFFF;			// PE8 = UART78_TX
  GPIOE->MODER |= 0x00020000;
  GPIOE->AFR[1] &= 0xFFFFFFF0;                  // AFIO 8
  GPIOE->AFR[1] |= 0x00000008;
  
  RCC->APB1ENR |= 0x40000000;			// enable UART7 clock

  UART7->CR1 = 0x0000000D;			// TE=RE=UE = 1, 8 data bit, oversampling by 16
  UART7->CR2 = 0x00000000;			// asynchronous mode, 1 stop bit
  UART7->CR3 = 0x00000000;			// 3 sampling bit
  UART7->BRR = 5625;				// 9600 bps = 54MHz/5625
  Delay_ms(1);
  RXD = UART7->RDR;				// dummy read

  count = 0;					// initialize received data display
  color = Cyan;
  TFT_xy(0,20);
  TFT_color(color,Black);
  
  while(1)
    { switch(Key_input())                       // key input
        { case KEY1 : TX7_string("  KEY1 is OK !  ");
                      TX7_char(0x0D); TX7_char(0x0A);
                      break;
          case KEY2 : TX7_string("  KEY2 is OK !  ");
                      TX7_char(0x0D); TX7_char(0x0A);
                      break;
          case KEY3 : TX7_string("  KEY3 is OK !  ");
                      TX7_char(0x0D); TX7_char(0x0A);
                      break;
          case KEY4 : TX7_string("  KEY4 is OK !  ");
                      TX7_char(0x0D); TX7_char(0x0A);
                      break;
          default:    break;
        }

      RXD = RX7_char();				// receive data if any
      if((RXD >= 0x20) && (RXD <= 0x7F))
        { TFT_English(RXD);			// display a character
          count++;				// 120 character OK ?
          if(count >= 120)			// if yes, go home
            { count = 0;
	      TFT_xy(0,20);
              color ^= 0xFFFF;
              TFT_color(color,Black);
            }
        }
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
