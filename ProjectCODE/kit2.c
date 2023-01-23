/*2번 키트*/

#include "stm32f767xx.h"
#include "OK-STM767.h"
#include "OK-STM767_large.h"

/*인터럽트 핸들러*/
void EXTI0_IRQHandler(void);			/* EXTI0 interrupt function  버튼1*/
void EXTI1_IRQHandler(void);			/* EXTI1 interrupt function  버튼 2*/
void EXTI2_IRQHandler(void);			/* EXTI2 interrupt function  버튼 3*/
void EXTI3_IRQHandler(void);			/* EXTI3 interrupt function  버튼 4*/
void ADC_IRQHandler(void);			//온도 ADC 인터럽트 핸들러 
void TIM4_IRQHandler(void);			// 온도 1초
/*레지스터 설정*/
void init_TIM2(void);                   		//타이머 2 1CNT가 1us 로 작동되게 설정하는 함수
void init_TIM4(void);		    	       //타이머 3 온도 ADC를 1초마다  
void init_TIM5(void);               	       //타이머 3 1CNT가 1us 로 작동되게 설정하는 함수
void Key_enable(void);				//키 작동
void init_ADC1(void);				//온도센서 GPIO세팅
void SetUltraSonic_func(void);		//초음파 작동 GPIO
void UltraSonic1_func(void);			//초음파 1
void UltraSonic2_func(void);			//초음파 2
void SET_USART3(void);				//USART3통신
void TX3_char(U08 data);			/* transmit a character by USART3 */
void TX3_string(U08* string);			/* transmit a string by USART3 */
unsigned char RX3_char(void);			/* receive a character by USART3 */
/*변수 선언*/
volatile unsigned short temp;			//온도 값을 저장하는 변수  
volatile unsigned short result;		//온도 값을 저장하는 변수  
volatile float distance1;                       // 초음파 센서 거리 값을 저장하는 변수
volatile float distance2;                       // 초음파 센서 거리 값을 저장하는 변수
volatile int flag01;					//zone 1 위급환자 flag01 = 1
volatile int flag02;					//zone 2 위급환자 flag02 = 1
volatile unsigned int second;			//systik 타이머 동작변수
volatile unsigned char run_flag;		//systik 타이머 동작변수
volatile unsigned char RXD, count;	//USART 동작을 위한 변수


void EXTI0_IRQHandler(void)			/* EXTI0 interrupt function */
{
    /*여기에 key 1 기능을 적어주세요
    zone 1에 재난 발생*/
    TFT_string_large(0, 4, Black, Green, " Zone 3 ");
    TX3_string(" Zone 3 emergency ");
    TX3_char(0x0D); TX3_char(0x0A);

    while ((GPIOC->IDR & 0x00000001) != 0x00000001); // debouncing
    Delay_ms(20);
    EXTI->PR = 0x00000001;			// clear pending bit of EXTI0
    NVIC->ICPR[0] = 0x00000040;			// clear pending bit of (6)EXTI0

}

void EXTI1_IRQHandler(void)			/* EXTI1 interrupt function */
{
    /*여기에 key 2 기능을 적어주세요
    zone 2에 재난 발생*/
    TFT_string_large(9, 4, Black, Green, " Zone 4 ");
    TX3_string(" Zone 4 emergency ");
    TX3_char(0x0D); TX3_char(0x0A);

    while ((GPIOC->IDR & 0x00000002) != 0x00000002); // debouncing
    Delay_ms(20);
    EXTI->PR = 0x00000002;			// clear pending bit of EXTI1
    NVIC->ICPR[0] = 0x00000080;			// clear pending bit of (7)EXTI1
}

void EXTI2_IRQHandler(void)			/* EXTI2 interrupt function */
{
    /*여기에 key 3기능을 적어주세요
    */

    while ((GPIOC->IDR & 0x00000004) != 0x00000004); // debouncing
    Delay_ms(20);
    EXTI->PR = 0x00000004;			// clear pending bit of EXTI2
    NVIC->ICPR[0] = 0x00000100;			// clear pending bit of (8)EXTI2
}

void EXTI3_IRQHandler(void)			/* EXTI3 interrupt function */
{
    /*여기에 key 4기능을 적어주세요
        화재감지 동작 확인*/
    TFT_string_large(18, 4, White, Magenta, "화재 발생");
    ADC1->CR2 = 0x00000000;
    TX3_string("  FIRE !  ");
    TX3_char(0x0D); TX3_char(0x0A);
    TX3_string(" location : yonginsi 351-00 ");
    TX3_char(0x0D); TX3_char(0x0A);
    TX3_string(" employee : 300 ");
    TX3_char(0x0D); TX3_char(0x0A);
    TX3_string(" type : ware house ");
    TX3_char(0x0D); TX3_char(0x0A);

    while ((GPIOC->IDR & 0x00000008) != 0x00000008); // debouncing
    Delay_ms(20);
    EXTI->PR = 0x00000008;			// clear pending bit of EXTI3
    NVIC->ICPR[0] = 0x00000200;			// clear pending bit of (9)EXTI3
}

void ADC_IRQHandler(void) {

    result = ADC1->DR;                            // AD변환값 읽기
    temp = result * 330 / 4095;
    TFT_xy(20, 4);
    TFT_color(Cyan, Black);
    TFT_signed_float_large((float)result * 330. / 4095., 2, 2);   // TFT LCD에 온도 표시

    if (temp > 90.0) {
        TFT_string_large(18, 4, White, Magenta, "화재 발생");

        ADC1->CR2 = 0x00000000;

        TX3_string("  FIRE !  ");
        TX3_char(0x0D); TX3_char(0x0A);
        TX3_string(" location : yonginsi 351-00 ");
        TX3_char(0x0D); TX3_char(0x0A);
        TX3_string(" employee : 300 ");
        TX3_char(0x0D); TX3_char(0x0A);
        TX3_string(" type : ware house ");

        for (int i = 0; i < 50; i++) {
            Beep();
            Delay_ms(20);
        }
    }
}

int main(void) {
    Initialize_MCU();
    Delay_ms(50);
    Initialize_LCD();
    Initialize_TFT_LCD();

    Key_enable();
    init_ADC1();
    init_TIM2();
    init_TIM4();
    init_TIM5();
    SetUltraSonic_func();
    SET_USART3();

    Kfont_type_large = 'G';
    Delay_ms(50);
    TFT_string_large(0, 0, White, Magenta, "구조대상자의 위치");
    TFT_string_large(18, 0, White, Magenta, "현재 온도");
    TFT_string_large(0, 24, White, Magenta, " 초음파 거리재기 ");
    TFT_string(0, 28, Cyan, Black, "Sensor 1 : ");
    TFT_string(18, 28, Cyan, Black, "// Sensor 2 : ");


    while (1) {
        UltraSonic1_func();
        UltraSonic2_func();
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
    RXD = USART3->RDR;				// dummy read
}
unsigned char RX3_char(void)			/* receive a character by USART3 */
{
    if (USART3->ISR & 0x00000020)   		// if RXNE=1, return with a received character
        return USART3->RDR;
    else						// if RXNE=0, return with 0x00
        return 0x00;
}

void TX3_char(U08 data)				/* transmit a character by USART3 */
{
    while (!(USART3->ISR & 0x00000080));		// wait until TXE = 1
    USART3->TDR = data;
}

void TX3_string(U08* string)			/* transmit a string by USART3 */
{
    while (*string != '\0')
    {
        TX3_char(*string);
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
void init_TIM1(void) {
    RCC->APB2ENR |= 0x00000001;
    TIM1->PSC = 1079;				//108MHz / (1079+1) = 100KHZ
    TIM1->ARR = 49999;			//100KHZ/(49999+1) = 2Hz
    TIM1->CNT = 0;
    TIM1->DIER = 0x0001;
    TIM1->CR1 = 0x0005;
}

/*타이머2 함수 CNT 주기가 1us*/
void init_TIM2(void) {                   // Output compare mode, PWM

    RCC->APB1ENR |= (1 << 0);               // Bit 0 TIM2EN
    TIM2->PSC = 72 - 1;                     // 1us
    TIM2->EGR = (1 << 0);                   // Bit 0 UG
    TIM2->CR1 = (1 << 0);                   // Bit 0 CEN
}

/*타이머5 함수 CNT 주기가 1us*/
void init_TIM5(void) {                   // Output compare mode, PWM

    RCC->APB1ENR |= (1 << 3);               // Bit 1 TIM5EN
    TIM5->PSC = 72 - 1;                     // 1us
    TIM5->EGR = (1 << 0);                   // Bit 0 UG
    TIM5->CR1 = (1 << 0);                   // Bit 0 CEN
}
/*온도 ADC 구현을 위한 TIM3*/
void init_TIM4(void) {

    RCC->APB1ENR |= 0x00000004;		 // enable TIM3 clock
    TIM4->PSC = 2159;				        // 108MHz/(2159+1) = 50kHz
    TIM4->ARR = 49999;				// 50kHz/(49999+1) = 1Hz
    TIM4->CNT = 0;				        // clear counter
    TIM4->CR1 = 0x0005;				// enable TIM2 and update event
    TIM4->CR2 = 0x0020;				// update event가 TRGO로 사용됨  
}

/*ADC 1 설정*/
void init_ADC1(void) {
    GPIOA->MODER |= 0x000000C0;

    RCC->APB2ENR |= 0x00000100;

    ADC->CCR = 0x00000000;			// ADCCLK = 54MHz/2 = 27MHz

    ADC1->SMPR2 = 0x00000200;			// sampling time of channel 3 = 15 cycle
    ADC1->CR1 = 0x00000020;			// 12-bit resolution, ADC EOC interrupt enable
    ADC1->CR2 = 0x1C000001;			// right alignment, trigger by TIM2_trgo,single conversion, ADON = 1
    ADC1->SQR1 = 0x00000000;			// total regular channel number = 1,  
    ADC1->SQR3 = 0x00000003;			// channel 3 (LM35DZ) : : ADC1에 CH3 연결

    NVIC->ISER[0] |= 0x00040000;			// enable ADC interrupt
}

void SetUltraSonic_func(void) {

    RCC->AHB1ENR |= 0x00000010;           // 포트 E클락 온
    GPIOE->MODER &= 0xFF00FFFF;
    GPIOE->MODER |= 0x00110000;           // 포트8출력모드(초음파의 트리거 신호) 포트9 입력모드(에코신호를 키트의 입력으로 받음) 
                                          // 포트9출력모드(초음파의 트리거 신호) 포트10 입력모드(에코신호 키트의 입력으로 받음)
}

/*초음파 센서결과값 연산 함수 while문에서 작동 TFT에 값 출력*/
void UltraSonic1_func(void) {

    GPIOE->BSRR = 0x01000100;             //GPIOE_ODR 출력레지스터를 세트
    TIM2->CNT = 0;                        //카운터 초기화

    while (TIM2->CNT < 12);                  // 12us 시간 대기 초음파 센서 작동 시간 (12us)기다리기 (Sensor 1)
    GPIOE->BSRR = 0x01000000;           //GPIOE_ODR 출력레지스터를 리셋

    while (!(GPIOE->IDR & 0x0200));        //GPIOE_IDR (ECHO1 - PE8) 입력레지스터 SET을 기다린다 (Sensor 1)
    TIM2->CNT = 0;                      //TIM2 카운터 초기화

    while (GPIOE->IDR & 0x0200);           //GPIOE_IDR 입력레지스터 OFF를 기다린다. (Sensor 1)
    distance1 = (TIM2->CNT + 1) / 58;       // cm 거리 연산


    TFT_xy(10, 28);
    TFT_color(Cyan, Black);
    TFT_signed_float((float)distance1, 4, 1);


    if (distance1 <= 20) {
        TFT_string_large(0, 4, Black, Green, " Zone 3 ");
    }
    else {
        TFT_string_large(0, 4, Black, Black, "        ");
    }


    LED_toggle();                         // 제대로 반복하여 동작하는지 확인하는 LED
    Delay_ms(200);
}

void UltraSonic2_func(void) {

    GPIOE->BSRR = 0x04000400;             //GPIOE_ODR 출력레지스터를 세트
    TIM5->CNT = 0;

    while (TIM5->CNT < 12);                  // 12us 시간 대기 초음파 센서 작동 시간 (12us)기다리기 (Sensor 2)
    GPIOE->BSRR = 0x04000000;           //GPIOE_ODR 출력레지스터를 리셋

    while (!(GPIOE->IDR & 0x0800));        //GPIOE_IDR (ECHO2 - PE10) 입력레지스터 SET을 기다린다 (Sensor 2)
    TIM5->CNT = 0;                      //TIM5 카운터 초기화

    while (GPIOE->IDR & 0x0800);           //GPIOE_IDR 입력레지스터 OFF를 기다린다. (Sensor 2)
    distance2 = (TIM5->CNT + 1) / 58;       // cm 거리 연산


    TFT_xy(31, 28);
    TFT_color(Cyan, Black);
    TFT_signed_float((float)distance2, 4, 1);


    if (distance2 <= 20) {
        TFT_string_large(9, 4, Black, Green, " Zone 4 ");
    }
    else {
        TFT_string_large(9, 4, Black, Black, "        ");
    }

    Delay_ms(200);
}
