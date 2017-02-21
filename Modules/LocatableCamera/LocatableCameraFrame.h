#pragma once

#include <SimpleMath.h>

namespace HoloLensCppModules
{
	class LocatableCameraFrame
	{
	public:
		LocatableCameraFrame(
			unsigned int id,
			Windows::Graphics::Imaging::SoftwareBitmap^ softwareBitmap,
			Windows::Perception::Spatial::SpatialCoordinateSystem^ coordinateSystem,
			DirectX::SimpleMath::Matrix viewTransform,
			DirectX::SimpleMath::Matrix projectionTransform);
		~LocatableCameraFrame();
		unsigned int GetId() { return m_id; }
		Windows::Graphics::Imaging::SoftwareBitmap^ GetSoftwareBitmap() { return m_softwareBitmap; }
		Windows::Perception::Spatial::SpatialCoordinateSystem^ GetCoordinateSystem() { return m_coordinateSystem; }
		DirectX::SimpleMath::Matrix GetViewTransform() { return m_viewTransform; }
		DirectX::SimpleMath::Matrix GetProjectionTransform() { return m_projectionTransform; }

	private:
		unsigned int m_id;
		Windows::Graphics::Imaging::SoftwareBitmap^ m_softwareBitmap;
		Windows::Perception::Spatial::SpatialCoordinateSystem^ m_coordinateSystem;
		DirectX::SimpleMath::Matrix m_viewTransform;
		DirectX::SimpleMath::Matrix m_projectionTransform;
	};
}