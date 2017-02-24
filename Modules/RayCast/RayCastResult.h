#pragma once

namespace HoloLensCppModules
{
	// Contains a result from RayCaster.
	class RayCastResult
	{
	public:
		RayCastResult(
			bool hit,
			DirectX::SimpleMath::Ray ray,
			DirectX::SimpleMath::Vector3 position);
		~RayCastResult();
		// Whether the raycast hit anything.
		bool IsHit() { return m_hit; }
		// The ray that has been casted.
		DirectX::SimpleMath::Ray GetRay() { return m_ray; }
		// Where the raycast hit.
		DirectX::SimpleMath::Vector3 GetPosition() { return m_position; }
		// How far the ray was casted to hit something.
		float GetDistance() { return DirectX::SimpleMath::Vector3::Distance(m_ray.position, m_position); }

	private:
		bool m_hit;
		DirectX::SimpleMath::Ray m_ray;
		DirectX::SimpleMath::Vector3 m_position;
	};
}