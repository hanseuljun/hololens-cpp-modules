#include "HoloLensCppModulesPch.h"
#include "SpatialMappingModule.h"

#include <SimpleMath.h>
#include "Utility.h"

namespace HoloLensCppModules
{
	SpatialMappingModule::SpatialMappingModule()
		: m_observer(ref new Windows::Perception::Spatial::Surfaces::SpatialSurfaceObserver())
		, m_maxTrianglesPerCubicMeters(1000.0)
		, m_options(ref new Windows::Perception::Spatial::Surfaces::SpatialSurfaceMeshOptions())
		, m_surfaceId(0)
	{
		using std::placeholders::_1;
		using std::placeholders::_2;
		using Windows::Foundation::TypedEventHandler;
		using Windows::Perception::Spatial::Surfaces::SpatialSurfaceObserver;

		m_options->IncludeVertexNormals = true;
		m_options->VertexPositionFormat = Windows::Graphics::DirectX::DirectXPixelFormat::R32G32B32A32Float;
		m_options->VertexNormalFormat = Windows::Graphics::DirectX::DirectXPixelFormat::R32G32B32A32Float;
		m_options->TriangleIndexFormat = Windows::Graphics::DirectX::DirectXPixelFormat::R16UInt;

		m_observer->ObservedSurfacesChanged +=
			ref new TypedEventHandler<SpatialSurfaceObserver^, Platform::Object^>(
				std::bind(&SpatialMappingModule::OnSurfacesChanged, this, _1, _2)
				);
	}

	SpatialMappingModule::~SpatialMappingModule()
	{
	}

	void SpatialMappingModule::UpdateBoundingVolume(
		Windows::Perception::Spatial::SpatialCoordinateSystem^ currentCoordinateSystem)
	{
		using Windows::Perception::Spatial::SpatialBoundingBox;
		using Windows::Perception::Spatial::SpatialBoundingVolume;

		SpatialBoundingBox axisAlignedBoundingBox = {
			{ 0.f,  0.f, 0.f },
			{ 20.f, 20.f, 5.f },
		};

		auto volume(SpatialBoundingVolume::FromBox(currentCoordinateSystem, axisAlignedBoundingBox));

		// Keep the surface observer positioned at the device's location.
		m_observer->SetBoundingVolume(volume);
	}

	std::shared_ptr<SpatialMappingFrame> SpatialMappingModule::GetFrame()
	{
		std::lock_guard<std::shared_mutex> lock(m_propertiesLock);
		return m_frame;
	}

	void SpatialMappingModule::OnSurfacesChanged(
		Windows::Perception::Spatial::Surfaces::SpatialSurfaceObserver^ sender,
		Platform::Object^ args)
	{
		using Windows::Perception::Spatial::Surfaces::SpatialSurfaceMesh;

		auto observedSurfaces = sender->GetObservedSurfaces();
		auto saveLatestMeshTaskGroup(concurrency::task_from_result());

		m_entries.clear();

		for (auto& surfaceInfoPair : observedSurfaces)
		{
			auto guid(surfaceInfoPair->Key);
			auto surfaceInfo(surfaceInfoPair->Value);

			bool duplicated = false;
			// If the detected mesh is found and not updated since then, reuse the processed one.
			if (m_frame != nullptr && m_frame->HasEntry(guid))
			{
				auto latestUpdateTime = m_frame->GetEntry(guid)->GetUpdateTime();
				auto updateTime = surfaceInfo->UpdateTime.UniversalTime;

				if (latestUpdateTime == updateTime)
				{
					duplicated = true;
				}
			}

			if (duplicated)
			{
				auto container = m_frame->GetEntry(guid);
				m_entries[guid] = container;
			}
			else
			{
				auto computeLatestMeshTask(concurrency::create_task(surfaceInfo->TryComputeLatestMeshAsync(m_maxTrianglesPerCubicMeters, m_options)));

				auto saveLatestMeshTask(computeLatestMeshTask.then([this](SpatialSurfaceMesh^ spatialSurfaceMesh)
				{
					if (spatialSurfaceMesh != nullptr)
					{
						auto guid = spatialSurfaceMesh->SurfaceInfo->Id;
						m_entries[guid] = CreateFrameEntry(spatialSurfaceMesh);
					}
				}));

				saveLatestMeshTaskGroup = saveLatestMeshTaskGroup && saveLatestMeshTask;
			}

		}

		saveLatestMeshTaskGroup.wait();

		{
			std::lock_guard<std::shared_mutex> lock(m_propertiesLock);
			m_frame = std::make_shared<SpatialMappingFrame>(++m_surfaceId, m_entries);
		}
	}

	//reference: https://forums.hololens.com/discussion/2070/world-coordinates-from-meshes-vertexpositions-data
	std::shared_ptr<SpatialMappingFrameEntry> SpatialMappingModule::CreateFrameEntry(
		Windows::Perception::Spatial::Surfaces::SpatialSurfaceMesh^ spatialSurfaceMesh)
	{
		// Read mesh data from IBuffers and move them to a vector.
		auto positionPointer = Utility::ConvertIBufferToPointer<DirectX::SimpleMath::Vector4>(spatialSurfaceMesh->VertexPositions->Data);
		std::vector<DirectX::SimpleMath::Vector4> positions(positionPointer, positionPointer + spatialSurfaceMesh->VertexPositions->ElementCount);

		auto normalPointer = Utility::ConvertIBufferToPointer<DirectX::SimpleMath::Vector4>(spatialSurfaceMesh->VertexNormals->Data);
		std::vector<DirectX::SimpleMath::Vector4> normals(normalPointer, normalPointer + spatialSurfaceMesh->VertexNormals->ElementCount);

		auto indexPointer = Utility::ConvertIBufferToPointer<unsigned short>(spatialSurfaceMesh->TriangleIndices->Data);
		std::vector<unsigned short> indices(indexPointer, indexPointer + spatialSurfaceMesh->TriangleIndices->ElementCount);

		long long updateTime = spatialSurfaceMesh->SurfaceInfo->UpdateTime.UniversalTime;
		auto coordinateSystem = spatialSurfaceMesh->CoordinateSystem;
		auto vertexPositionScale = Utility::LoadVector3(spatialSurfaceMesh->VertexPositionScale);
		auto surfaceInfo = spatialSurfaceMesh->SurfaceInfo;

		return std::make_shared<SpatialMappingFrameEntry>(
			positions,
			normals,
			indices,
			updateTime,
			coordinateSystem,
			vertexPositionScale,
			surfaceInfo);
	}
}
