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

#pragma once

#include <ppltasks.h>
#include <shared_mutex>

#include "LocatableCameraFrame.h"

namespace HoloLensCppModules
{
	class LocatableCameraModule
	{
	public:
		static Concurrency::task<std::shared_ptr<LocatableCameraModule>> CreateAsync();

		LocatableCameraModule(
			Platform::Agile<Windows::Media::Capture::MediaCapture> mediaCapture,
			Windows::Media::Capture::Frames::MediaFrameReader^ reader,
			Windows::Media::Capture::Frames::MediaFrameSource^ source);

		std::shared_ptr<LocatableCameraFrame> GetFrameContainer();
		Windows::Media::Capture::Frames::VideoMediaFrameFormat^ GetCurrentFormat();

	private:
		void OnFrameArrived(
			Windows::Media::Capture::Frames::MediaFrameReader^ sender,
			Windows::Media::Capture::Frames::MediaFrameArrivedEventArgs^ args);

		Platform::Guid m_coordinateSystemGuid;
		Platform::Guid m_viewTransformGuid;
		Platform::Guid m_projectionTransformGuid;

		Platform::Agile<Windows::Media::Capture::MediaCapture> m_mediaCapture;
		Windows::Media::Capture::Frames::MediaFrameReader^ m_mediaFrameReader;

		std::shared_mutex m_propertiesLock;
		Windows::Media::Capture::Frames::MediaFrameSource^ m_mediaFrameSource;
		std::shared_ptr<LocatableCameraFrame> m_frameContainer;
		uint32 m_frameId;
	};
}