// Запихнем интерфейс для упаковки JSON формата
//--------------JSON-------------------------------------------
//#define _CRT_SECURE_NO_WARNINGS - уже объявлена в jwrite.c // stop complaining about deprecated functions
#include <stdio.h>
#include "jWrite.h"
#include "syscalls.c"
#define MAXANSWERLENGTH 256     //массив для json
//--------------------------------CAN----------------------------------------
int Can1_Rx_Count;              //Счетсчик используем в CAN RX Interrupt
unsigned char   Can1_Recv_Buf[10]={0};

// Операционку пока отключил
/*пока не работате
 * #include "FreeRTOSConfig.h"
#include "projdefs.h"
#include "portmacro.h"
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "croutine.h"
#include "task.h"
#include "queue.h"
*/

char type;   // флажок для того чтобы когда ответит модуль передать данные json
unsigned int len; // дЛЯ определения длины буффера
unsigned char buffer_wifi[MAXANSWERLENGTH]; // массив - ложим наш формат


//----------------------------------------------------------------------------
/* Includes ------------------------------------------------------------------*/
#include "stm32f30x.h"
#include "stm32f30x_gpio.h"
#include "stm32f30x_rcc.h"
#include "stm32f30x_can.h"
#include "stm32f30x_misc.h"
//#include "stm32303c_eval.h" --------Отладочные сетододы-------------------
#define LED1    GPIO_Pin_8
#define LED2    GPIO_Pin_9
#define LED3    GPIO_Pin_10
#define LED4    GPIO_Pin_11
//---------------для отправки  TX в ручном режиме ------------------------------------
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
/*Протокол юарт на CMSIS /Отправка ок в передаче на теминал в цикле. Прием 1 и 0 в прерывании для зажигания диода
 * при использовании передачи в прерывании по приему и дет зависание*/
//#include "stm32f30x.h" //описание регистров, необходимо перейти в него и раскоментировать необходимую строку
void UART_Init (void); // Объявляем функцию
#include <string.h>
uint8_t  FLAG_REPLY;//флаг окончания ответа
uint8_t RXi;//индекс массива
char RXc;// элемент массива
//размер буфера
#define RX_BUF_SIZE 64
static char RXBuffer[RX_BUF_SIZE];
static char RXBuffer1[RX_BUF_SIZE];

void UART_Init (void){
	RCC->APB1ENR |= RCC_APB1ENR_UART4EN;//подаем тактирование на уарт - 36Мгц
	RCC->AHBENR|=RCC_AHBENR_GPIOCEN;//включаем тактирование порта GPIOC - 72Мгц
    //Настраиваем порт Tx
	GPIOC->MODER |= GPIO_MODER_MODER10_1;//10 - режим альтернативных функций
	GPIOC->OTYPER &= ~GPIO_OTYPER_OT_10;//0 - 0: двухтактный выход или push-pull сокращено PP(после сброса)
	GPIOC->PUPDR &= ~GPIO_PUPDR_PUPDR10_0;//01 - подтяжка к плюсу питания или pull-up сокращено PU
	GPIOC->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR10;//11 - 50 MHz, высокочастотный выход
	GPIOC->AFR[1]|=(0x05<<(4*2));//назначаем 10 выводу альтернативную функцию
	//Настаиваем Rx   PC11
	GPIOC->MODER |= GPIO_MODER_MODER11_1;//10 - режим альтернативных функций
	GPIOC->PUPDR &= ~GPIO_PUPDR_PUPDR11;//00 - состояние после сброса, no pull up, pull down
	GPIOC->AFR[1]|=(0x05<<(4*3));//назначаем 11 выводу альтернативную функцию

	UART4->BRR=0xea6;//  (16)(36000000+4800)/9600 = 0xea6 (3750); работает  0х139
	UART4->CR1|=USART_CR1_UE|USART_CR1_TE|USART_CR1_RE;//вкл. uart, приема и передачи
	UART4->CR1|=USART_CR1_RXNEIE;//Разрешаем генерировать прерывание по приему
	NVIC_EnableIRQ(UART4_IRQn);//Dключаем прерывание, указываем вектор
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
//дОБАВЛЯЮ НАСТРОЙКУ  http://forum.easyelectronics.ru/viewtopic.php?f=35&t=12806
void CAN1_Init(void)
{
  GPIO_InitTypeDef      GPIO_InitStructure;
  CAN_InitTypeDef       CAN_InitStructure;
  CAN_FilterInitTypeDef CAN_FilterInitStructure;
  NVIC_InitTypeDef      NVIC_InitStructure;

  /*
   * !!! При инициализации сначала использую настройку прерываний, т.к. некотороые жалуются на неправильную раюоту
    * Настройка прерываний на прием сообщений CAN
 С прерываниями дела обстоят ни чуть не сложнее, чем со структурой инициализации. Для начала надо настроить и
  проинициализировать контроллер прерываний (NVIC — Nested vectored interrupt controller).
  В архитектуре STM32 каждому прерыванию можно выставить свой приоритет для случаев,
  когда возникает несколько прерываний одновременно. Поэтому NVIC представляет нам несколько вариантов формирования
   приоритетных групп. Я не буду вдаваться в подробности, в нашем случае это не актуально, так как у нас на данный момент
   настроено лишь одно прерывание на обработку пакетов CAN/

 Для того, чтобы мы смогли получать сообщения из CAN шины, мы должны включить прерывания на получение сообщений шины,
  а также разрешить прерывания по факту появления сообщений в почтовом ящике.
*/

   //NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
   NVIC_InitStructure.NVIC_IRQChannel                   = USB_LP_CAN1_RX0_IRQn;//|USB_HP_CAN1_TX_IRQn;
   //Подключение FreeRTOS Пока отключил.
   //NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY - 2;
   NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
   NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0x00;
   NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
   NVIC_Init(&NVIC_InitStructure);

   //на отправку пока отключу
   //NVIC_InitStructure.NVIC_IRQChannel                   = USB_HP_CAN1_TX_IRQn;//|USB_HP_CAN1_TX_IRQn;
   //NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
   ////////NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY - 1;
   //NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0x00;
   //NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
   //NVIC_Init(&NVIC_InitStructure);

   /*
    * Следует обратить внимание, что прерывание для CAN шины и USB одно и тоже. Если Вы будете использовать оба устройства
    * в своей разработке, то необходимо более тщательно подойти к обработке прерываний от них. Могут возникнуть проблемы вплоть
    *  до полного отказа одного из устройств.
 В параметре "NVIC_IRQChannel" мы указываем, какое именно прерывание мы инициализируем. Константа "USB_LP_CAN1_RX0_IRQn"
 обозначает канал, отвечающий за прерывания, связанные с CAN1. Найдя ее определение в файле stm32f10x.h, вы увидите еще
 множество констант (ADC1_IRQn, TIM1_TRG_COM_TIM17_IRQn и др.), обозначающих прерывания от других периферийных устройств.
 Следующими двумя строками мы указываем приоритет прерываний (максимальные значения этих двух параметров определяются
 выбранной приоритетной группой). Затем указываем, что прерывание активно.
 Последняя строка, собственно, включает использование прерывания.
 На этом настройку CAN можно считать законченной. Мы учли все основные моменты, которые могут возникнут на начальном
 этапе изучения возможностей CAN шины. В зависимости от моделей контроллеров код может несколько меняться, но общие
 принципы работы остаются и легко портируются на разные семейства микроконтроллеров.
    * */

  /* CAN_1 clock enable */
  RCC_AHBPeriphClockCmd (RCC_AHBPeriph_GPIOB, ENABLE); // Тактиованеи портва
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE); // модуля

  /* Connect CAN_RX & CAN_TX to AF9 Конфигурация альтеративных функций. Своего рода ремапинг. Перенес вверх */
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource8, GPIO_AF_9); //CAN_RX = PB8
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource9, GPIO_AF_9); //CAN_TX = PB9
  /* Configure CAN RX and TX pins */
  //Работает - конфигурация сразу двух ножек
   /*GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
   GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
   GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
   GPIO_Init(GPIOB, &GPIO_InitStructure);*/

  // также работает
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
  /*Enable or disable the time triggered communication mode. Включение или отключение режима Time Triggered Mode.
   * Этот параметр влияет на включение таймера. Внутренний 16-битный таймер используется для привязки метки времени к каждому принятому и отправленному сообщению. Этот таймер начинает счет с момента разрешения работы контроллера CAN.
	Этот счетчик может быть сброшен приложением или автоматически после приема в последний mailbox, когда установлен Time Triggered Mode*/
  CAN_InitStructure.CAN_ABOM = DISABLE;
  /*Enable or disable the automatic bus-off management. Включение или отключение автоматического отключения шины.
	Если режим включен, то при накоплении ошибок приема данных из шины, CAN автоматически будет отключен.
	В любом случае необходимо контролировать ошибки приема, содержимое почтовых ящиков и, при необходимости сбрасывать ошибки вручную.
   * */
  CAN_InitStructure.CAN_AWUM = DISABLE;
  /*Enable or disable the automatic wake-up mode.
Включение или отключение автоматического пробуждения устройства по сигналу с CAN-шины.
	При включении этого параметра устройство будет автоматически просыпаться, но следует обратить внимание что на активацию устройства требуется некоторое время и первый пакет, переданный по шине, может быть утерян.
   * */
  CAN_InitStructure.CAN_NART = DISABLE;
  /*Enable or disable the no-automatic retransmission mode.Включение или отключение режима проверки получения пакета.
Если этот режим включен, то микроконтроллер при передаче кадра не будет проверять подтверждение получения пакета всеми устройством на шине.
Если выключен - то, при передаче кадра, микроконтроллер будет слушать шину на предмет получения подтверждения от всех устройства о том, что пакет получен и, если не получен хотя бы одним устройтсвом, то будет пытаться отправить повторно до тех пор, пока все устройства не подтвердят получение пакета.
Другими словами: При включенном параметре передал и забыл, при выключенном - передал и проверил, если не смог отправить, то пытается еще раз, пока не сможет отправить.
Если устройство не подключено к шине или если неправильно настроены тайминги, то при выключении этого параметра контроллер будет бесконечно пытаться отправлять пакет в шину.
   * */
  CAN_InitStructure.CAN_RFLM = DISABLE;
  /* Enable or disable the Receive FIFO Locked mode.Включение или отключение режима блокировки Receive FIFO
0 - при переполнении RX_FIF0 прием не прерывается (3 сообщения до заполнения FIFO), каждый новый пакет затирает предыдущий
1 - при переполнении RX_FIF0 прием прерывается (3 сообщения до заполнения FIFO), новые пакеты отбрасываются до освобождения RX_FIFO.
(Спасибо maxx_ir  за подсказку)
   * */
  CAN_InitStructure.CAN_TXFP = DISABLE;
  /* * Enable or disable the transmit FIFO priority.
Включение или отключение приоритета передачи FIFO Включение этого параметра определяет, в каком порядке сообщения будут отправляться в шину.
Если параметр включен, то сообщения отправляются в хронологическом порядке: FIFO - First Input First Output - Первый пришел - первым ушел.
Если же выключен, то пакеты передаются в зависимости от приоритета ID пакета. Т.е. пакет с более высоким приоритетом будет отправлен раньше.
   * */

  /*
   * Для первого запуска и тестирования устройства нам будет достаточно включить режим "CAN_NART".
   * При подключении осциллографа к шине, если устройство нормально функционирует, мы увидим всплески активности
   * в зависимости от того, как часто передаем данные по шине.
Если же "CAN_NART" выключен, то при включенном параметре "CAN_ABOM" будет предпринято всего 128 попыток передать пакет,
 а потом микроконтроллер отключит модуль Can (если Вы не будете обрабатывать ошибки и сбрасывать счетчики ошибок).
  При выключенном "CAN_ABOM" автоматического отключения модуля Can не произойдет.
Если же устройство не будет получать подтверждение о получении пакетов, то осциллограф нарисует "расческу",
так как устройство будет бесконечно пытаться передать пакеты, пока не получит подтверждения.
   * */

//Следующий параметр CAN_Mode определяет в каком режиме контроллер будет работать с CAN-шиной:
  /*
   * CAN_Mode_Normal
	Normal -нормальный режим работы	При этом параметре МК будет работать в обычном (нормальном) режиме работы. Данные будут передаваться и читаться из шины.
   *
   * CAN_Mode_LoopBack
	LoopBack - слушать себя	При выборе режима LoopBack, контроллер будет передавать данные в шину и слушать себя же одновременно.
Это равносильно тому, что в USART мы бы замкнули ножку TX на ножку RX.
Но пакеты из шины доходить до контроллера не будут.
   *
   * CAN_Mode_Silent
	Silent - слушать шину	Идеален при настройке устройств, которым нужно только слушать шину.
	 Например, если нам нужно подключиться к CAN шине автомобиля, но мы боимся допустить какие либо сбои из-за неправильной отправки пакетов в шину, то этот режим будет идеальным, так как в шину автомобиля пакеты из устройства попадать не будут.
   *
   * CAN_Mode_Silent_LoopBack
	Это объединенные режимы Silent и LoopBack - слушать только себя	В данном режиме все пакеты будут полностью крутится внутри контроллера без выхода в общую шину.
Из шины соответственно ни один пакет данных не дойдет до устройства.
Этот режим идеален для отладки устройства. При включении его мы можем как передать данные в шину, так и обработать те данные, которые мы же и отправили, при этом не имея физического подключения к шине.
   * */

  CAN_InitStructure.CAN_Mode = CAN_Mode_Normal; // CAN_Mode_Silent(режим чтения - слушаю);// CAN_Mode_LoopBack CAN_Mode_Normal

  /*Настройка таймингов
Синхронизация и тайминги в CAN — отдельный, важный и сложный вопрос. Однако, благодаря сложности и продуманности становится не так важна возможная рассинхронизация и нестабильность тактовых частот узлов сети, и связь становится возможной даже в тяжёлых условиях.

Вкратце разберем как выглядит передача одного бита информации при передаче сообщения в CAN шине.
• сегмент синхронизации
	• сегмент воспроизведения
	• сегмент фазы 1
	• сегмент фазы 2
	Схема бита данных шины CAN:

	     |----------One bit-----------------------|
	____  _______________ ________________________  __________
	    \/      |        |		|	      \/
	____/\Sync__|Prop-seg|Phase 1___|_Phase 2_____/\__________
					|
				Sampling point

				Всё время делится на кванты длиной t_q, и номинальная длительность бита равна 1+BS1+BS2 квантов.
				 Захват значения бита происходит на границе BS1 и BS2. В процессе приёма приёмник определяет,
				 в какой из временных периодов произошёл перепад сигнала (т.е. начало приёма нового бита).
				 В норме перепад должен произойти на границе SYNC и BS1, если он произошёл раньше —
				 контроллер уменьшает BS1, если позже — увеличивает BS2 на величину SJW (от 1 до 4 квантов времени). Таким образом, происходит постоянная пересинхронизация с частотой других приёмников.
В нашем примере мы настраиваем тайминги с учетом того, что перефирия настроена на частоту 8MHz. С помощью калькулятора таймингов, выбираем оптимальные под нашу шину и микроконтроллер.
Скорость шины настраивается с помощью прескалера. При указанных параметрах тайминга и CAN_Prescaler равным 50 - скорости передачи по шине составит 10 Кбит.
Для изменения скорости передачи, в нашем примере достаточно изменить CAN_Prescaler, например при значении равном "1", скорость передачи составит 500 Кбит, при "2" - 250 Кбит, ну и так далее. В примере кода, предоставленном во вложении к данной статье, расписаны все значения CAN_Prescaler, доступные для данного проекта. В принципе можно добится скорости вплоть до 1 Мбита.
За настройку таймингов при инициализации CAN устройства отвечают четыре параметра:


AN_SJW	Размер SJW	SJW (reSynchronization Jump Width) определяет максимальное количество квантов времени, на которое может быть увеличено или уменьшено количество квантов времени битовых сегментов. Возможные значения этого показателя от 1-го до 4-х квантов.
CAN_BS1	Длина сегмента фазы 1	BS1 (Bit Segment 1) - определяет местоположение точки захвата (Sample Point). Он включает в себя Prop_Seg и PHASE_SEG1 стандарта CAN. Его продолжительность программируется от 1 до 16 квантов времени.
CAN_BS2	Длина сегмента фазы 2	BS2 (Bit Segment 2) - определяет местоположение точки передачи. Он представляет собой PHASE_SEG2 стандарта CAN. Его продолжительность программируется от 1 до 8 квантов времени.
CAN_Prescaler	Множитель	Множитель, из значения которого рассчитывается размер кванта времени. Рассчитывается исходя от частоты работы периферии микроконтроллера.
Важно не путать с частотой работы самого контроллера!



*/
  /* CAN Baudrate = 500kbps*/
   // APB1 у нас работает на частоте 36МГц, - но  уверенности нет
 //Рекомендуется принимать следующие значения таймингов
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
  //1МБ
  	CAN_InitStructure.CAN_SJW = CAN_SJW_1tq;
    /* CAN Baudrate = 1MBps (CAN clocked at 36 MHz) */
    CAN_InitStructure.CAN_BS1 = CAN_BS1_9tq;
    CAN_InitStructure.CAN_BS2 = CAN_BS2_8tq;
    CAN_InitStructure.CAN_Prescaler = 2;
    CAN_Init(CAN1, &CAN_InitStructure);

    /*36/2 = 18    1/18000000 = 5,55 * 10 (-8) = 55,5 нс
		 1000/55,5 = 18,01



		ще раз пример расчета
     *
     * 	1. Выбираем скорость 1 Мбит . находим битовый интервал  1/1000000 = 0,000001 сек = 1 мкс.  = 1000нс.
     *	2. Устанавливаем делитель шины тактирования PCLK1 = 36МГц/4, т.е. BRP = 4, тогда время одного кванта получится около 111 нс.
     *	   Т.е. 36/4 = 9 МГц, 1/9000000 =  1,11 *10 (-7) или 111 нс  =  tq = 1Q (Время одного кванта). SYNC_SEG = 1TQ.
     *	3. Битовый интервал равен 1000нс/111нс = 9Q.
     *	4. SYNC_SEG  = 1TQ.
     *  5. (BS1+BS2) = 8TQ - смотри мануал
     *  6. Устанавливаем их значение по 4TQ.
     *  7.
		YNC_SEG всегда равен 1TQ, а сумма
	(BS1 + BS2) = 7...24TQ. При этом BS1 = 1..16TQ, BS2 = 1...8TQ.
	BRP - это делитель входной частоты. С помощью него мы получаем нужный битовый интервал.

	Тайминги
Синхронизация и тайминги в CAN — отдельный, важный и сложный вопрос. Однако, благодаря сложности и продуманности становится не так важна возможная рассинхронизация и нестабильность тактовых частот узлов сети, и связь становится возможной даже в тяжёлых условиях.



Всё время делится на кванты длиной t_q, и номинальная длительность бита равна 1+BS1+BS2 квантов. Захват значения бита происходит
на границе BS1 и BS2. В процессе приёма приёмник определяет, в какой из временных периодов произошёл перепад сигнала
(т.е. начало приёма нового бита). В норме перепад должен произойти на границе SYNC и BS1, если он произошёл раньше —
 контроллер уменьшает BS1, если позже — увеличивает BS2 на величину SJW (от 1 до 4 квантов времени). Таким образом, происходит
  постоянная пересинхронизация с частотой других приёмников.
  Необходимо чётко понимать этот процесс, потому что при правильной настройке он даёт отличные результаты захвата частоты передатчика. Рекомендуется принимать следующие значения таймингов:

SJW = 1
BS1 = 3-5
BS2 = 3-5
Конечно, конкретные значения зависят от частоты шины и частоты процессора, однако от них можно отталкиваться. Стоит начать с равенства периодов BS1 и BS2, а в случае ошибок приёма — пытаться их изменять.  К сожалению, для чёткой настройки в сложных случаях всё-таки понадобится осциллограф.

С другой стороны, при связи двух узлов сети можно просто принять одинаковые тайминги, и всё будет работать.

Для работы с автомобильной CAN-шиной на скорости 125 кбит/с и частотой процессора 42МГц я использовал прескалер величиной 12, SJW=1, BS1=3, BS2=3.

	*/
    //-------------------------------------------------------------------

  /*
   * Настройка фильтрации пакетов CAN
Все входящие сообщения проходят через фильтр. Фильтр можно настроить на отслеживание как какого-то конкретного сообщения так и группы - настройка маски. Благодаря этому уменьшается нагрузка на процессор, так как ему не приходится самому отфильтровывать ненужные сообщения — это делается на аппаратном уровне.
В рамках данной публикации я не буду подробно описывать настройку работы с фильтрами, это материал для отдельной статьи (см. Почтовые ящики. Фильтры пакетов CAN). Скажу лишь, что для начала достаточно установить фильтры без ограничений, а впоследствии можно самостоятельно их настроить  под свои требования.

Необходимо обратить внимание, что настройка фильтров и включение их обязательно, иначе Вы не сможет получать сообщение из шины.


скорость CAN: скорость=(частота шины)/((CAN_SJW{1...4}+CAN_BS1_{1...16}+CAN_BS2{1...8})* CAN_Prescaler{1...1024})
По осцилограаме скорость определяется время последнего бита= 1/Время последнего бита(us) получаем скорость в битах/с


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
 * Были проблемы с CAN на STM32F303. Не работал в режиме Normal, только LoopBack. Счас пофиксил. Не забывайте в прерывании сбрасывать его флаг!
 * Он не сбрасывается а только читается
 * void USB_LP_CAN1_RX0_IRQHandler(void)
{
  CanRxMsg RxMessage;
  memset(&RxMessage,0,sizeof(RxMessage));

  if (CAN_GetITStatus(CAN1,CAN_IT_FMP0))   CAN_GetITStatus(CAN1,CAN_IT_FF0)
  {
    CAN_ClearITPendingBit(CAN1,CAN_IT_FMP0); // ВОТ ТУТ!
    CAN_Receive(CAN1,CAN_FIFO0,&RxMessage);
    CAN_FIFORelease(CAN1,CAN_FIFO0);
  }
}
 *
 * Вобщем необходимо так
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
 *В конце прерывания по приёму, нужно очистить входной буфер.
Код:
CAN1 -> RF0R |= CAN_RF0R_RFOM0; /* Release FIFO 0 Output Mailbox * /
На ассемблере эта функция выглядит так:
Код:
   LDR R1, [R0, CAN_RF0R]
   ORR R1, CAN_RF0R_RFOM0
   STR R1, [R0, CAN_RF0R]
   RET
Нельзя сразу же RET использовать. Нужно дождаться, когда завершится очистка буфера. Это занимает 8 машинных циклов. Иначе по команде RET мы сразу же залетаем в новое прерывание. Итоговый рабочий код выглядит так:
Код:
   LDR R1, [R0, CAN_RF0R]
   ORR R1, CAN_RF0R_RFOM0
   STR R1, [R0, CAN_RF0R]

   Ждём_освобождение_RFOM0:
   LDR R1, [R0, CAN_RF0R]
   TST R1, CAN_RF0R_RFOM0
   BNE Ждём_освобождение_RFOM0

   RET
Теперь, как говорится, внимание вопрос - где нибудь об этом сказано? Нигде. Тут ещё ладно, можно самому догадаться. А есть и менее очевидные вещи. И таких граблей я уже с десяток накопал. Уже есть инициализация на пол-камня F4 :-)

И как я говорил ранее, очень странно, что приходится методом тыка догадываться, как в реальности работают эти камни. Разрабы фигнёй страдают. Вместо своих монструозных кошмарных cmsis, лучше бы сделали компактные фрагменты на ассемблере.

Ну ничего.. За 4 дня поднял CAN со всем фаршем, FSMC, и ч/б экранчик на D137000.
 *
 *
 *
 * *///CAN_IT_FF0
  // Прерывания на прием
	CAN_ITConfig(CAN1, CAN_IT_FF0/*CAN_IT_FMP0*/, ENABLE);  //Флаги CAN_IT_FMP0/1 являются readonly. Это счётчик принятых, но не обработанных кадров. Счётчик глубины занятости буфера.
	/* Transmit Structure preparation */
	  //TxMessage.StdId = 0x85;
	  //TxMessage.ExtId = 0x00;
	  //TxMessage.RTR = CAN_RTR_DATA;
	  //TxMessage.IDE = CAN_ID_STD;
	  //TxMessage.DLC = 6;
	//CAN_ITConfig(CAN1, CAN_IT_TME,  ENABLE);     // прерывания на передачу

	/*
	 *  Use CAN interrupts through the function CAN_ITConfig() at initialization
             phase and CAN_GetITStatus() function into interrupt routines to check
             if the event has occurred or not.
             After checking on a flag you should clear it using CAN_ClearFlag()
             function. And after checking on an interrupt event you should clear it
             using CAN_ClearITPendingBit() function.
	 *
	 *
Используйте CAN прерывания через функцию CAN_ITConfig () при инициализации
             фаза и CAN_GetITStatus () работают в подпрограммах прерывания для проверки
             произошло событие или нет.
             После проверки флага вы должны очистить его с помощью CAN_ClearFlag ()
             функция. И после проверки события прерывания вы должны очистить его
             используя функцию CAN_ClearITPendingBit ().
	 * */
	/*
	 * Прерывание на передачу не срабатывает, что я делаю не так?

Я делаю следующим образом:
1. Включаю прерывание CAN_ITConfig(CAN1, CAN_IT_TME, ENABLE);
2. Отправляю пустое (любое) сообщение :
Код:
void CAN_TxMessage(CanTxMsg TxMessage)
{
CAN_TransmitStatus(CAN1, 0);
CAN_Transmit(CAN1, &TxMessage);
CAN_TransmitStatus(CAN1, 0);
return;
}

Отправка пустого сообщения (не знаю как) (чудным образом) включает контроллер CAN (до этого никакой реакции нет).
3. Обработчик отправки выглядит так:
Код:
void USB_HP_CAN1_TX_IRQHandler(void)
{
  if (CAN_GetITStatus(CAN1,CAN_IT_TME))
  {
  CAN_ClearITPendingBit(CAN1,CAN_IT_TME);
  if (...) //если есть что отправлять - отправляю
   CAN_Transmit(CAN1, TxBuff);
  else
   CAN_ITConfig(CAN1, CAN_IT_TME,  DISABLE);  // Иначе отключаю прерывание.

  }
   return;
}

4. Теперь для отправки копирую сообщение в TxBuff и делаю CAN_ITConfig(CAN1, CAN_IT_TME, ENABLE);
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

	  UART_Init();//инициализируем


  /*!< At this stage the microcontroller clock setting is already configured, 
       this is done through SystemInit() function which is called from startup
       file (startup_stm32f30x.s) before to branch to application main.
       To reconfigure the default setting of SystemInit() function, refer to
       system_stm32f30x.c file
     */     
 //конфигуреруем порт для индикации
	GPIO_InitTypeDef gpio;  //структура
	RCC_AHBPeriphClockCmd( RCC_AHBPeriph_GPIOE, ENABLE );   // тактирование
	 GPIO_StructInit( &gpio );                              //объявляем и инициализируем
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
– LD1 (red) for 3.3 V power-on.
– LD2 (red/green) for USB communication.
– Eight user LEDs, LD3/10 (red), LD4/9 (blue), LD5/8 (orange) and LD6/7 (green)
   *
   *
   *
   * User LD3: red LED is a user LED connected to the I/O PE9 of the STM32F303VCT6.
• User LD4: blue LED is a user LED connected to the I/O PE8 of the STM32F303VCT6.
• User LD5: orange LED is a user LED connected to the I/O PE10 of the
STM32F303VCT6.
• User LD6: green LED is a user LED connected to the I/O PE15 of the
STM32F303VCT6.
• User LD7: green LED is a user LED connected to the I/O PE11 of the
STM32F303VCT6.
• User LD8: orange LED is a user LED connected to the I/O PE14 of the
STM32F303VCT6.
• User LD9: blue LED is a user LED connected to the I/O PE12 of the STM32F303VCT6.
• User LD10: red LED is a user LED connected to the I/O PE13 of the STM32F303VCT6.
   *
   *
   * */


  //if (TestRx !=  FAILED)
  //{ /* OK */

    /* Turn on LED1 */
  //  STM_EVAL_LEDOn(LED1);
	  //GPIO_SetBits( GPIOE, LED1 );   //синий     LD4    Pin 8 selected
  //}
  //else
  //{ /* KO */

    /* Turn on LED3 */
  //  STM_EVAL_LEDOn(LED3);
  // 	  GPIO_SetBits( GPIOE, LED2 );  // Pin 9 selected  красный - ошибка
  //}

  /* CAN transmit at 500Kb/s and receive by interrupt in loopback mode */
//	  TestRx = CAN_Interrupt();

  //if (TestRx !=  FAILED)
  //{ /* OK */

    /* Turn on LED4 */
  //  STM_EVAL_LEDOn(LED4);
  //	  GPIO_SetBits( GPIOE, LED3 );   // Pin 10 selected    оранжевый LD5
  //}
  //else
  //{ /* KO */

    /* Turn on LED2 */
  //  STM_EVAL_LEDOn(LED2);
	//  GPIO_SetBits( GPIOE, LED4 );
  //}
      // записываем формат json
  	  //jWriteTest();
  	  //считываем  размер буфера под форматом
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


      //порты под светоиоды сконфигурированы ранее
      UART4_Send_String ("AT\r\n");
      uart_wite_for("Ok");/*Ответ*/
      GPIOE->ODR|=GPIO_ODR_11;
      UART4_Send_String ("AT+CIPMODE=0\r\n");
      uart_wite_for("Ok");/*Ответ*/
      GPIOE->ODR&=~(0x100);
      delay1();
      UART4_Send_String ("AT+CIPMUX=1\r\n");
      uart_wite_for("Ok");/*Ответ*/
      GPIOE->ODR|=0x100;
      delay1();

      UART4_Send_String ("AT+CIPSERVER=1,8080\r\n");//Конфигурирует и запускает сервер на модуле ESP8266.
       delay1();
     UART4_Send_String ("AT+CIPSTO=5\r\n");/*Таймаут сервера, после его старта. Может быть установлен в диапазоне 0...7200 секунд.*/
     uart_wite_for("Ok");/*Ответ*/
     GPIOE->ODR|=0x100;
         delay1();
         //UART4_Send_String ("AT+CIPSEND=0,5\r\n");/* При одиночном соединении указывается только длина
            /* отправляемых байт. При множественном идентификатор и длина. Длина данных в пакете до 2048 байт.
             Между пакетами интервал 20мс. После получения данной команды модуль возвращает «>» и переходит в
             режим приема данных по RX, после приема данных необходимой длины передает их в радиоканал.
             При успешной передаче возвращает «SEND OK». Прервать режим приема данных и перейти в командный
             режим можно последовательностью «+++» */
             /*Ждем символ “>” И вводим данные. Мы здесь упускаем проверку на этот символ*/

             //UART4_Send_String ("stm32\r\n");
              /*Также упускаем проверку на SEND ОК*/
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
	            //UART4_Send_String(RXBuffer); //про

	            delay ();
	            delay ();



  }
}


void UART4_Send (char chr){
	while (!(UART4->ISR & USART_ISR_TC)); //сравниваем регистр статуса с разрядом установки флага об окончании прерывания
	UART4->TDR = chr;
}

void UART4_Send_String (char* str){
	uint8_t i = 0;
	while(str[i])
	UART4_Send (str[i++]);
}


void UART4_IRQHandler  (void){
	unsigned char data;
	if (UART4->ISR & USART_CR1_RXNEIE){//следим за состоянием даннго флага 1 - данные пришли, 0- пусто    Е
	    //UART4->ISR &= ~USART_CR1_RXNEIE; //обнулим данный бит
        data = UART4->RDR;
        //if (data == '1'){
        //    GPIOE->ODR|=GPIO_ODR_10;
        //}
        //if (data == '4'){
        //    GPIOE->ODR|=GPIO_ODR_11;
        //}
       // RX_getc(data);
        FLAG_REPLY=1;//флаг
        RXBuffer[RXi++]=data;
		GPIOE->ODR|=GPIO_ODR_9;
  }
}

// This function called from uart.c file (* - Make better if you wish)
//функция вызывается с библиотеки - получение символа
void RX_getc(unsigned char RXc) {
	if ((RXc != 255) && (RXc != 0) && (RXc != 10)) {//если код символа не равняется 255 , 0, 10
		if (RXc != 13) {     // а также 13     . Модулю необходимо для окончания ввода команды два символа /n and /r - их коды соответственно 13 и 10

			RXBuffer[RXi] = RXc; //зансим в буффер
			RXi++;          //инкремент

			if (RXi > RX_BUF_SIZE-1) {// если превыим размер буфффера то очистим его
				clear_RXBuffer();
			}
		}
		else { // если enter был то устанавливаем флаг окончания приема ответа и можем сравнивать строки в функции uart_wite_for
			FLAG_REPLY = 1;
		}
	}
}

void clear_RXBuffer(void)  //Здесь очистка буффера uart.
{
	for (RXi=0;RXi<RX_BUF_SIZE;RXi++)
		RXBuffer[RXi] = 0;//обнуляем
	RXi = 0; // устанавливаем в 0
	FLAG_REPLY = 0;//устанавливаем в 0
}

//функция ожидания символа. Она здесь пока не используется
//void uart_wite_char(char str) {
//	while (RXc != str)
//		RXc = uart_getc();

//}

void uart_wite_for(const char * str) { //функция ожидания ответа. Здесь мы дудем передавать в основном OK
	char result = 0;
	while (result == 0) {
		FLAG_REPLY = 0;
		clear_RXBuffer();
		while (FLAG_REPLY == 0) {// ждем пока прийдут все символы в функции RX_getc. Я пока его отключил
			//if (RXi>0) {FLAG_REPLY=1;
		//	}
		}

		result = strspn(RXBuffer, str);//поставил данную функцию, которое ищет вхождение str в RXBuffer. Дело в том, что (в моем случае) почемуто
		// приходит много не разделенного мусора и среди него ОК. Поэтому пришлось использовать ее заместь функии ниже
		//result = strstr(RXBuffer, str); //сравнение подстроки в строке
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


// превращаем в формат JSON, пока вручную забиваю данные
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
	 	resultCode=jwClose();   // ддля обнаруженеи ошибки, поа не использую
	//}
}


/**
  * @brief  Configures the CAN, transmit and receive by polling
  * @param  None
  * @retval PASSED if the reception is well done, FAILED in other case
  */
//Передача и получение по опросу
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
  //Передача
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
    //CAN_ClearITPendingBit(CAN1,CAN_IT_FF0); // ВОТ ТУТ!
    //CAN_Receive(CAN1,CAN_FIFO0,&RxMessage);
    //CAN_FIFORelease(CAN1,CAN_FIFO0);
  //}
//}
// закоментил void USB_LP_CAN1_RX0_IRQHandler(void) в stm32f30x_it , выдывало ошибку  [cc] collect2.exe: error: ld returned 1 exit status
void CAN1_TxDebugMessage(void)
{
  CanTxMsg TxMessage;

  /* CAN message to send */
  TxMessage.StdId = 0x85;
  TxMessage.ExtId = 0x00;
  TxMessage.RTR = CAN_RTR_DATA;
  TxMessage.IDE = CAN_ID_STD;
  TxMessage.DLC = 3;
  // На запись работает
  /*TxMessage.Data[0] = 0xe3;
  TxMessage.Data[1] = 0x22;
  TxMessage.Data[2] = 0x0;
  TxMessage.Data[3] = 0x80;
  TxMessage.Data[4] = 0x34;   //0x36 =  Lzad
  TxMessage.Data[5] = 0xf;
  TxMessage.Data[6] = 0x0;
  TxMessage.Data[7] = 0;*/
 //Команда на чтение
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
	/*memset – заполнения массива указанными символами.
	 * Синтаксис:

#include < string.h >
void *memset (void *destination, int c, size_t n);
	 * Аргументы:

destination – указатель на заполняемый массив
с – код символа для заполнения
n – размер заполняемой части массива в байтах
Возвращаемое значение:

Функция возвращает указатель на заполняемый массив.

Описание:

Функция memset заполняет первые n байт области памяти, на которую указывает аргумент destination,
 символом, код которого указывается аргументом c.

Пример:

В примере создается массив src, содержащий строку «123456789», затем первые 10 байт этого массива
 заполняются символом ‘1’ и массив src выводится на консоль.
	 * #include < stdio.h >  //Для printf
#include < string.h  >   //Для memset

int main (void)
{
   // Исходный массив
   unsigned char src[15]=”1234567890”;

   // Заполняем массив символом ‘1’
   memset (src, ‘1’, 10);

   // Вывод массива src на консоль
   printf (“src: %s\n”,src);

   return 0;
}
Результат:

Вывод в консоль:
src: 1111111111
	 *
	 *
	 * */

	if (CAN_GetITStatus(CAN1,CAN_IT_FF0))   //CAN_GetITStatus(CAN1,CAN_IT_FF0)
	{
		CAN_ClearITPendingBit(CAN1,CAN_IT_FF0); // ВОТ ТУТ!
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









 //Файлы var_tab                     cled_cnst

 /*"E",       &EDS_dop_kod,       "Ud",      &UdSr,    "T-fSec",  &Timer1_fSec,    "N#",      &Z_Skor,
  "N#R",     &ZISkor,          "N",       &Skor,       "Id#",     &OuRegS_dop_kod,  "Id#R",    &ZIDN,
  "Id",      &Id_dop_kod,       "Id2",     &Id2_dop_kod,     "Ids",     &Id_sr      ,     "Id2s",    &Id2_sr     ,
  "RDNout",  &RDN_Out,            "L#"    ,  &S.Alfa,             "L"     ,  &S.Alfa_Old,       "F#",      &V.Fv_zad,
   "F",       &V.Fv    ,     "If#",     &V.Iv_zad,   "If",      &V.Iv,  "M_Nvs" ,  &S.Most_Tir,
  "RVTout",  &S2.Out_rvt,    "dL2",     &S2.Alfa,         "M_Nvs2",  &S2.Most_Tir,
  "Lf#",     &V.Alfa,            "Lf",      &V.Alfa_Old,   "M"    ,   &Moment,  "Istat",   &Id_Stat_flt.out,
 "Idsum",   &Id_sum,
*/

/*Ниже стандартная таблица CAN + след с КТЕ
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
// Группа 2  - УПРАВЛЕНИЕ ПРИВОДОМ - 57 - контрольная строка
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
// Группа 3  - АНАЛОГОВЫЙ ВВОД-ВЫВОД - 90 - контрольная строка
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
// Группа 4  - ДИСКРЕТНЫЙ ВВОД-ВЫВОД - 123 - контрольная строка
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
  { "Po0",     &Po0_c.all,          1, _CHAR_type,   0,         _READ_access       }, // Po0_c.all изменяется в любой части программы . Po0_nm.all - не изменяется .
  { "r3_06",   (w*)&crezerv,        2, _SHORT_type,  0,         _READ_access       },
  { "AsPo0",   &PoS_c[0].all,       1, _CHAR_type,   0,         _READ_WRITE_access },
  { "AsPo1",   &PoS_c[1].all,       1, _CHAR_type,   0,         _READ_WRITE_access },
#endif //_CANOPEN
  _Var_tab_InsertSled ,
  _Var_tab_InsertSled2
} ;
 *
 *
 *,,,,,,,,Й!!!!!!!!!!!!!!!!!!!!!!!СЛЕД
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
    {"KpRS     ", &_r_KRSP ,_sl_form(2,0,2,3,1), 256        ,(w*)&_ind1_sld  ,"ед" },
#endif
    {"Kp-RS    ", &KRSP_ind    ,_sl_form(2,0,2,3,1), 256    ,(w*)&_ind1_sld  ,"ед" },
    {"Ti-RS    ", &KRSI_ind    ,_sl_form(2,0,2,3,1), 1      ,(w*)&_ind1_sld  ,"ms "},
    {"Kp-REDS  ", &Kp_REDS_ind ,_sl_form(2,0,2,3,1), 256    ,(w*)&_ind1_sld  ,"ед" },
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
    {"Счетч_T  ", (w*)&pdf_drv_data[0].Scht_T    , _sl_form(2,1,1,0,0), 1   , (w*)&_ind1_sld  , "h " },
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
  #else  // 590-й процессор :
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
   #else // 590-й процессор :
    {"Syn2apEPA", (w*)&IPCP5       ,_sl_form(2,1,1,0,0), 1         ,(w*)&_ind1_sld,"h " },
   #endif
 #endif
    {"Syn2_EPA ", (w*)&Syn2.NS_EPA ,_sl_form(2,1,1,0,0), 1         ,(w*)&_ind1_sld,"h " },
    {"Syn2_NS2 ", (w*)&Syn2.NS2    ,_sl_form(2,1,1,0,0), 1         ,(w*)&_ind1_sld,"h " },
    {"Tsyn2    ", &Tsyn2           ,_sl_form(2,1,1,0,0), 1         ,(w*)&_ind1_sld,"h " },
#endif
#ifdef _Rev_Pola   // если включен алгоритм реверса поля.
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
    {"I1pr-tir ", &Iproboy1    ,_sl_form(2,0,2,3,1), _Id_Nom ,(w*)&_ind100_sld,"% " },  // для помощи в определении пробитого тиристора
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
    {"EDS_P    ", &EDS_P           ,_sl_form(2,0,2,3,1), _EDS_Nom  ,(w*)&_ind100_sld,"% " }, // добавлено Головиным
    {"ZnEDS_P  ", &ZnEDS_P         ,_sl_form(1,1,1,0,0), 1         ,(w*)&_ind1_sld  ,"h " }   // для помощи в настр. ДЭДС
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







