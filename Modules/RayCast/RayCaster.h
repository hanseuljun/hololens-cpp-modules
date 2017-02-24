#pragma once

#include "RayCastResult.h"
#include "..\LocatableCamera\LocatableCameraFrame.h"
#include "..\SpatialMapping\SpatialMappingFrame.h"

namespace HoloLensCppModules
{
	class RayCaster
	{
	private:
		RayCaster() {}

	public:
		static std::shared_ptr<RayCastResult> RayCast(
			DirectX::SimpleMath::Vector2 pixel,
			std::shared_ptr<LocatableCameraFrame> locatableCameraFrame,
			std::shared_ptr<SpatialMappingFrame> spatialMappingFrame,
			Windows::Perception::Spatial::SpatialCoordinateSystem^ currentCoordinateSystem);

		static std::shared_ptr<RayCastResult> RayCast(
			DirectX::SimpleMath::Ray ray,
			std::shared_ptr<SpatialMappingFrame> spatialMappingFrame,
			Windows::Perception::Spatial::SpatialCoordinateSystem^ currentCoordinateSystem);

	private:
		static DirectX::SimpleMath::Vector3 CalculateHololensPreviewCameraDirection(
			DirectX::SimpleMath::Vector2 pixel,
			std::shared_ptr<LocatableCameraFrame> locatableCameraFrame);
	};
}