/*
 * move_angle.hpp
 *
 *  Created on: 2019/04/06
 *      Author: User
 */

#ifndef MYHEADERS_MOVE_ANGLE_HPP_
#define MYHEADERS_MOVE_ANGLE_HPP_

#include <functional>

#include <stm32f4xx.h>
#include <stm32f4xx_conf.h>

#include <interrupt.hpp>
#include <position_pid.hpp>
#include <board_io.hpp>
#include <led.hpp>
#include <emergencty.hpp>
#include <uart.hpp>

class MoveAngle : private PositionPID
{
public:
	enum class movePositions : int32_t
	{
		downPosition		= -1000,
		throwingPosition	= -500000,
		upPosition			= -1000000
	};
	MoveAngle() : PositionPID(initStruct_)
	{
		EXTI1Intrrupt::init(0);
		EXTI1Intrrupt::additionCallFunction( [&]{ zeroPointIntrrupt_(); } );
	}

	const bool& isGotZeroPoint = isGotZeroPoint_;

	inline bool isAnglePID_OK(){ return PositionPID::errorCheck(); }

	virtual void update() override
	{
		moveAngle_Update();
		PositionPID::update();
	}

	inline void setTargetMovePosition(const movePositions setPosition)
			{ PositionPID::setTargetPosition((int32_t)setPosition); }

private:
	static constexpr uint8_t useMotorDriverAdd_ = 0x10;
	static constexpr int32_t driveSpeed_ = 1000;
	static constexpr int32_t getZeroPointSpeed = 100;

	LED<ledColor::Red> PositionOK;

	const PositionPID_initStructType initStruct_
	{
		{0.100f, 0.0f, 0.070f},

		encoderName::RotEnc2,
		0x10,
		100,
		1200,

		{0.05f, 0.005f, 0.005f, 10000, 8192}
	};

	bool isGotZeroPoint_ = false;

	IO_sigPins<ioName::sig1, ioState::input, pinPullDirection::no> zeroPoint;

	IO_sigPins<ioName::sig2, ioState::input, pinPullDirection::up> upSig;
	IO_sigPins<ioName::sig3, ioState::input, pinPullDirection::up> downSig;

	void switchControl_();
	void getZeroPoint_();
	void zeroPointIntrrupt_();
	void moveAngle_Update();
};

#endif /* MYHEADERS_MOVE_ANGLE_HPP_ */
