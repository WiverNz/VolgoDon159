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

void CMovementCalculator::SetFrameTime(const float frameTime)
{
	m_frameTime = frameTime;
}

void CMovementCalculator::Calculate() const
{
	AccelerationCalculation();

}

void CMovementCalculator::AccelerationCalculation() const
{

}

Vec3 CMovementCalculator::GetNextPos(const Vec3& currPos) const
{
	if (!m_shipParameters || !m_statusShip)
	{
		return currPos;
	}

	auto currX = /* currPos.x + */ crymath::cos(m_statusShip->steer) * m_statusShip->speed * m_frameTime;
	auto currY = /* currPos.y + */ crymath::sin(m_statusShip->steer) * m_statusShip->speed * m_frameTime;

	return Vec3(currX, currY, currPos.z);
}

} // namespace Calculations
