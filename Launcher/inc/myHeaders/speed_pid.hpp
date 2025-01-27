/*
 * speed_pid.hpp
 *
 *  Created on: 2019/03/31
 *      Author: User
 */

#ifndef MYHEADERS_SPEED_PID_HPP_
#define MYHEADERS_SPEED_PID_HPP_

#include <motor.hpp>
#include <rotary_encoder.hpp>

#include <array>
#include <map>

typedef struct
{
	const double P;
	const double I;
	const double D;
}PIDgain_speed;

class Speed_pid : public MotorControl, public RotaryEncoder
{
public:

	Speed_pid() = delete;

	Speed_pid(const encoderName useRotEnc, const uint16_t motorPPR, const PIDgain_speed& useGains, const double controlInterval)
	: RotaryEncoder(useRotEnc),
	  ppr_(motorPPR),
	  useGains_speed_(useGains),
	  controlInterval_(controlInterval)
	{

	}

	inline void setTargetSpeed(const double setSpeed){ targetSpeed_ = setSpeed; }
	const double& targetSpeed = targetSpeed_;

	virtual void update()
	{
		speedPID_update();
	}

	inline void setPositionCount(uint32_t setCount) { nowCount_ = setCount; }
	template<typename T>
	inline T readPositionCount() const { return (T)nowCount_; }

	inline void setEnableState(const bool setState){ speed_pid_enable_ = setState; }
	const bool& isSpeedPID_Enable = speed_pid_enable_;

	const double& nowSpeed = nowSpeed_;

	virtual ~Speed_pid(){}

private:
	const uint16_t ppr_;
	const PIDgain_speed useGains_speed_;
	const double controlInterval_;

	void speedPID_update();

protected:
	double nowSpeed_		= 0.0;
	double targetSpeed_		= 0.0;
	bool speed_pid_enable_	= false;
	uint32_t nowCount_ = RotaryEncoder::readCount<uint32_t>();

};

#endif /* MYHEADERS_SPEED_PID_HPP_ */
