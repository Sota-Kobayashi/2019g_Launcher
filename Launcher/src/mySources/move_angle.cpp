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
		setMotorPidSpeed_safety(driveSpeed_);
	}
	else if(!downSig.readNowState() && !emergencySwitch.readNowState())
	{
		setMotorPidSpeed_safety(-1*driveSpeed_);
	}
	else
	{
		if(isGotZeroPoint && !moveAngleMotor_.isPositionPID_Enable)moveAngleMotor_.SetSpeed(0, MotorDriver::MotorDriveMode::DutyControl);
	}
}
void MoveAngle::zeroPointIntrrupt_()
{
	if(isGotZeroPoint_)return;
	moveAngleMotor_.SetSpeed(0, MotorDriver::MotorDriveMode::DutyControl);
	moveAngleMotor_.setPositionCount((int32_t)movePositions::downPosition);
	isGotZeroPoint_ = true;
}
void MoveAngle::moveAngle_Update()
{
	switchControl_();

	if(!upSig.readNowState() || !downSig.readNowState() || emergencySwitch.readNowState())
	{
		moveAngleMotor_.setEnableState(false);
	}
	else if(!emergencySwitch.readNowState())
	{
		if(!isGotZeroPoint)
		{
			getZeroPoint_();
			moveAngleMotor_.setEnableState(false);
		}
		else
		{
			moveAngleMotor_.setEnableState(true);
		}
	}
}
void MoveAngle::getZeroPoint_()
{
	setMotorPidSpeed_safety(getZeroPointSpeed);
}

void MoveAngle::setMotorPidSpeed_safety(int32_t setSpeed)
{
	if(zeroPoint.readNowState() && ((setSpeed > 0) == downDirectionIsTrue))setSpeed = 0;
	if(!emergencySwitch.readNowState())moveAngleMotor_.SetSpeed(setSpeed, MotorDriver::MotorDriveMode::PID);
}
