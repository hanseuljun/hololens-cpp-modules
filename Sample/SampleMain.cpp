#include "SamplePch.h"
#include "SampleMain.h"
#include "Common\DirectXHelper.h"

#include <windows.graphics.directx.direct3d11.interop.h>
#include <Collection.h>

namespace HoloLensCppModules
{
	// Loads and initializes application assets when the application is loaded.
	SampleMain::SampleMain(const std::shared_ptr<DX::DeviceResources>& deviceResources) :
		m_deviceResources(deviceResources)
	{
		// Register to be notified if the device is lost or recreated.
		m_deviceResources->RegisterDeviceNotify(this);
	}

	void SampleMain::SetHolographicSpace(Windows::Graphics::Holographic::HolographicSpace^ holographicSpace)
	{
		using Platform::Object;
		using Windows::Graphics::Holographic::HolographicSpace;
		using Windows::Graphics::Holographic::HolographicSpaceCameraAddedEventArgs;
		using Windows::Graphics::Holographic::HolographicSpaceCameraRemovedEventArgs;
		using Windows::Perception::Spatial::SpatialLocator;
		using std::placeholders::_1;
		using std::placeholders::_2;

		UnregisterHolographicEventHandlers();

		m_holographicSpace = holographicSpace;

		//
		// TODO: Add code here to initialize your holographic content.
		//

		// Use the default SpatialLocator to track the motion of the device.
		m_locator = SpatialLocator::GetDefault();

		// Be able to respond to changes in the positional tracking state.
		m_locatabilityChangedToken =
			m_locator->LocatabilityChanged +=
			ref new Windows::Foundation::TypedEventHandler<SpatialLocator^, Object^>(
				std::bind(&SampleMain::OnLocatabilityChanged, this, _1, _2)
				);

		// Respond to camera added events by creating any resources that are specific
		// to that camera, such as the back buffer render target view.
		// When we add an event handler for CameraAdded, the API layer will avoid putting
		// the new camera in new HolographicFrames until we complete the deferral we created
		// for that handler, or return from the handler without creating a deferral. This
		// allows the app to take more than one frame to finish creating resources and
		// loading assets for the new holographic camera.
		// This function should be registered before the app creates any HolographicFrames.
		m_cameraAddedToken =
			m_holographicSpace->CameraAdded +=
			ref new Windows::Foundation::TypedEventHandler<HolographicSpace^, HolographicSpaceCameraAddedEventArgs^>(
				std::bind(&SampleMain::OnCameraAdded, this, _1, _2)
				);

		// Respond to camera removed events by releasing resources that were created for that
		// camera.
		// When the app receives a CameraRemoved event, it releases all references to the back
		// buffer right away. This includes render target views, Direct2D target bitmaps, and so on.
		// The app must also ensure that the back buffer is not attached as a render target, as
		// shown in DeviceResources::ReleaseResourcesForBackBuffer.
		m_cameraRemovedToken =
			m_holographicSpace->CameraRemoved +=
			ref new Windows::Foundation::TypedEventHandler<HolographicSpace^, HolographicSpaceCameraRemovedEventArgs^>(
				std::bind(&SampleMain::OnCameraRemoved, this, _1, _2)
				);

		// The simplest way to render world-locked holograms is to create a stationary reference frame
		// when the app is launched. This is roughly analogous to creating a "world" coordinate system
		// with the origin placed at the device's position as the app is launched.
		m_referenceFrame = m_locator->CreateStationaryFrameOfReferenceAtCurrentLocation();

		// Notes on spatial tracking APIs:
		// * Stationary reference frames are designed to provide a best-fit position relative to the
		//   overall space. Individual positions within that reference frame are allowed to drift slightly
		//   as the device learns more about the environment.
		// * When precise placement of individual holograms is required, a SpatialAnchor should be used to
		//   anchor the individual hologram to a position in the real world - for example, a point the user
		//   indicates to be of special interest. Anchor positions do not drift, but can be corrected; the
		//   anchor will use the corrected position starting in the next frame after the correction has
		//   occurred.

		LoadLocatableCameraModuleAsync();
	}

	void SampleMain::UnregisterHolographicEventHandlers()
	{
		if (m_holographicSpace != nullptr)
		{
			// Clear previous event registrations.

			if (m_cameraAddedToken.Value != 0)
			{
				m_holographicSpace->CameraAdded -= m_cameraAddedToken;
				m_cameraAddedToken.Value = 0;
			}

			if (m_cameraRemovedToken.Value != 0)
			{
				m_holographicSpace->CameraRemoved -= m_cameraRemovedToken;
				m_cameraRemovedToken.Value = 0;
			}
		}

		if (m_locator != nullptr)
		{
			m_locator->LocatabilityChanged -= m_locatabilityChangedToken;
		}
	}

	SampleMain::~SampleMain()
	{
		// Deregister device notification.
		m_deviceResources->RegisterDeviceNotify(nullptr);

		UnregisterHolographicEventHandlers();
	}

	// Updates the application state once per frame.
	Windows::Graphics::Holographic::HolographicFrame^ SampleMain::Update()
	{
		using Windows::Graphics::Holographic::HolographicFrame;
		using Windows::Graphics::Holographic::HolographicFramePrediction;
		using Windows::Graphics::Holographic::HolographicCameraRenderingParameters;
		using Windows::Perception::Spatial::SpatialCoordinateSystem;
		using Windows::UI::Input::Spatial::SpatialInteractionSourceState;

		// Before doing the timer update, there is some work to do per-frame
		// to maintain holographic rendering. First, we will get information
		// about the current frame.

		// The HolographicFrame has information that the app needs in order
		// to update and render the current frame. The app begins each new
		// frame by calling CreateNextFrame.
		HolographicFrame^ holographicFrame = m_holographicSpace->CreateNextFrame();

		// Get a prediction of where holographic cameras will be when this frame
		// is presented.
		HolographicFramePrediction^ prediction = holographicFrame->CurrentPrediction;

		// Back buffers can change from frame to frame. Validate each buffer, and recreate
		// resource views and depth buffers as needed.
		m_deviceResources->EnsureCameraResources(holographicFrame, prediction);

		// Next, we get a coordinate system from the attached frame of reference that is
		// associated with the current frame. Later, this coordinate system is used for
		// for creating the stereo view matrices when rendering the sample content.
		SpatialCoordinateSystem^ currentCoordinateSystem = m_referenceFrame->CoordinateSystem;

		m_timer.Tick([&]()
		{
			//
			// TODO: Update scene objects.
			//
			// Put time-based updates here. By default this code will run once per frame,
			// but if you change the StepTimer to use a fixed time step this code will
			// run as many times as needed to get to the current step.
			//
		});

		// We complete the frame update by using information about our content positioning
		// to set the focus point.

		for (auto cameraPose : prediction->CameraPoses)
		{
		}

		if (m_locatableCameraModule != nullptr)
		{
			// Access to the current locatable camera frame.
			auto locatableCameraFrame = m_locatableCameraModule->GetFrame();
			
			if (locatableCameraFrame != nullptr)
			{
				int id = locatableCameraFrame->GetId();
				if (id % 100 == 0 && id != m_locatableCameraFrame->GetId())
				{
					Logger::Log(L"Found a locatable camera frame with ID " + id + L"!");
				}
			}
			else
			{
				Logger::Log(L"Locatable camera frame is not available yet.");
			}

			m_locatableCameraFrame = locatableCameraFrame;
		}

		if (m_spatialMappingModule != nullptr)
		{
			// Update the bounding volume of the spatial mapping module with the application's starting point.
			m_spatialMappingModule->UpdateBoundingVolume(currentCoordinateSystem);

			// Access to the current spatial mapping frame.
			auto spatialMappingFrame = m_spatialMappingModule->GetFrame();
			if (spatialMappingFrame != nullptr)
			{
				int id = spatialMappingFrame->GetId();
				if (id % 5 == 0 && id != m_spatialMappingFrame->GetId())
				{
					Logger::Log(L"Found a spatial mapping frame with ID " + id + L"!");
					int surfaceCount = spatialMappingFrame->GetEntries().size();
					Logger::Log(L"It contains " + surfaceCount + L" surfaces.");
				}
			}
			else
			{
				Logger::Log(L"SpatialMappingFrame is not available yet.");
			}

			m_spatialMappingFrame = spatialMappingFrame;
		}

		// The holographic frame will be used to get up-to-date view and projection matrices and
		// to present the swap chain.
		return holographicFrame;
	}

	// Renders the current frame to each holographic camera, according to the
	// current application and spatial positioning state. Returns true if the
	// frame was rendered to at least one camera.
	bool SampleMain::Render(Windows::Graphics::Holographic::HolographicFrame^ holographicFrame)
	{
		using Windows::Graphics::Holographic::HolographicFramePrediction;

		// Don't try to render anything before the first Update.
		if (m_timer.GetFrameCount() == 0)
		{
			return false;
		}

		//
		// TODO: Add code for pre-pass rendering here.
		//
		// Take care of any tasks that are not specific to an individual holographic
		// camera. This includes anything that doesn't need the final view or projection
		// matrix, such as lighting maps.
		//

		// Lock the set of holographic camera resources, then draw to each camera
		// in this frame.
		return m_deviceResources->UseHolographicCameraResources<bool>(
			[this, holographicFrame](std::map<UINT32, std::unique_ptr<DX::CameraResources>>& cameraResourceMap)
		{
			// Up-to-date frame predictions enhance the effectiveness of image stablization and
			// allow more accurate positioning of holograms.
			holographicFrame->UpdateCurrentPrediction();
			HolographicFramePrediction^ prediction = holographicFrame->CurrentPrediction;

			bool atLeastOneCameraRendered = false;
			for (auto cameraPose : prediction->CameraPoses)
			{
				// This represents the device-based resources for a HolographicCamera.
				DX::CameraResources* pCameraResources = cameraResourceMap[cameraPose->HolographicCamera->Id].get();

				// Get the device context.
				const auto context = m_deviceResources->GetD3DDeviceContext();
				const auto depthStencilView = pCameraResources->GetDepthStencilView();

				// Set render targets to the current holographic camera.
				ID3D11RenderTargetView *const targets[1] = { pCameraResources->GetBackBufferRenderTargetView() };
				context->OMSetRenderTargets(1, targets, depthStencilView);

				// Clear the back buffer and depth stencil view.
				context->ClearRenderTargetView(targets[0], DirectX::Colors::Transparent);
				context->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

				//
				// TODO: Replace the sample content with your own content.
				//
				// Notes regarding holographic content:
				//    * For drawing, remember that you have the potential to fill twice as many pixels
				//      in a stereoscopic render target as compared to a non-stereoscopic render target
				//      of the same resolution. Avoid unnecessary or repeated writes to the same pixel,
				//      and only draw holograms that the user can see.
				//    * To help occlude hologram geometry, you can create a depth map using geometry
				//      data obtained via the surface mapping APIs. You can use this depth map to avoid
				//      rendering holograms that are intended to be hidden behind tables, walls,
				//      monitors, and so on.
				//    * Black pixels will appear transparent to the user wearing the device, but you
				//      should still use alpha blending to draw semitransparent holograms. You should
				//      also clear the screen to Transparent as shown above.
				//


				// The view and projection matrices for each holographic camera will change
				// every frame. This function refreshes the data in the constant buffer for
				// the holographic camera indicated by cameraPose.
				pCameraResources->UpdateViewProjectionBuffer(m_deviceResources, cameraPose, m_referenceFrame->CoordinateSystem);

				// Attach the view/projection constant buffer for this camera to the graphics pipeline.
				bool cameraActive = pCameraResources->AttachViewProjectionBuffer(m_deviceResources);

				atLeastOneCameraRendered = true;
			}

			return atLeastOneCameraRendered;
		});
	}

	void SampleMain::SaveAppState()
	{
		//
		// TODO: Insert code here to save your app state.
		//       This method is called when the app is about to suspend.
		//
		//       For example, store information in the SpatialAnchorStore.
		//
	}

	void SampleMain::LoadAppState()
	{
		//
		// TODO: Insert code here to load your app state.
		//       This method is called when the app resumes.
		//
		//       For example, load information from the SpatialAnchorStore.
		//
	}

	// Notifies classes that use Direct3D device resources that the device resources
	// need to be released before this method returns.
	void SampleMain::OnDeviceLost()
	{
		m_locatableCameraModule = nullptr;
		m_spatialMappingModule = nullptr;
	}

	// Notifies classes that use Direct3D device resources that the device resources
	// may now be recreated.
	void SampleMain::OnDeviceRestored()
	{
		LoadLocatableCameraModuleAsync();
	}

	void SampleMain::OnLocatabilityChanged(Windows::Perception::Spatial::SpatialLocator^ sender, Platform::Object^ args)
	{
		using Platform::String;
		using Windows::Perception::Spatial::SpatialLocatability;

		switch (sender->Locatability)
		{
		case SpatialLocatability::Unavailable:
			// Holograms cannot be rendered.
		{
			String^ message = L"Warning! Positional tracking is " +
				sender->Locatability.ToString() + L".\n";
			OutputDebugStringW(message->Data());
		}
		break;

		// In the following three cases, it is still possible to place holograms using a
		// SpatialLocatorAttachedFrameOfReference.
		case SpatialLocatability::PositionalTrackingActivating:
			// The system is preparing to use positional tracking.

		case SpatialLocatability::OrientationOnly:
			// Positional tracking has not been activated.

		case SpatialLocatability::PositionalTrackingInhibited:
			// Positional tracking is temporarily inhibited. User action may be required
			// in order to restore positional tracking.
			break;

		case SpatialLocatability::PositionalTrackingActive:
			// Positional tracking is active. World-locked content can be rendered.
			break;
		}
	}

	void SampleMain::OnCameraAdded(
		Windows::Graphics::Holographic::HolographicSpace^ sender,
		Windows::Graphics::Holographic::HolographicSpaceCameraAddedEventArgs^ args
	)
	{
		using Windows::Foundation::Deferral;
		using Windows::Graphics::Holographic::HolographicCamera;

		Deferral^ deferral = args->GetDeferral();
		HolographicCamera^ holographicCamera = args->Camera;
		Concurrency::create_task([this, deferral, holographicCamera]()
		{
			//
			// TODO: Allocate resources for the new camera and load any content specific to
			//       that camera. Note that the render target size (in pixels) is a property
			//       of the HolographicCamera object, and can be used to create off-screen
			//       render targets that match the resolution of the HolographicCamera.
			//

			// Create device-based resources for the holographic camera and add it to the list of
			// cameras used for updates and rendering. Notes:
			//   * Since this function may be called at any time, the AddHolographicCamera function
			//     waits until it can get a lock on the set of holographic camera resources before
			//     adding the new camera. At 60 frames per second this wait should not take long.
			//   * A subsequent Update will take the back buffer from the RenderingParameters of this
			//     camera's CameraPose and use it to create the ID3D11RenderTargetView for this camera.
			//     Content can then be rendered for the HolographicCamera.
			m_deviceResources->AddHolographicCamera(holographicCamera);

			// Holographic frame predictions will not include any information about this camera until
			// the deferral is completed.
			deferral->Complete();
		});
	}

	void SampleMain::OnCameraRemoved(
		Windows::Graphics::Holographic::HolographicSpace^ sender,
		Windows::Graphics::Holographic::HolographicSpaceCameraRemovedEventArgs^ args
	)
	{
		Concurrency::create_task([this]()
		{
			//
			// TODO: Asynchronously unload or deactivate content resources (not back buffer 
			//       resources) that are specific only to the camera that was removed.
			//
		});

		// Before letting this callback return, ensure that all references to the back buffer 
		// are released.
		// Since this function may be called at any time, the RemoveHolographicCamera function
		// waits until it can get a lock on the set of holographic camera resources before
		// deallocating resources for this camera. At 60 frames per second this wait should
		// not take long.
		m_deviceResources->RemoveHolographicCamera(args->Camera);
	}

	void SampleMain::LoadLocatableCameraModuleAsync()
	{
		using Windows::Perception::Spatial::SpatialPerceptionAccessStatus;
		using Windows::Perception::Spatial::Surfaces::SpatialSurfaceObserver;

		LocatableCameraModule::CreateAsync().then([this](std::shared_ptr<LocatableCameraModule> module)
		{
			m_locatableCameraModule = std::move(module);
		});

		// The spatial mapping API reads information about the user's environment. The user must
		// grant permission to the app to use this capability of the Windows Holographic device.
		auto requestSpatialMappingAccessTask = concurrency::create_task(SpatialSurfaceObserver::RequestAccessAsync());
		requestSpatialMappingAccessTask.then([this](SpatialPerceptionAccessStatus status)
		{
			// For info on what else can cause this, see: http://msdn.microsoft.com/library/windows/apps/mt621422.aspx
			if (status == SpatialPerceptionAccessStatus::Allowed)
			{
				// Create the spatial mapping module.
				m_spatialMappingModule = std::move(std::make_shared<SpatialMappingModule>());
			}
			// Access was denied. This usually happens because your AppX manifest file does not declare the
			// spatialPerception capability.
			else
			{
				Logger::Log(L"Access denied.");
			}
		});
	}
}