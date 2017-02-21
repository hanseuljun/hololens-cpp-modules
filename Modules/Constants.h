#pragma once

namespace HoloLensCppModules
{
	namespace HololensPreviewCameraConstant
	{
		// The projection matrix P of Hololens' preview camera is given as below. 
		// s_x		0		c_x		0
		// 0		s_y		c_y		0
		// 0		0		-1		0 
		// 0		0		-1		0
		// where s_x = 2.4357, s_y = 4.32573, c_x = 0.0738882, c_y = -0.111142
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

		// Values of the Hololens camera's projection matrix.
		// TODO: This value is different for each devices. Make it adjustable.
		static const float c_sx = 2.4357f;
		static const float c_sy = 4.32573f;
		static const float c_cx = 0.0738882f;
		static const float c_cy = -0.111142f;

		// Pixel-wise resolution of HoloLens.
		static const int c_width = 1280;
		static const int c_height = 720;
	};
}
