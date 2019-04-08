/*
 * move_angle.cpp
 *
 *  Created on: 2019/04/06
 *      Author: User
 */
#include <move_angle.hpp>
#include <string>
#include <uart.hpp>

void MoveAngle::switchControl_()
{
	if(!upSig.readNowState() && !emergencySwitch.readNowState())
	{
		PositionPID::SetSpeed(driveSpeed_, MotorDriver::MotorDriveMode::PID);
	}
	else if(!downSig.readNowState() && !emergencySwitch.readNowState())
	{
		PositionPID::SetSpeed(-1*driveSpeed_, MotorDriver::MotorDriveMode::PID);
	}
	else
	{
		if(isGotZeroPoint && !isPositionPID_Enable)PositionPID::SetSpeed(0, MotorDriver::MotorDriveMode::DutyControl);
	}
}
void MoveAngle::zeroPointIntrrupt_()
{
	if(isGotZeroPoint_)return;
	PositionPID::SetSpeed(0, MotorDriver::MotorDriveMode::DutyControl);
	PositionPID::setPositionCount((int32_t)movePositions::downPosition);
	isGotZeroPoint_ = true;
}
void MoveAngle::moveAngle_Update()
{
	switchControl_();
	if(emergencySwitch.readNowState())PositionOK.setNewState(state::ON);
	else PositionOK.setNewState(state::OFF);

	if(!upSig.readNowState() || !downSig.readNowState())
	{
		PositionPID::setEnableState(false);
	}
	else if(!emergencySwitch.readNowState())
	{
		if(!isGotZeroPoint)
		{
			getZeroPoint_();
			PositionPID::setEnableState(false);
		}
		else
		{
			PositionPID::setEnableState(true);
		}
	}
}
void MoveAngle::getZeroPoint_()
{
	PositionPID::SetSpeed(getZeroPointSpeed, MotorDriver::MotorDriveMode::PID);
}

