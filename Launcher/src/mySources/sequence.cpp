/*
 * sequence.cpp
 *
 *  Created on: 2019/04/19
 *      Author: User
 */

#include <sequence.hpp>

void Sequence::sequenceUpdate_()
{
	static uint16_t waitCount = 0;

	/*This lambda function returns true if it has to wait. */
	auto waiting = [&]	{
							if(waitCount > 0)waitCount--;
							return waitCount == 0 ? false : true;
						};

	if(emergencySwitch.readNowState())
	{
		emergencyStateLED.setNewState(state::ON);
	}
	else
	{
		emergencyStateLED.setNewState(state::OFF);

		//receiveCmd_ = receiveOrderFormat::throwShagai;
		switch(nowSequence_)
		{
		case sequenceName::start :
			moveAngleMechanism_.setTargetMovePosition(moveAngleMechanism_.movePositions::upPosition);
			launcherMechanism_.setLauncherSequence(Launcher::launcherSequence::returnZeroPoint);
			eleValve.setNewState(EV::state::Reset, expendArmValve, fallArmValve, holdShagaiValve);
			//eleValve.setNewState(EV::state::Set, holdShagaiValve);
			if(receiveCmd_ != receiveOrderFormat::start && launcherMechanism_.isGotZeroPoint && moveAngleMechanism_.isGotZeroPoint)
			{
				nowSequence_ = sequenceName::setGettingAngle;
			}
			break;

		case sequenceName::setGettingAngle :
			moveAngleMechanism_.setTargetMovePosition(moveAngleMechanism_.movePositions::downPosition);
			launcherMechanism_.setLauncherSequence(Launcher::launcherSequence::returnZeroPoint);

			if(moveAngleMechanism_.readNowAngleCount() > ExpandArmAngle)
			{
				eleValve.setNewState(EV::state::Set, expendArmValve);
				eleValve.setNewState(EV::state::Set, holdShagaiValve);
				if(expandArmSensor.readNowState())eleValve.setNewState(EV::state::Set, fallArmValve);
			}

			if(moveAngleMechanism_.isAnglePID_OK() && expandArmSensor.readNowState())
			{
				waitCount = 200;
				nowSequence_ = sequenceName::waitSettingGetttingAngle;
			}
			break;


		case sequenceName::waitSettingGetttingAngle :

			if(!waiting())nowSequence_ = sequenceName::fallArm;
			break;

		case sequenceName::fallArm:
			eleValve.setNewState(EV::state::Set, fallArmValve);
			sendConpliteCmd_(compliteCmdFormat::standbyGettingShagai);
			if(receiveCmd_ == receiveOrderFormat::getShagai || receiveCmd_ == receiveOrderFormat::throwShagai)
			{
				nowSequence_ = sequenceName::getShagai;
			}
			break;

		case sequenceName::getShagai:
			eleValve.setNewState(EV::state::Reset, holdShagaiValve);
			waitCount = 1000;
			nowSequence_ = sequenceName::waitGettingShagai;
			break;

		case sequenceName::waitGettingShagai:
			if(!waiting())nowSequence_ = sequenceName::liftArm;
			break;

		case sequenceName::liftArm:
			eleValve.setNewState(EV::state::Reset, fallArmValve);
			waitCount = 1000;
			nowSequence_ = sequenceName::waitLiftingArm;
			break;

		case sequenceName::waitLiftingArm:
			if(!waiting())nowSequence_ = sequenceName::shortenArm;
			break;

		case sequenceName::shortenArm:
			eleValve.setNewState(EV::state::Reset, expendArmValve);
			waitCount = 1000;
			nowSequence_ = sequenceName::waitShortenningArm;
			break;

		case sequenceName::waitShortenningArm:
			if(!waiting())nowSequence_ = sequenceName::setThrowingAngle;
			break;

		case sequenceName::setThrowingAngle:
			moveAngleMechanism_.setTargetMovePosition(moveAngleMechanism_.movePositions::throwingPosition);
			if(moveAngleMechanism_.isAnglePID_OK())
			{
				waitCount = 200;
				nowSequence_ = sequenceName::waitSettingThrowingAngle;
			}
			break;

		case sequenceName::waitSettingThrowingAngle:
			if(!waiting())
			{
				sendConpliteCmd_(compliteCmdFormat::gettigShagai);
				if(receiveCmd_ == receiveOrderFormat::throwShagai)nowSequence_ = sequenceName::openArm;
			}
			break;

		case sequenceName::openArm:
			eleValve.setNewState(EV::state::Set, holdShagaiValve);
			waitCount = 550;
			nowSequence_ = sequenceName::waitOpennningArm;
			break;

		case sequenceName::waitOpennningArm:
			if(!waiting())
			{
				launcherMechanism_.setLauncherSequence(Launcher::launcherSequence::launch);
				nowSequence_ = sequenceName::throwShagai;
			}
			break;

		case sequenceName::throwShagai:
			if(launcherMechanism_.nowSequence == Launcher::launcherSequence::returnZeroPoint)
			{
				waitCount = 500;
				nowSequence_ = sequenceName::waitThrowingShagai;
			}
			break;

		case sequenceName::waitThrowingShagai:
			if(!waiting())
			{
				//eleValve.setNewState(EV::state::Reset, holdShagaiValve);
				sendConpliteCmd_(compliteCmdFormat::throwingComplite);
				nowSequence_ = sequenceName::waitCommand;
				receiveCmd_ = receiveOrderFormat::start;
			}
			break;

		case sequenceName::waitCommand:
			if(receiveCmd_ != receiveOrderFormat::start)nowSequence_ = sequenceName::setGettingAngle;
			break;

		}
	}
}
void Sequence::sendConpliteCmd_(const compliteCmdFormat sendCmd)
{
	const std::array<uint8_t, 2> sendData = { OwnAddress, (uint8_t)sendCmd };
	ControlAreaNetwork::sendData(sendData, MainBoardAddress);
}
void Sequence::orderReceive_(const CanRxMsg& receiveData)
{
	//if(receiveData.Data[0] == MainBoardAddress) receiveCmd_ = (receiveOrderFormat)receiveData.Data[1];
	if(receiveData.Data[0] == MainBoardAddress && receiveData.Data[1] == CmdIncrementSequence)
	{
		switch(receiveCmd_)
		{
		case receiveOrderFormat::start:
			receiveCmd_ = receiveOrderFormat::fallArm;
			break;

		case receiveOrderFormat::fallArm:
			receiveCmd_ = receiveOrderFormat::getShagai;
			break;

		case receiveOrderFormat::getShagai:
			receiveCmd_ = receiveOrderFormat::throwShagai;
			break;

		case receiveOrderFormat::throwShagai:
			receiveCmd_ = receiveOrderFormat::fallArm;
			break;
		}
	}
}

