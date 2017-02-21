#pragma once

#include <SimpleMath.h>
#include <robuffer.h>

namespace HoloLensCppModules
{
	// All the utility functions for AR-handwriting is here.
	class Utility
	{
	private:
		Utility() {}

	public:
		static bool IsEmulator();
		static Platform::Guid StringToGuid(Platform::String^ str);
		static int GetGuidHashcode(Platform::Guid guid);
		static DirectX::SimpleMath::Matrix IBoxArrayToMatrix(Platform::IBoxArray<uint8>^ array);
		static Platform::String^ Vector3ToString(DirectX::SimpleMath::Vector3 vector3);
		static Platform::String^ Float3ToString(Windows::Foundation::Numerics::float3 float3);
		static Platform::String^ Vector4ToString(DirectX::SimpleMath::Vector4 vector4);
		static Platform::String^ MatrixToString(DirectX::SimpleMath::Matrix matrix);
		static DirectX::SimpleMath::Vector3 LoadVector3(Windows::Foundation::Numerics::float3 float3);
		static DirectX::SimpleMath::Quaternion LoadQuaternion(Windows::Foundation::Numerics::quaternion quaternion);
		static DirectX::SimpleMath::Matrix LoadMatrix(Windows::Foundation::Numerics::float4x4 float4x4);
		static DirectX::SimpleMath::Vector3 ConvertVector4ToVector3(DirectX::SimpleMath::Vector4 vector4);
		static DirectX::SimpleMath::Matrix CalculateLocatableCameraModelMatrix(
			Windows::Perception::Spatial::SpatialCoordinateSystem^ baseCoordinateSystem,
			Windows::Perception::Spatial::SpatialCoordinateSystem^ locatableCameraCoordinateSystem,
			DirectX::SimpleMath::Matrix locatableCameraViewMatrix);
		static DirectX::SimpleMath::Matrix CalculateSpatialSurfaceModelMatrix(
			Windows::Perception::Spatial::SpatialCoordinateSystem^ baseCoordinateSystem,
			Windows::Perception::Spatial::SpatialCoordinateSystem^ spatialSurfaceCoordinateSystem,
			DirectX::SimpleMath::Vector3 vertexPositionScale);
		static DirectX::SimpleMath::Vector3 CalculateHololensPreviewCameraDirection(float pixelX, float pixelY);
		static DirectX::SimpleMath::Matrix GetTransformWithoutTranslation(DirectX::SimpleMath::Matrix transform);
		static DirectX::SimpleMath::Ray GetTransformedRay(DirectX::SimpleMath::Ray ray, DirectX::SimpleMath::Matrix transform);

		/*
		* Template functions should be implemented in a header file.
		*/

		template<typename T>
		static void RemoveFromVector(std::vector<T>& vector, const T& element)
		{
			vector.erase(std::remove(vector.begin(), vector.end(), element), vector.end());
		}

		template<typename K, typename V>
		static bool MapContainsKey(const std::map<K, V>& map, K key)
		{
			return map.find(key) != map.end();
		}

		// reference: https://github.com/Microsoft/Windows-universal-samples/blob/9e7fa281b4a4c208992ce22385ebcf57d37f9175/Samples/HolographicSpatialMapping/cpp/Content/GetDataFromIBuffer.h
		template <typename T>
		static T* ConvertIBufferToPointer(Windows::Storage::Streams::IBuffer^ buffer)
		{
			using Microsoft::WRL::ComPtr;
			using Windows::Storage::Streams::IBufferByteAccess;

			if (buffer == nullptr)
			{
				return nullptr;
			}

			if (buffer->Length <= 0)
			{
				return nullptr;
			}

			ComPtr<IUnknown> pUnknown = reinterpret_cast<IUnknown*>(buffer);
			ComPtr<IBufferByteAccess> spByteAccess;
			if (FAILED(pUnknown.As(&spByteAccess)))
			{
				return nullptr;
			}

			byte* pRawData = nullptr;
			if (FAILED(spByteAccess->Buffer(&pRawData)))
			{
				return nullptr;
			}

			return reinterpret_cast<T*>(pRawData);
		}

		template <typename T>
		static size_t GetByteSizeOfVector(const std::vector<T>& vector)
		{
			return vector.size() * sizeof(T);
		}

		template <typename T>
		static size_t GetByteSizeOfElement(const std::vector<T>& vector)
		{
			return sizeof(T);
		}

		template <typename T>
		static void CreateD3D11VertexBuffer(
			ID3D11Device* context,
			std::vector<T> vector,
			Microsoft::WRL::ComPtr<ID3D11Buffer>& buffer)
		{
			D3D11_SUBRESOURCE_DATA bufferData = { vector.data(), 0, 0 };
			const CD3D11_BUFFER_DESC bufferDescription(Utility::GetByteSizeOfVector(vector), D3D11_BIND_VERTEX_BUFFER);
			DX::ThrowIfFailed(
				context->CreateBuffer(
					&bufferDescription,
					&bufferData,
					&buffer)
			);
		}

		template <typename T>
		static void CreateD3D11VertexBuffer(
			ID3D11Device* context,
			Windows::Storage::Streams::IBuffer^ iBuffer,
			Microsoft::WRL::ComPtr<ID3D11Buffer>& buffer)
		{
			auto length = iBuffer->Length;

			D3D11_SUBRESOURCE_DATA bufferData = { ConvertIBufferToPointer<T>(iBuffer), 0, 0 };
			CD3D11_BUFFER_DESC bufferDescription(iBuffer->Length, D3D11_BIND_VERTEX_BUFFER);
			DX::ThrowIfFailed(
				context->CreateBuffer(
					&bufferDescription,
					&bufferData,
					&buffer)
			);
		}

		template <typename T>
		static void CreateD3D11IndexBuffer(
			ID3D11Device* context,
			std::vector<T> vector,
			Microsoft::WRL::ComPtr<ID3D11Buffer>& buffer)
		{
			D3D11_SUBRESOURCE_DATA bufferData = { vector.data(), 0, 0 };
			const CD3D11_BUFFER_DESC bufferDescription(Utility::GetByteSizeOfVector(vector), D3D11_BIND_INDEX_BUFFER);
			DX::ThrowIfFailed(
				context->CreateBuffer(
					&bufferDescription,
					&bufferData,
					&buffer)
			);
		}

		template <typename T>
		static void CreateD3D11IndexBuffer(
			ID3D11Device* context,
			Windows::Storage::Streams::IBuffer^ iBuffer,
			Microsoft::WRL::ComPtr<ID3D11Buffer>& buffer)
		{
			auto length = iBuffer->Length;

			D3D11_SUBRESOURCE_DATA bufferData = { ConvertIBufferToPointer<T>(iBuffer), 0, 0 };
			CD3D11_BUFFER_DESC bufferDescription(iBuffer->Length, D3D11_BIND_INDEX_BUFFER);
			DX::ThrowIfFailed(
				context->CreateBuffer(
					&bufferDescription,
					&bufferData,
					&buffer)
			);
		}
	};
}