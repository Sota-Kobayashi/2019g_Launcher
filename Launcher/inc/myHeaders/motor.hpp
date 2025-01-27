/*
 * motor.h
 *
 *  Created on: 2019/02/23
 *      Author: User
 */

#ifndef MYHEADERS_MOTOR_HPP_
#define MYHEADERS_MOTOR_HPP_

#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"

#include <uart.hpp>

#include <cmath>
#include <string>

class MotorControl
{
public:
	MotorControl();
	MotorControl(const uint16_t resolution, const uint32_t frequency);

	enum class gateEnableState : bool { Enable = true, Disable = false };
	inline void setNewStateGate(gateEnableState newState)
	{
		if(newState == gateEnableState::Enable)GPIO_SetBits(GPIOA, GPIO_Pin_5);
		else GPIO_ResetBits(GPIOA, GPIO_Pin_5);
	}

	enum class driveMode : uint8_t{ SMB, LAP };
	inline void setDriveMode(const driveMode setMode) { nowDriveMode_ = setMode; }
	const driveMode& getNowDriveMode = nowDriveMode_;

	void setSpeed(const int32_t setValue);
	void setSpeed(const int32_t setValue, const driveMode setMode);

	virtual ~MotorControl();

private:
	static constexpr uint16_t defaultResolution_	= 100;
	static constexpr uint32_t defaultFrequency_		= 20000;

protected:
	driveMode nowDriveMode_ = driveMode::SMB;
	uint16_t nowResolution_ = defaultResolution_;

	constexpr uint16_t prescaler_
		(const uint16_t resolution, const uint32_t pwmFreq, const uint32_t timerFrequency);

	void GPIO_PinsInit_();
	void timerInit_
		(const uint16_t resolution = defaultResolution_, const uint32_t pwmFreq = defaultFrequency_);
	void gateEnablePinInit_();

	void setSpeedSMB_(const int32_t setValue);
	void setSpeedLAP_(const int32_t setValue);
};


#endif /* MYHEADERS_MOTOR_HPP_ */
