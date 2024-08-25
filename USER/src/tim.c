#include "main.h"

_TIME _time = {0,0,0,0,0,0};

void tim3_delay_ms(u16 ms)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);
	
	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStruct.TIM_Period = ms * 84 - 1;
	TIM_TimeBaseInitStruct.TIM_Prescaler = 1000 - 1;
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStruct);
	
	TIM_SetCounter(TIM3, 0);
	TIM_UpdateDisableConfig(TIM3, DISABLE);
	TIM_ARRPreloadConfig(TIM3, DISABLE);
	TIM_UpdateRequestConfig(TIM3, TIM_UpdateSource_Global);
	TIM_ClearFlag(TIM3, TIM_FLAG_Update);
	TIM_Cmd(TIM3, ENABLE);
	while(TIM_GetFlagStatus(TIM3, TIM_FLAG_Update) == RESET);
	TIM_Cmd(TIM3, DISABLE);
}

void tim3_delay_s(u32 s)
{
	for(int i = 0;i <= s;i++)
	{
		tim3_delay_ms(500);
	}
}

void tim3_delay_us(u32 us)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);
	
	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStruct.TIM_Period = us * 84 - 1;
	TIM_TimeBaseInitStruct.TIM_Prescaler = 1 - 1;
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStruct);
	
	TIM_SetCounter(TIM3, 0);
	TIM_UpdateDisableConfig(TIM3, DISABLE);
	TIM_ARRPreloadConfig(TIM3, DISABLE);
	TIM_UpdateRequestConfig(TIM3, TIM_UpdateSource_Global);
	TIM_ClearFlag(TIM3, TIM_FLAG_Update);
	TIM_Cmd(TIM3, ENABLE);
	while(TIM_GetFlagStatus(TIM3, TIM_FLAG_Update) == RESET);
	TIM_Cmd(TIM3, DISABLE);
}

void tim6_init(u16 arr,u16 psc)
{
	NVIC_InitTypeDef NVIC_InitStruct;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6,ENABLE);
	
	TIM_TimeBaseInitStruct.TIM_Period = arr - 1;
	TIM_TimeBaseInitStruct.TIM_Prescaler = psc - 1;
	TIM_TimeBaseInit(TIM6, &TIM_TimeBaseInitStruct);
	
	TIM_SetCounter(TIM6, 0);
	TIM_UpdateDisableConfig(TIM6, DISABLE);
	TIM_ARRPreloadConfig(TIM6, DISABLE);
	TIM_UpdateRequestConfig(TIM6, TIM_UpdateSource_Global);
	TIM_ClearFlag(TIM6, TIM_FLAG_Update);
	TIM_Cmd(TIM6, ENABLE);
	
	TIM_ITConfig(TIM6, TIM_IT_Update, ENABLE);
	
	NVIC_SetPriorityGrouping(NVIC_PriorityGroup_2);
	NVIC_InitStruct.NVIC_IRQChannel = TIM6_DAC_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 3;
	NVIC_Init(&NVIC_InitStruct);
}

void TIM6_DAC_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM6, TIM_IT_Update) ==SET)
	{
		TIM_ClearITPendingBit(TIM6, TIM_IT_Update);
		//SentHeart();
	}
}