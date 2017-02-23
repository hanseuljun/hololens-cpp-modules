#include "HoloLensCppModulesPch.h"
#include "SpatialMappingFrameEntry.h"

namespace HoloLensCppModules
{
	SpatialMappingFrameEntry::SpatialMappingFrameEntry(
		std::vector<DirectX::SimpleMath::Vector4> positions,
		std::vector<DirectX::SimpleMath::Vector4> normals,
		std::vector<unsigned short> indices,
		long long updateTime,
		Windows::Perception::Spatial::SpatialCoordinateSystem^ coordinateSystem,
		DirectX::SimpleMath::Vector3 vertexPositionScale,
		Windows::Perception::Spatial::Surfaces::SpatialSurfaceInfo^ surfaceInfo)
		: m_positions(positions)
		, m_normals(normals)
		, m_indices(indices)
		, m_updateTime(updateTime)
		, m_coordinateSystem(coordinateSystem)
		, m_vertexPositionScale(vertexPositionScale)
		, m_surfaceInfo(surfaceInfo)
	{
	}

	SpatialMappingFrameEntry::~SpatialMappingFrameEntry()
	{
	}
}
