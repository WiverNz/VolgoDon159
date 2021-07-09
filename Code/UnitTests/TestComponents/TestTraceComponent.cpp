#include "StdAfx.h"

#include "TestTraceComponent.h"
#include <CryPhysics/IPhysics.h>
#include <CryRenderer/IRenderAuxGeom.h>
#include <CrySchematyc/Env/Elements/EnvComponent.h>
#include <CryCore/StaticInstanceList.h>

namespace
{
static void RegisterTestTraceComponent(Schematyc::IEnvRegistrar& registrar)
{
	Schematyc::CEnvRegistrationScope scope = registrar.Scope(IEntity::GetEntityScopeGUID());
	{
		Schematyc::CEnvRegistrationScope componentScope = scope.Register(SCHEMATYC_MAKE_ENV_COMPONENT(TestComponents::CTestTraceComponent));
	}
}

CRY_STATIC_AUTO_REGISTER_FUNCTION(&RegisterTestTraceComponent);
}

namespace TestComponents
{

CTestTraceComponent::CTestTraceComponent()
{

}

CTestTraceComponent::~CTestTraceComponent()
{
}

void CTestTraceComponent::Physicalize()
{
	SDrawTextInfo ti;
	ti.scale = Vec2(m_textSize);
	ti.flags = eDrawText_FixedSize;
	ti.color[0] = m_textColor.r;
	ti.color[1] = m_textColor.g;
	ti.color[2] = m_textColor.b;
	ti.color[3] = m_textColor.a;
	auto pRnd = gEnv->pRenderer->GetIRenderAuxGeom();
	Vec3 prevPoint = m_pEntity->GetPos();
	pRnd->DrawSphere(prevPoint, m_pointRadius, m_pointColor);
	pRnd->RenderTextQueued(prevPoint + m_textOffset, ti, "Start");
	for (const auto& currPoint : m_points.points)
	{
		pRnd->DrawLine(prevPoint, m_lineColor, currPoint.point, m_lineColor, m_lineThickness);
		pRnd->DrawSphere(currPoint.point, m_pointRadius, m_pointColor);
		pRnd->RenderTextQueued(currPoint.point + m_textOffset, ti, currPoint.description.c_str());
		prevPoint = currPoint.point;
	}
	//
	//Matrix34 mtx = GetWorldTransformMatrix();
	//
}

void CTestTraceComponent::ProcessEvent(const SEntityEvent& ev)
{
	switch (ev.event)
	{
	case ENTITY_EVENT_UPDATE:
		{
			Physicalize();
			break;
		}
	}
}

} // namespace TestComponents