
//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

#include "HoloLensCppModulesPch.h"
#include "LocatableCameraModule.h"

namespace HoloLensCppModules
{
	// strings for GUIDs are from https://developer.microsoft.com/ko-kr/windows/holographic/locatable_camera
	LocatableCameraModule::LocatableCameraModule(
		Platform::Agile<Windows::Media::Capture::MediaCapture> mediaCapture,
		Windows::Media::Capture::Frames::MediaFrameReader^ reader,
		Windows::Media::Capture::Frames::MediaFrameSource^ source)
		: m_coordinateSystemGuid(Utility::StringToGuid(L"{9D13C82F-2199-4E67-91CD-D1A4181F2534}"))
		, m_viewTransformGuid(Utility::StringToGuid(L"{4E251FA4-830F-4770-859A-4B8D99AA809B}"))
		, m_projectionTransformGuid(Utility::StringToGuid(L"{47F9FCB5-2A02-4F26-A477-792FDF95886A}"))
		, m_mediaCapture(std::move(mediaCapture))
		, m_mediaFrameReader(std::move(reader))
		, m_mediaFrameSource(std::move(source))
		, m_frameId(0)
	{
		using Windows::Foundation::TypedEventHandler;
		using Windows::Media::Capture::Frames::MediaFrameArrivedEventArgs;
		using Windows::Media::Capture::Frames::MediaFrameReader;
		using std::placeholders::_1;
		using std::placeholders::_2;

		// Listen for new frames, so we know when to update our m_latestFrame
		m_mediaFrameReader->FrameArrived +=
			ref new TypedEventHandler<MediaFrameReader^, MediaFrameArrivedEventArgs^>(
				std::bind(&LocatableCameraModule::OnFrameArrived, this, _1, _2));
	}

	concurrency::task<std::shared_ptr<LocatableCameraModule>> LocatableCameraModule::CreateAsync()
	{
		using Windows::Media::Capture::MediaCapture;
		using Windows::Media::Capture::MediaCaptureInitializationSettings;
		using Windows::Media::Capture::MediaCaptureMemoryPreference;
		using Windows::Media::Capture::StreamingCaptureMode;
		using Windows::Media::Capture::Frames::MediaFrameSource;
		using Windows::Media::Capture::Frames::MediaFrameSourceGroup;
		using Windows::Media::Capture::Frames::MediaFrameSourceInfo;
		using Windows::Media::Capture::Frames::MediaFrameSourceKind;
		using Windows::Media::Capture::Frames::MediaFrameReader;
		using Windows::Media::Capture::Frames::MediaFrameReaderStartStatus;

		return Concurrency::create_task(MediaFrameSourceGroup::FindAllAsync())
			.then([](Windows::Foundation::Collections::IVectorView<MediaFrameSourceGroup^>^ groups)
		{
			MediaFrameSourceGroup^ selectedGroup = nullptr;
			MediaFrameSourceInfo^ selectedSourceInfo = nullptr;

			// Pick first color source.
			for (auto sourceGroup : groups)
			{
				for (auto sourceInfo : sourceGroup->SourceInfos)
				{
					if (sourceInfo->SourceKind == MediaFrameSourceKind::Color)
					{
						selectedSourceInfo = sourceInfo;
						break;
					}
				}

				if (selectedSourceInfo != nullptr)
				{
					selectedGroup = sourceGroup;
					break;
				}
			}

			// No valid camera was found. This will happen on the emulator.
			if (selectedGroup == nullptr || selectedSourceInfo == nullptr)
			{
				return concurrency::task_from_result(std::shared_ptr<LocatableCameraModule>(nullptr));
			}

			auto settings = ref new MediaCaptureInitializationSettings();
			settings->MemoryPreference = MediaCaptureMemoryPreference::Cpu; // Need SoftwareBitmaps for FaceAnalysis
			settings->StreamingCaptureMode = StreamingCaptureMode::Video;   // Only need to stream video
			settings->SourceGroup = selectedGroup;

			Platform::Agile<MediaCapture> mediaCapture(ref new MediaCapture());

			return concurrency::create_task(mediaCapture->InitializeAsync(settings))
				.then([=]
			{
				MediaFrameSource^ selectedSource = mediaCapture->FrameSources->Lookup(selectedSourceInfo->Id);

				return concurrency::create_task(mediaCapture->CreateFrameReaderAsync(selectedSource))
					.then([=](MediaFrameReader^ reader)
				{
					return concurrency::create_task(reader->StartAsync())
						.then([=](MediaFrameReaderStartStatus status)
					{
						// Only create a VideoFrameProcessor if the reader successfully started
						if (status == MediaFrameReaderStartStatus::Success)
						{
							return std::make_shared<LocatableCameraModule>(mediaCapture, reader, selectedSource);
						}
						else
						{
							return std::shared_ptr<LocatableCameraModule>(nullptr);
						}
					});
				});
			});
		});
	}

	std::shared_ptr<LocatableCameraFrame> LocatableCameraModule::GetFrame()
	{
		auto lock = std::shared_lock<std::shared_mutex>(m_propertiesLock);
		return m_frame;
	}

	void LocatableCameraModule::OnFrameArrived(
		Windows::Media::Capture::Frames::MediaFrameReader^ sender,
		Windows::Media::Capture::Frames::MediaFrameArrivedEventArgs^ args)
	{
		using Windows::Graphics::Imaging::BitmapPixelFormat;
		using Windows::Graphics::Imaging::SoftwareBitmap;
		using Windows::Media::Capture::Frames::MediaFrameReference;
		using Windows::Perception::Spatial::SpatialCoordinateSystem;

		if (MediaFrameReference^ frame = sender->TryAcquireLatestFrame())
		{
			if (auto videoMediaFrame = frame->VideoMediaFrame)
			{
				if (auto softwareBitmap = videoMediaFrame->SoftwareBitmap)
				{
					try
					{
						// Accessing properties can result in throwing an exception since mediaFrameReference can be disposed inside the VideoFrameProcessor.
						// Handling this part before performance demanding processes tends to prevent throw exceptions.

						// cameraSpatialCoordinateSystem only contains translation
						auto cameraSpatialCoordinateSystemObject(frame->Properties->Lookup(m_coordinateSystemGuid));
						auto cameraSpatialCoordinateSystem(safe_cast<SpatialCoordinateSystem^>(cameraSpatialCoordinateSystemObject));
						// cameraViewTransform consists of rotation and translation
						auto cameraViewTransformBox(safe_cast<Platform::IBoxArray<uint8>^>(frame->Properties->Lookup(m_viewTransformGuid)));
						auto cameraProjectionTransformBox = safe_cast<Platform::IBoxArray<uint8>^>(frame->Properties->Lookup(m_projectionTransformGuid));
						auto cameraViewTransform(Utility::IBoxArrayToMatrix(cameraViewTransformBox));
						auto cameraProjectionTransform = Utility::IBoxArrayToMatrix(cameraProjectionTransformBox);

						softwareBitmap = SoftwareBitmap::Convert(softwareBitmap, BitmapPixelFormat::Bgra8);

						{
							std::lock_guard<std::shared_mutex> lock(m_propertiesLock);

							m_frame = std::make_shared<LocatableCameraFrame>(
								++m_frameId, softwareBitmap, cameraSpatialCoordinateSystem,
								cameraViewTransform, cameraProjectionTransform);
						}
					}
					catch (Platform::InvalidCastException^ e)
					{
						Logger::Log(L"InvalidCastException occured while using mediaFrameReference->Properties in VideoFrameProcessor::OnFrameArrived");
						Logger::Log(e->Message);
						// Remove this exception throw when not debugging.
						throw e;
					}
				}
			}

		}
	}
}