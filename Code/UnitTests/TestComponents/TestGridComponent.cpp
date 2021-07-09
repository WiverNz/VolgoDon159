#include "StdAfx.h"

#include "TestGridComponent.h"
#include <CryPhysics/IPhysics.h>
#include <CryRenderer/IRenderAuxGeom.h>
#include <CrySchematyc/Env/Elements/EnvComponent.h>
#include <CryCore/StaticInstanceList.h>

namespace
{
static void RegisterTestGridComponent(Schematyc::IEnvRegistrar& registrar)
{
	Schematyc::CEnvRegistrationScope scope = registrar.Scope(IEntity::GetEntityScopeGUID());
	{
		Schematyc::CEnvRegistrationScope componentScope = scope.Register(SCHEMATYC_MAKE_ENV_COMPONENT(TestComponents::CTestGridComponent));
	}
}

CRY_STATIC_AUTO_REGISTER_FUNCTION(&RegisterTestGridComponent);
}

namespace TestComponents
{

CTestGridComponent::~CTestGridComponent()
{
}

void CTestGridComponent::Physicalize()
{
	auto pRnd = gEnv->pRenderer->GetIRenderAuxGeom();
	Matrix34 mtx = GetWorldTransformMatrix();
	Vec2_tpl<EPowerOf2> size(m_sizex, m_sizey);
	for (int i = 0; i < 2; i++)
	{
		Vec3 pt[2] = { Vec3(ZERO), Vec3(ZERO) }, h(0, 0, m_height);
		pt[1][i ^ 1] = m_cellSize[i ^ 1] * size[i ^ 1];
		for (int j = 0, flip = i; j <= (int) size[i]; j++, flip = i ^ 1)
		{
			pt[0][i] = pt[1][i] = m_cellSize[i] * j;
			pRnd->DrawLine(mtx * pt[0], m_colorLeft, mtx * pt[1], m_colorRight, m_thickness);
		}
	}
}

void CTestGridComponent::ProcessEvent(const SEntityEvent& ev)
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