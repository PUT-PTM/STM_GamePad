#include "stm32f4xx_conf.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_exti.h"
#include "misc.h"
#include "stm32f4xx_tim.h"
#include "stm32f4xx_syscfg.h"
#include "stm32f4xx_usart.h"
#include "stm32f4xx_adc.h"
#include "stm32f4xx_dma.h"


int B1,B2,B3,B4;
int A1,A2,A3,A4;
int C1,C2,C3,C4;
int D1,D2;

char *adc1Analog1;
char *adc2Analog1;
char *adc1Analog2;
char *adc2Analog2;

#define ADC_1_ADDRESS_BASE 0x40012000
//ADC_DR = ADC regular Data Register;
#define ADC_DR_ADDRESS_OFFSET 0x4C
volatile uint16_t valueFromADC[16];

void MY_DMA_initP2M(void)
{
   	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);
   	DMA_InitTypeDef strukturaDoInicjalizacjiDMA;
   	// wybór kana³u DMA
   	strukturaDoInicjalizacjiDMA.DMA_Channel = DMA_Channel_0;
   	// ustalenie rodzaju transferu (memory2memory / peripheral2memory / memory2peripheral)
   	strukturaDoInicjalizacjiDMA.DMA_DIR = DMA_DIR_PeripheralToMemory;
   	// tryb pracy - pojedynczy transfer b¹dŸ powtarzany
   	strukturaDoInicjalizacjiDMA.DMA_Mode = DMA_Mode_Circular;
   	// ustalenie priorytetu danego kana³u DMA
   	strukturaDoInicjalizacjiDMA.DMA_Priority = DMA_Priority_Medium;
   	// liczba danych do przes³ania
   	strukturaDoInicjalizacjiDMA.DMA_BufferSize = (uint32_t)4;
   	// adres Ÿród³owy
   	strukturaDoInicjalizacjiDMA.DMA_PeripheralBaseAddr = (uint32_t)(ADC_1_ADDRESS_BASE+ADC_DR_ADDRESS_OFFSET);
   	// adres docelowy
   	strukturaDoInicjalizacjiDMA.DMA_Memory0BaseAddr = (uint32_t)&valueFromADC;
   	// okreslenie, czy adresy maj¹ byæ inkrementowane po ka¿dej przes³anej paczce danych
   	strukturaDoInicjalizacjiDMA.DMA_MemoryInc = DMA_MemoryInc_Enable;
   	strukturaDoInicjalizacjiDMA.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
   	// ustalenie rozmiaru przesy³anych danych
   	strukturaDoInicjalizacjiDMA.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
   	strukturaDoInicjalizacjiDMA.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
   	// ustalenie trybu pracy - jednorazwe przes³anie danych
   	strukturaDoInicjalizacjiDMA.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
   	strukturaDoInicjalizacjiDMA.DMA_MemoryBurst = DMA_MemoryBurst_Single;
   	// wy³¹czenie kolejki FIFO (nie u¿ywana w tym przykadzie)
   	strukturaDoInicjalizacjiDMA.DMA_FIFOMode = DMA_FIFOMode_Disable;
   	// wype³nianie wszystkich pól struktury jest niezbêdne w celu poprawnego dzia³ania, wpisanie jednej z dozwolonych wartosci
   	strukturaDoInicjalizacjiDMA.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
   	// zapisanie wype³nionej struktury do rejestrów wybranego po³¹czenia DMA
   	DMA_Init(DMA2_Stream4, &strukturaDoInicjalizacjiDMA);
   	// uruchomienie odpowiedniego po³¹czenia DMA
   	DMA_Cmd(DMA2_Stream4, ENABLE);
}

void MY_ADC_init(void)
{
   	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB , ENABLE); // wejscie ADC
   	//inicjalizacja wejœcia ADC
   	GPIO_InitTypeDef GPIO_InitStructure;
   	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4;
   	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
   	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
   	GPIO_Init(GPIOA, &GPIO_InitStructure);
   	ADC_CommonInitTypeDef ADC_CommonInitStructure;
   	// niezale¿ny tryb pracy przetworników
   	ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;
   	// zegar g³ówny podzielony przez 2
   	ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div2;
   	// opcja istotna tylko dla tryby multi ADC
   	ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
   	// czas przerwy pomiêdzy kolejnymi konwersjami
   	ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
   	ADC_CommonInit(&ADC_CommonInitStructure);
   	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE); //ADC
   	ADC_InitTypeDef ADC_InitStructure;
   	//ustawienie rozdzielczoœci przetwornika na maksymaln¹ (12 bitów)
   	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
   	//wy³¹czenie trybu skanowania (odczytywaæ bêdziemy jedno wejœcie ADC
   	//w trybie skanowania automatycznie wykonywana jest konwersja na wielu //wejœciach/kana³ach)
   	ADC_InitStructure.ADC_ScanConvMode = ENABLE;
   	//w³¹czenie ci¹g³ego trybu pracy
   	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
   	//wy³¹czenie zewnêtrznego wyzwalania
   	//konwersja mo¿e byæ wyzwalana timerem, stanem wejœcia itd. (szczegó³y w //dokumentacji)
   	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;
   	ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
   	//wartoœæ binarna wyniku bêdzie podawana z wyrównaniem do prawej
   	//funkcja do odczytu stanu przetwornika ADC zwraca wartoœæ 16-bitow¹
   	//dla przyk³adu, wartoœæ 0xFF wyrównana w prawo to 0x00FF, w lewo 0x0FF0
   	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
   	//liczba konwersji równa 1, bo 1 kana³
   	ADC_InitStructure.ADC_NbrOfConversion = 4;
   	// zapisz wype³nion¹ strukturê do rejestrów przetwornika numer 1
   	ADC_Init(ADC1, &ADC_InitStructure);
   	// konfiguracja czasu próbkowania sygna³u
   	ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 1, ADC_SampleTime_84Cycles);
   	ADC_RegularChannelConfig(ADC1, ADC_Channel_2, 2, ADC_SampleTime_84Cycles);
   	ADC_RegularChannelConfig(ADC1, ADC_Channel_3, 3, ADC_SampleTime_84Cycles);
   	ADC_RegularChannelConfig(ADC1, ADC_Channel_4, 4, ADC_SampleTime_84Cycles);
   	// w³¹czenie wyzwalania DMA po ka¿dym zakoñczeniu konwersji
   	ADC_DMARequestAfterLastTransferCmd(ADC1, ENABLE);

   	// w³¹czenie DMA dla ADC
   	ADC_DMACmd(ADC1, ENABLE);
   	// uruchomienie modu³y ADC
   	ADC_Cmd(ADC1, ENABLE);
}



void _GPIO_Init(void)
{

	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_10|GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOC,&GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|
			GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_100MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_11|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_7|GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_100MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);

}

void _GPIO_AF_USART(void)
{
	// ustawienie funkcji alternatywnej dla pinów (USART)
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource10, GPIO_AF_USART3);
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource11, GPIO_AF_USART3);
}

void _USART_Init(void)
{
	USART_InitTypeDef USART_InitStructure;
	// predkosc transmisji (mozliwe standardowe opcje: 9600, 19200, 38400, 57600, 115200, ...)
	USART_InitStructure.USART_BaudRate = 9600;
	// d³ugoœæ s³owa (USART_WordLength_8b lub USART_WordLength_9b)
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	// liczba bitów stopu (USART_StopBits_1, USART_StopBits_0_5, USART_StopBits_2, USART_StopBits_1_5)
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	// sprawdzanie parzystoœci (USART_Parity_No, USART_Parity_Even, USART_Parity_Odd)
	USART_InitStructure.USART_Parity = USART_Parity_No;
	// sprzêtowa kontrola przep³ywu (USART_HardwareFlowControl_None, USART_HardwareFlowControl_RTS, USART_HardwareFlowControl_CTS, USART_HardwareFlowControl_RTS_CTS)
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	// tryb nadawania/odbierania (USART_Mode_Rx, USART_Mode_Rx )
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	// konfiguracja
	USART_Init(USART3, &USART_InitStructure);

}

void _NVIC_Init(void)
{
	//struktura do konfiguracji kontrolera NVIC
	NVIC_InitTypeDef NVIC_InitStructure;
	// wlaczenie przerwania zwi¹zanego z odebraniem danych (pozostale zrodla przerwan zdefiniowane sa w pliku stm32f4xx_usart.h)
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	// konfiguracja kontrolera przerwan
	NVIC_Init(&NVIC_InitStructure);
	// wlaczenie przerwan od ukladu USART
	NVIC_EnableIRQ(USART3_IRQn);

}

void usartSendChar(char c)
{
	//czekaj na opró¿nienie bufora wyjœciowego
	while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);
	// wyslanie danych
	USART_SendData(USART3, c);
	// czekaj az dane zostana wyslane

}

//ciag znakow ;)
void usartSendString(const char* s)
{
	while(*s)
		usartSendChar(*s++);
}

void TIM3_IRQHandler(void)
{
    if(TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
       {

    	adc1Analog1 = malloc(6);
    	snprintf(adc1Analog1, 6, "%d", valueFromADC[0]);

    	adc2Analog1 = malloc(6);
    	snprintf(adc2Analog1, 6 , "%d", valueFromADC[1]);

    	adc1Analog2 = malloc(6);
    	snprintf(adc1Analog2, 6, "%d", valueFromADC[2]);

    	adc2Analog2 = malloc(6);
    	snprintf(adc2Analog2, 6 , "%d", valueFromADC[3]);

 		usartSendString("O\r");


 		A1 = GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_0);
 		A2 = GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_1);
 		A3 = GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_11);
 		A4 = GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_4);
 		B1 = GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_5);
 		B2 = GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_5);
 		B3 = GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_6);
 		B4 = GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_7);
 		C1 = GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_8);
 		C2 = GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_9);
 		C3 = GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_10);
 		C4 = GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_7);
 	 	D1 = GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_8);
 	 	D2 = GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_15);

        if(A1 == 0){
         	usartSendString("A1=0\r");
       		}
        else{
         	usartSendString("A1=1\r");
         	}
   		if(A2 == 0){
			usartSendString("A2=0\r");
			}
   		else{
   			usartSendString("A2=1\r");
   			}
   		if(A3 == 0){
   			usartSendString("A3=0\r");
       		}
   		else{
         	usartSendString("A3=1\r");
   			}
   		if(A4 == 0){
   			usartSendString("A4=0\r");
   			}
   		else{
   			usartSendString("A4=1\r");
   			}
   		if(B1 == 0){
   			usartSendString("B1=0\r");
       		}
   		else{
   			usartSendString("B1=1\r");
           	}
   		if(B2 == 0){
   			usartSendString("B2=0\r");
   			}
   		else{
   			usartSendString("B2=1\r");
   			}
   		if(B3 == 0){
   			usartSendString("B3=0\r");
   			}
   		else{
   			usartSendString("B3=1\r");
           	}
   		if(B4 == 0){
   			usartSendString("B4=0\r");
   			}
   		else{
   			usartSendString("B4=1\r");
   			}
       	if(C1 == 0){
       		usartSendString("C1=0\r");
       		}
       	else{
       		usartSendString("C1=1\r");
       		}
       	if(C2 == 0){
       		usartSendString("C2=0\r");
       		}
       	else{
       		usartSendString("C2=1\r");
       		}
       	if(C3 == 0){
       		usartSendString("C3=0\r");
       		}
       	else{
       		usartSendString("C3=1\r");
       		}
       	if(C4 == 0){
       		usartSendString("C4=0\r");
       		}
       	else{
       		usartSendString("C4=1\r");
       		}
       	if(D1 == 0){
       		usartSendString("D1=0\r");
       		}
       	else{
       		usartSendString("D1=1\r");
       		}
       	if(D2 == 0){
       		usartSendString("D2=0\r");
       		}
       	else{
       		usartSendString("D2=1\r");
       		}
		usartSendString("AN1X=");
		usartSendString(adc1Analog1);
		usartSendString("AN1Y=");
		usartSendString(adc2Analog1);
		usartSendString("AN2X=");
		usartSendString(adc1Analog2);
		usartSendString("AN2Y=");
		usartSendString(adc2Analog2);
		usartSendString("!\n");
		free(adc1Analog1);
		free(adc2Analog1);
		free(adc1Analog2);
		free(adc2Analog2);

       	TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
   	}
}


int main(void)
{
	SystemInit();

		// wlaczenie taktowania wybranego portu
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
		// wlaczenie taktowania wybranego uk³adu USART
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

		TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
		/* Time base configuration */
		TIM_TimeBaseStructure.TIM_Period = 9999;
		TIM_TimeBaseStructure.TIM_Prescaler = 8399;
		TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
		TIM_TimeBaseStructure.TIM_CounterMode =  TIM_CounterMode_Up;
		TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

		TIM_Cmd(TIM3, ENABLE);

		NVIC_InitTypeDef NVIC_InitStructure;
			// numer przerwania
		NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
			// priorytet g³ówny
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
			// subpriorytet
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
			// uruchom dany kana³
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
			// zapisz wype³nion¹ strukturê do rejestrów
		NVIC_Init(&NVIC_InitStructure);

			// wyczyszczenie przerwania od timera 3 (wyst¹pi³o przy konfiguracji timera)
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
				// zezwolenie na przerwania od przepe³nienia dla timera 3
		TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);

		_GPIO_Init();
		_GPIO_AF_USART();
		_USART_Init();
		_NVIC_Init();

		MY_DMA_initP2M();
		MY_ADC_init();
		ADC_SoftwareStartConv(ADC1);

		USART_Cmd(USART3, ENABLE);


	while(1)
	{

	}

}
