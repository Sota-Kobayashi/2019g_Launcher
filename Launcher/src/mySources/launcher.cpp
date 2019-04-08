/*
 * launcher.cpp
 *
 *  Created on: 2019/04/06
 *      Author: User
 */

#include <launcher.hpp>

#include <array>
#include <charconv>

void Launcher::throwingShagai()
{
	launchSign.setNewState(state::OFF);
	brakingSign.setNewState(state::OFF);

	switch(nowSequence_)
	{
	case launcherSequence::launch:
		launchSign.setNewState(state::ON);
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
		brakingSign.setNewState(state::ON);
		if(Speed_pid::nowSpeed > 10.0)
		{
			nowSequence_ = launcherSequence::returnZeroPoint;
		}
		else
		{
			Speed_pid::setEnableState(false);
			Speed_pid::MotorControl::setSpeed(brakeDuty);
		}

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

	if(emergencySwitch.readNowState())Speed_pid::setEnableState(false);
}

void Launcher::launcherUpdate()
{
	getZeroPoint();

	//std::string str = std::to_string(Speed_pid::readPositionCount<uint32_t>()) + '\n';
	//uartSendString(str);

	if(emergencySwitch.readNowState())MotorControl::setNewStateGate(MotorControl::gateEnableState::Disable);
	else MotorControl::setNewStateGate(MotorControl::gateEnableState::Enable);

	if(isGotZeroPoint)
	{
		throwingShagai();
	}
	if(emergencySwitch.readNowState())Speed_pid::setEnableState(false);
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
}

void Launcher::limitSensorIntrrupt()
{
	if(!isGotZeroPoint)
	{
		isGotZeroPoint_ = true;
		Speed_pid::setPositionCount(defaultEncoderCount);
		Speed_pid::setTargetSpeed(0.0);
		Speed_pid::setEnableState(false);
		Speed_pid::MotorControl::setSpeed(0);
	}
}


