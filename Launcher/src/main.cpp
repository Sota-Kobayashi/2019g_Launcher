
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

#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"

#include <motor.hpp>
#include <electric_valve.hpp>
#include <led.hpp>
#include <rotary_encoder.hpp>
#include <uart.hpp>
#include <speed_pid.hpp>
#include <functional>
#include <vector>
#include <interrupt.hpp>
#include <board_io.hpp>
#include <motor_driver.hpp>
#include <can.hpp>
#include <position_pid.hpp>
#include <launcher.hpp>
#include <move_angle.hpp>

constexpr int gcc_version = __GNUC__ ;
constexpr int gcc_minorVersion = __GNUC_MINOR__;

constexpr uint8_t MainBoardAddress	= 0x00;
constexpr uint8_t OwnAddress		= 0x01;

void clockInit();

class Sequence : private MoveAngle, private Launcher
{
public:
	Sequence()
	{
		systick::additionCallFunction( [&]{ update(); } );
		CAN_intrrupt::additionCallFunction( [&](const CanRxMsg& receiveData){ orderReceive_(receiveData); } );
	}
	virtual ~Sequence(){}
private:
	enum class sequenceName
	{
		start,

		setGettingAngle,
		waitSettingGetttingAngle,

		expendArm,
		waitExpendingArm,

		fallArm,
		waitFallingArm,

		getShagai,
		waitGettingShagai,

		liftArm,
		waitLiftingArm,

		shortenArm,
		waitShortenningArm,

		setThrowingAngle,
		waitSettingThrowingAngle,

		openArm,
		waitOpennningArm,

		throwShagai,
		waitThrowingShagai
	};
	enum class receiveOrderFormat : uint8_t
	{
		start = 0x01,
		fallArm = 0x02,
		getShagai = 0x03,
		throwShagai = 0x04
	};
	enum class compliteCmdFormat : uint8_t
	{
		standbyGettingShagai = 0x01,
		gettigShagai = 0x02,
		throwingComplite = 0x03
	};

	sequenceName nowSequence_ = sequenceName::start;
	receiveOrderFormat receiveCmd_ = receiveOrderFormat::start;

	EV eleValve;

	static constexpr EV::name expendArmValve = EV::name::EV0;
	static constexpr EV::name fallArmValve = EV::name::EV1;
	static constexpr EV::name holdShagaiValve = EV::name::EV2;

	virtual void update() final
	{
		sequenceUpdate_();
		MoveAngle::update();
		Launcher::update();
	}
	void sequenceUpdate_();
	void sendConpliteCmd_(const compliteCmdFormat sendCmd);
	void orderReceive_(const CanRxMsg& receiveData);
};




int main(void)
{
	clockInit();
	systick::init();
	ControlAreaNetwork::Config(OwnAddress);

	IO_sigPins<ioName::sig1, ioState::input> input1;

	UART uart1(UART::name::uart1, 115200);
	uartSendString = [&](const std::string& sendString){ uart1.TransmitData(sendString); };

	Sequence sequence;

	while(true);
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

void Sequence::sequenceUpdate_()
{
	static uint16_t waitCount = 0;

	/*This lambda function returns true if it has to wait. */
	auto waiting = [&]	{
							if(waitCount > 0)waitCount--;
							return waitCount == 0 ? false : true;
						};

	//receiveCmd_ = receiveOrderFormat::throwShagai;

	switch(nowSequence_)
	{
	case sequenceName::start :
		MoveAngle::setTargetMovePosition(MoveAngle::movePositions::upPosition);
		Launcher::setLauncherSequence(Launcher::launcherSequence::returnZeroPoint);
		eleValve.setNewState(EV::state::Reset, expendArmValve, fallArmValve);
		eleValve.setNewState(EV::state::Set, holdShagaiValve);
		if(receiveCmd_ != receiveOrderFormat::start && Launcher::isGotZeroPoint && MoveAngle::isGotZeroPoint)
		{
			nowSequence_ = sequenceName::setGettingAngle;
		}
		break;

	case sequenceName::setGettingAngle :
		MoveAngle::setTargetMovePosition(MoveAngle::movePositions::downPosition);
		Launcher::setLauncherSequence(Launcher::launcherSequence::returnZeroPoint);
		if(MoveAngle::isAnglePID_OK())
		{
			waitCount = 200;
			nowSequence_ = sequenceName::waitSettingGetttingAngle;
		}
		break;


	case sequenceName::waitSettingGetttingAngle :
		if(!waiting())nowSequence_ = sequenceName::expendArm;
		break;

	case sequenceName::expendArm:
		eleValve.setNewState(EV::state::Set, expendArmValve);
		waitCount = 1000;
		nowSequence_ = sequenceName::waitExpendingArm;
		break;

	case sequenceName::waitExpendingArm:
		if(!waiting())nowSequence_ = sequenceName::fallArm;
		break;

	case sequenceName::fallArm:
		eleValve.setNewState(EV::state::Set, fallArmValve);
		waitCount = 1000;
		nowSequence_ = sequenceName::waitFallingArm;
		break;

	case sequenceName::waitFallingArm:
		if(!waiting())
		{
			sendConpliteCmd_(compliteCmdFormat::standbyGettingShagai);
			if(receiveCmd_ == receiveOrderFormat::getShagai || receiveCmd_ == receiveOrderFormat::throwShagai)
			{
				nowSequence_ = sequenceName::getShagai;
			}
		}
		break;

	case sequenceName::getShagai:
		eleValve.setNewState(EV::state::Reset, holdShagaiValve);
		waitCount = 1000;
		nowSequence_ = sequenceName::waitGettingShagai;
		break;

	case sequenceName::waitGettingShagai:
		if(!waiting())nowSequence_ = sequenceName::liftArm;
		break;

	case sequenceName::liftArm:
		eleValve.setNewState(EV::state::Reset, fallArmValve);
		waitCount = 1000;
		nowSequence_ = sequenceName::waitLiftingArm;
		break;

	case sequenceName::waitLiftingArm:
		if(!waiting())nowSequence_ = sequenceName::shortenArm;
		break;

	case sequenceName::shortenArm:
		eleValve.setNewState(EV::state::Reset, expendArmValve);
		waitCount = 1000;
		nowSequence_ = sequenceName::waitShortenningArm;
		break;

	case sequenceName::waitShortenningArm:
		if(!waiting())nowSequence_ = sequenceName::setThrowingAngle;
		break;

	case sequenceName::setThrowingAngle:
		MoveAngle::setTargetMovePosition(MoveAngle::movePositions::throwingPosition);
		if(MoveAngle::isAnglePID_OK())
		{
			waitCount = 200;
			nowSequence_ = sequenceName::waitSettingThrowingAngle;
		}
		break;

	case sequenceName::waitSettingThrowingAngle:
		if(!waiting())
		{
			sendConpliteCmd_(compliteCmdFormat::gettigShagai);
			if(receiveCmd_ == receiveOrderFormat::throwShagai)nowSequence_ = sequenceName::openArm;
		}
		break;

	case sequenceName::openArm:
		eleValve.setNewState(EV::state::Set, holdShagaiValve);
		waitCount = 500;
		nowSequence_ = sequenceName::waitOpennningArm;
		break;

	case sequenceName::waitOpennningArm:
		if(!waiting())
		{
			Launcher::setLauncherSequence(Launcher::launcherSequence::launch);
			nowSequence_ = sequenceName::throwShagai;
		}
		break;

	case sequenceName::throwShagai:
		if(Launcher::nowSequence == Launcher::launcherSequence::returnZeroPoint)
		{
			waitCount = 500;
			nowSequence_ = sequenceName::waitThrowingShagai;
		}
		break;

	case sequenceName::waitThrowingShagai:
		if(!waiting())
		{
			sendConpliteCmd_(compliteCmdFormat::throwingComplite);
			nowSequence_ = sequenceName::start;
			receiveCmd_ = receiveOrderFormat::start;
		}
		break;
	}
}
void Sequence::sendConpliteCmd_(const compliteCmdFormat sendCmd)
{
	const std::array<uint8_t, 2> sendData = { OwnAddress, (uint8_t)sendCmd };
	ControlAreaNetwork::SendData(sendData, MainBoardAddress);
}
void Sequence::orderReceive_(const CanRxMsg& receiveData)
{
	if(receiveData.Data[0] == MainBoardAddress) receiveCmd_ = (receiveOrderFormat)receiveData.Data[1];
}
