#include "HoloLensCppModulesPch.h"
#include "SpatialMappingFrame.h"

#include "Utility.h"

namespace HoloLensCppModules
{
	SpatialMappingFrame::SpatialMappingFrame(
		unsigned int id,
		std::map<Platform::Guid, std::shared_ptr<SpatialMappingFrameEntry>> containers)
		: m_id(id)
		, m_entries(containers)
	{
	}

	SpatialMappingFrame::~SpatialMappingFrame()
	{
	}

	bool SpatialMappingFrame::HasEntry(Platform::Guid guid)
	{
		return Utility::MapContainsKey(m_entries, guid);
	}
}
