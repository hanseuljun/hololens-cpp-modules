#pragma once

#include <SimpleMath.h>

namespace HoloLensCppModules
{
	class Mesh;

	// This class contains the preprocessed data of a SpaitalSurface.
	// A list of instances of this class consists SpatialSurfaceContainerGroup.
	// Details of creating this class can be found in SpatialSurfaceProcessor::CreateSpatialSurfaceContainer().
	class SpatialMappingFrameEntry
	{
	public:
		SpatialMappingFrameEntry(
			std::vector<DirectX::SimpleMath::Vector4> positions,
			std::vector<DirectX::SimpleMath::Vector4> normals,
			std::vector<unsigned short> indices,
			long long updateTime,
			Windows::Perception::Spatial::SpatialCoordinateSystem^ coordinateSystem,
			DirectX::SimpleMath::Vector3 vertexPositionScale,
			Windows::Perception::Spatial::Surfaces::SpatialSurfaceInfo^ surfaceInfo);
		~SpatialMappingFrameEntry();
		const std::vector<DirectX::SimpleMath::Vector4>& GetPositions() { return m_positions; }
		const std::vector<DirectX::SimpleMath::Vector4>& GetNormals() { return m_normals; }
		const std::vector<unsigned short>& GetIndices() { return m_indices; }
		long long GetUpdateTime() { return m_updateTime; }
		Windows::Perception::Spatial::SpatialCoordinateSystem^ GetCoordinateSystem() { return m_coordinateSystem; }
		DirectX::SimpleMath::Vector3 GetVertexPositionScale() { return m_vertexPositionScale; }
		Windows::Perception::Spatial::Surfaces::SpatialSurfaceInfo^ GetSurfaceInfo() { return m_surfaceInfo; }

	private:
		// Vertex positions of the surface.
		std::vector<DirectX::SimpleMath::Vector4> m_positions;
		// Normal vectors of the surface.
		std::vector<DirectX::SimpleMath::Vector4> m_normals;
		// Indices of surface's triangles.
		std::vector<unsigned short> m_indices;
		long long m_updateTime;
		// The surface's SpatialCoordinateSystem.
		// Needed to obtain the model matrix.
		Windows::Perception::Spatial::SpatialCoordinateSystem^ m_coordinateSystem;
		// A scaling factor for the vertex positions.
		// Also needed to obtain the model matrix.
		// Though the existance of this may seem akward, in my (Hanseul's) opinion, 
		// this was included by Microsoft for a memory optimizaion issue.
		// They use short instead of float to store vertex positions.
		DirectX::SimpleMath::Vector3 m_vertexPositionScale;
		// The instance containing other infos of the surface.
		Windows::Perception::Spatial::Surfaces::SpatialSurfaceInfo^ m_surfaceInfo;
	};
}
