// �������� ��������� ��� �������� JSON �������
//--------------JSON-------------------------------------------
//#define _CRT_SECURE_NO_WARNINGS - ��� ��������� � jwrite.c // stop complaining about deprecated functions
#include <stdio.h>
#include "jWrite.h"
#include "syscalls.c"
#define MAXANSWERLENGTH 256     //������ ��� json
//--------------------------------CAN----------------------------------------
int Can1_Rx_Count;              //�������� ���������� � CAN RX Interrupt
unsigned char   Can1_Recv_Buf[10]={0};

// ����������� ���� ��������
/*���� �� ��������
 * #include "FreeRTOSConfig.h"
#include "projdefs.h"
#include "portmacro.h"
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "croutine.h"
#include "task.h"
#include "queue.h"
*/

char type;   // ������ ��� ���� ����� ����� ������� ������ �������� ������ json
unsigned int len; // ��� ����������� ����� �������
unsigned char buffer_wifi[MAXANSWERLENGTH]; // ������ - ����� ��� ������


//----------------------------------------------------------------------------
/* Includes ------------------------------------------------------------------*/
#include "stm32f30x.h"
#include "stm32f30x_gpio.h"
#include "stm32f30x_rcc.h"
#include "stm32f30x_can.h"
#include "stm32f30x_misc.h"
//#include "stm32303c_eval.h" --------���������� ��������-------------------
#define LED1    GPIO_Pin_8
#define LED2    GPIO_Pin_9
#define LED3    GPIO_Pin_10
#define LED4    GPIO_Pin_11
//---------------��� ��������  TX � ������ ������ ------------------------------------
uint8_t txStat;
uint8_t transmit_mailbox = 0;
CAN_FilterInitTypeDef  CAN_FilterInitStructure;
CanTxMsg TxMessage = {0};
CanRxMsg RxMessage = {0};
uint8_t KeyNumber = 0x0;
extern __IO uint8_t MsgReceived;
/* Private function prototypes -----------------------------------------------*/
static void CAN_Config(void);
static void Display_TransmittedMsg(uint8_t PushNumber);

//---------------------------------UART---------------------------------------!!!!!!!!!!!!!!!!!!!!!!!!
/*�������� ���� �� CMSIS /�������� �� � �������� �� ������� � �����. ����� 1 � 0 � ���������� ��� ��������� �����
 * ��� ������������� �������� � ���������� �� ������ � ��� ���������*/
//#include "stm32f30x.h" //�������� ���������, ���������� ������� � ���� � ���������������� ����������� ������
void UART_Init (void); // ��������� �������
#include <string.h>
uint8_t  FLAG_REPLY;//���� ��������� ������
uint8_t RXi;//������ �������
char RXc;// ������� �������
//������ ������
#define RX_BUF_SIZE 64
static char RXBuffer[RX_BUF_SIZE];
static char RXBuffer1[RX_BUF_SIZE];

void UART_Init (void){
	RCC->APB1ENR |= RCC_APB1ENR_UART4EN;//������ ������������ �� ���� - 36���
	RCC->AHBENR|=RCC_AHBENR_GPIOCEN;//�������� ������������ ����� GPIOC - 72���
    //����������� ���� Tx
	GPIOC->MODER |= GPIO_MODER_MODER10_1;//10 - ����� �������������� �������
	GPIOC->OTYPER &= ~GPIO_OTYPER_OT_10;//0 - 0: ����������� ����� ��� push-pull ��������� PP(����� ������)
	GPIOC->PUPDR &= ~GPIO_PUPDR_PUPDR10_0;//01 - �������� � ����� ������� ��� pull-up ��������� PU
	GPIOC->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR10;//11 - 50 MHz, ��������������� �����
	GPIOC->AFR[1]|=(0x05<<(4*2));//��������� 10 ������ �������������� �������
	//���������� Rx   PC11
	GPIOC->MODER |= GPIO_MODER_MODER11_1;//10 - ����� �������������� �������
	GPIOC->PUPDR &= ~GPIO_PUPDR_PUPDR11;//00 - ��������� ����� ������, no pull up, pull down
	GPIOC->AFR[1]|=(0x05<<(4*3));//��������� 11 ������ �������������� �������

	UART4->BRR=0xea6;//  (16)(36000000+4800)/9600 = 0xea6 (3750); ��������  0�139
	UART4->CR1|=USART_CR1_UE|USART_CR1_TE|USART_CR1_RE;//���. uart, ������ � ��������
	UART4->CR1|=USART_CR1_RXNEIE;//��������� ������������ ���������� �� ������
	NVIC_EnableIRQ(UART4_IRQn);//D������� ����������, ��������� ������
}
unsigned int i;
void delay () {
     for (i=0;i<1000000;i++);
}
void delay1() {
     for (i=0;i<5000000;i++);
}
//-------------------------------------------------------------------------------------------------------------


/* Private define ------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
typedef enum {FAILED = 0, PASSED = !FAILED} TestStatus;

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
__IO uint32_t ret = 0; /* for return of the interrupt handling */
__IO TestStatus TestRx;


//-------------------------JSON
void jWriteTest();
//----------------------------------------
//--------------------------------------------------------------------------------------------
//�������� ���������  http://forum.easyelectronics.ru/viewtopic.php?f=35&t=12806
void CAN1_Init(void)
{
  GPIO_InitTypeDef      GPIO_InitStructure;
  CAN_InitTypeDef       CAN_InitStructure;
  CAN_FilterInitTypeDef CAN_FilterInitStructure;
  NVIC_InitTypeDef      NVIC_InitStructure;

  /*
   * !!! ��� ������������� ������� ��������� ��������� ����������, �.�. ���������� �������� �� ������������ ������
    * ��������� ���������� �� ����� ��������� CAN
 � ������������ ���� ������� �� ���� �� �������, ��� �� ���������� �������������. ��� ������ ���� ��������� �
  ������������������� ���������� ���������� (NVIC � Nested vectored interrupt controller).
  � ����������� STM32 ������� ���������� ����� ��������� ���� ��������� ��� �������,
  ����� ��������� ��������� ���������� ������������. ������� NVIC ������������ ��� ��������� ��������� ������������
   ������������ �����. � �� ���� ��������� � �����������, � ����� ������ ��� �� ���������, ��� ��� � ��� �� ������ ������
   ��������� ���� ���� ���������� �� ��������� ������� CAN/

 ��� ����, ����� �� ������ �������� ��������� �� CAN ����, �� ������ �������� ���������� �� ��������� ��������� ����,
  � ����� ��������� ���������� �� ����� ��������� ��������� � �������� �����.
*/

   //NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
   NVIC_InitStructure.NVIC_IRQChannel                   = USB_LP_CAN1_RX0_IRQn;//|USB_HP_CAN1_TX_IRQn;
   //����������� FreeRTOS ���� ��������.
   //NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY - 2;
   NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
   NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0x00;
   NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
   NVIC_Init(&NVIC_InitStructure);

   //�� �������� ���� �������
   //NVIC_InitStructure.NVIC_IRQChannel                   = USB_HP_CAN1_TX_IRQn;//|USB_HP_CAN1_TX_IRQn;
   //NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
   ////////NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY - 1;
   //NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0x00;
   //NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
   //NVIC_Init(&NVIC_InitStructure);

   /*
    * ������� �������� ��������, ��� ���������� ��� CAN ���� � USB ���� � ����. ���� �� ������ ������������ ��� ����������
    * � ����� ����������, �� ���������� ����� ��������� ������� � ��������� ���������� �� ���. ����� ���������� �������� ������
    *  �� ������� ������ ������ �� ���������.
 � ��������� "NVIC_IRQChannel" �� ���������, ����� ������ ���������� �� ��������������. ��������� "USB_LP_CAN1_RX0_IRQn"
 ���������� �����, ���������� �� ����������, ��������� � CAN1. ����� �� ����������� � ����� stm32f10x.h, �� ������� ���
 ��������� �������� (ADC1_IRQn, TIM1_TRG_COM_TIM17_IRQn � ��.), ������������ ���������� �� ������ ������������ ���������.
 ���������� ����� �������� �� ��������� ��������� ���������� (������������ �������� ���� ���� ���������� ������������
 ��������� ������������ �������). ����� ���������, ��� ���������� �������.
 ��������� ������, ����������, �������� ������������� ����������.
 �� ���� ��������� CAN ����� ������� �����������. �� ���� ��� �������� �������, ������� ����� ��������� �� ���������
 ����� �������� ������������ CAN ����. � ����������� �� ������� ������������ ��� ����� ��������� ��������, �� �����
 �������� ������ �������� � ����� ����������� �� ������ ��������� �����������������.
    * */

  /* CAN_1 clock enable */
  RCC_AHBPeriphClockCmd (RCC_AHBPeriph_GPIOB, ENABLE); // ����������� ������
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE); // ������

  /* Connect CAN_RX & CAN_TX to AF9 ������������ ������������� �������. ������ ���� ��������. ������� ����� */
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource8, GPIO_AF_9); //CAN_RX = PB8
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource9, GPIO_AF_9); //CAN_TX = PB9
  /* Configure CAN RX and TX pins */
  //�������� - ������������ ����� ���� �����
   /*GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
   GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
   GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
   GPIO_Init(GPIOB, &GPIO_InitStructure);*/

  // ����� ��������
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8; // Rx
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;  // Tx
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  CAN_DeInit(CAN1); //
  /* CAN1 configuration */
  CAN_StructInit(&CAN_InitStructure);
  CAN_InitStructure.CAN_TTCM = ENABLE;
  /*Enable or disable the time triggered communication mode. ��������� ��� ���������� ������ Time Triggered Mode.
   * ���� �������� ������ �� ��������� �������. ���������� 16-������ ������ ������������ ��� �������� ����� ������� � ������� ��������� � ������������� ���������. ���� ������ �������� ���� � ������� ���������� ������ ����������� CAN.
	���� ������� ����� ���� ������� ����������� ��� ������������� ����� ������ � ��������� mailbox, ����� ���������� Time Triggered Mode*/
  CAN_InitStructure.CAN_ABOM = DISABLE;
  /*Enable or disable the automatic bus-off management. ��������� ��� ���������� ��������������� ���������� ����.
	���� ����� �������, �� ��� ���������� ������ ������ ������ �� ����, CAN ������������� ����� ��������.
	� ����� ������ ���������� �������������� ������ ������, ���������� �������� ������ �, ��� ������������� ���������� ������ �������.
   * */
  CAN_InitStructure.CAN_AWUM = DISABLE;
  /*Enable or disable the automatic wake-up mode.
��������� ��� ���������� ��������������� ����������� ���������� �� ������� � CAN-����.
	��� ��������� ����� ��������� ���������� ����� ������������� �����������, �� ������� �������� �������� ��� �� ��������� ���������� ��������� ��������� ����� � ������ �����, ���������� �� ����, ����� ���� ������.
   * */
  CAN_InitStructure.CAN_NART = DISABLE;
  /*Enable or disable the no-automatic retransmission mode.��������� ��� ���������� ������ �������� ��������� ������.
���� ���� ����� �������, �� ��������������� ��� �������� ����� �� ����� ��������� ������������� ��������� ������ ����� ����������� �� ����.
���� �������� - ��, ��� �������� �����, ��������������� ����� ������� ���� �� ������� ��������� ������������� �� ���� ���������� � ���, ��� ����� ������� �, ���� �� ������� ���� �� ����� �����������, �� ����� �������� ��������� �������� �� ��� ���, ���� ��� ���������� �� ���������� ��������� ������.
������� �������: ��� ���������� ��������� ������� � �����, ��� ����������� - ������� � ��������, ���� �� ���� ���������, �� �������� ��� ���, ���� �� ������ ���������.
���� ���������� �� ���������� � ���� ��� ���� ����������� ��������� ��������, �� ��� ���������� ����� ��������� ���������� ����� ���������� �������� ���������� ����� � ����.
   * */
  CAN_InitStructure.CAN_RFLM = DISABLE;
  /* Enable or disable the Receive FIFO Locked mode.��������� ��� ���������� ������ ���������� Receive FIFO
0 - ��� ������������ RX_FIF0 ����� �� ����������� (3 ��������� �� ���������� FIFO), ������ ����� ����� �������� ����������
1 - ��� ������������ RX_FIF0 ����� ����������� (3 ��������� �� ���������� FIFO), ����� ������ ������������� �� ������������ RX_FIFO.
(������� maxx_ir  �� ���������)
   * */
  CAN_InitStructure.CAN_TXFP = DISABLE;
  /* * Enable or disable the transmit FIFO priority.
��������� ��� ���������� ���������� �������� FIFO ��������� ����� ��������� ����������, � ����� ������� ��������� ����� ������������ � ����.
���� �������� �������, �� ��������� ������������ � ��������������� �������: FIFO - First Input First Output - ������ ������ - ������ ����.
���� �� ��������, �� ������ ���������� � ����������� �� ���������� ID ������. �.�. ����� � ����� ������� ����������� ����� ��������� ������.
   * */

  /*
   * ��� ������� ������� � ������������ ���������� ��� ����� ���������� �������� ����� "CAN_NART".
   * ��� ����������� ������������ � ����, ���� ���������� ��������� �������������, �� ������ �������� ����������
   * � ����������� �� ����, ��� ����� �������� ������ �� ����.
���� �� "CAN_NART" ��������, �� ��� ���������� ��������� "CAN_ABOM" ����� ����������� ����� 128 ������� �������� �����,
 � ����� ��������������� �������� ������ Can (���� �� �� ������ ������������ ������ � ���������� �������� ������).
  ��� ����������� "CAN_ABOM" ��������������� ���������� ������ Can �� ����������.
���� �� ���������� �� ����� �������� ������������� � ��������� �������, �� ����������� �������� "��������",
��� ��� ���������� ����� ���������� �������� �������� ������, ���� �� ������� �������������.
   * */

//��������� �������� CAN_Mode ���������� � ����� ������ ���������� ����� �������� � CAN-�����:
  /*
   * CAN_Mode_Normal
	Normal -���������� ����� ������	��� ���� ��������� �� ����� �������� � ������� (����������) ������ ������. ������ ����� ������������ � �������� �� ����.
   *
   * CAN_Mode_LoopBack
	LoopBack - ������� ����	��� ������ ������ LoopBack, ���������� ����� ���������� ������ � ���� � ������� ���� �� ������������.
��� ����������� ����, ��� � USART �� �� �������� ����� TX �� ����� RX.
�� ������ �� ���� �������� �� ����������� �� �����.
   *
   * CAN_Mode_Silent
	Silent - ������� ����	������� ��� ��������� ���������, ������� ����� ������ ������� ����.
	 ��������, ���� ��� ����� ������������ � CAN ���� ����������, �� �� ������ ��������� ����� ���� ���� ��-�� ������������ �������� ������� � ����, �� ���� ����� ����� ���������, ��� ��� � ���� ���������� ������ �� ���������� �������� �� �����.
   *
   * CAN_Mode_Silent_LoopBack
	��� ������������ ������ Silent � LoopBack - ������� ������ ����	� ������ ������ ��� ������ ����� ��������� �������� ������ ����������� ��� ������ � ����� ����.
�� ���� �������������� �� ���� ����� ������ �� ������ �� ����������.
���� ����� ������� ��� ������� ����������. ��� ��������� ��� �� ����� ��� �������� ������ � ����, ��� � ���������� �� ������, ������� �� �� � ���������, ��� ���� �� ���� ����������� ����������� � ����.
   * */

  CAN_InitStructure.CAN_Mode = CAN_Mode_Normal; // CAN_Mode_Silent(����� ������ - ������);// CAN_Mode_LoopBack CAN_Mode_Normal

  /*��������� ���������
������������� � �������� � CAN � ���������, ������ � ������� ������. ������, ��������� ��������� � ������������� ���������� �� ��� ����� ��������� ���������������� � �������������� �������� ������ ����� ����, � ����� ���������� ��������� ���� � ������ ��������.

������� �������� ��� �������� �������� ������ ���� ���������� ��� �������� ��������� � CAN ����.
� ������� �������������
	� ������� ���������������
	� ������� ���� 1
	� ������� ���� 2
	����� ���� ������ ���� CAN:

	     |----------One bit-----------------------|
	____  _______________ ________________________  __________
	    \/      |        |		|	      \/
	____/\Sync__|Prop-seg|Phase 1___|_Phase 2_____/\__________
					|
				Sampling point

				�� ����� ������� �� ������ ������ t_q, � ����������� ������������ ���� ����� 1+BS1+BS2 �������.
				 ������ �������� ���� ���������� �� ������� BS1 � BS2. � �������� ����� ������� ����������,
				 � ����� �� ��������� �������� ��������� ������� ������� (�.�. ������ ����� ������ ����).
				 � ����� ������� ������ ��������� �� ������� SYNC � BS1, ���� �� ��������� ������ �
				 ���������� ��������� BS1, ���� ����� � ����������� BS2 �� �������� SJW (�� 1 �� 4 ������� �������). ����� �������, ���������� ���������� ����������������� � �������� ������ ���������.
� ����� ������� �� ����������� �������� � ������ ����, ��� ��������� ��������� �� ������� 8MHz. � ������� ������������ ���������, �������� ����������� ��� ���� ���� � ���������������.
�������� ���� ������������� � ������� ����������. ��� ��������� ���������� �������� � CAN_Prescaler ������ 50 - �������� �������� �� ���� �������� 10 ����.
��� ��������� �������� ��������, � ����� ������� ���������� �������� CAN_Prescaler, �������� ��� �������� ������ "1", �������� �������� �������� 500 ����, ��� "2" - 250 ����, �� � ��� �����. � ������� ����, ��������������� �� �������� � ������ ������, ��������� ��� �������� CAN_Prescaler, ��������� ��� ������� �������. � �������� ����� ������� �������� ������ �� 1 �����.
�� ��������� ��������� ��� ������������� CAN ���������� �������� ������ ���������:


AN_SJW	������ SJW	SJW (reSynchronization Jump Width) ���������� ������������ ���������� ������� �������, �� ������� ����� ���� ��������� ��� ��������� ���������� ������� ������� ������� ���������. ��������� �������� ����� ���������� �� 1-�� �� 4-� �������.
CAN_BS1	����� �������� ���� 1	BS1 (Bit Segment 1) - ���������� �������������� ����� ������� (Sample Point). �� �������� � ���� Prop_Seg � PHASE_SEG1 ��������� CAN. ��� ����������������� ��������������� �� 1 �� 16 ������� �������.
CAN_BS2	����� �������� ���� 2	BS2 (Bit Segment 2) - ���������� �������������� ����� ��������. �� ������������ ����� PHASE_SEG2 ��������� CAN. ��� ����������������� ��������������� �� 1 �� 8 ������� �������.
CAN_Prescaler	���������	���������, �� �������� �������� �������������� ������ ������ �������. �������������� ������ �� ������� ������ ��������� ����������������.
����� �� ������ � �������� ������ ������ �����������!



*/
  /* CAN Baudrate = 500kbps*/
   // APB1 � ��� �������� �� ������� 36���, - ��  ����������� ���
 //������������� ��������� ��������� �������� ���������
 //BaudRate= 1000  BRP=  4  SJW = 1  BS1= 4 BS2= 4
 //BaudRate= 500   BRP=  9  SJW = 1  BS1= 3 BS2= 4
 //BaudRate= 250   BRP= 16  SJW = 1  BS1= 4 BS2= 4
 //BaudRate= 125   BRP= 32  SJW = 1  BS1= 4 BS2= 4


  //CAN_InitStructure.CAN_SJW  = CAN_SJW_1tq;
//	CAN_InitStructure.CAN_BS1  = CAN_BS1_7tq;
 // CAN_InitStructure.CAN_BS2  = CAN_BS2_4tq;
 // CAN_InitStructure.CAN_Prescaler = 16; //125
  /* CAN Baudrate = 500kBps (CAN clocked at 36 MHz) */
  //CAN_Init(CAN1, &CAN_InitStructure);

//------------------------------------------------------------------
  //1��
  	CAN_InitStructure.CAN_SJW = CAN_SJW_1tq;
    /* CAN Baudrate = 1MBps (CAN clocked at 36 MHz) */
    CAN_InitStructure.CAN_BS1 = CAN_BS1_9tq;
    CAN_InitStructure.CAN_BS2 = CAN_BS2_8tq;
    CAN_InitStructure.CAN_Prescaler = 2;
    CAN_Init(CAN1, &CAN_InitStructure);

    /*36/2 = 18    1/18000000 = 5,55 * 10 (-8) = 55,5 ��
		 1000/55,5 = 18,01



		�� ��� ������ �������
     *
     * 	1. �������� �������� 1 ���� . ������� ������� ��������  1/1000000 = 0,000001 ��� = 1 ���.  = 1000��.
     *	2. ������������� �������� ���� ������������ PCLK1 = 36���/4, �.�. BRP = 4, ����� ����� ������ ������ ��������� ����� 111 ��.
     *	   �.�. 36/4 = 9 ���, 1/9000000 =  1,11 *10 (-7) ��� 111 ��  =  tq = 1Q (����� ������ ������). SYNC_SEG = 1TQ.
     *	3. ������� �������� ����� 1000��/111�� = 9Q.
     *	4. SYNC_SEG  = 1TQ.
     *  5. (BS1+BS2) = 8TQ - ������ ������
     *  6. ������������� �� �������� �� 4TQ.
     *  7.
		YNC_SEG ������ ����� 1TQ, � �����
	(BS1 + BS2) = 7...24TQ. ��� ���� BS1 = 1..16TQ, BS2 = 1...8TQ.
	BRP - ��� �������� ������� �������. � ������� ���� �� �������� ������ ������� ��������.

	��������
������������� � �������� � CAN � ���������, ������ � ������� ������. ������, ��������� ��������� � ������������� ���������� �� ��� ����� ��������� ���������������� � �������������� �������� ������ ����� ����, � ����� ���������� ��������� ���� � ������ ��������.



�� ����� ������� �� ������ ������ t_q, � ����������� ������������ ���� ����� 1+BS1+BS2 �������. ������ �������� ���� ����������
�� ������� BS1 � BS2. � �������� ����� ������� ����������, � ����� �� ��������� �������� ��������� ������� �������
(�.�. ������ ����� ������ ����). � ����� ������� ������ ��������� �� ������� SYNC � BS1, ���� �� ��������� ������ �
 ���������� ��������� BS1, ���� ����� � ����������� BS2 �� �������� SJW (�� 1 �� 4 ������� �������). ����� �������, ����������
  ���������� ����������������� � �������� ������ ���������.
  ���������� ����� �������� ���� �������, ������ ��� ��� ���������� ��������� �� ��� �������� ���������� ������� ������� �����������. ������������� ��������� ��������� �������� ���������:

SJW = 1
BS1 = 3-5
BS2 = 3-5
�������, ���������� �������� ������� �� ������� ���� � ������� ����������, ������ �� ��� ����� �������������. ����� ������ � ��������� �������� BS1 � BS2, � � ������ ������ ����� � �������� �� ��������.  � ���������, ��� ������ ��������� � ������� ������� ��-���� ����������� �����������.

� ������ �������, ��� ����� ���� ����� ���� ����� ������ ������� ���������� ��������, � �� ����� ��������.

��� ������ � ������������� CAN-����� �� �������� 125 ����/� � �������� ���������� 42��� � ����������� ��������� ��������� 12, SJW=1, BS1=3, BS2=3.

	*/
    //-------------------------------------------------------------------

  /*
   * ��������� ���������� ������� CAN
��� �������� ��������� �������� ����� ������. ������ ����� ��������� �� ������������ ��� ������-�� ����������� ��������� ��� � ������ - ��������� �����. ��������� ����� ����������� �������� �� ���������, ��� ��� ��� �� ���������� ������ ��������������� �������� ��������� � ��� �������� �� ���������� ������.
� ������ ������ ���������� � �� ���� �������� ��������� ��������� ������ � ���������, ��� �������� ��� ��������� ������ (��. �������� �����. ������� ������� CAN). ����� ����, ��� ��� ������ ���������� ���������� ������� ��� �����������, � ������������ ����� �������������� �� ���������  ��� ���� ����������.

���������� �������� ��������, ��� ��������� �������� � ��������� �� �����������, ����� �� �� ������ �������� ��������� �� ����.


�������� CAN: ��������=(������� ����)/((CAN_SJW{1...4}+CAN_BS1_{1...16}+CAN_BS2{1...8})* CAN_Prescaler{1...1024})
�� ������������ �������� ������������ ����� ���������� ����= 1/����� ���������� ����(us) �������� �������� � �����/�


*/

  /* CAN1 filter configuration */
  CAN_FilterInitStructure.CAN_FilterNumber         = 0; // filter number = 0 (0<=x<=13)
  CAN_FilterInitStructure.CAN_FilterMode           = CAN_FilterMode_IdMask; // filter mode = identifier mask based filtering
  CAN_FilterInitStructure.CAN_FilterScale          = CAN_FilterScale_16bit;
  CAN_FilterInitStructure.CAN_FilterIdHigh         = 0x0000;// << 5; //0x0000;
  CAN_FilterInitStructure.CAN_FilterIdLow          = 0x0000;
  CAN_FilterInitStructure.CAN_FilterMaskIdHigh     = 0x0000;// << 5;
  CAN_FilterInitStructure.CAN_FilterMaskIdLow      = 0x0000;
  CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_FIFO0; // FIFO = 0
  CAN_FilterInitStructure.CAN_FilterActivation     = ENABLE;
  CAN_FilterInit(&CAN_FilterInitStructure);




  /* Interrupts and flags management functions **********************************/
/*
 * ���� �������� � CAN �� STM32F303. �� ������� � ������ Normal, ������ LoopBack. ���� ��������. �� ��������� � ���������� ���������� ��� ����!
 * �� �� ������������ � ������ ��������
 * void USB_LP_CAN1_RX0_IRQHandler(void)
{
  CanRxMsg RxMessage;
  memset(&RxMessage,0,sizeof(RxMessage));

  if (CAN_GetITStatus(CAN1,CAN_IT_FMP0))   CAN_GetITStatus(CAN1,CAN_IT_FF0)
  {
    CAN_ClearITPendingBit(CAN1,CAN_IT_FMP0); // ��� ���!
    CAN_Receive(CAN1,CAN_FIFO0,&RxMessage);
    CAN_FIFORelease(CAN1,CAN_FIFO0);
  }
}
 *
 * ������ ���������� ���
 *
 * //...
CAN_ITConfig(CAN1, CAN_IT_FF0, ENABLE);
//...

void CAN1_RX0_IRQHandler(void)
{
  unsigned char msginbuff = 0;
  unsigned char i         = 0;

  if (CAN_GetITStatus(CAN1,CAN_IT_FF0))
  {
    CAN_ClearITPendingBit(CAN1,CAN_IT_FF0);

    msginbuff = CAN_MessagePending(CAN1,CAN_FIFO0);

    for (i = 0; i < msginbuff; i++)
    {
      CAN_Receive(CAN1,CAN_FIFO0,&can_msg[i]);
    }
    CAN_FIFORelease(CAN1,CAN_FIFO0);
  }
  return;
}
 *
 *
 *� ����� ���������� �� �����, ����� �������� ������� �����.
���:
CAN1 -> RF0R |= CAN_RF0R_RFOM0; /* Release FIFO 0 Output Mailbox * /
�� ���������� ��� ������� �������� ���:
���:
   LDR R1, [R0, CAN_RF0R]
   ORR R1, CAN_RF0R_RFOM0
   STR R1, [R0, CAN_RF0R]
   RET
������ ����� �� RET ������������. ����� ���������, ����� ���������� ������� ������. ��� �������� 8 �������� ������. ����� �� ������� RET �� ����� �� �������� � ����� ����������. �������� ������� ��� �������� ���:
���:
   LDR R1, [R0, CAN_RF0R]
   ORR R1, CAN_RF0R_RFOM0
   STR R1, [R0, CAN_RF0R]

   ���_������������_RFOM0:
   LDR R1, [R0, CAN_RF0R]
   TST R1, CAN_RF0R_RFOM0
   BNE ���_������������_RFOM0

   RET
������, ��� ���������, �������� ������ - ��� ������ �� ���� �������? �����. ��� ��� �����, ����� ������ ����������. � ���� � ����� ��������� ����. � ����� ������� � ��� � ������� �������. ��� ���� ������������� �� ���-����� F4 :-)

� ��� � ������� �����, ����� �������, ��� ���������� ������� ���� ������������, ��� � ���������� �������� ��� �����. ������� ����� ��������. ������ ����� ������������ ��������� cmsis, ����� �� ������� ���������� ��������� �� ����������.

�� ������.. �� 4 ��� ������ CAN �� ���� ������, FSMC, � �/� �������� �� D137000.
 *
 *
 *
 * *///CAN_IT_FF0
  // ���������� �� �����
	CAN_ITConfig(CAN1, CAN_IT_FF0/*CAN_IT_FMP0*/, ENABLE);  //����� CAN_IT_FMP0/1 �������� readonly. ��� ������� ��������, �� �� ������������ ������. ������� ������� ��������� ������.
	/* Transmit Structure preparation */
	  //TxMessage.StdId = 0x85;
	  //TxMessage.ExtId = 0x00;
	  //TxMessage.RTR = CAN_RTR_DATA;
	  //TxMessage.IDE = CAN_ID_STD;
	  //TxMessage.DLC = 6;
	//CAN_ITConfig(CAN1, CAN_IT_TME,  ENABLE);     // ���������� �� ��������

	/*
	 *  Use CAN interrupts through the function CAN_ITConfig() at initialization
             phase and CAN_GetITStatus() function into interrupt routines to check
             if the event has occurred or not.
             After checking on a flag you should clear it using CAN_ClearFlag()
             function. And after checking on an interrupt event you should clear it
             using CAN_ClearITPendingBit() function.
	 *
	 *
����������� CAN ���������� ����� ������� CAN_ITConfig () ��� �������������
             ���� � CAN_GetITStatus () �������� � ������������� ���������� ��� ��������
             ��������� ������� ��� ���.
             ����� �������� ����� �� ������ �������� ��� � ������� CAN_ClearFlag ()
             �������. � ����� �������� ������� ���������� �� ������ �������� ���
             ��������� ������� CAN_ClearITPendingBit ().
	 * */
	/*
	 * ���������� �� �������� �� �����������, ��� � ����� �� ���?

� ����� ��������� �������:
1. ������� ���������� CAN_ITConfig(CAN1, CAN_IT_TME, ENABLE);
2. ��������� ������ (�����) ��������� :
���:
void CAN_TxMessage(CanTxMsg TxMessage)
{
CAN_TransmitStatus(CAN1, 0);
CAN_Transmit(CAN1, &TxMessage);
CAN_TransmitStatus(CAN1, 0);
return;
}

�������� ������� ��������� (�� ���� ���) (������ �������) �������� ���������� CAN (�� ����� ������� ������� ���).
3. ���������� �������� �������� ���:
���:
void USB_HP_CAN1_TX_IRQHandler(void)
{
  if (CAN_GetITStatus(CAN1,CAN_IT_TME))
  {
  CAN_ClearITPendingBit(CAN1,CAN_IT_TME);
  if (...) //���� ���� ��� ���������� - ���������
   CAN_Transmit(CAN1, TxBuff);
  else
   CAN_ITConfig(CAN1, CAN_IT_TME,  DISABLE);  // ����� �������� ����������.

  }
   return;
}

4. ������ ��� �������� ������� ��������� � TxBuff � ����� CAN_ITConfig(CAN1, CAN_IT_TME, ENABLE);
	 *
	 *
	 *
	 *
	 *
	 * */

	return;
}


int main(void)
{

	  UART_Init();//��������������


  /*!< At this stage the microcontroller clock setting is already configured, 
       this is done through SystemInit() function which is called from startup
       file (startup_stm32f30x.s) before to branch to application main.
       To reconfigure the default setting of SystemInit() function, refer to
       system_stm32f30x.c file
     */     
 //������������� ���� ��� ���������
	GPIO_InitTypeDef gpio;  //���������
	RCC_AHBPeriphClockCmd( RCC_AHBPeriph_GPIOE, ENABLE );   // ������������
	 GPIO_StructInit( &gpio );                              //��������� � ��������������
	 gpio.GPIO_Mode = GPIO_Mode_OUT;
	 gpio.GPIO_Pin  = LED1|LED2|LED3|LED4;
	 GPIO_Init( GPIOE, &gpio );
	 CAN1_Init();

  /* CAN Periph clock enable */
  //RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);


  /* NVIC Configuration */
  //NVIC_Config();
  
  /* Configures LED 1..4 */
  //STM_EVAL_LEDInit(LED1);
  //STM_EVAL_LEDInit(LED2);
  //STM_EVAL_LEDInit(LED3);
  //STM_EVAL_LEDInit(LED4);

  /* CAN transmit at 125Kb/s and receive by polling in loopback mode */
  //TestRx = CAN_Polling();

  /*
   * Ten LEDs:
� LD1 (red) for 3.3 V power-on.
� LD2 (red/green) for USB communication.
� Eight user LEDs, LD3/10 (red), LD4/9 (blue), LD5/8 (orange) and LD6/7 (green)
   *
   *
   *
   * User LD3: red LED is a user LED connected to the I/O PE9 of the STM32F303VCT6.
� User LD4: blue LED is a user LED connected to the I/O PE8 of the STM32F303VCT6.
� User LD5: orange LED is a user LED connected to the I/O PE10 of the
STM32F303VCT6.
� User LD6: green LED is a user LED connected to the I/O PE15 of the
STM32F303VCT6.
� User LD7: green LED is a user LED connected to the I/O PE11 of the
STM32F303VCT6.
� User LD8: orange LED is a user LED connected to the I/O PE14 of the
STM32F303VCT6.
� User LD9: blue LED is a user LED connected to the I/O PE12 of the STM32F303VCT6.
� User LD10: red LED is a user LED connected to the I/O PE13 of the STM32F303VCT6.
   *
   *
   * */


  //if (TestRx !=  FAILED)
  //{ /* OK */

    /* Turn on LED1 */
  //  STM_EVAL_LEDOn(LED1);
	  //GPIO_SetBits( GPIOE, LED1 );   //�����     LD4    Pin 8 selected
  //}
  //else
  //{ /* KO */

    /* Turn on LED3 */
  //  STM_EVAL_LEDOn(LED3);
  // 	  GPIO_SetBits( GPIOE, LED2 );  // Pin 9 selected  ������� - ������
  //}

  /* CAN transmit at 500Kb/s and receive by interrupt in loopback mode */
//	  TestRx = CAN_Interrupt();

  //if (TestRx !=  FAILED)
  //{ /* OK */

    /* Turn on LED4 */
  //  STM_EVAL_LEDOn(LED4);
  //	  GPIO_SetBits( GPIOE, LED3 );   // Pin 10 selected    ��������� LD5
  //}
  //else
  //{ /* KO */

    /* Turn on LED2 */
  //  STM_EVAL_LEDOn(LED2);
	//  GPIO_SetBits( GPIOE, LED4 );
  //}
      // ���������� ������ json
  	  //jWriteTest();
  	  //���������  ������ ������ ��� ��������
  	  //len=sizeof(buffer_wifi);
  	  //itoa(len,buffer_wifi);
  	  //buffer_wifi="{"ANSWER"':'"Device"','"NAME"':'"VE-MS001"'}\r\n";

  	//jwObj_int("E", 0);  // writes "int":1
    //		jwObj_int("Ud", 0);



  /*strcat(buffer_wifi,"{");
  	strcat(buffer_wifi,"\"Ud\":");
  	strcat(buffer_wifi, "0");
	strcat(buffer_wifi,",");
	strcat(buffer_wifi,"\"E\":");
	strcat(buffer_wifi, "0" );
	strcat(buffer_wifi,"}");
	strcat(buffer_wifi,"\r\n");
*/

  	//buffer_wifi += "  \"Ud\": "; buffer_wifi += 0; buffer_wifi += ",\n";
  	  	  //buffer_wifi += "  \"E\": "; buffer_wifi += 0; buffer_wifi += ",\n";
   	  	  //buffer_wifi += "}";


      //����� ��� ��������� ���������������� �����
      UART4_Send_String ("AT\r\n");
      uart_wite_for("Ok");/*�����*/
      GPIOE->ODR|=GPIO_ODR_11;
      UART4_Send_String ("AT+CIPMODE=0\r\n");
      uart_wite_for("Ok");/*�����*/
      GPIOE->ODR&=~(0x100);
      delay1();
      UART4_Send_String ("AT+CIPMUX=1\r\n");
      uart_wite_for("Ok");/*�����*/
      GPIOE->ODR|=0x100;
      delay1();

      UART4_Send_String ("AT+CIPSERVER=1,8080\r\n");//������������� � ��������� ������ �� ������ ESP8266.
       delay1();
     UART4_Send_String ("AT+CIPSTO=5\r\n");/*������� �������, ����� ��� ������. ����� ���� ���������� � ��������� 0...7200 ������.*/
     uart_wite_for("Ok");/*�����*/
     GPIOE->ODR|=0x100;
         delay1();
         //UART4_Send_String ("AT+CIPSEND=0,5\r\n");/* ��� ��������� ���������� ����������� ������ �����
            /* ������������ ����. ��� ������������� ������������� � �����. ����� ������ � ������ �� 2048 ����.
             ����� �������� �������� 20��. ����� ��������� ������ ������� ������ ���������� �>� � ��������� �
             ����� ������ ������ �� RX, ����� ������ ������ ����������� ����� �������� �� � ����������.
             ��� �������� �������� ���������� �SEND OK�. �������� ����� ������ ������ � ������� � ���������
             ����� ����� ������������������� �+++� */
             /*���� ������ �>� � ������ ������. �� ����� �������� �������� �� ���� ������*/

             //UART4_Send_String ("stm32\r\n");
              /*����� �������� �������� �� SEND ��*/
                 //UART4_Send_String ("AT+CIPSEND=0,5\r\n");
             //delay ();//delay_ms (2000);


  /* Infinite loop */
	 //CAN_Send_Test();
	 //CAN1_TxDebugMessage();
  while (1)
  {
	  CAN1_TxDebugMessage();
	  //transmit_mailbox = 0;
	  //TxMessage.Data[0] = 0xe3;
	  //do
	    //     {
	      //       transmit_mailbox = CAN_Transmit( CAN1, &TxMessage );
	        // }
	        // while( transmit_mailbox =! CAN_NO_MB );
	        // txStat = CAN_TransmitStatus(CAN1, 0);

	       // if (txStat == CANTXOK) GPIO_SetBits( GPIOE, LED2 );;


	 // delay1 ();
	 // buffer_wifi1 += RxMessage.Data[0];
	  jWriteTest();
	  UART4_Send_String ("AT+CIPSEND=0,33\r\n");
	            GPIOE->ODR|=0x100;
	            uart_wite_for("OK");
	            delay ();
	            delay ();
	            delay ();


	            GPIOE->ODR&=~(0x100);
	            UART4_Send_String(buffer_wifi);
	            //UART4_Send_String("stm32\r\n");
	            //UART4_Send_String(RXBuffer); //���

	            delay ();
	            delay ();



  }
}


void UART4_Send (char chr){
	while (!(UART4->ISR & USART_ISR_TC)); //���������� ������� ������� � �������� ��������� ����� �� ��������� ����������
	UART4->TDR = chr;
}

void UART4_Send_String (char* str){
	uint8_t i = 0;
	while(str[i])
	UART4_Send (str[i++]);
}


void UART4_IRQHandler  (void){
	unsigned char data;
	if (UART4->ISR & USART_CR1_RXNEIE){//������ �� ���������� ������ ����� 1 - ������ ������, 0- �����    �
	    //UART4->ISR &= ~USART_CR1_RXNEIE; //������� ������ ���
        data = UART4->RDR;
        //if (data == '1'){
        //    GPIOE->ODR|=GPIO_ODR_10;
        //}
        //if (data == '4'){
        //    GPIOE->ODR|=GPIO_ODR_11;
        //}
       // RX_getc(data);
        FLAG_REPLY=1;//����
        RXBuffer[RXi++]=data;
		GPIOE->ODR|=GPIO_ODR_9;
  }
}

// This function called from uart.c file (* - Make better if you wish)
//������� ���������� � ���������� - ��������� �������
void RX_getc(unsigned char RXc) {
	if ((RXc != 255) && (RXc != 0) && (RXc != 10)) {//���� ��� ������� �� ��������� 255 , 0, 10
		if (RXc != 13) {     // � ����� 13     . ������ ���������� ��� ��������� ����� ������� ��� ������� /n and /r - �� ���� �������������� 13 � 10

			RXBuffer[RXi] = RXc; //������ � ������
			RXi++;          //���������

			if (RXi > RX_BUF_SIZE-1) {// ���� ������� ������ �������� �� ������� ���
				clear_RXBuffer();
			}
		}
		else { // ���� enter ��� �� ������������� ���� ��������� ������ ������ � ����� ���������� ������ � ������� uart_wite_for
			FLAG_REPLY = 1;
		}
	}
}

void clear_RXBuffer(void)  //����� ������� ������� uart.
{
	for (RXi=0;RXi<RX_BUF_SIZE;RXi++)
		RXBuffer[RXi] = 0;//��������
	RXi = 0; // ������������� � 0
	FLAG_REPLY = 0;//������������� � 0
}

//������� �������� �������. ��� ����� ���� �� ������������
//void uart_wite_char(char str) {
//	while (RXc != str)
//		RXc = uart_getc();

//}

void uart_wite_for(const char * str) { //������� �������� ������. ����� �� ����� ���������� � �������� OK
	char result = 0;
	while (result == 0) {
		FLAG_REPLY = 0;
		clear_RXBuffer();
		while (FLAG_REPLY == 0) {// ���� ���� ������� ��� ������� � ������� RX_getc. � ���� ��� ��������
			//if (RXi>0) {FLAG_REPLY=1;
		//	}
		}

		result = strspn(RXBuffer, str);//�������� ������ �������, ������� ���� ��������� str � RXBuffer. ���� � ���, ��� (� ���� ������) ��������
		// �������� ����� �� ������������ ������ � ����� ���� ��. ������� �������� ������������ �� ������� ������ ����
		//result = strstr(RXBuffer, str); //��������� ��������� � ������
		//if (result==0)
		//{OSSCAL++;
		//if (OSSCAL>256) OSSCAL=0;}
	}
}



/*"E",            "Ud",        "T-fSec",      "N#",
  "N#R",             "N",             "Id#",      "Id#R",
  "Id",            "Id2",         "Ids",        ,     "Id2s",      ,
  "RDNout",          "L#"    ,            "L"     ,        "F#",
  "F",          "If#",      "If",       "M_Nvs" ,
  "RVTout",     "dL2",             "M_Nvs2",
  "Lf#",               "Lf",        "M"    ,    "Istat",
  "Idsum",    */


// ���������� � ������ JSON, ���� ������� ������� ������
void jWriteTest(){
	int resultCode;

	//if(type==1) //Answer Device
	//{
		jwOpen( buffer_wifi, MAXANSWERLENGTH, JW_OBJECT, JW_PRETTY );    // open root node as object
		//jwObj_string("ANSWER", "Device");        // writes "key":"value"
		//jwObj_string("NAME", device.name);        // writes "key":"value"
  		jwObj_int("E", 0);  // writes "int":1
  		jwObj_int("N#R", 21);

		//jwObj_int("SENSORS", 1);  // writes "int":1

		//jwObj_int("RS485_ADRESS", device.rs485_addr);  // writes "int":1
		//jwObj_array("IP_ADDRESS");            // start "anArray": [...]
		/*jwArr_int(device.ip_addr[0]);          // add a few integers to the array
		jwArr_int(device.ip_addr[1]);
		jwArr_int(device.ip_addr[2]);
		jwArr_int(device.ip_addr[3]);
		jwEnd();
		jwObj_array("NETMASK");              // start "anArray": [...]
		jwArr_int(device.netmask[0]);          // add a few integers to the array
		jwArr_int(device.netmask[1]);
		jwArr_int(device.netmask[2]);
		jwArr_int(device.netmask[3]);
		jwEnd();
		jwObj_array("GATEWAY");              // start "anArray": [...]
		jwArr_int(device.gw[0]);            // add a few integers to the array
		jwArr_int(device.gw[1]);
		jwArr_int(device.gw[2]);
		jwArr_int(device.gw[3]);*/
		jwEnd();                    // end the array
	 	resultCode=jwClose();   // ���� ����������� ������, ��� �� ���������
	//}
}


/**
  * @brief  Configures the CAN, transmit and receive by polling
  * @param  None
  * @retval PASSED if the reception is well done, FAILED in other case
  */
//�������� � ��������� �� ������
/*TestStatus CAN_Polling(void)
{
  CAN_InitTypeDef        CAN_InitStructure;
  CAN_FilterInitTypeDef  CAN_FilterInitStructure;
  CanTxMsg TxMessage;
  CanRxMsg RxMessage;
  uint32_t i = 0;
  uint8_t TransmitMailbox = 0;

  /* CAN register init * /
  CAN_DeInit(CAN1);

  CAN_StructInit(&CAN_InitStructure);

  /* CAN cell init * /
  CAN_InitStructure.CAN_TTCM = DISABLE;
  CAN_InitStructure.CAN_ABOM = DISABLE;
  CAN_InitStructure.CAN_AWUM = DISABLE;
  CAN_InitStructure.CAN_NART = DISABLE;
  CAN_InitStructure.CAN_RFLM = DISABLE;
  CAN_InitStructure.CAN_TXFP = DISABLE;
  CAN_InitStructure.CAN_Mode = CAN_Mode_LoopBack;
  CAN_InitStructure.CAN_SJW = CAN_SJW_1tq;

  /* CAN Baudrate = 125kbps (CAN clocked at 36 MHz) * /
  CAN_InitStructure.CAN_BS1 = CAN_BS1_9tq;
  CAN_InitStructure.CAN_BS2 = CAN_BS2_8tq;
  CAN_InitStructure.CAN_Prescaler = 16;
  CAN_Init(CAN1, &CAN_InitStructure);

  /* CAN filter init * /
  CAN_FilterInitStructure.CAN_FilterNumber = 0;
  CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;
  CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;
  CAN_FilterInitStructure.CAN_FilterIdHigh = 0x0000;
  CAN_FilterInitStructure.CAN_FilterIdLow = 0x0000;
  CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0x0000;
  CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0x0000;  
  CAN_FilterInitStructure.CAN_FilterFIFOAssignment = 0;

  CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
  CAN_FilterInit(&CAN_FilterInitStructure);

  /* transmit * /
  TxMessage.StdId = 0x11;
  TxMessage.RTR = CAN_RTR_DATA;
  TxMessage.IDE = CAN_ID_STD;
  TxMessage.DLC = 2;
  //��������
  TxMessage.Data[0] = 0xCB;
  TxMessage.Data[1] = 0xFE;

  TransmitMailbox = CAN_Transmit(CAN1, &TxMessage);
  i = 0;
  while((CAN_TransmitStatus(CAN1, TransmitMailbox)  !=  CANTXOK) && (i  !=  0xFFFF))
  {
    i++;
  }

  i = 0;
  while((CAN_MessagePending(CAN1, CAN_FIFO0) < 1) && (i  !=  0xFFFF))
  {
    i++;
  }

  /* receive * /
  RxMessage.StdId = 0x00;
  RxMessage.IDE = CAN_ID_STD;
  RxMessage.DLC = 0;
  RxMessage.Data[0] = 0x00;
  RxMessage.Data[1] = 0x00;
  CAN_Receive(CAN1, CAN_FIFO0, &RxMessage);

  if (RxMessage.StdId != 0x11)
  {
    return FAILED;  
  }

  if (RxMessage.IDE != CAN_ID_STD)
  {
    return FAILED;
  }

  if (RxMessage.DLC != 2)
  {
    return FAILED;  
  }

  if ((RxMessage.Data[0]<<8|RxMessage.Data[1]) != 0xCBFE)
  {
    return FAILED;
  }
  
  return PASSED; /* Test Passed * /
}*/

/**
  * @brief  Configures the CAN, transmit and receive using interrupt.
  * @param  None
  * @retval PASSED if the reception is well done, FAILED in other case
  */
/*TestStatus CAN_Interrupt(void)
{
  CAN_InitTypeDef        CAN_InitStructure;
  CAN_FilterInitTypeDef  CAN_FilterInitStructure;
  CanTxMsg TxMessage;
  uint32_t i = 0;

  /* CAN register init * /
  CAN_DeInit(CAN1);

  CAN_StructInit(&CAN_InitStructure);

  /* CAN cell init * /
  CAN_InitStructure.CAN_TTCM = DISABLE;
  CAN_InitStructure.CAN_ABOM = DISABLE;
  CAN_InitStructure.CAN_AWUM = DISABLE;
  CAN_InitStructure.CAN_NART = DISABLE;
  CAN_InitStructure.CAN_RFLM = DISABLE;
  CAN_InitStructure.CAN_TXFP = DISABLE;
  CAN_InitStructure.CAN_Mode = CAN_Mode_LoopBack;
  CAN_InitStructure.CAN_SJW = CAN_SJW_1tq;
  
  /* Baudrate = 500 Kbps (CAN clocked with 36Mhz)* /
  CAN_InitStructure.CAN_BS1 = CAN_BS1_9tq;
  CAN_InitStructure.CAN_BS2 = CAN_BS2_8tq;
  CAN_InitStructure.CAN_Prescaler = 4;
  CAN_Init(CAN1, &CAN_InitStructure);

  /* CAN filter init * /
  CAN_FilterInitStructure.CAN_FilterNumber = 1;
  CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;
  CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;
  CAN_FilterInitStructure.CAN_FilterIdHigh = 0x0000;
  CAN_FilterInitStructure.CAN_FilterIdLow = 0x0000;
  CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0x0000;
  CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0x0000;
  CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_FIFO0;
  CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
  CAN_FilterInit(&CAN_FilterInitStructure);

  /* CAN FIFO0 message pending interrupt enable * /
  CAN_ITConfig(CAN1, CAN_IT_FMP0, ENABLE);

  /* transmit 1 message * /
  TxMessage.StdId = 0;
  TxMessage.ExtId = 0x1234;
  TxMessage.IDE = CAN_ID_EXT;
  TxMessage.RTR = CAN_RTR_DATA;
  TxMessage.DLC = 2;
  TxMessage.Data[0] = 0xDE;
  TxMessage.Data[1] = 0xCA;
  CAN_Transmit(CAN1, &TxMessage);

  /* initialize the value that will be returned * /
  ret = 0xFF;
       
  /* Receive message with interrupt handling * /
  i = 0;
  while((ret ==  0xFF) && (i < 0xFFF))
  {
    i++;
  }
  
  if (i ==  0xFFF)
  {
    ret = 0;  
  }

  /* disable interrupt handling * /
  CAN_ITConfig(CAN1, CAN_IT_FMP0, DISABLE);

  return (TestStatus)ret;
}*/

/**
  * @brief  Configures the NVIC.
  * @param  None
  * @retval None
  */

// void USB_LP_CAN1_RX0_IRQHandler(void)
//{
  //CanRxMsg RxMessage;
  //memset(&RxMessage,0,sizeof(RxMessage));

  //if (CAN_GetITStatus(CAN1,CAN_IT_FF0))   //CAN_GetITStatus(CAN1,CAN_IT_FF0)
  //{
    //CAN_ClearITPendingBit(CAN1,CAN_IT_FF0); // ��� ���!
    //CAN_Receive(CAN1,CAN_FIFO0,&RxMessage);
    //CAN_FIFORelease(CAN1,CAN_FIFO0);
  //}
//}
// ���������� void USB_LP_CAN1_RX0_IRQHandler(void) � stm32f30x_it , �������� ������  [cc] collect2.exe: error: ld returned 1 exit status
void CAN1_TxDebugMessage(void)
{
  CanTxMsg TxMessage;

  /* CAN message to send */
  TxMessage.StdId = 0x85;
  TxMessage.ExtId = 0x00;
  TxMessage.RTR = CAN_RTR_DATA;
  TxMessage.IDE = CAN_ID_STD;
  TxMessage.DLC = 3;
  // �� ������ ��������
  /*TxMessage.Data[0] = 0xe3;
  TxMessage.Data[1] = 0x22;
  TxMessage.Data[2] = 0x0;
  TxMessage.Data[3] = 0x80;
  TxMessage.Data[4] = 0x34;   //0x36 =  Lzad
  TxMessage.Data[5] = 0xf;
  TxMessage.Data[6] = 0x0;
  TxMessage.Data[7] = 0;*/
 //������� �� ������
  TxMessage.Data[0] = 0x02;
    TxMessage.Data[1] = 0x22;
    TxMessage.Data[2] = 0x34;
    TxMessage.Data[3] = 0x00;
    TxMessage.Data[4] = 0x00;   //0x36 =  PU_N_7.all
    TxMessage.Data[5] = 0x00;
    TxMessage.Data[6] = 0x0;
    TxMessage.Data[7] = 0;


  /*TxMessage.Data[0] = 'C';
  TxMessage.Data[1] = 'A';
  TxMessage.Data[2] = 'N';
  TxMessage.Data[3] = '1';
  TxMessage.Data[4] = ' ';
  TxMessage.Data[5] = 'O';
  TxMessage.Data[6] = 'K';
  TxMessage.Data[7] = '!';*/

  //CAN_TransmitStatus(CAN1, 0);
  CAN_Transmit(CAN1, &TxMessage);

  do
  	         {
  	             transmit_mailbox = CAN_Transmit( CAN1, &TxMessage );
  	         }
  	         while( transmit_mailbox =! CAN_NO_MB );
  	         txStat = CAN_TransmitStatus(CAN1, 0);

  	        if (txStat == CANTXOK) GPIO_SetBits( GPIOE, LED2 );;

  //CAN_TransmitStatus(CAN1, 0);

  return;
}
void CAN1_TX_IRQHandler(void)
{
	//static uint8_t i = 0;

	CanTxMsg TxMessage;

	  /* CAN message to send */
	  TxMessage.StdId = 0x00;//0x85;
	  TxMessage.ExtId = 0x00;
	  TxMessage.RTR = CAN_RTR_DATA;
	  TxMessage.IDE = CAN_ID_STD;
	  TxMessage.DLC = 6;
	  TxMessage.Data[0] = 0xe2;
	  TxMessage.Data[1] = 0x22;
	  TxMessage.Data[2] = 0x0;
	  TxMessage.Data[3] = 0x80;
	  TxMessage.Data[4] = 0x36;
	  TxMessage.Data[5] = 0xf;
	  TxMessage.Data[6] = 0x0;
	  TxMessage.Data[7] = 0;

	  /*Data[0]	0xe3
	  Data[1]	0x22
	  Data[2]	0x0
	  Data[3]	0x80
	  Data[4]	0x36
	  Data[5]	0xf
	  Data[6]	0x0
	  Data[7]	0x0
*/
	  if (CAN_GetITStatus(CAN1,CAN_IT_TME))
  {
		CAN_ClearITPendingBit(CAN1,CAN_IT_TME);

		CAN_Transmit(CAN1, &TxMessage);

		//if (++i >= can_buffer.m)
		//{
		//	CAN_ITConfig(CAN1, CAN_IT_TME,  DISABLE);
		//	i = 0;
		//}
  }
	return;
}



void CAN1_RX0_IRQHandler(void)
{
//	CanRxMsg RxMessage;
	memset(&RxMessage,0,sizeof(RxMessage));
	/*memset � ���������� ������� ���������� ���������.
	 * ���������:

#include < string.h >
void *memset (void *destination, int c, size_t n);
	 * ���������:

destination � ��������� �� ����������� ������
� � ��� ������� ��� ����������
n � ������ ����������� ����� ������� � ������
������������ ��������:

������� ���������� ��������� �� ����������� ������.

��������:

������� memset ��������� ������ n ���� ������� ������, �� ������� ��������� �������� destination,
 ��������, ��� �������� ����������� ���������� c.

������:

� ������� ��������� ������ src, ���������� ������ �123456789�, ����� ������ 10 ���� ����� �������
 ����������� �������� �1� � ������ src ��������� �� �������.
	 * #include < stdio.h >  //��� printf
#include < string.h  >   //��� memset

int main (void)
{
   // �������� ������
   unsigned char src[15]=�1234567890�;

   // ��������� ������ �������� �1�
   memset (src, �1�, 10);

   // ����� ������� src �� �������
   printf (�src: %s\n�,src);

   return 0;
}
���������:

����� � �������:
src: 1111111111
	 *
	 *
	 * */

	if (CAN_GetITStatus(CAN1,CAN_IT_FF0))   //CAN_GetITStatus(CAN1,CAN_IT_FF0)
	{
		CAN_ClearITPendingBit(CAN1,CAN_IT_FF0); // ��� ���!
	    CAN_Receive(CAN1,CAN_FIFO0,&RxMessage);

	    for(i=0;i < RxMessage.DLC;i++)
	    {
	         //Can1_Recv_Buf[i] =  RxMessage.Data[i];
	         buffer_wifi[Can1_Rx_Count] = RxMessage.Data[i];
	         if(Can1_Rx_Count < MAXANSWERLENGTH) Can1_Rx_Count++;
	         //Can1_Rx_Count++;
	         //if(Can1_Rx_Count > MAXANSWERLENGTH)
	           //          Can1_Rx_Count =0;
	    }
	    //strcat (buffer_wifi, Can1_Recv_Buf);
	    //if (sizeof(buffer_wifi)> MAXANSWERLENGTH) buffer_wifi = {0};
	    //memset(Can1_Recv_Buf, 0, sizeof(Can1_Recv_Buf));
   	    CAN_FIFORelease(CAN1,CAN_FIFO0);
	    GPIO_SetBits( GPIOE, LED1 );
	}

  /*unsigned char msginbuff = 0;
  unsigned char i         = 0;

  if (CAN_GetITStatus(CAN1,CAN_IT_FF0))
  {
    CAN_ClearITPendingBit(CAN1,CAN_IT_FF0);

    msginbuff = CAN_MessagePending(CAN1,CAN_FIFO0);

    for (i = 0; i < msginbuff; i++)
    {
      CAN_Receive(CAN1,CAN_FIFO0,&can_msg[i]);
    }
    CAN_FIFORelease(CAN1,CAN_FIFO0);
  }
  return;*/
}


#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}

#endif

/**
  * @}
  */

/**
  * @}
  */ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
/*"E",            "Ud",        "T-fSec",      "N#",
  "N#R",             "N",             "Id#",      "Id#R",
  "Id",            "Id2",         "Ids",        ,     "Id2s",      ,
  "RDNout",          "L#"    ,            "L"     ,        "F#",
  "F",          "If#",      "If",       "M_Nvs" ,
  "RVTout",     "dL2",             "M_Nvs2",
  "Lf#",               "Lf",        "M"    ,    "Istat",
  "Idsum",    */









 //����� var_tab                     cled_cnst

 /*"E",       &EDS_dop_kod,       "Ud",      &UdSr,    "T-fSec",  &Timer1_fSec,    "N#",      &Z_Skor,
  "N#R",     &ZISkor,          "N",       &Skor,       "Id#",     &OuRegS_dop_kod,  "Id#R",    &ZIDN,
  "Id",      &Id_dop_kod,       "Id2",     &Id2_dop_kod,     "Ids",     &Id_sr      ,     "Id2s",    &Id2_sr     ,
  "RDNout",  &RDN_Out,            "L#"    ,  &S.Alfa,             "L"     ,  &S.Alfa_Old,       "F#",      &V.Fv_zad,
   "F",       &V.Fv    ,     "If#",     &V.Iv_zad,   "If",      &V.Iv,  "M_Nvs" ,  &S.Most_Tir,
  "RVTout",  &S2.Out_rvt,    "dL2",     &S2.Alfa,         "M_Nvs2",  &S2.Most_Tir,
  "Lf#",     &V.Alfa,            "Lf",      &V.Alfa_Old,   "M"    ,   &Moment,  "Istat",   &Id_Stat_flt.out,
 "Idsum",   &Id_sum,
*/

/*���� ����������� ������� CAN + ���� � ���
 *   { "r0_00",   (w*)&crezerv,        2, _SHORT_type,  0,         _READ_access       },
  { "E",       &EDS_dop_kod,        2, _SHORT_type,  _Ud_Nom,   _READ_access       },
  { "Ud",      &UdSr,               2, _SHORT_type,  _Ud_Nom,   _READ_access       },
  { "T-fSec",  &Timer1_fSec,        2, _SHORT_type,  0,         _READ_access       },
  { "r0_02",   (w*)&crezerv,        2, _SHORT_type,  0,         _READ_access       },
  { "N#",      &Z_Skor,             2, _SHORT_type,  _Skor_Nom, _READ_access       },
  { "N#R",     &ZISkor,             2, _SHORT_type,  _Skor_Nom, _READ_access       },
  { "N",       &Skor,               2, _SHORT_type,  _Skor_Nom, _READ_access       },
  { "r0_03",   (w*)&crezerv,        2, _SHORT_type,  0,         _READ_access       },
  { "r0_04",   (w*)&crezerv,        2, _SHORT_type,  0,         _READ_access       },
  { "Id#",     &OuRegS_dop_kod,     2, _SHORT_type,  _Id_Nom,   _READ_access       },
  { "Id#R",    &ZIDN,               2, _SHORT_type,  _Id_Nom,   _READ_access       },
  { "Id",      &Id_dop_kod,         2, _SHORT_type,  _Id_Nom,   _READ_access       },
#ifdef _SIFU2_
  { "Id2",     &Id2_dop_kod,        2, _SHORT_type,  _Id_Nom,   _READ_access       },
#else
  { "Id2",     (w*)&crezerv,        2, _SHORT_type,  _Id_Nom,   _READ_access       },
#endif
#ifdef  Id_sr_ach
  { "Ids",     &Id_sr      ,        2, _SHORT_type,  _Id_Nom,   _READ_access       },
#else
  { "Ids",     (w*)&crezerv,        2, _SHORT_type,  _Id_Nom,   _READ_access       },
#endif
#ifdef  Id2_sr_ach
  { "Id2s",    &Id2_sr     ,        2, _SHORT_type,  _Id_Nom,   _READ_access       },
#else
  { "Id2s",    (w*)&crezerv,        2, _SHORT_type,  _Id_Nom,   _READ_access       },
#endif
#ifdef _RDN
  { "RDNout",  &RDN_Out,            2, _SHORT_type,  _Skor_Nom, _READ_access       },
#else
  { "RDNout",  (w*)&crezerv,        2, _SHORT_type,  0,         _READ_access       },
#endif
  { "L#"    ,  &S.Alfa,             2, _SHORT_type,  _Grad(1),  _READ_access       },
  { "L"     ,  &S.Alfa_Old,         2, _SHORT_type,  _Grad(1),  _READ_access       },
#ifndef  _WM591
  { "F#",      &V.Fv_zad,           2, _SHORT_type,  _Fv_Nom,   _READ_access       },
  { "F",       &V.Fv    ,           2, _SHORT_type,  _Fv_Nom,   _READ_access       },
  { "If#",     &V.Iv_zad,           2, _SHORT_type,  _Iv_Nom,   _READ_access       },
  { "If",      &V.Iv,               2, _SHORT_type,  _Iv_Nom,   _READ_access       },
#else
  { "F#",      (w*)&crezerv,        2, _SHORT_type,  _Fv_Nom,   _READ_access       },
  { "F",       (w*)&crezerv,        2, _SHORT_type,  _Fv_Nom,   _READ_access       },
  { "If#",     (w*)&crezerv,        2, _SHORT_type,  _Iv_Nom,   _READ_access       },
  { "If",      (w*)&crezerv,        2, _SHORT_type,  _Iv_Nom,   _READ_access       },
#endif
  { "M_Nvs" ,  &S.Most_Tir,         1,_CHAR_type,    0,         _READ_access       },
#ifdef _SIFU2_
  { "RVTout",  &S2.Out_rvt,         2, _SHORT_type,  _Grad(1),  _READ_access       },
  { "dL2",     &S2.Alfa,            2, _SHORT_type,  _Grad(1),  _READ_access       },
  { "M_Nvs2",  &S2.Most_Tir,        1,_CHAR_type,    0,         _READ_access       },
#else
  { "RVTout",  (w*)&crezerv,        2, _SHORT_type,  0,         _READ_access       },
  { "dL2",     (w*)&crezerv,        2, _SHORT_type,  0,         _READ_access       },
  { "M_Nvs2",  (w*)&crezerv,        2, _SHORT_type,  0,         _READ_access       },
#endif
#ifdef _Vozb_KTE
  { "Lf#",     &V.Alfa,             2, _SHORT_type,  _Grad(1),  _READ_access       },
  { "Lf",      &V.Alfa_Old,         2, _SHORT_type,  _Grad(1),  _READ_access       },
#else
  { "Lf#",     (w*)&crezerv,        2, _SHORT_type,  0,         _READ_access       },
  { "Lf",      (w*)&crezerv,        2, _SHORT_type,  0,         _READ_access       },
#endif
#ifndef _WM591
  { "M"    ,   &Moment,             2, _SHORT_type,  _Mom_Nom,  _READ_access       },
#else
  { "M"    ,   (w*)&crezerv,        2, _SHORT_type,  _Mom_Nom,  _READ_access       },
#endif
#ifdef _RRS
  { "Istat",   &Id_Stat_flt.out,    2, _SHORT_type,  0,         _READ_access       },
#else
  { "Istat",   (w*)&crezerv,        2, _SHORT_type,  0,         _READ_access       },
#endif
#ifdef _SIFU2_
  { "Idsum",   &Id_sum,             2, _SHORT_type,  _Id_Nom,   _READ_access       },
#else
  { "Idsum",   (w*)&crezerv,        2, _SHORT_type,  0,         _READ_access       },
#endif
// ������ 2  - ���������� �������� - 57 - ����������� ������
  { "UnitSt",  &Status_KTE,         1, _CHAR_type,   0,         _READ_access       },
  { "cStsR",   &canr.StatReg.all,   2, _SHORT_type,  0,         _READ_access       },
  { "cCfgR",   &canr.CfgReg.all,    2, _SHORT_type,  0,         _READ_WRITE_access },
  { "r1_00",   (w*)&crezerv,        2, _SHORT_type,  0,         _READ_access       },
  { "Id#_t",   &canr.Temp_Zad,      1, _CHAR_type ,  0,         _READ_WRITE_access },
  { "cN#",     &canr.N_zad,         2, _SHORT_type,  _Skor_Nom, _READ_WRITE_access },
  { "cdId#",   &canr.dId_zad,       2, _SHORT_type,  _Id_Nom,   _READ_WRITE_access },
  { "cId#",    &canr.Id_zad,        2, _SHORT_type,  _Id_Nom,   _READ_WRITE_access },
  { "cId#P",   &canr.Idz_MaxP,      2, _SHORT_type,  _Id_Nom,   _READ_WRITE_access },
  { "cId#M",   &canr.Idz_MaxM,      2, _SHORT_type,  _Id_Nom,   _READ_WRITE_access },
  { "r1_02",   (w*)&crezerv,        2, _SHORT_type,  0,         _READ_access       },
  { "r1_03",   (w*)&crezerv,        2, _SHORT_type,  0,         _READ_access       },
  { "r1_04",   (w*)&crezerv,        2, _SHORT_type,  0,         _READ_access       },
  { "cL#",     &canr.L_zad,         2, _SHORT_type,  _Grad(1),  _READ_WRITE_access },
  { "cL#min",  &canr.L_min,         2, _SHORT_type,  _Grad(1),  _READ_WRITE_access },
  { "cS#",     &canr.S_zad,         2, _SHORT_type,  _Grad(1),  _READ_WRITE_access },
  { "cF#",     &canr.F_zad,         2, _SHORT_type,  _Fv_Nom,   _READ_WRITE_access },
  { "r1_06",   (w*)&crezerv,        2, _SHORT_type,  0,         _READ_access       },
  { "r1_07",   (w*)&crezerv,        2, _SHORT_type,  0,         _READ_access       },
  { "r1_08",   (w*)&crezerv,        2, _SHORT_type,  0,         _READ_access       },
  { "r1_09",   (w*)&crezerv,        2, _SHORT_type,  0,         _READ_access       },
  { "r1_10",   (w*)&crezerv,        2, _SHORT_type,  0,         _READ_access       },
  { "r1_11",   (w*)&crezerv,        2, _SHORT_type,  0,         _READ_access       },
  { "r1_12",   (w*)&crezerv,        2, _SHORT_type,  0,         _READ_access       },
  { "r1_13",   (w*)&crezerv,        2, _SHORT_type,  0,         _READ_access       },
  { "r1_14",   (w*)&crezerv,        2, _SHORT_type,  0,         _READ_access       },
  { "r1_15",   (w*)&crezerv,        2, _SHORT_type,  0,         _READ_access       },
  { "r1_16",   (w*)&crezerv,        2, _SHORT_type,  0,         _READ_access       },
  { "r1_17",   (w*)&crezerv,        2, _SHORT_type,  0,         _READ_access       },
  { "r1_18",   (w*)&crezerv,        2, _SHORT_type,  0,         _READ_access       },
  { "UartRx",  &UartCAN_Rx.all,     2, _SHORT_type,  0,         _READ_WRITE_access },
  { "UartTx",  &UartCAN_Tx.all,     2, _SHORT_type,  0,         _READ_access       },
// ������ 3  - ���������� ����-����� - 90 - ����������� ������
  { "Ai0",     &canr.Ai[0],         2, _SHORT_type,  0,         _READ_access       },
  { "Ai1",     &canr.Ai[1],         2, _SHORT_type,  0,         _READ_access       },
  { "Ai2",     &canr.Ai[2],         2, _SHORT_type,  0,         _READ_access       },
  { "Ai3",     &canr.Ai[3],         2, _SHORT_type,  0,         _READ_access       },
  { "Ai4",     &canr.Ai[4],         2, _SHORT_type,  0,         _READ_access       },
  { "r2_00",   (w*)&crezerv,        2, _SHORT_type,  0,         _READ_access       },
  { "r2_01",   (w*)&crezerv,        2, _SHORT_type,  0,         _READ_access       },
  { "Ao0",     &canr.Ao[0],         2, _SHORT_type,  0,         _READ_WRITE_access },
  { "Ao1",     &canr.Ao[1],         2, _SHORT_type,  0,         _READ_WRITE_access },
  { "r2_02",   (w*)&crezerv,        2, _SHORT_type,  0,         _READ_access       },
  { "r2_03",   (w*)&crezerv,        2, _SHORT_type,  0,         _READ_access       },
#ifndef _WM591
  { "Sels0g",  &Sels[0].out_g,      2, _SHORT_type, _Grad(1),   _READ_access       },
  { "Sels0m",  &Sels[0].out_m,      2, _SHORT_type, _Skor_Nom,  _READ_access       },
  { "Sels1g",  &Sels[1].out_g,      2, _SHORT_type, _Grad(1),   _READ_access       },
  { "Sels1m",  &Sels[1].out_m,      2, _SHORT_type, _Skor_Nom,  _READ_access       },
  { "PDF0m" ,  &PDF[0].out_m,       2, _SHORT_type, _Skor_Nom,  _READ_access       },
  { "PDF0g" ,  &PDF[0].out_g ,      2, _SHORT_type, _Grad(1),   _READ_access       },
  { "PDF0o" ,  &PDF[0].out_ob,      2, _SHORT_type,  1,         _READ_access       },
#else
  { "Sels0g",  (w*)&crezerv,        2, _SHORT_type, _Grad(1),   _READ_access       },
  { "Sels0m",  (w*)&crezerv,        2, _SHORT_type, _Skor_Nom,  _READ_access       },
  { "Sels1g",  (w*)&crezerv,        2, _SHORT_type, _Grad(1),   _READ_access       },
  { "Sels1m",  (w*)&crezerv,        2, _SHORT_type, _Skor_Nom,  _READ_access       },
  { "PDF0m" ,  (w*)&crezerv,        2, _SHORT_type, _Skor_Nom,  _READ_access       },
  { "PDF0g" ,  (w*)&crezerv,        2, _SHORT_type, _Grad(1),   _READ_access       },
  { "PDF0o" ,  (w*)&crezerv,        2, _SHORT_type,  1,         _READ_access       },
#endif
  { "r2_04",   (w*)&crezerv,        2, _SHORT_type,  0,         _READ_access       },
  { "r2_05",   (w*)&crezerv,        2, _SHORT_type,  0,         _READ_access       },
  { "r2_06",   (w*)&crezerv,        2, _SHORT_type,  0,         _READ_access       },
  { "r2_07",   (w*)&crezerv,        2, _SHORT_type,  0,         _READ_access       },
  { "r2_08",   (w*)&crezerv,        2, _SHORT_type,  0,         _READ_access       },
  { "r2_09",   (w*)&crezerv,        2, _SHORT_type,  0,         _READ_access       },
  { "r2_10",   (w*)&crezerv,        2, _SHORT_type,  0,         _READ_access       },
  { "r2_11",   (w*)&crezerv,        2, _SHORT_type,  0,         _READ_access       },
  { "r2_12",   (w*)&crezerv,        2, _SHORT_type,  0,         _READ_access       },
  { "r2_13",   (w*)&crezerv,        2, _SHORT_type,  0,         _READ_access       },
  { "r2_14",   (w*)&crezerv,        2, _SHORT_type,  0,         _READ_access       },
  { "r2_15",   (w*)&crezerv,        2, _SHORT_type,  0,         _READ_access       },
  { "r2_16",   (w*)&crezerv,        2, _SHORT_type,  0,         _READ_access       },
  { "r2_17",   (w*)&crezerv,        2, _SHORT_type,  0,         _READ_access       },
// ������ 4  - ���������� ����-����� - 123 - ����������� ������
  { "Pi0",     &Pi0_f.all,          1, _CHAR_type,   0,         _READ_access       },
  { "Pi1",     &Pi1_f.all,          1, _CHAR_type,   0,         _READ_access       },
#ifdef _Pi2_
  { "Pi2",     &Pi2_f.all,          1, _CHAR_type,   0,         _READ_access       },
#else
  { "Pi2",     (w*)&crezerv,        2, _SHORT_type,  0,         _READ_access       },
#endif
  { "Pi0f",    &Pi0_cf.all,         1, _CHAR_type,   0,         _READ_access       },
  { "Pi1f",    &Pi1_cf.all,         1, _CHAR_type,   0,         _READ_access       },
#ifdef _Pi2_
  { "Pi2f",    &Pi2_cf.all,         1, _CHAR_type,   0,         _READ_access       },
#else
  { "Pi2f",    (w*)&crezerv,        2, _SHORT_type,  0,         _READ_access       },
#endif
  { "AsPi0",   &PiS_f[0].all,       1, _CHAR_type,   0,         _READ_access       },
  { "AsPi1",   &PiS_f[1].all,       1, _CHAR_type,   0,         _READ_access       },
  { "AsPi2",   &PiS_f[2].all,       1, _CHAR_type,   0,         _READ_access       },
  { "AsPi3",   &PiS_f[3].all,       1, _CHAR_type,   0,         _READ_access       },
  { "r3_02",   (w*)&crezerv,        2, _SHORT_type,  0,         _READ_access       },
  { "r3_03",   (w*)&crezerv,        2, _SHORT_type,  0,         _READ_access       },
  { "AsPi0f",  &PiS_cf[0].all,      1, _CHAR_type,   0,         _READ_access       },
  { "AsPi1f",  &PiS_cf[1].all,      1, _CHAR_type,   0,         _READ_access       },
  { "AsPi2f",  &PiS_cf[2].all,      1, _CHAR_type,   0,         _READ_access       },
  { "AsPi3f",  &PiS_cf[3].all,      1, _CHAR_type,   0,         _READ_access       },
  { "r3_04",   (w*)&crezerv,        2, _SHORT_type,  0,         _READ_access       },
  { "r3_05",   (w*)&crezerv,        2, _SHORT_type,  0,         _READ_access       },
  { "Po0",     &Po0_c.all,          1, _CHAR_type,   0,         _READ_access       }, // Po0_c.all ���������� � ����� ����� ��������� . Po0_nm.all - �� ���������� .
  { "r3_06",   (w*)&crezerv,        2, _SHORT_type,  0,         _READ_access       },
  { "AsPo0",   &PoS_c[0].all,       1, _CHAR_type,   0,         _READ_WRITE_access },
  { "AsPo1",   &PoS_c[1].all,       1, _CHAR_type,   0,         _READ_WRITE_access },
#endif //_CANOPEN
  _Var_tab_InsertSled ,
  _Var_tab_InsertSled2
} ;
 *
 *
 *,,,,,,,,�!!!!!!!!!!!!!!!!!!!!!!!����
 *
 * {"Time-sec ", (w*)&Puls_counter, _sl_form(2,1,2,2,3), _pSec(1.0),(w*)&_ind1_sld,"s " },
#ifndef _CM3_
    {"Time-msec", (w*)&timer1    ,_sl_form(2,1,2,2,1), _MkSec(1000),(w*)&_ind1_sld,"ms " },
#else
    {"Time-msec", (w*)&LPC_TIM0->TC,_sl_form(2,1,2,2,1), _MkSec(1000),(w*)&_ind1_sld,"ms " },
#endif
    {"Tckl     ", (w*)&Tckl      ,_sl_form(2,1,2,2,1), _MkSec(1000),(w*)&_ind1_sld,"ms " },
    {"Time-ovr ", (w*)&Timer1_Ovr,_sl_form(2,1,2,2,3), _Sec(1.0)   ,(w*)&_ind1_sld,"s "  },
    {"E        ", &EDS_dop_kod   ,_sl_form(2,0,2,3,1), _EDS_Nom    ,(w*)&_ind100_sld,"% "},
#ifdef   EDS_ach
    {"Eap      ", &iEDS ,_sl_form(2,0,2,3,1),_EDS_Nom>>_Skor_Shift ,(w*)&_ind100_sld,"% "},
    {"Epr      ", &prEDS,_sl_form(2,0,2,3,1),_EDS_Nom>>_Skor_Shift ,(w*)&_ind100_sld,"% "},
#endif
    {"Ud       ", &UdSr        ,_sl_form(2,0,2,3,1), _Ud_Nom ,(w*)&_ind100_sld,"% " },
#ifdef _REG_POL
    {"S#       ", &ZUgol       ,_sl_form(2,0,2,3,1),_Grad(1.0),(w*)&_ind1_sld ,"g " },
    {"RPINsqr  ", &RP_in_sqr   ,_sl_form(2,0,2,3,1),_Grad(1.0),(w*)&_ind1_sld ,"g " },
    {"S        ", &Ugol        ,_sl_form(2,0,2,3,1),_Grad(1.0),(w*)&_ind1_sld ,"g " },
    {"IntRP    ", &Integr_RP   ,_sl_form(2,0,2,3,1),_Skor_Nom,(w*)&_ind100_sld,"% " },
    {"RPOut    ", &OuRP        ,_sl_form(2,0,2,3,1),_Skor_Nom,(w*)&_ind100_sld,"% " },
#endif
    {"N#       ", &Z_Skor      ,_sl_form(2,0,2,3,1),_Skor_Nom,(w*)&_ind100_sld,"% " },
    {"N#R      ", &ZISkor      ,_sl_form(2,0,2,3,1),_Skor_Nom,(w*)&_ind100_sld,"% " },
    {"N#_Anal  ", &ZSkA        ,_sl_form(2,0,2,3,1),_Skor_Nom,(w*)&_ind100_sld,"% " },
    {"N#_IZU_BM", &ZSkBM       ,_sl_form(2,0,2,3,1),_Skor_Nom,(w*)&_ind100_sld,"% " },
    {"N#_Tolch ", &ZSkT        ,_sl_form(2,0,2,3,1),_Skor_Nom,(w*)&_ind100_sld,"% " },
    {"N#_Shtat ", &ZSk_Shtat   ,_sl_form(2,0,2,3,1),_Skor_Nom,(w*)&_ind100_sld,"% " },
    {"N        ", &Skor        ,_sl_form(2,0,2,3,1),_Skor_Nom,(w*)&_ind100_sld,"% " },
    {"Nnf      ", &Skor_ind    ,_sl_form(2,0,2,3,1),_Skor_Nom,(w*)&_ind100_sld,"% " },
    {"Nf       ", &Skor_f      ,_sl_form(2,0,2,3,1),_Skor_Nom,(w*)&_ind100_sld,"% " },
    {"Nx       ", &Skor_r      ,_sl_form(2,0,2,3,1),_Skor_Nom,(w*)&_ind100_sld,"% " },
    {"TG       ", &TG          ,_sl_form(2,0,2,3,1),_Skor_Nom,(w*)&_ind100_sld,"% " },
#ifdef   _TGX8
    {"TGx1     ", &TGx1        ,_sl_form(2,0,2,3,2),_Skor_Nom,(w*)&_ind100_sld,"% " },
    {"TGx8     ", &TGx8        ,_sl_form(2,0,2,3,2),_Skor_Nom,(w*)&_ind100_sld,"% " },
#endif
    {"IntRS    ", &OuIchRS     ,_sl_form(2,0,2,3,1),_Id_Nom  ,(w*)&_ind100_sld,"% " },
    {"Prz_flg  ", &Prz.all     ,_sl_form(1,1,1,0,0), 1         ,(w*)&_ind1_sld,"h " },
#ifdef bi_RS_P
    {"KpRS     ", &_r_KRSP ,_sl_form(2,0,2,3,1), 256        ,(w*)&_ind1_sld  ,"��" },
#endif
    {"Kp-RS    ", &KRSP_ind    ,_sl_form(2,0,2,3,1), 256    ,(w*)&_ind1_sld  ,"��" },
    {"Ti-RS    ", &KRSI_ind    ,_sl_form(2,0,2,3,1), 1      ,(w*)&_ind1_sld  ,"ms "},
    {"Kp-REDS  ", &Kp_REDS_ind ,_sl_form(2,0,2,3,1), 256    ,(w*)&_ind1_sld  ,"��" },
    {"Ti-REDS  ", &Ki_REDS_ind ,_sl_form(2,0,2,3,1), 1      ,(w*)&_ind1_sld  ,"ms "},
    {"Id#      ", &OuRegS_dop_kod,_sl_form(2,0,2,3,1), _Id_Nom ,(w*)&_ind100_sld,"% " },
#ifndef _SIFU2_
    {"Id#R     ", &ZIDN        ,_sl_form(2,0,2,3,1), _Id_Nom ,(w*)&_ind100_sld,"% " },
    {"Id       ", &Id_dop_kod  ,_sl_form(2,0,2,3,1), _Id_Nom ,(w*)&_ind100_sld,"% " },
    {"IntRT    ", &OIRT        ,_sl_form(2,1,2,3,1),_Grad(1.0),(w*)&_ind1_sld ,"g " },
    {"IDN      ", &IDN         ,_sl_form(2,0,2,3,1), _Id_Nom ,(w*)&_ind100_sld,"% " },
    {"INN      ", &INN         ,_sl_form(2,0,2,3,1), _Id_Nom ,(w*)&_ind100_sld,"% " },
#else
    {"Id#R_RT1 ", &ZIDN        ,_sl_form(2,0,2,3,1), _Id_Nom ,(w*)&_ind100_sld,"% " },
    {"Id#R_RT2 ", &ZIDN_2_     ,_sl_form(2,0,2,3,1), _Id_Nom ,(w*)&_ind100_sld,"% " },
    {"Id1      ", &Id_dop_kod  ,_sl_form(2,0,2,3,1), _Id_Nom ,(w*)&_ind100_sld,"% " },
    {"Id2      ", &Id2_dop_kod ,_sl_form(2,0,2,3,1), _Id_Nom ,(w*)&_ind100_sld,"% " },
    {"IntRT1   ", &OIRT        ,_sl_form(2,1,2,3,1),_Grad(1.0),(w*)&_ind1_sld ,"g " },
    {"IntRT2   ", &OIRT_2_     ,_sl_form(2,1,2,3,1),_Grad(1.0),(w*)&_ind1_sld ,"g " },
    {"IDN_RT1  ", &IDN         ,_sl_form(2,0,2,3,1), _Id_Nom ,(w*)&_ind100_sld,"% " },
    {"IDN_RT2  ", &IDN_2_      ,_sl_form(2,0,2,3,1), _Id_Nom ,(w*)&_ind100_sld,"% " },
    {"INN-dv   ", &INN_dvig    ,_sl_form(2,0,2,3,1), _Id_Nom ,(w*)&_ind100_sld,"% " },
    {"INN_RT1  ", &INN         ,_sl_form(2,0,2,3,1), _Id_Nom ,(w*)&_ind100_sld,"% " },
    {"INN_RT2  ", &INN_2_      ,_sl_form(2,0,2,3,1), _Id_Nom ,(w*)&_ind100_sld,"% " },
    {"Idsum    ", &Id_sum      ,_sl_form(2,0,2,3,1), _Id_Nom ,(w*)&_ind100_sld,"% " },
#endif
    {"RTout    ", &ORT         ,_sl_form(2,1,2,3,1),_Grad(1.0),(w*)&_ind1_sld ,"g " },
    {"RegFlg   ", &RegFlg.all  ,_sl_form(2,1,1,0,0), 1       ,(w*)&_ind1_sld  ,"h " },
    {"flgDopIzm", &flg_RaTe.all,_sl_form(2,1,1,0,0), 1       ,(w*)&_ind1_sld  ,"h " },
#ifdef   Itt_ach
    {"Idtt     ", &Idtt        ,_sl_form(2,0,2,3,1), _Id_Nom ,(w*)&_ind100_sld,"% " },
#endif
#ifdef   Itt1_ach
    {"Idtt1    ", &Idtt1       ,_sl_form(2,0,2,3,1), _Id_Nom ,(w*)&_ind100_sld,"% " },
#endif
#ifdef   Itt2_ach
    {"Idtt2    ", &Idtt2       ,_sl_form(2,0,2,3,1), _Id_Nom ,(w*)&_ind100_sld,"% " },
#endif
#ifdef _RDN
    {"Id-2d    ", &Id_2v           ,_sl_form(2,0,2,3,1), _Id_Nom   ,(w*)&_ind100_sld,"% " },
    {"dId      ", &dId             ,_sl_form(2,0,2,3,1), _Id_Nom   ,(w*)&_ind100_sld,"% " },
    {"RDNout   ", &RDN_Out         ,_sl_form(2,0,2,3,1), _Skor_Nom ,(w*)&_ind100_sld,"% " },
#endif
#ifdef _BUKSOVKA
    {"N-2d     ", &N_2v            ,_sl_form(2,0,2,3,1), _Skor_Nom ,(w*)&_ind100_sld,"% " },
    {"dN       ", &dN              ,_sl_form(2,0,2,3,1), _Skor_Nom ,(w*)&_ind100_sld,"% " },
#endif
#ifdef _RRS
    {"IdDin    ", &Id_Din          ,_sl_form(2,0,2,3,1), _Id_Nom   ,(w*)&_ind100_sld,"% " },
    {"dSkor    ", &dSkor           ,_sl_form(2,0,2,3,1), _Skor_Nom ,(w*)&_ind100_sld,"% " },
    {"IdStat   ", &Id_Stat         ,_sl_form(2,0,2,3,1), _Id_Nom   ,(w*)&_ind100_sld,"% " },
    {"IdStatFlt", &Id_Stat_flt.out ,_sl_form(2,0,2,3,1), _Id_Nom   ,(w*)&_ind100_sld,"% " },
    {"IdSt-2d  ", &Id_stat_2v      ,_sl_form(2,0,2,3,1), _Id_Nom   ,(w*)&_ind100_sld,"% " },
    {"flgLiga  ", &liga.all        ,_sl_form(2,1,1,0,0), 1         ,(w*)&_ind1_sld  ,"h " },
#endif
    {"M        ", &Moment      ,_sl_form(2,0,2,3,1),_Mom_Nom ,(w*)&_ind100_sld,"% " },
#ifdef  _RVId
    { "Id1     ", &Id1             ,_sl_form(2,0,2,3,1), _Id_Nom   ,(w*)&_ind100_sld,"% " },
    { "Id2     ", &Id2             ,_sl_form(2,0,2,3,1), _Id_Nom   ,(w*)&_ind100_sld,"% " },
    { "If2     ", &V2.Iv           ,_sl_form(2,0,2,3,1), _Iv_Nom   ,(w*)&_ind100_sld,"% " },
    { "If#2    ", &V2.Iv_zad       ,_sl_form(2,0,2,3,1), _Iv_Nom   ,(w*)&_ind100_sld,"% " },
    { "dId     ", &dRVId           ,_sl_form(2,0,2,3,1), _Id_Nom   ,(w*)&_ind100_sld,"% " },
    { "dIf     ", &dIv             ,_sl_form(2,0,2,3,1), _Iv_Nom   ,(w*)&_ind100_sld,"% " },
#endif
#ifdef  Id_sr_ach
    {"Idsr     ", &Id_sr       ,_sl_form(2,0,2,3,1), _Id_Nom ,(w*)&_ind100_sld,"% " },
#endif
#ifdef  Id2_sr_ach
    {"Id2sr    ", &Id2_sr      ,_sl_form(2,0,2,3,1), _Id_Nom ,(w*)&_ind100_sld,"% " },
#endif
#ifndef _SIFU2_
    {"L#       ", &S.Alfa      ,_sl_form(2,1,2,3,1),_Grad(1.0),(w*)&_ind1_sld ,"g " },
    {"L        ", &S.Alfa_Old  ,_sl_form(2,1,2,3,1),_Grad(1.0),(w*)&_ind1_sld ,"g " },
    {"M_Nvs    ", &S.Most_Tir  ,_sl_form(1,1,1,0,0), 1         ,(w*)&_ind1_sld,"h " },
#else
    {"L1#      ", &S.Alfa      ,_sl_form(2,1,2,3,1),_Grad(1.0),(w*)&_ind1_sld ,"g " },
    {"L1       ", &S.Alfa_Old  ,_sl_form(2,1,2,3,1),_Grad(1.0),(w*)&_ind1_sld ,"g " },
    {"M_Nvs1   ", &S.Most_Tir  ,_sl_form(1,1,1,0,0), 1         ,(w*)&_ind1_sld,"h " },
    {"L2#      ", &S2.Alfa     ,_sl_form(2,1,2,3,1),_Grad(1.0),(w*)&_ind1_sld ,"g " },
    {"L2       ", &S2.Alfa_Old ,_sl_form(2,1,2,3,1),_Grad(1.0),(w*)&_ind1_sld ,"g " },
    {"M_Nvs2   ", &S2.Most_Tir ,_sl_form(1,1,1,0,0), 1         ,(w*)&_ind1_sld,"h " },
    {"IntRVT   ", &S2.Integr   ,_sl_form(2,0,2,3,1),_Grad(1.0)*64,(w*)&_ind1_sld ,"g " },
    {"RVTout   ", &S2.Out_rvt  ,_sl_form(2,0,2,3,1),_Grad(1.0),(w*)&_ind1_sld ,"g " },
    {"dL2      ", &S2.Alfa_RVT ,_sl_form(2,0,2,3,1),_Grad(1.0),(w*)&_ind1_sld ,"g " },
#endif
    {"Id_TplInt", &Id_TeploIntegr,_sl_form(2,0,2,3,1), _Id_Nom,(w*)&_ind100_sld,"% "},
    {"F#       ", &V.Fv_zad    ,_sl_form(2,0,2,3,1), _Fv_Nom ,(w*)&_ind100_sld,"% " },
    {"F        ", &V.Fv        ,_sl_form(2,0,2,3,1), _Fv_Nom ,(w*)&_ind100_sld,"% " },
    {"IntREDS  ", &Integr_REDS ,_sl_form(2,0,2,3,1), _Fv_Nom ,(w*)&_ind100_sld,"% " },
    {"If#      ", &V.Iv_zad    ,_sl_form(2,0,2,3,1), _Iv_Nom ,(w*)&_ind100_sld,"% " },
    {"If       ", &V.Iv        ,_sl_form(2,0,2,3,1), _Iv_Nom ,(w*)&_ind100_sld,"% " },
#ifdef _Vozb_KTE
    {"IntRTf   ", &V.Integr_rt ,_sl_form(2,1,2,3,1),_Grad(1.0),(w*)&_ind1_sld ,"g " },
    {"Lf#      ", &V.Alfa      ,_sl_form(2,1,2,3,1),_Grad(1.0),(w*)&_ind1_sld ,"g " },
    {"Lf       ", &V.Alfa_Old  ,_sl_form(2,1,2,3,1),_Grad(1.0),(w*)&_ind1_sld ,"g " },
    {"V_Nvs    ", &V.N         ,_sl_form(1,1,1,0,0), 1         ,(w*)&_ind1_sld,"h " },
    {"V_NumInt ", &V.NumInt    ,_sl_form(1,1,1,0,0), 1         ,(w*)&_ind1_sld,"h " },
    {"V_TZ     ", &V.TZ        ,_sl_form(2,1,1,0,0), 1         ,(w*)&_ind1_sld,"h " },
    {"V_TZreal ", &V.TZ_real   ,_sl_form(2,1,1,0,0), 1         ,(w*)&_ind1_sld,"h " },
    {"V_DZV    ", &V.DZV       ,_sl_form(1,1,1,0,0), 1         ,(w*)&_ind1_sld,"h " },
#endif
    {"V_flgL   ", &V.flg.all   ,_sl_form(2,1,1,0,0), 1         ,(w*)&_ind1_sld,"h " },
    {"V_flgH   ", ((b*)&V.flg.all)+2   ,_sl_form(2,1,1,0,0), 1         ,(w*)&_ind1_sld,"h " },
#ifdef   It_ach
    {"It       ", &It          ,_sl_form(2,0,2,3,1), _Iv_Nom ,(w*)&_ind100_sld,"% " },
#endif
#ifdef  IA1_ach
    {"IA1      ", &IA1         ,_sl_form(2,1,2,3,1), _Id_Nom  ,(w*)&_ind100_sld,"% "},
#endif
#ifdef  IB1_ach
    {"IB1      ", &IB1         ,_sl_form(2,1,2,3,1), _Id_Nom  ,(w*)&_ind100_sld,"% "},
#endif
#ifdef  IC1_ach
    {"IC1      ", &IC1         ,_sl_form(2,1,2,3,1), _Id_Nom  ,(w*)&_ind100_sld,"% "},
#endif
#ifdef  IA2_ach
    {"IA2      ", &IA2         ,_sl_form(2,1,2,3,1), _Id_Nom  ,(w*)&_ind100_sld,"% "},
#endif
#ifdef  IB2_ach
    {"IB2      ", &IB2         ,_sl_form(2,1,2,3,1), _Id_Nom  ,(w*)&_ind100_sld,"% "},
#endif
#ifdef  IC2_ach
    {"IC2      ", &IC2         ,_sl_form(2,1,2,3,1), _Id_Nom  ,(w*)&_ind100_sld,"% "},
#endif
#ifdef  IA3_ach
    {"IA3      ", &IA3         ,_sl_form(2,1,2,3,1), _Id_Nom  ,(w*)&_ind100_sld,"% "},
#endif
#ifdef  IB3_ach
    {"IB3      ", &IB3         ,_sl_form(2,1,2,3,1), _Id_Nom  ,(w*)&_ind100_sld,"% "},
#endif
#ifdef  IC3_ach
    {"IC3      ", &IC3         ,_sl_form(2,1,2,3,1), _Id_Nom  ,(w*)&_ind100_sld,"% "},
#endif
#ifdef  IA4_ach
    {"IA4      ", &IA4         ,_sl_form(2,1,2,3,1), _Id_Nom  ,(w*)&_ind100_sld,"% "},
#endif
#ifdef  IB4_ach
    {"IB4      ", &IB4         ,_sl_form(2,1,2,3,1), _Id_Nom  ,(w*)&_ind100_sld,"% "},
#endif
#ifdef  IC4_ach
    {"IC4      ", &IC4         ,_sl_form(2,1,2,3,1), _Id_Nom  ,(w*)&_ind100_sld,"% "},
#endif
#ifdef _Dat_Temp_Max
       _F0_ADC_multiplex_temp,
#endif
    {"cCfgR    ", &canr.CfgReg.all ,_sl_form(2,1,1,0,0), 1         ,(w*)&_ind1_sld,"h " },
    {"cStsR    ", &canr.StatReg.all,_sl_form(2,1,1,0,0), 1         ,(w*)&_ind1_sld  ,"h " },
    {"cN#      ", &canr.N_zad      ,_sl_form(2,0,2,3,1),_Skor_Nom,(w*)&_ind100_sld,"% " },
    {"cId#     ", &canr.Id_zad     ,_sl_form(2,0,2,3,1), _Id_Nom ,(w*)&_ind100_sld,"% " },
    {"cId#P    ", &canr.Idz_MaxP   ,_sl_form(2,0,2,3,1), _Id_Nom ,(w*)&_ind100_sld,"% " },
    {"cId#M    ", &canr.Idz_MaxM   ,_sl_form(2,0,2,3,1), _Id_Nom ,(w*)&_ind100_sld,"% " },
    {"cF#      ", &canr.F_zad      ,_sl_form(2,0,2,3,1), _Fv_Nom ,(w*)&_ind100_sld,"% " },
    {"cL#      ", &canr.L_zad      ,_sl_form(2,1,2,3,1),_Grad(1.0),(w*)&_ind1_sld ,"g " },
    {"cL#min   ", &canr.L_min      ,_sl_form(2,1,2,3,1),_Grad(1.0),(w*)&_ind1_sld ,"g " },
    {"PDF0m    ", &PDF[0].out_m    ,_sl_form(2,0,2,3,1),_Skor_Nom,(w*)&_ind100_sld,"% " },
    {"PDF0g    ", &PDF[0].out_g    ,_sl_form(2,0,2,3,1),_Grad(1.0),(w*)&_ind1_sld ,"g " },
#ifdef _T_PDF
    {"flg_PDF0 ", &PDF[0].flg.all          , _sl_form(1,1,1,0,0), 1         , (w*)&_ind1_sld  , "h " },
    {"N_F_PDF0 ", &PDF[0].N_Fpdf           , _sl_form(2,0,2,3,0), _Skor_Nom , (w*)&_ind100_sld, "% " },
    {"N_T_PDF0 ", &PDF[0].N_Tpdf           , _sl_form(2,0,2,3,0), _Skor_Nom , (w*)&_ind100_sld, "% " },
  //{"T_PDF0   ", (w*)&pdf_drv_data[0].Tpdf, _sl_form(2,1,1,0,0), 1         , (w*)&_ind1_sld  , "h " },
    {"F_PDF0   ", (w*)&PDF[0].Fpdf         , _sl_form(2,1,1,0,0), 1         , (w*)&_ind1_sld  , "h " },
    {"�����_T  ", (w*)&pdf_drv_data[0].Scht_T    , _sl_form(2,1,1,0,0), 1   , (w*)&_ind1_sld  , "h " },
    {"Tpdf_time", (w*)&pdf_drv_data[0].Tpdf_time , _sl_form(2,1,1,0,0), 1   , (w*)&_ind1_sld  , "h " },
#endif
    {"Sels0g   ", &Sels[0].out_g   ,_sl_form(2,0,2,3,1), _Grad(1.0),(w*)&_ind1_sld  ,"g " },
    {"Sels0m   ", &Sels[0].out_m   ,_sl_form(2,0,2,3,1), _Skor_Nom ,(w*)&_ind100_sld,"% " },
    {"Sels1g   ", &Sels[1].out_g   ,_sl_form(2,0,2,3,1), _Grad(1.0),(w*)&_ind1_sld  ,"g " },
    {"Sels1m   ", &Sels[1].out_m   ,_sl_form(2,0,2,3,1), _Skor_Nom ,(w*)&_ind100_sld,"% " },
    {"Po0      ", &Po0_c.all       ,_sl_form(1,1,1,0,0), 1         ,(w*)&_ind1_sld  ,"h " },
#ifdef  _AsPo0_
    {"AsPo0    ", &PoS_c[0].all    ,_sl_form(1,1,1,0,0), 1         ,(w*)&_ind1_sld,"h " },
#endif
#ifdef  _AsPo1_
    {"AsPo1    ", &PoS_c[1].all    ,_sl_form(1,1,1,0,0), 1         ,(w*)&_ind1_sld  ,"h " },
#endif
#ifdef  _AsPo2_
    {"AsPo2    ", &PoS_c[2].all    ,_sl_form(1,1,1,0,0), 1         ,(w*)&_ind1_sld  ,"h " },
#endif
#ifdef  _AsPo3_
    {"AsPo3    ", &PoS_c[3].all    ,_sl_form(1,1,1,0,0), 1         ,(w*)&_ind1_sld  ,"h " },
#endif
#ifdef  _AsPo4_
    {"AsPo4    ", &PoS_c[4].all    ,_sl_form(1,1,1,0,0), 1         ,(w*)&_ind1_sld  ,"h " },
#endif
#ifdef  _AsPo5_
    {"AsPo5    ", &PoS_c[5].all    ,_sl_form(1,1,1,0,0), 1         ,(w*)&_ind1_sld  ,"h " },
#endif
#ifdef  _AsPo6_
    {"AsPo6    ", &PoS_c[6].all    ,_sl_form(1,1,1,0,0), 1         ,(w*)&_ind1_sld  ,"h " },
#endif
#ifdef  _AsPo7_
    {"AsPo7    ", &PoS_c[7].all    ,_sl_form(1,1,1,0,0), 1         ,(w*)&_ind1_sld  ,"h " },
#endif
    {"Pi0      ", &Pi0_c.all       ,_sl_form(1,1,1,0,0), 1         ,(w*)&_ind1_sld,"h " },
#ifdef  _Pi1_
    {"Pi1      ", &Pi1_c.all       ,_sl_form(1,1,1,0,0), 1         ,(w*)&_ind1_sld,"h " },
#endif
#ifdef  _Pi2_
    {"Pi2      ", &Pi2_c.all       ,_sl_form(1,1,1,0,0), 1         ,(w*)&_ind1_sld,"h " },
#endif
    {"Pi0f     ", &Pi0_cf.all      ,_sl_form(1,1,1,0,0), 1         ,(w*)&_ind1_sld,"h " },
#ifdef  _Pi1_
    {"Pi1f     ", &Pi1_cf.all      ,_sl_form(1,1,1,0,0), 1         ,(w*)&_ind1_sld,"h " },
#endif
#ifdef  _Pi2_
    {"Pi2f     ", &Pi2_cf.all      ,_sl_form(1,1,1,0,0), 1         ,(w*)&_ind1_sld,"h " },
#endif
#ifdef  _AsPi0_
    {"AsPi0    ", &PiS_c[0].all    ,_sl_form(1,1,1,0,0), 1         ,(w*)&_ind1_sld,"h " },
#endif
#ifdef  _AsPi1_
    {"AsPi1    ", &PiS_c[1].all    ,_sl_form(1,1,1,0,0), 1         ,(w*)&_ind1_sld,"h " },
#endif
#ifdef  _AsPi2_
    {"AsPi2    ", &PiS_c[2].all    ,_sl_form(1,1,1,0,0), 1         ,(w*)&_ind1_sld,"h " },
#endif
#ifdef  _AsPi3_
    {"AsPi3    ", &PiS_c[3].all    ,_sl_form(1,1,1,0,0), 1         ,(w*)&_ind1_sld,"h " },
#endif
#ifdef  _AsPi4_
    {"AsPi4    ", &PiS_c[4].all    ,_sl_form(1,1,1,0,0), 1         ,(w*)&_ind1_sld,"h " },
#endif
#ifdef  _AsPi5_
    {"AsPi5    ", &PiS_c[5].all    ,_sl_form(1,1,1,0,0), 1         ,(w*)&_ind1_sld,"h " },
#endif
#ifdef  _AsPi6_
    {"AsPi6    ", &PiS_c[6].all    ,_sl_form(1,1,1,0,0), 1         ,(w*)&_ind1_sld,"h " },
#endif
#ifdef  _AsPi7_
    {"AsPi7    ", &PiS_c[7].all    ,_sl_form(1,1,1,0,0), 1         ,(w*)&_ind1_sld,"h " },
#endif
#ifdef  _AsPi8_
    {"AsPi8    ", &PiS_c[8].all    ,_sl_form(1,1,1,0,0), 1         ,(w*)&_ind1_sld,"h " },
#endif
#ifdef  _AsPi9_
    {"AsPi9    ", &PiS_c[9].all    ,_sl_form(1,1,1,0,0), 1         ,(w*)&_ind1_sld,"h " },
#endif
#ifdef  _AsPi10_
    {"AsPi10   ", &PiS_c[10].all   ,_sl_form(1,1,1,0,0), 1         ,(w*)&_ind1_sld,"h " },
#endif
#ifdef  _AsPi11_
    {"AsPi11   ", &PiS_c[11].all   ,_sl_form(1,1,1,0,0), 1         ,(w*)&_ind1_sld,"h " },
#endif
#ifdef  _AsPi12_
    {"AsPi12   ", &PiS_c[12].all   ,_sl_form(1,1,1,0,0), 1         ,(w*)&_ind1_sld,"h " },
#endif
#ifdef  _AsPi13_
    {"AsPi13   ", &PiS_c[13].all   ,_sl_form(1,1,1,0,0), 1         ,(w*)&_ind1_sld,"h " },
#endif
#ifdef  _AsPi14_
    {"AsPi14   ", &PiS_c[14].all   ,_sl_form(1,1,1,0,0), 1         ,(w*)&_ind1_sld,"h " },
#endif
#ifdef  _AsPi15_
    {"AsPi15   ", &PiS_c[15].all   ,_sl_form(1,1,1,0,0), 1         ,(w*)&_ind1_sld,"h " },
#endif
#ifdef  _AsPi0_
    {"AsPi0f   ", &PiS_cf[0].all   ,_sl_form(1,1,1,0,0), 1         ,(w*)&_ind1_sld  ,"h " },
#endif
#ifdef  _AsPi1_
    {"AsPi1f   ", &PiS_cf[1].all   ,_sl_form(1,1,1,0,0), 1         ,(w*)&_ind1_sld  ,"h " },
#endif
#ifdef  _AsPi2_
    {"AsPi2f   ", &PiS_cf[2].all   ,_sl_form(1,1,1,0,0), 1         ,(w*)&_ind1_sld  ,"h " },
#endif
#ifdef  _AsPi3_
    {"AsPi3f   ", &PiS_cf[3].all   ,_sl_form(1,1,1,0,0), 1         ,(w*)&_ind1_sld  ,"h " },
#endif
#ifdef  _AsPi4_
    {"AsPi4f   ", &PiS_cf[4].all   ,_sl_form(1,1,1,0,0), 1         ,(w*)&_ind1_sld  ,"h " },
#endif
#ifdef  _AsPi5_
    {"AsPi5f   ", &PiS_cf[5].all   ,_sl_form(1,1,1,0,0), 1         ,(w*)&_ind1_sld  ,"h " },
#endif
#ifdef  _AsPi6_
    {"AsPi6f   ", &PiS_cf[6].all   ,_sl_form(1,1,1,0,0), 1         ,(w*)&_ind1_sld  ,"h " },
#endif
#ifdef  _AsPi7_
    {"AsPi7f   ", &PiS_cf[7].all   ,_sl_form(1,1,1,0,0), 1         ,(w*)&_ind1_sld  ,"h " },
#endif
#ifdef  _AsPi8_
    {"AsPi8f   ", &PiS_cf[8].all   ,_sl_form(1,1,1,0,0), 1         ,(w*)&_ind1_sld  ,"h " },
#endif
#ifdef  _AsPi9_
    {"AsPi9f   ", &PiS_cf[9].all   ,_sl_form(1,1,1,0,0), 1         ,(w*)&_ind1_sld  ,"h " },
#endif
#ifdef  _AsPi10_
    {"AsPi10f  ", &PiS_cf[10].all  ,_sl_form(1,1,1,0,0), 1         ,(w*)&_ind1_sld  ,"h " },
#endif
#ifdef  _AsPi11_
    {"AsPi11f  ", &PiS_cf[11].all  ,_sl_form(1,1,1,0,0), 1         ,(w*)&_ind1_sld  ,"h " },
#endif
#ifdef  _AsPi12_
    {"AsPi12f  ", &PiS_cf[12].all  ,_sl_form(1,1,1,0,0), 1         ,(w*)&_ind1_sld  ,"h " },
#endif
#ifdef  _AsPi13_
    {"AsPi13f  ", &PiS_cf[13].all  ,_sl_form(1,1,1,0,0), 1         ,(w*)&_ind1_sld  ,"h " },
#endif
#ifdef  _AsPi14_
    {"AsPi14f  ", &PiS_cf[14].all  ,_sl_form(1,1,1,0,0), 1         ,(w*)&_ind1_sld  ,"h " },
#endif
#ifdef  _AsPi15_
    {"AsPi15f  ", &PiS_cf[15].all  ,_sl_form(1,1,1,0,0), 1         ,(w*)&_ind1_sld  ,"h " },
#endif
#ifndef  Num_obl
    {"NumRez   ", &Num_obl         ,_sl_form(1,1,2,1,0), 1         ,(w*)&_ind1_sld," "  },
#endif
    {"S_NumInt ", &S.NumInt        ,_sl_form(1,1,1,0,0), 1         ,(w*)&_ind1_sld,"h " },
    {"S_flg    ", &S.flg.all       ,_sl_form(2,1,1,0,0), 1         ,(w*)&_ind1_sld,"h " },
    {"S_TZ     ", &S.TZ            ,_sl_form(2,1,1,0,0), 1         ,(w*)&_ind1_sld,"h " },
#ifdef _SIFU2_
    {"S2_NumInt", &S2.NumInt       ,_sl_form(1,1,1,0,0), 1         ,(w*)&_ind1_sld,"h " },
    {"S2_flg   ", &S2.flg.all      ,_sl_form(2,1,1,0,0), 1         ,(w*)&_ind1_sld,"h " },
    {"S2_TZ    ", &S2.TZ           ,_sl_form(2,1,1,0,0), 1         ,(w*)&_ind1_sld,"h " },
#endif
    {"Prg_all  ", &Prg.all         ,_sl_form(2,1,1,0,0), 1         ,(w*)&_ind1_sld,"h " },
    {"Otkl_Imp ", &Otkl_Imp        ,_sl_form(2,1,1,0,0), 1         ,(w*)&_ind1_sld,"h " },
    {"S_DZV    ", &S.DZV           ,_sl_form(1,1,1,0,0), 1         ,(w*)&_ind1_sld,"h " },
    {"dt_revers", &dt_revers       ,_sl_form(2,1,2,2,3), _pSec(1.0),(w*)&_ind1_sld,"s " },
    {"dn_revers", &dn_revers       ,_sl_form(1,1,2,1,0), 1         ,(w*)&_ind1_sld," "  },
    {"Av_all   ", &Av.all          ,_sl_form(2,1,1,0,0), 1         ,(w*)&_ind1_sld,"h " },
    {"Av2_all  ", &Av2.all         ,_sl_form(2,1,1,0,0), 1         ,(w*)&_ind1_sld,"h " },
    {"Rev_label", &Revers_Label    ,_sl_form(1,1,2,1,0), 1         ,(w*)&_ind1_sld," "  },
#ifdef   _Union_FR
    {"Syn_apEPA", (w*)&IPCP0       ,_sl_form(2,1,1,0,0), 1         ,(w*)&_ind1_sld,"h " },
#else
  #ifdef _CM3_
    {"Syn_apEPA", (w*)&LPC_TIM3->CR0,_sl_form(2,1,1,0,0), 1        ,(w*)&_ind1_sld,"h " },
  #else  // 590-� ��������� :
    {"Syn_apEPA", (w*)&IPCP3       ,_sl_form(2,1,1,0,0), 1         ,(w*)&_ind1_sld,"h " },
  #endif
#endif
    {"Syn_EPA  ", (w*)&Syn.NS_EPA  ,_sl_form(2,1,1,0,0), 1         ,(w*)&_ind1_sld,"h " },
    {"Syn_NS2  ", (w*)&Syn.NS2     ,_sl_form(2,1,1,0,0), 1         ,(w*)&_ind1_sld,"h " },
    {"Tsyn     ", &Tsyn            ,_sl_form(2,1,1,0,0), 1         ,(w*)&_ind1_sld,"h " },
#ifdef   _SIFU_Syn2
 #ifdef  _Union_FR
    {"Syn2apEPA", (w*)&IPCP2       ,_sl_form(2,1,1,0,0), 1         ,(w*)&_ind1_sld,"h " },
 #else
   #ifdef _CM3_
    {"Syn2apEPA", (w*)&LPC_TIM1->CR0,_sl_form(2,1,1,0,0), 1        ,(w*)&_ind1_sld,"h " },
   #else // 590-� ��������� :
    {"Syn2apEPA", (w*)&IPCP5       ,_sl_form(2,1,1,0,0), 1         ,(w*)&_ind1_sld,"h " },
   #endif
 #endif
    {"Syn2_EPA ", (w*)&Syn2.NS_EPA ,_sl_form(2,1,1,0,0), 1         ,(w*)&_ind1_sld,"h " },
    {"Syn2_NS2 ", (w*)&Syn2.NS2    ,_sl_form(2,1,1,0,0), 1         ,(w*)&_ind1_sld,"h " },
    {"Tsyn2    ", &Tsyn2           ,_sl_form(2,1,1,0,0), 1         ,(w*)&_ind1_sld,"h " },
#endif
#ifdef _Rev_Pola   // ���� ������� �������� ������� ����.
    {"RevPo_flg", (w*)&RP_Pz.all   ,_sl_form(2,1,1,0,0), 1         ,(w*)&_ind1_sld,"h " },
#endif
#ifndef _CM3_
    {"timer1   ", (w*)&timer1      ,_sl_form(2,1,1,0,0), 1         ,(w*)&_ind1_sld,"h " },
#else
    {"timer1   ", (w*)&LPC_TIM0->TC,_sl_form(2,1,1,0,0), 1         ,(w*)&_ind1_sld,"h " },
#endif
    {"Timer1Ovr", (w*)&Timer1_Ovr  ,_sl_form(2,1,1,0,0), 1         ,(w*)&_ind1_sld,"h " },
  //{"zi_prz   ", &zi_rs.prz.all   ,_sl_form(1,1,1,0,0), 1         ,(w*)&_ind1_sld,"h " },
  //{"zi_in2   ", &zi_rs.in_2      ,_sl_form(2,0,2,3,1), _Skor_Nom ,(w*)&_ind100_sld,"% " },
    {"zi_out   ", &zi_rs.out       ,_sl_form(2,0,2,3,1), _Skor_Nom ,(w*)&_ind100_sld,"% " },
  //{"zi_out2  ", &zi_rs.out_2     ,_sl_form(2,0,2,3,1), _Skor_Nom ,(w*)&_ind100_sld,"% " },
    {"zi_outlin", &zi_rs.out_lin   ,_sl_form(2,0,2,3,1), _Skor_Nom ,(w*)&_ind100_sld,"% " },
    {"z_res_out", (w*)&zad_res.out ,_sl_form(2,1,1,0,0), 1         ,(w*)&_ind1_sld,"h "   },
    {"I1pr-tir ", &Iproboy1    ,_sl_form(2,0,2,3,1), _Id_Nom ,(w*)&_ind100_sld,"% " },  // ��� ������ � ����������� ��������� ���������
    {"i-tyr    ", &i           ,_sl_form(1,1,2,3,0), 1         ,(w*)&_ind1_sld," "  },
    {"n-tyr    ", &n           ,_sl_form(1,1,2,1,0), 1         ,(w*)&_ind1_sld," "  },
    {"nn-tyr   ", &nn          ,_sl_form(1,1,2,1,0), 1         ,(w*)&_ind1_sld," "  },
#ifdef _SIFU2_
    {"I2pr-tir ", &Iproboy2    ,_sl_form(2,0,2,3,1), _Id_Nom ,(w*)&_ind100_sld,"% " },
    {"n2-tyr   ", &n2          ,_sl_form(1,1,2,1,0), 1         ,(w*)&_ind1_sld," "  },
    {"nn2-tyr  ", &nn2         ,_sl_form(1,1,2,1,0), 1         ,(w*)&_ind1_sld," "  },
#endif
    {"Kompen_Ud", &Kompen_Ud       ,_sl_form(2,1,2,3,0), 1         ,(w*)&_ind1_sld,  "d " },
    {"Ud_Komp  ", &UdSr_Komp       ,_sl_form(2,0,2,3,1), _Ud_Nom   ,(w*)&_ind100_sld,"% " },
    {"EDS_P    ", &EDS_P           ,_sl_form(2,0,2,3,1), _EDS_Nom  ,(w*)&_ind100_sld,"% " }, // ��������� ���������
    {"ZnEDS_P  ", &ZnEDS_P         ,_sl_form(1,1,1,0,0), 1         ,(w*)&_ind1_sld  ,"h " }   // ��� ������ � �����. ����
#ifdef _Sld_cnst_Obj
  ,_Sld_cnst_Obj
#endif
 *
 *
 *
 *
 *
 *
 *
 *
 *
 * */







