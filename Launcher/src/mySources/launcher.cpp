/*
 * launcher.cpp
 *
 *  Created on: 2019/04/06
 *      Author: User
 */

#include <launcher.hpp>

void Launcher::throwingShagai()
{
	static std::string sendString = std::to_string(Speed_pid::nowSpeed) + '\n';
	uartSendString(sendString);
	sendString.clear();


	switch(nowSequence_)
	{
	case launcherSequence::launch:
		if(Speed_pid::readPositionCount<uint32_t>() < encCount_stopPoint)
		{
			Speed_pid::setEnableState(false);
			nowSequence_ = launcherSequence::breaking;
		}
		else
		{
			Speed_pid::setEnableState(true);
			Speed_pid::setTargetSpeed(throwing_motorDriveSpeed_);
		}
		break;

	case launcherSequence::breaking:
		if(Speed_pid::nowSpeed > 1.0)nowSequence_ = launcherSequence::returnZeroPoint;
		else Speed_pid::MotorControl::setSpeed(-30);
		break;

	case launcherSequence::returnZeroPoint:
		if(zeroPointLimit.readNowState())
		{
			Speed_pid::setEnableState(false);
			Speed_pid::setSpeed(0, MotorControl::driveMode::SMB);
		}
		else
		{
			Speed_pid::setEnableState(true);
			Speed_pid::setTargetSpeed(-1.0*getZeroPoint_motorDriveSpeed_);
		}
		break;
	}
}

void Launcher::launcherUpdate()
{
	getZeroPoint();
	if(isGotZeroPoint)
	{
		throwingShagai();
	}

	std::string a = std::to_string(Speed_pid::readPositionCount<uint16_t>()) + '\n';
	uartSendString(a);
}

void Launcher::getZeroPoint()
{
	if(!isGotZeroPoint)
	{
		Speed_pid::setEnableState(true);
		if(zeroPointLimit.readNowState())
		{
			Speed_pid::setTargetSpeed(getZeroPoint_motorDriveSpeed_);
		}
		else
		{
			Speed_pid::setTargetSpeed(-1.0*getZeroPoint_motorDriveSpeed_);
		}


	}
	if(zeroPointLimit.readNowState())
	{
		//led.setNewState(LED::ledColor::Red, LED::state::ON);
	}
	else
	{
		//led.setNewState(LED::ledColor::Red, LED::state::OFF);
	}
}

void Launcher::limitSensorIntrrupt()
{
	//led.setNewState(LED::ledColor::Yellow, LED::state::OFF);
	if(!isGotZeroPoint)
	{
		isGotZeroPoint_ = true;
		Speed_pid::setPositionCount(defaultEncoderCount);
		Speed_pid::setTargetSpeed(0.0);
		Speed_pid::setEnableState(false);
		Speed_pid::MotorControl::setSpeed(0);
	}
}


