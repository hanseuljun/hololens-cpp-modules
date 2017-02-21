#include "HololensCppModulesPch.h"
#include "LocatableCameraFrame.h"

namespace HoloLensCppModules
{
	LocatableCameraFrame::LocatableCameraFrame(
		unsigned int id,
		Windows::Graphics::Imaging::SoftwareBitmap^ softwareBitmap,
		Windows::Perception::Spatial::SpatialCoordinateSystem^ coordinateSystem,
		DirectX::SimpleMath::Matrix viewTransform,
		DirectX::SimpleMath::Matrix projectionTransform)
		: m_id(id)
		, m_softwareBitmap(softwareBitmap)
		, m_coordinateSystem(coordinateSystem)
		, m_viewTransform(viewTransform)
		, m_projectionTransform(projectionTransform)
	{
	}

	LocatableCameraFrame::~LocatableCameraFrame()
	{
	}
}