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

	void Initialize(float aspect, float fovAngle = 90.f, Vector3 origin = {0.f,0.f,0.f}, float nearPlane = 1.0f, float farPlane = 1000.f);
	void Update(const Timer& pTimer);

	const Matrix<float>& GetViewProjectionMatrix() const;
	const float* GetViewProjectionMatrixAsFloatArray() const;
	const Matrix<float>& GetViewMatrix() const;
	const Matrix<float>& GetProjectionMatrix() const;
	const Vector3& GetForwardVector() const;

	void CalculateViewMatrix();

private:
	Vector3 m_Origin{};
	float m_FovAngle{90.f};
	float m_Fov{ tanf((m_FovAngle * Utils::TO_RADIANS) / 2.f) };

	Vector3 m_Forward{Vector3::UnitZ};
	Vector3 m_Up{Vector3::UnitY};
	Vector3 m_Right{Vector3::UnitX};

	float m_TotalPitch{};
	float m_TotalYaw{};
	float m_Speed{10};
	float m_ShiftSpeedMultiply{2.0f};

	Matrix<float> m_InvViewMatrix{};
	Matrix<float> m_ViewMatrix{};

	Matrix<float> m_ProjectionMatrix{};
	Matrix<float> m_ProjectionViewMatrix{};

	float m_NearPlane{};
	float m_FarPlane{};
};
