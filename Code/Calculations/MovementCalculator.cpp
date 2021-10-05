#include "StdAfx.h"
#include "MovementCalculator.h"

#include <CryCore/Assert/CryAssert.h>
#include <CryMath/Angle.h>

namespace Calculations
{
CMovementCalculator::CMovementCalculator(const float frameTime, const Navigation::SShipParameters* shipParameters, Navigation::SStatusShip* statusShip)
{
	m_frameTime = frameTime;
	m_shipParameters = shipParameters;
	m_statusShip = statusShip;
}

void CMovementCalculator::Calculate(const float& frameTime)
{
	m_frameTime = frameTime;
	m_statusShip->rudder = std::min(m_statusShip->steer, decltype(m_statusShip->steer)(30.f));
	m_statusShip->rudder = std::max(m_statusShip->rudder, decltype(m_statusShip->rudder)(-30.f));
	AccelerationCalculation();
	RotateCalculation();

	m_nextPosVecLenght = m_statusShip->speed * frameTime / 1000.f / 3600.f;
}

void CMovementCalculator::AccelerationCalculation()
{

}

void CMovementCalculator::RotateCalculation()
{
	CryTransform::CAngle turnAngle = CryTransform::CAngle::FromDegrees(m_statusShip->rudder * m_frameTime / 10000.f);
	m_statusShip->course = m_statusShip->course + turnAngle;
}

void CMovementCalculator::LeftClickHandler()
{
	m_statusShip->steer = m_statusShip->steer - 1;
}

void CMovementCalculator::RightClickHandler()
{
	m_statusShip->steer = m_statusShip->steer + 1;
}

void CMovementCalculator::ForwardClickHandler()
{
	m_statusShip->leftEngineRPM = std::min(m_statusShip->leftEngineRPM + 30, m_shipParameters->engineSpeed);
	m_statusShip->rightEngineRPM = std::min(m_statusShip->rightEngineRPM + 30, m_shipParameters->engineSpeed);
	m_statusShip->speed = m_statusShip->rightEngineRPM / m_shipParameters->engineSpeed * m_shipParameters->maxSpeed;
}

void CMovementCalculator::BackClickHandler()
{
	m_statusShip->leftEngineRPM = std::max(m_statusShip->leftEngineRPM - 30, -m_shipParameters->engineSpeed);
	m_statusShip->rightEngineRPM = std::max(m_statusShip->rightEngineRPM - 30, -m_shipParameters->engineSpeed);
	m_statusShip->speed = m_statusShip->rightEngineRPM / m_shipParameters->engineSpeed * m_shipParameters->maxSpeed;
}

Vec3 CMovementCalculator::GetNextPos(const Vec3& currPos) const
{
	if (!m_shipParameters || !m_statusShip)
	{
		return currPos;
	}

	auto currX = currPos.x + crymath::sin(m_statusShip->course.ToRadians()) * m_nextPosVecLenght;
	auto currY = currPos.y + crymath::cos(m_statusShip->course.ToRadians()) * m_nextPosVecLenght;

	return Vec3(currX, currY, currPos.z);
}

Vec3 CMovementCalculator::GetNextDiffPos(const Vec3& currPos) const
{
	if (!m_shipParameters || !m_statusShip)
	{
		return currPos;
	}

	auto currX = crymath::sin(m_statusShip->course.ToRadians()) * m_nextPosVecLenght;
	auto currY = crymath::cos(m_statusShip->course.ToRadians()) * m_nextPosVecLenght;

	return Vec3(currX, currY, 0);
}

} // namespace Calculations
