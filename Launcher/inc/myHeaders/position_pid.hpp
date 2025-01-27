/*
 * position_pid.hpp
 *
 *  Created on: 2019/04/06
 *      Author: User
 */

#ifndef MYHEADERS_POSITION_PID_HPP_
#define MYHEADERS_POSITION_PID_HPP_

#include <map>

#include <motor_driver.hpp>
#include <rotary_encoder.hpp>

typedef struct
{
	const float P;
	const float I;
	const float D;
}PIDgain_positon;

typedef struct
{
	const PIDgain_positon useGain;

	const encoderName useRotEnc;
	const uint8_t useDriverAdd;
	const int32_t tolerance;
	const int32_t maxSpeed;

	const MotorDriver::SendParamsType motorDriverParams;

}PositionPID_initStructType;

class PositionPID : public RotaryEncoder, public MotorDriver
{
public:
	PositionPID() = delete;
	PositionPID(const PositionPID_initStructType initStruct) :
		RotaryEncoder(initStruct.useRotEnc),
		MotorDriver(initStruct.useDriverAdd),
		constantValues_(initStruct)
	{
		MotorDriver::MD_ParamSend(initStruct.motorDriverParams);
	}
	inline void setTargetPosition(const int32_t setPosition){ targetPosition_ = setPosition; }
	const int32_t& targetPosition = targetPosition_;

	virtual void update() override
	{
		static uint8_t count = 0;
		if(count >= 10)
		{
			positionPID_update();
			MotorDriver::update();
			count = 0;
		}
		else
		{
			count++;
		}
	}

	inline void setPositionCount(int32_t setCount)
	{
		nowCount_ = setCount;
		RotaryEncoder::clearCount();
	}
	template<typename T>
	inline T readPositionCount() const { return (T)nowCount_; }

	inline void setEnableState(const bool setState){ position_pid_enable_ = setState; }
	const bool& isPositionPID_Enable = position_pid_enable_;

	inline bool errorCheck() const
		{ return std::abs(targetPosition - readPositionCount<int32_t>()) < constantValues_.tolerance ? true : false; }

	virtual ~PositionPID(){}

private:
	const PositionPID_initStructType constantValues_;
	int32_t targetPosition_ = 0;
	bool position_pid_enable_ = false;
	int32_t nowCount_ = RotaryEncoder::readCount<int32_t>();
	void positionPID_update();

};



#endif /* MYHEADERS_POSITION_PID_HPP_ */
