
/**
  ******************************************************************************
  * @file    main.c
  * @author  Ac6
  * @version V1.0
  * @date    01-December-2013
  * @brief   Default main function.
  ******************************************************************************
*/

#define _DEFAULT_SOURCE
#define _USE_MATH_DEFINES
#include <cmath>
#include <string>
#include <charconv>

#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"

#include <sequence.hpp>

constexpr int gcc_version = __GNUC__ ;
constexpr int gcc_minorVersion = __GNUC_MINOR__;

UART* uart_ = nullptr;

void a(const CanRxMsg& receiveRx)
{
	if(receiveRx.Data[0] == 0x00 && receiveRx.Data[1] == 0x27)
	{
		/*
		const int16_t x_value = (uint16_t)receiveRx.Data[3] << 8 | (uint16_t)receiveRx.Data[2];
		const int16_t y_value = (uint16_t)receiveRx.Data[5] << 8 | (uint16_t)receiveRx.Data[4];

		char sendChar_x[10] = {};
		char sendChar_y[10] = {};
		auto sendChar_x_begin = std::begin(sendChar_x);
		auto sendChar_x_end = std::end(sendChar_x);
		auto sendChar_y_begin = std::begin(sendChar_y);
		auto sendChar_y_end = std::end(sendChar_y);

		std::to_chars(sendChar_x_begin, sendChar_x_end, x_value);
		std::to_chars(sendChar_y_begin, sendChar_y_end, y_value);

		uart_->TransmitData(sendChar_x, 10);
		uart_->TransmitData(',');
		uart_->TransmitData(' ');
		uart_->TransmitData(sendChar_y, 10);
		uart_->TransmitData('\n');
		*/
	}
}

void clockInit();

int main(void)
{
	clockInit();
	systick::init();
	ControlAreaNetwork::config(OwnAddress);

	CAN_intrrupt::additionCallFunction(a);
	//IO_sigPins<ioName::sig1, ioState::input> input1;

	/*configuration uart1.*/
	uart_ = new UART(UART::name::uart1, 9600);

	//uartSendString = [&](const std::string& sendString){ uart1.TransmitData(sendString); };

	Sequence sequence;

	while(true)
	{

	}
	return 0;
}

void clockInit()
{
	/*
	 * PLLM = 16, PLLN = 168, PLLP = 2, PLLQ = 8
	 * HSI clock is 16MHz. This is constant value.
	 * Expression of System clock is	: HSI x PLLN / (PLLM x PLLP) = 84MHz
	 * Expression of HCLK clock is		: SystemClock / HCLK_prescaler = 84MHz
	 * Expression of PCLK1 clock is		: HCLK / PCLK1_prescaler = 42MHz
	 * Expression of PCLK2 clock is		: HCLK / PCLK2_prescaler = 84MHz
	 */
	RCC_DeInit();

	FLASH_SetLatency(FLASH_Latency_2);

	RCC_HCLKConfig(RCC_SYSCLK_Div1);					// HCLK prescaler is 1.

	RCC_PCLK1Config(RCC_HCLK_Div2);						// PCLK1 prescaler is 2.
	RCC_PCLK2Config(RCC_HCLK_Div1);						// PCLK2 prescaler is 1.

	RCC_PLLCmd(DISABLE);
	RCC_PLLConfig(RCC_PLLSource_HSI, 16, 168, 2, 8);
	RCC_PLLCmd(ENABLE);
	while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)

	RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

	while(RCC_GetSYSCLKSource() != 0x08);
}
