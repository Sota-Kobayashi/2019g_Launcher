/*
 * speed_pid.cpp
 *
 *  Created on: 2019/03/31
 *      Author: User
 */

#include <speed_pid.hpp>

void Speed_pid::speedPID_update()
{
	static double dev_before = 0.0;

	enum class termName {P, I, D};
	static std::map<termName, double> terms = {{termName::P, 0.0},{termName::I, 0.0},{termName::D, 0.0}};

	constexpr double Min = 60.0;
	nowSpeed_ = ( Min * readCount<double>() / ((double)ppr_ * controlInterval_));

	const double dev_now = targetSpeed_ - nowSpeed;
	nowCount_ += (uint32_t)RotaryEncoder::readCount<int32_t>();
	RotaryEncoder::clearCount();

	/*Calculation terms of PID.*/
	terms[termName::P] =  useGains_speed_.P * dev_now;
	terms[termName::I] += useGains_speed_.I * dev_now;
	terms[termName::D] =  useGains_speed_.D * (dev_now - dev_before);

	/*addition terms of PID.*/
	double setSpeed = 0.0;
	for(auto i : terms)setSpeed += i.second;

	if(isSpeedPID_Enable)
	{
		MotorControl::setSpeed(std::abs(setSpeed) > 90.0 ?90 :(int32_t)setSpeed, MotorControl::driveMode::SMB);
		dev_before = dev_now;
	}
	else
	{
		dev_before = 0.0;
		terms[termName::I] = 0.0;
	}
}
