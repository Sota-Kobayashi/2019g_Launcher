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

class MoveAngle : private PositionPID
{
public:
	enum class movePositions : int32_t
	{
		downPosition		= 0,
		throwingPosition	= 1,
		upPosition			= 2
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
	static constexpr int32_t driveSpeed_ = 90;

	static constexpr PositionPID_initStructType initStruct_
	{
		{0.0, 0.0, 0.0},
		encoderName::RotEnc2,
		8192,
		0x10,
		100,
		10
	};

	bool isGotZeroPoint_ = false;

	IO_sigPins<ioName::sig1, ioState::input, pinPullDirection::no> zeroPoint;

	IO_sigPins<ioName::sig2, ioState::input, pinPullDirection::up> upSig;
	IO_sigPins<ioName::sig3, ioState::input, pinPullDirection::up> downSig;

	static constexpr int32_t getZeroPointSpeed = 100;

	void switchControl_();
	inline void getZeroPoint_(){ PositionPID::SetSpeed(getZeroPointSpeed, MotorDriver::MotorDriveMode::PID); }
	void zeroPointIntrrupt_();
	void moveAngle_Update();
};

#endif /* MYHEADERS_MOVE_ANGLE_HPP_ */
