#include "HoloLensCppModulesPch.h"
#include "Utility.h"

#include <robuffer.h>

namespace HoloLensCppModules
{
	bool Utility::IsEmulator()
	{
		auto eas = ref new Windows::Security::ExchangeActiveSyncProvisioning::EasClientDeviceInformation;
		return eas->SystemProductName != L"HoloLens";
	}

	Platform::Guid Utility::StringToGuid(Platform::String^ str)
	{
		GUID rawguid;
		HRESULT hr = IIDFromString(str->Data(), &rawguid);
		if (SUCCEEDED(hr))
		{
			Platform::Guid guid(rawguid);
			return guid;
		}

		Logger::Log(L"failed to create a Guid");
		throw new std::exception("failed to create Guid");
	}

	int Utility::GetGuidHashcode(Platform::Guid guid)
	{
		return guid.GetHashCode();
	}

	DirectX::SimpleMath::Matrix Utility::IBoxArrayToMatrix(Platform::IBoxArray<uint8>^ array)
	{
		float* matrixData = reinterpret_cast<float*>(array->Value->Data);
		return DirectX::SimpleMath::Matrix(
			matrixData[0], matrixData[1], matrixData[2], matrixData[3],
			matrixData[4], matrixData[5], matrixData[6], matrixData[7],
			matrixData[8], matrixData[9], matrixData[10], matrixData[11],
			matrixData[12], matrixData[13], matrixData[14], matrixData[15]);
	}

	Platform::String^ Utility::Vector3ToString(DirectX::SimpleMath::Vector3 vector3)
	{
		return vector3.x + ", " + vector3.y + ", " + vector3.z;
	}

	Platform::String^ Utility::Float3ToString(Windows::Foundation::Numerics::float3 float3)
	{
		return float3.x + ", " + float3.y + ", " + float3.z;
	}

	Platform::String^ Utility::Vector4ToString(DirectX::SimpleMath::Vector4 vector4)
	{
		return vector4.x + ", " + vector4.y + ", " + vector4.z + ", " + vector4.w;
	}

	Platform::String^ Utility::MatrixToString(DirectX::SimpleMath::Matrix matrix)
	{
		Platform::String^ str =
			matrix.m[0][0] + ", " + matrix.m[0][1] + ", " + matrix.m[0][2] + ", " + matrix.m[0][3] + "\n" +
			matrix.m[1][0] + ", " + matrix.m[1][1] + ", " + matrix.m[1][2] + ", " + matrix.m[1][3] + "\n" +
			matrix.m[2][0] + ", " + matrix.m[2][1] + ", " + matrix.m[2][2] + ", " + matrix.m[2][3] + "\n" +
			matrix.m[3][0] + ", " + matrix.m[3][1] + ", " + matrix.m[3][2] + ", " + matrix.m[3][3] + "\n";

		return str;
	}

	DirectX::SimpleMath::Vector3 Utility::LoadVector3(Windows::Foundation::Numerics::float3 float3)
	{
		return DirectX::SimpleMath::Vector3(DirectX::XMLoadFloat3(&float3));
	}

	DirectX::SimpleMath::Quaternion Utility::LoadQuaternion(Windows::Foundation::Numerics::quaternion quaternion)
	{
		return DirectX::SimpleMath::Quaternion(DirectX::XMLoadQuaternion(&quaternion));
	}

	DirectX::SimpleMath::Matrix Utility::LoadMatrix(Windows::Foundation::Numerics::float4x4 float4x4)
	{
		return DirectX::SimpleMath::Matrix(DirectX::XMLoadFloat4x4(&float4x4));
	}

	// Assumes that w is 1.
	DirectX::SimpleMath::Vector3 Utility::ConvertVector4ToVector3(DirectX::SimpleMath::Vector4 vector4)
	{
		float x = vector4.x;
		float y = vector4.y;
		float z = vector4.z;
		float w = vector4.w;

		if (w != 1.0f)
		{
			Logger::Log(L"non-zero w found: " + w);
		}

		return DirectX::SimpleMath::Vector3(x, y, z);
	}

	DirectX::SimpleMath::Matrix Utility::CalculateLocatableCameraModelMatrix(
		Windows::Perception::Spatial::SpatialCoordinateSystem^ baseCoordinateSystem,
		Windows::Perception::Spatial::SpatialCoordinateSystem^ locatableCameraCoordinateSystem,
		DirectX::SimpleMath::Matrix locatableCameraViewMatrix)
	{
		auto cameraCoordinateSystemTransformBox(locatableCameraCoordinateSystem->TryGetTransformTo(baseCoordinateSystem));
		auto cameraCoordinateSystemTransform = Utility::LoadMatrix(cameraCoordinateSystemTransformBox->Value);
		return locatableCameraViewMatrix.Invert() * cameraCoordinateSystemTransform;
	}

	DirectX::SimpleMath::Matrix Utility::CalculateSpatialSurfaceModelMatrix(
		Windows::Perception::Spatial::SpatialCoordinateSystem^ baseCoordinateSystem,
		Windows::Perception::Spatial::SpatialCoordinateSystem^ spatialSurfaceCoordinateSystem,
		DirectX::SimpleMath::Vector3 vertexPositionScale)
	{
		auto spatialSurfaceCoordinateSystemTransformBox = spatialSurfaceCoordinateSystem->TryGetTransformTo(baseCoordinateSystem);
		auto spatialSurfaceCoordinateSystemTransform = Utility::LoadMatrix(spatialSurfaceCoordinateSystemTransformBox->Value);

		return DirectX::SimpleMath::Matrix::CreateScale(vertexPositionScale) * spatialSurfaceCoordinateSystemTransform;
	}

	DirectX::SimpleMath::Matrix Utility::GetTransformWithoutTranslation(DirectX::SimpleMath::Matrix transform)
	{
		auto transformWithoutTranslation(transform);
		transformWithoutTranslation._41 = 0.0f;
		transformWithoutTranslation._42 = 0.0f;
		transformWithoutTranslation._43 = 0.0f;

		return transformWithoutTranslation;
	}

	DirectX::SimpleMath::Ray Utility::GetTransformedRay(DirectX::SimpleMath::Ray ray, DirectX::SimpleMath::Matrix transform)
	{
		using DirectX::SimpleMath::Vector3;

		auto position = Vector3::Transform(ray.position, transform);
		auto direction = Vector3::TransformNormal(ray.direction, transform);
		if (direction.Length() == 0)
		{
			Logger::Log(L"zero length direction");
			throw std::exception("zero length direction");
		}
		direction.Normalize();

		return DirectX::SimpleMath::Ray(position, direction);
	}
}
