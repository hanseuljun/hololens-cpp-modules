#pragma once

#include "SpatialMappingFrame.h"

namespace HoloLensCppModules
{
	// Asynchronouslly obtains spatial data that is needed by HoloLens applications.
	class SpatialMappingModule
	{
	public:
		SpatialMappingModule();
		~SpatialMappingModule();
		void UpdateBoundingVolume(Windows::Perception::Spatial::SpatialCoordinateSystem^ currentCoordinateSystem);
		std::shared_ptr<SpatialMappingFrame> GetFrame();

	private:
		void OnSurfacesChanged(
			Windows::Perception::Spatial::Surfaces::SpatialSurfaceObserver^ sender,
			Platform::Object^ args);
		std::shared_ptr<SpatialMappingFrameEntry> CreateFrameEntry(
			Windows::Perception::Spatial::Surfaces::SpatialSurfaceMesh^ spatialSurfaceMesh);
			
	private:
		Windows::Perception::Spatial::Surfaces::SpatialSurfaceObserver^ m_observer;
		double m_maxTrianglesPerCubicMeters;
		Windows::Perception::Spatial::Surfaces::SpatialSurfaceMeshOptions^ m_options;

		std::shared_mutex m_propertiesLock;
		std::map<Platform::Guid, std::shared_ptr<SpatialMappingFrameEntry>> m_entries;
		std::shared_ptr<SpatialMappingFrame> m_frame;
		uint32 m_surfaceId;
	};
}