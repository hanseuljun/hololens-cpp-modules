#include "HoloLensCppModulesPch.h"
#include "RayCaster.h"

#include "Utility.h"

namespace HoloLensCppModules
{
	std::shared_ptr<RayCastResult> RayCaster::RayCast(
		DirectX::SimpleMath::Vector2 pixel,
		std::shared_ptr<LocatableCameraFrame> locatableCameraFrame,
		std::shared_ptr<SpatialMappingFrame> spatialMappingFrame,
		Windows::Perception::Spatial::SpatialCoordinateSystem^ currentCoordinateSystem)
	{
		using DirectX::SimpleMath::Vector3;

		// Converts a SpatialCoordinateSystem to a matrix.
		auto locatableCameraModelMatrix = Utility::CalculateLocatableCameraModelMatrix(
			currentCoordinateSystem,
			locatableCameraFrame->GetCoordinateSystem(),
			locatableCameraFrame->GetViewTransform());

		// Gets the ray using camera's intrinsic parameters.
		auto locatableCameraRayDirection = CalculateHololensPreviewCameraDirection(pixel, locatableCameraFrame);

		// The ray in the locatable camera's coordinate system.
		DirectX::SimpleMath::Ray locatableCameraRay(Vector3::Zero, locatableCameraRayDirection);

		// The ray in the world coordinate system (in other words, baseCoordinateSystem).
		auto worldRay = Utility::GetTransformedRay(locatableCameraRay, locatableCameraModelMatrix);

		return RayCast(worldRay, spatialMappingFrame, currentCoordinateSystem);
	}

	std::shared_ptr<RayCastResult> RayCaster::RayCast(
		DirectX::SimpleMath::Ray ray,
		std::shared_ptr<SpatialMappingFrame> spatialMappingFrame,
		Windows::Perception::Spatial::SpatialCoordinateSystem^ currentCoordinateSystem)
	{
		using DirectX::SimpleMath::Vector3;
		using DirectX::SimpleMath::Ray;

		std::vector<Vector3> worldRayCastHitPositions;

		// Iterates through all containers in the group and saves a result (raycast's destination) from each of them.
		for (auto& entryPair : spatialMappingFrame->GetEntries())
		{
			auto spatialSurfaceContainer = entryPair.second;

			// Gets the surface's bounding box and raycast to it.
			auto boundsBox = spatialSurfaceContainer->GetSurfaceInfo()->TryGetBounds(currentCoordinateSystem);
			auto bounds = boundsBox->Value;

			DirectX::BoundingOrientedBox boundingOrientedBox(
				Utility::LoadVector3(bounds.Center),
				Utility::LoadVector3(bounds.Extents),
				Utility::LoadQuaternion(bounds.Orientation));
			float boundsDistance;

			bool boundsIntersect = boundingOrientedBox.Intersects(ray.position, ray.direction, boundsDistance);
			// If the ray does not meet the bounding box, skip this surface for better performance.
			if (!boundsIntersect)
			{
				continue;
			}

			auto spatialSurfaceModelMatrix = Utility::CalculateSpatialSurfaceModelMatrix(
				currentCoordinateSystem,
				spatialSurfaceContainer->GetCoordinateSystem(),
				spatialSurfaceContainer->GetVertexPositionScale()
			);

			// The ray in the spatialSurfaceContainer's coordinate system.
			auto spatialSurfaceRay = Utility::GetTransformedRay(ray, spatialSurfaceModelMatrix.Invert());

			auto positions = spatialSurfaceContainer->GetPositions();
			auto indices = spatialSurfaceContainer->GetIndices();
			size_t triangleCount = indices.size() / 3;

			for (size_t i = 0; i < triangleCount; ++i)
			{
				float distance;
				// This is possible since the address of Vector4's x, y, and z coincides those of Vector3.
				if (spatialSurfaceRay.Intersects(
					*reinterpret_cast<Vector3*>(&positions[indices[i * 3]]),
					*reinterpret_cast<Vector3*>(&positions[indices[i * 3 + 1]]),
					*reinterpret_cast<Vector3*>(&positions[indices[i * 3 + 2]]),
					distance))
				{
					auto intersectionPosition = spatialSurfaceRay.position + spatialSurfaceRay.direction * distance;
					auto worldRayCastHitPosition = Vector3::Transform(intersectionPosition, spatialSurfaceModelMatrix);
					worldRayCastHitPositions.push_back(worldRayCastHitPosition);
				}
			}
		}
		RayCastResult result(false, Ray(Vector3::Zero, Vector3::Zero), Vector3::Zero);
		for (auto worldRayCastHitPosition : worldRayCastHitPositions)
		{
			float distance = Vector3::Distance(worldRayCastHitPosition, ray.position);

			if (!result.IsHit()
				|| (distance < result.GetDistance()))
			{
				result = RayCastResult(true, ray, worldRayCastHitPosition);
			}
		}

		return std::make_shared<RayCastResult>(result);
	}

	// The projection matrix P of Hololens' preview camera is given as below. 
	// s_x		0		c_x		0
	// 0		s_y		c_y		0
	// 0		0		-1		0 
	// 0		0		-1		0
	// Since, [u, v, 1, 1]^T ~= [a, b, -z, -z] = P * [x, y, z, 1]^T, (the range of u and v is [-1, 1])
	// we have
	// s_x * x / z + c_x = -u * z
	// s_y * y / z + c_y = -v * z.
	// Therefore,
	// x = -(u + c_x) * z / s_x, y = -(v + c_y) * z / s_y.
	// To get the center point, put u = 0 and v = 0.
	// The width and height can be obtained as
	// (width) = x|u=1 - x|u=-1
	// (height) = y|v=1 - y|v=-1
	// where x|u=1 means the value of x when u is 1.
	DirectX::SimpleMath::Vector3 RayCaster::CalculateHololensPreviewCameraDirection(
		DirectX::SimpleMath::Vector2 pixel,
		std::shared_ptr<LocatableCameraFrame> locatableCameraFrame)
	{
		float sx = locatableCameraFrame->GetProjectionTransform()._11;
		float sy = locatableCameraFrame->GetProjectionTransform()._22;
		float cx = locatableCameraFrame->GetProjectionTransform()._13;
		float cy = locatableCameraFrame->GetProjectionTransform()._23;

		const float z = -1.0f;
		float x = -(pixel.x + cx) * z / sx;
		float y = -(pixel.y + cy) * z / sy;

		DirectX::SimpleMath::Vector3 direction(x, y, z);
		direction.Normalize();

		return direction;
	}
}