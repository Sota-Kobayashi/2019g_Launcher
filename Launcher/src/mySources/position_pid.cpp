/*
 * position_pid.cpp
 *
 *  Created on: 2019/04/06
 *      Author: User
 */

#include <position_pid.hpp>
#include <uart.hpp>

void PositionPID::positionPID_update()
{
	nowCount_ += RotaryEncoder::readCount<int32_t>();
	RotaryEncoder::clearCount();

	static int32_t dev_before = 0.0;

	enum class termName {P, I, D};
	static std::map<termName, double> terms = {{termName::P, 0.0},{termName::I, 0.0},{termName::D, 0.0}};

	const int32_t dev_now = targetPosition_ - nowCount_;
	if(isPositionPID_Enable)
	{
		/*Calculation terms of PID.*/
		terms[termName::P] =  constantValues_.useGain.P * (double)dev_now;
		terms[termName::I] += constantValues_.useGain.I * (double)dev_now;
		terms[termName::D] =  constantValues_.useGain.D * (double)(dev_now - dev_before);

		/*addition terms of PID.*/
		double setSpeed = 0.0;
		for(auto i : terms)setSpeed += i.second;

		if(setSpeed > 0.0 && std::abs(setSpeed) > constantValues_.maxSpeed)
		{
			setSpeed = constantValues_.maxSpeed;
		}
		else if(0.0 > setSpeed && std::abs(setSpeed) > constantValues_.maxSpeed)
		{
			setSpeed = -constantValues_.maxSpeed;
		}

		dev_before = dev_now;

		MotorDriver::SetSpeed((int32_t)setSpeed, MotorDriver::MotorDriveMode::PID);
	}
	else
	{
		dev_before = 0;
		terms[termName::I] = 0.0;
	}
}



