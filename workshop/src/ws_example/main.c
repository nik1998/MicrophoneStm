#include "main.h"

#define SWITCH_DELAY 500000
#define LED1_PIN GPIO_Pin_12
#define LED2_PIN GPIO_Pin_13
#define LED3_PIN GPIO_Pin_14
#define LED4_PIN GPIO_Pin_15

void MyMicrophone();
int main(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;

 // RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
//button interrupt
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
   SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE,EXTI_PinSource0);

  //Init Leds 
 // GPIO_PinAFConfig(GPIOA,GPIO_PinSource8,GPIO_AF_TIM1);
  GPIO_InitStructure.GPIO_Pin= GPIO_Pin_8;//|GPIO_Pin_9|GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  /*//Timer
  TIM_TimeBaseInitTypeDef ttt;
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
  ttt.TIM_Period=1000000-1;
  ttt.TIM_Prescaler=84-1;
  ttt.TIM_ClockDivision=0;
  ttt.TIM_CounterMode=TIM_CounterMode_Up;
  TIM_TimeBaseInit(TIM2,&ttt);
  TIM_ITConfig(TIM2, TIM_IT_Update,ENABLE);
  TIM_Cmd(TIM2,ENABLE);
  //Timer vector
  NVIC_InitTypeDef nv;
  nv.NVIC_IRQChannel=TIM2_IRQn;
  nv.NVIC_IRQChannelPreemptionPriority=0x00;
  nv.NVIC_IRQChannelSubPriority=0x01;
  nv.NVIC_IRQChannelCmd=ENABLE;
  NVIC_Init(&nv);*/

 //button
 GPIO_InitTypeDef GPIO_In;
GPIO_In.GPIO_Pin= GPIO_Pin_0;
  GPIO_In.GPIO_Mode = GPIO_Mode_IN;
  GPIO_In.GPIO_OType = GPIO_OType_PP;
  GPIO_In.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_In.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOE, &GPIO_In);
 //interrupt
   EXTI_InitTypeDef eee;
   eee.EXTI_Line=EXTI_Line0;
   eee.EXTI_LineCmd=ENABLE;
   eee.EXTI_Mode=EXTI_Mode_Interrupt;
   eee.EXTI_Trigger=EXTI_Trigger_Rising;
   EXTI_Init(&eee);

   //vector
  NVIC_InitTypeDef nvec;
  nvec.NVIC_IRQChannel=EXTI0_IRQn;
  nvec.NVIC_IRQChannelPreemptionPriority=0x00;
  nvec.NVIC_IRQChannelSubPriority=0x01;
  nvec.NVIC_IRQChannelCmd=ENABLE;
  NVIC_Init(&nvec);
  GPIO_SetBits(GPIOA,GPIO_Pin_10|GPIO_Pin_9|GPIO_Pin_8);

  GPIO_InitTypeDef gpio;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOC,ENABLE);

  gpio.GPIO_Mode=GPIO_Mode_AF;
  gpio.GPIO_OType=GPIO_OType_PP;
  gpio.GPIO_PuPd=GPIO_PuPd_NOPULL;
  gpio.GPIO_Speed=GPIO_Speed_50MHz;

  gpio.GPIO_Pin=GPIO_Pin_10;
  GPIO_Init(GPIOB,&gpio);
  GPIO_PinAFConfig(GPIOB,GPIO_PinSource10, GPIO_AF_SPI2);

  gpio.GPIO_Pin=GPIO_Pin_3;
  GPIO_Init(GPIOC,&gpio);
  GPIO_PinAFConfig(GPIOC,GPIO_PinSource3, GPIO_AF_SPI2);

  MyMicrophone();
  while (1)
  {    
  }
}
int ok=1;
int co=0;
 int ind=3;
/*void TIM2_IRQHandler(void)
{
  if(ok==0)
  {
    co++;
  }
  if(co==2)
  {
    co=0;
    ok=1;
  }
  if(TIM_GetITStatus(TIM2,TIM_IT_Update)!=RESET)
  {
    TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
  }
}*/
void EXTI0_IRQHandler(void)
{
  if(ok==1)
  if(EXTI_GetITStatus(EXTI_Line0)!=RESET)
  {
    GPIO_ResetBits(GPIOA,GPIO_Pin_8);
    EXTI_ClearITPendingBit(EXTI_Line0);
  }
}
//void AUDIO_REC_SPI_IRQHANDLER(void)
void DMA1_Stream3_IRQHandler(void)
{
 // if(SPI_GetITStatus(SPI2, SPI_I2S_IT_RXNE)!= RESET)
  if(DMA_GetFlagStatus(DMA1_Stream3, DMA_FLAG_TCIF3)!=RESET)
  {
    GPIO_ResetBits(GPIOA,GPIO_Pin_8);
    DMA_ClearFlag(DMA1_Stream3, DMA_FLAG_TCIF3);
  }
}
void MyMicrophone()
{ 
  //nvic
  NVIC_InitTypeDef nvic;
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_3);
  nvic.NVIC_IRQChannel = DMA1_Stream3_IRQn;
  nvic.NVIC_IRQChannelPreemptionPriority=1;
  nvic.NVIC_IRQChannelSubPriority=0;
  nvic.NVIC_IRQChannelCmd=ENABLE;
  NVIC_Init(&nvic);

  //I2C
  I2S_InitTypeDef i2struct;
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2,ENABLE);
  SPI_I2S_DeInit(SPI2);
  i2struct.I2S_AudioFreq=HSE_VALUE/2;
  i2struct.I2S_Standard= I2S_Standard_LSB;
  i2struct.I2S_DataFormat=I2S_DataFormat_16b;
  i2struct.I2S_CPOL= I2S_CPOL_High;
  i2struct.I2S_Mode=I2S_Mode_MasterRx;
  i2struct.I2S_MCLKOutput =I2S_MCLKOutput_Disable;
  I2S_Init(SPI2,&i2struct);
  SPI_I2S_ITConfig(SPI2,SPI_I2S_IT_RXNE, ENABLE);
  SPI_I2S_DMACmd(SPI2, SPI_I2S_DMAReq_Rx, ENABLE);
  //DMA
 // DMA_InitTypeDef dmastruct

 /* RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1,ENABLE);
  DMA_Cmd(DMA1_Stream3,DISABLE);

  dmastruct.DMA_Channel=DMA_Channel_0;
  dmastruct.DMA_PeripheralBaseAddr =SPI2_DR_ADRESS;
  dmastruct.DMA_Memory0BaseAddr=(uint32_t)Mic_DMA_PDM_Buffer0;
  dmastruct.DMA_DIR=DMA_DIR_PeripheralToMemory;
  dmastruct.DMA_BufferSize=(uint32_t)INTERNAL_BUFF_SIZE;
  dmastruct.DMA_PeripheralInc=DMA_PeripheralInc_Disable;
  dmastruct.DMA_MemoryInc=DMA_MemoryInc_Enable;
  dmastruct.DMA_Mode=DMA_Mode_Circular;
  dmastruct.DMA_Priority=DMA_Priority_High;
  dmastruct.DMA_FIFOMode=DMA_FIFOMode_Disable;
  dmastruct.DMA_FIFOThreshold=DMA_FIFOThreshold_1QuarterFull;
  dmastruct.DMA_MemoryBurst = DMA_MemoryBurst_Single;
  dmastruct.DMA_PeripheralBurst=DMA_PeripheralBurst_Single;
  //DMA_Init(DMA1_Stream3, &dmastruct);
 // DMA_DoubleBufferModeConfig(DMA1_Stream3, (uint32_t) &Mic_DMA_PDM_Buffer1,DMA_Memory_0);*/
  //DMA_Cmd(DMA1_Stream3,ENABLE);
}



