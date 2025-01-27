/*
 * launcher.hpp
 *
 *  Created on: 2019/04/06
 *      Author: User
 */

#ifndef MYHEADERS_LAUNCHER_HPP_
#define MYHEADERS_LAUNCHER_HPP_

#include <functional>

#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"

#include <speed_pid.hpp>
#include <board_io.hpp>
#include <interrupt.hpp>
#include <led.hpp>
#include <emergencty.hpp>



class Launcher
{
public:
	enum class launcherSequence : uint8_t
	{
		launch,
		breaking,
		returnZeroPoint
	};

	Launcher() : launchMotorPID(encoderName::RotEnc1, motorPPR_, useGain_, 0.001)
	{

		launchMotorPID.setNewStateGate(MotorControl::gateEnableState::Enable);
		launchMotorPID.setCount((uint32_t)defaultEncoderCount);

		EXTI0Intrrupt::init(0);

		EXTI0Intrrupt::additionCallFunction( [&]{ limitSensorIntrrupt(); } );

		launchMotorPID.setEnableState(true);
	}
	virtual void update()
	{
		/*
		std::string sendStr = std::to_string(Speed_pid::readPositionCount<uint32_t>()) + '\n';
		uartSendString(sendStr);
		sendStr.clear();
		*/
		launcherUpdate();
		launchMotorPID.update();

	}
	const bool& isGotZeroPoint = isGotZeroPoint_;

	inline void setLauncherSequence(const launcherSequence setSequence){ nowSequence_ = setSequence; }
	const launcherSequence& nowSequence = nowSequence_;

	virtual ~Launcher(){ }
private:
	Speed_pid launchMotorPID;

	void throwingShagai();

	launcherSequence nowSequence_ = launcherSequence::returnZeroPoint;

	static constexpr PIDgain_speed useGain_ = {-0.035, -0.0008, -0.002};
	static constexpr uint16_t motorPPR_ = 2048 * 4;

	static constexpr uint32_t defaultEncoderCount = 55000;
	static constexpr uint32_t encCount_stopPoint = 10000;

	/*members to get zero point*/
	static constexpr double getZeroPoint_motorDriveSpeed_ = -400;
	static constexpr double throwing_motorDriveSpeed_ = -4160;
	static constexpr int32_t brakeDuty = -70;
	bool isGotZeroPoint_ = false;

	LED<ledColor::Yellow> launchSign;
	LED<ledColor::Orange> brakingSign;

	IO_sigPins<ioName::sig0, ioState::input> zeroPointLimit;
	void getZeroPoint();
	void limitSensorIntrrupt();
	void launcherUpdate();
};



#endif /* MYHEADERS_LAUNCHER_HPP_ */
