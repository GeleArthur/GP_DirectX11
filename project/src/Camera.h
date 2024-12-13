#pragma once
#include <cassert>
#include <iostream>
#include <SDL_keyboard.h>
#include <SDL_mouse.h>

#include "Math.h"
#include "Timer.h"

namespace dae
{
	struct Camera
	{
	public:
		Camera() = default;
		
		void Initialize(float aspect, float _fovAngle = 90.f, Vector<3,float> _origin = {0.f,0.f,0.f}, float _nearPlane = 1.0f, float _farPlane = 1000.f);
		void Update(const Timer* pTimer);

		const dae::Matrix<float>& GetViewProjectionMatrix() const;
		const float* GetViewProjectionMatrixAsFloatArray() const;
		const dae::Matrix<float>& GetViewMatrix() const;
		const dae::Matrix<float>& GetProjectionMatrix() const;
		
		void CalculateViewMatrix();

	private:
		Vector<3,float> origin{};
		float fovAngle{90.f};
		float fov{ tanf((fovAngle * TO_RADIANS) / 2.f) };

		Vector<3,float> forward{Vector<3,float>::UnitZ};
		Vector<3,float> up{Vector<3,float>::UnitY};
		Vector<3,float> right{Vector<3,float>::UnitX};

		float totalPitch{};
		float totalYaw{};
		float speed{10};

		Matrix<float> invViewMatrix{};
		Matrix<float> viewMatrix{};

		Matrix<float> projectionMatrix{};
		Matrix<float> projectionViewMatrix{};
		
		float nearPlane{};
		float farPlane{};
	};
}
