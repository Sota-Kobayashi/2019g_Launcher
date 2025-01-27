#include "motor.hpp"

MotorControl::MotorControl()
{
	GPIO_PinsInit_();
	timerInit_();
	gateEnablePinInit_();
}

MotorControl::MotorControl(const uint16_t resolution, const uint32_t frequency)
{
	GPIO_PinsInit_();
	timerInit_(resolution, frequency);
	gateEnablePinInit_();
}

void MotorControl::setSpeed(const int32_t setValue)
{
	if(std::abs(setValue) > nowResolution_)return;
	if(nowDriveMode_ == driveMode::SMB)
	{
		const uint32_t setValue_abs = (uint32_t)std::abs(setValue);

		if(setValue > 0)
		{
			TIM_SetCompare1(TIM1, setValue_abs);
			TIM_SetCompare2(TIM1, nowResolution_);
		}
		else
		{
			TIM_SetCompare1(TIM1, 0);
			TIM_SetCompare2(TIM1, nowResolution_ - setValue_abs);
		}
	}
	else
	{
		const uint32_t setValue_LAP = (uint32_t)std::round(((float)setValue + (float)nowResolution_) / 2.0f);

		TIM_SetCompare1(TIM1, setValue_LAP);
		TIM_SetCompare2(TIM1, setValue_LAP);
	}
	const std::string sendString =
				"Duty : " + std::to_string(setValue) + '\n';
	//uartSendString(sendString);
}

void MotorControl::setSpeed(const int32_t setValue, const driveMode setMode)
{
	setDriveMode(setMode);
	setSpeed(setValue);
}

MotorControl::~MotorControl()
{

}

constexpr uint16_t MotorControl::prescaler_
	(const uint16_t resolution, const uint32_t pwmFreq, const uint32_t timerFrequency)
{
	return (uint16_t)(timerFrequency / ((uint32_t)pwmFreq * (uint32_t)resolution));
}

void MotorControl::GPIO_PinsInit_()
{

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_StructInit(&GPIO_InitStruct);
	GPIO_InitStruct.GPIO_Pin	= GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9;
	GPIO_InitStruct.GPIO_Mode	= GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_PuPd	= GPIO_PuPd_DOWN;
	GPIO_InitStruct.GPIO_OType	= GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Speed	= GPIO_Speed_100MHz;
	GPIO_Init(GPIOA, &GPIO_InitStruct);

	GPIO_InitStruct.GPIO_Pin	= GPIO_Pin_0;
	GPIO_Init(GPIOB, &GPIO_InitStruct);

	GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_TIM1);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource8, GPIO_AF_TIM1);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_TIM1);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource0, GPIO_AF_TIM1);
}

void MotorControl::timerInit_
	(const uint16_t resolution, const uint32_t pwmFreq)
{
	nowResolution_ = resolution;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1 , ENABLE);

	RCC_ClocksTypeDef RCC_ClocksStruct;
	RCC_GetClocksFreq(&RCC_ClocksStruct);
	const uint32_t nowTimer1Clock = RCC_ClocksStruct.PCLK2_Frequency;

	TIM_TimeBaseInitTypeDef timerBaseInitStructure;
	TIM_TimeBaseStructInit(&timerBaseInitStructure);
	timerBaseInitStructure.TIM_Period			= resolution - 1;
	timerBaseInitStructure.TIM_Prescaler		= (uint16_t)(prescaler_(resolution, pwmFreq, nowTimer1Clock) - 1);
	timerBaseInitStructure.TIM_ClockDivision 	= TIM_CKD_DIV4;
	TIM_TimeBaseInit(TIM1 , &timerBaseInitStructure);


	TIM_Cmd(TIM1 , ENABLE);

	TIM_CtrlPWMOutputs(TIM1 , ENABLE);

	TIM_OCInitTypeDef timerOCInitStructure;
	timerOCInitStructure.TIM_OCMode			= TIM_OCMode_PWM1;
	timerOCInitStructure.TIM_OutputState	= TIM_OutputState_Enable;
	timerOCInitStructure.TIM_OutputNState	= TIM_OutputNState_Enable;
	timerOCInitStructure.TIM_Pulse			= 0x0000;
	timerOCInitStructure.TIM_OCPolarity		= TIM_OCPolarity_High;
	timerOCInitStructure.TIM_OCNPolarity	= TIM_OCNPolarity_High;
	timerOCInitStructure.TIM_OCIdleState	= TIM_OCIdleState_Set;
	timerOCInitStructure.TIM_OCNIdleState	= TIM_OCNIdleState_Set;
	TIM_OC1Init(TIM1 , &timerOCInitStructure);
	TIM_OC2Init(TIM1 , &timerOCInitStructure);

	TIM_OC1PreloadConfig(TIM1 , TIM_OCPreload_Enable);
	TIM_OC2PreloadConfig(TIM1 , TIM_OCPreload_Enable);

	TIM_BDTRInitTypeDef TIM_BDTRInitStructure;
	TIM_BDTRStructInit(&TIM_BDTRInitStructure);
	TIM_BDTRInitStructure.TIM_DeadTime			= 7;
	TIM_BDTRInitStructure.TIM_AutomaticOutput	= TIM_AutomaticOutput_Enable;

	TIM_BDTRConfig(TIM1 , &TIM_BDTRInitStructure);
}
void MotorControl::gateEnablePinInit_()
{
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_StructInit(&GPIO_InitStruct);
	GPIO_InitStruct.GPIO_Pin	= GPIO_Pin_5;
	GPIO_InitStruct.GPIO_Mode	= GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_PuPd	= GPIO_PuPd_NOPULL;
	GPIO_InitStruct.GPIO_OType	= GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Speed	= GPIO_Speed_100MHz;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
}
