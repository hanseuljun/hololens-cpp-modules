#pragma once

#include "SpatialMappingFrameEntry.h"

namespace HoloLensCppModules
{
	// It contains information from SpatialSurfaceProcessor.
	// It is a group since the SpatialSurfaceProcessor retrives sptial data this way.
	class SpatialMappingFrame
	{
	public:
		SpatialMappingFrame(
			unsigned int id, 
			std::map<Platform::Guid, std::shared_ptr<SpatialMappingFrameEntry>> containers);
		~SpatialMappingFrame();
		bool HasEntry(Platform::Guid guid);
		unsigned int GetId() { return m_id; }
		std::shared_ptr<SpatialMappingFrameEntry> GetEntry(Platform::Guid guid) { return m_entries.at(guid); }
		const std::map<Platform::Guid, std::shared_ptr<SpatialMappingFrameEntry>>& GetEntries() { return m_entries; }

	private:
		unsigned int m_id;
		std::map<Platform::Guid, std::shared_ptr<SpatialMappingFrameEntry>> m_entries;
	};
}
