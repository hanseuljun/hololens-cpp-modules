#include "HoloLensCppModulesPch.h"
#include "RayCastResult.h"

namespace HoloLensCppModules
{
	RayCastResult::RayCastResult(
		bool hit,
		DirectX::SimpleMath::Ray ray,
		DirectX::SimpleMath::Vector3 position)
		: m_hit(hit)
		, m_ray(ray)
		, m_position(position)
	{
	}

	RayCastResult::~RayCastResult()
	{
	}
}