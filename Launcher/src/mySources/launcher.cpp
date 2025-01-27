/*
 * launcher.cpp
 *
 *  Created on: 2019/04/06
 *      Author: User
 */

#include <launcher.hpp>

#include <array>

void Launcher::throwingShagai()
{
	launchSign.setNewState(state::OFF);
	brakingSign.setNewState(state::OFF);

	switch(nowSequence_)
	{
	case launcherSequence::launch:
		launchSign.setNewState(state::ON);
		if(launchMotorPID.readPositionCount<uint32_t>() < encCount_stopPoint)
		{
			launchMotorPID.setEnableState(false);
			nowSequence_ = launcherSequence::breaking;
		}
		else
		{
			launchMotorPID.setEnableState(true);
			launchMotorPID.setTargetSpeed(throwing_motorDriveSpeed_);
		}
		break;

	case launcherSequence::breaking:
		brakingSign.setNewState(state::ON);
		if(launchMotorPID.nowSpeed > 10.0)
		{
			nowSequence_ = launcherSequence::returnZeroPoint;
		}
		else
		{
			launchMotorPID.setEnableState(false);
			launchMotorPID.MotorControl::setSpeed(brakeDuty);
		}

		break;

	case launcherSequence::returnZeroPoint:
		if(zeroPointLimit.readNowState())
		{
			launchMotorPID.setEnableState(false);
			launchMotorPID.setSpeed(0, MotorControl::driveMode::SMB);
		}
		else
		{
			launchMotorPID.setEnableState(true);
			launchMotorPID.setTargetSpeed(-1.0*getZeroPoint_motorDriveSpeed_);
		}
		break;
	}

	if(emergencySwitch.readNowState())launchMotorPID.setEnableState(false);
}

void Launcher::launcherUpdate()
{
	getZeroPoint();

	//std::string str = std::to_string(Speed_pid::readPositionCount<uint32_t>()) + '\n';
	//uartSendString(str);

	if(emergencySwitch.readNowState())launchMotorPID.setNewStateGate(MotorControl::gateEnableState::Disable);
	else launchMotorPID.setNewStateGate(launchMotorPID.gateEnableState::Enable);

	if(isGotZeroPoint)
	{
		throwingShagai();
	}
	if(emergencySwitch.readNowState())launchMotorPID.setEnableState(false);
}

void Launcher::getZeroPoint()
{
	if(!isGotZeroPoint)
	{
		launchMotorPID.setEnableState(true);
		if(zeroPointLimit.readNowState())
		{
			launchMotorPID.setTargetSpeed(getZeroPoint_motorDriveSpeed_);
		}
		else
		{
			launchMotorPID.setTargetSpeed(-1.0*getZeroPoint_motorDriveSpeed_);
		}
	}
}

void Launcher::limitSensorIntrrupt()
{
	if(!isGotZeroPoint)
	{
		isGotZeroPoint_ = true;
		launchMotorPID.setPositionCount(defaultEncoderCount);
		launchMotorPID.setTargetSpeed(0.0);
		launchMotorPID.setEnableState(false);
		launchMotorPID.setSpeed(0);
	}
}


