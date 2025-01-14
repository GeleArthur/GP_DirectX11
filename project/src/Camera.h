#pragma once
#include <cassert>
#include <iostream>
#include <SDL_keyboard.h>
#include <SDL_mouse.h>

#include "Timer.h"
#include "Utils.h"
#include "Vector.h"
#include "Matrix.h"

struct Camera
{
public:
	Camera() = default;

	void Initialize(float aspect, float _fovAngle = 90.f, Vector3 _origin = {0.f,0.f,0.f}, float _nearPlane = 1.0f, float _farPlane = 1000.f);
	void Update(const Timer& pTimer);

	const Matrix<float>& GetViewProjectionMatrix() const;
	const float* GetViewProjectionMatrixAsFloatArray() const;
	const Matrix<float>& GetViewMatrix() const;
	const Matrix<float>& GetProjectionMatrix() const;

	void CalculateViewMatrix();

private:
	Vector3 origin{};
	float fovAngle{90.f};
	float fov{ tanf((fovAngle * Utils::TO_RADIANS) / 2.f) };

	Vector3 forward{Vector3::UnitZ};
	Vector3 up{Vector3::UnitY};
	Vector3 right{Vector3::UnitX};

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
