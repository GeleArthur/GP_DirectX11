#include "Camera.h"

void Camera::Initialize(float aspect, float fovAngle, Vector3 origin, float nearPlane, float farPlane)
{
	m_FovAngle = fovAngle;
	m_Fov = tanf((m_FovAngle * Utils::TO_RADIANS) / 2.f);
	m_NearPlane = nearPlane;
	m_FarPlane = farPlane;
	m_Origin = origin;


	m_ProjectionMatrix = Matrix<float>::CreatePerspectiveFovLH(m_Fov, aspect, m_NearPlane, m_FarPlane);
	
}

void Camera::CalculateViewMatrix()
{
	m_Right = Vector3::Cross(Vector3::UnitY, m_Forward).Normalized();
	m_Up = Vector3::Cross(m_Forward, m_Right).Normalized();

	m_ViewMatrix = Matrix<float>{
			{m_Right.x,   m_Right.y,   m_Right.z,   0},
			{m_Up.x,	    m_Up.y,      m_Up.z,      0},
			{m_Forward.x, m_Forward.y, m_Forward.z, 0},
			{m_Origin.x,  m_Origin.y,   m_Origin.z, 1}
	};

	m_InvViewMatrix = m_ViewMatrix.Inverse();
}

void Camera::Update(const Timer& pTimer)
{
	const float deltaTime = pTimer.GetElapsed();

	//Keyboard Input
	const uint8_t* pKeyboardState = SDL_GetKeyboardState(nullptr);

	//Mouse Input
	int mouseX{}, mouseY{};
	const uint32_t mouseState = SDL_GetRelativeMouseState(&mouseX, &mouseY);

	Vector3 input{};
	if(pKeyboardState[SDL_SCANCODE_W])
	{
		input.z += m_Speed * deltaTime;
	}
	if(pKeyboardState[SDL_SCANCODE_S])
	{
		input.z -= m_Speed * deltaTime;
	}
	if(pKeyboardState[SDL_SCANCODE_A])
	{
		input.x -= m_Speed * deltaTime;
	}
	if(pKeyboardState[SDL_SCANCODE_D])
	{
		input.x += m_Speed * deltaTime;
	}

	if (mouseState == (SDL_BUTTON_RMASK | SDL_BUTTON_LMASK))
	{
		input.y += -static_cast<float>(mouseY) * 0.3f;
		input.x += static_cast<float>(mouseX) * 0.3f;
	}
	else if(mouseState == SDL_BUTTON_RMASK)
	{
		m_TotalPitch += -static_cast<float>(mouseY) * 0.01f;
		m_TotalYaw += static_cast<float>(mouseX) * 0.01f;
	}
	else if(mouseState == SDL_BUTTON_LMASK)
	{
		input.z += -static_cast<float>(mouseY) * 0.3f;
		m_TotalYaw += static_cast<float>(mouseX) * 0.01f;
	}
	
	
	m_Forward = Matrix<float>::CreateRotation(m_TotalPitch, m_TotalYaw, 0).TransformVector(Vector3::UnitZ);
	m_Forward.Normalize();
	Vector3 movement = (m_Forward * input.z + m_Right * input.x + m_Up * input.y);

	if (pKeyboardState[SDL_SCANCODE_LSHIFT])
	{
		movement *= m_ShiftSpeedMultiply;
	}
	
	m_Origin += movement;
	
	
	CalculateViewMatrix();

	m_ProjectionViewMatrix = m_InvViewMatrix * m_ProjectionMatrix;
}

const Matrix<float>& Camera::GetViewProjectionMatrix() const
{
	return m_ProjectionViewMatrix;
}

const float* Camera::GetViewProjectionMatrixAsFloatArray() const
{
	return reinterpret_cast<const float*>(&m_ProjectionViewMatrix);
}

const Matrix<float>& Camera::GetViewMatrix() const
{
	return m_ViewMatrix;
}

const Matrix<float>& Camera::GetProjectionMatrix() const
{
	return m_ProjectionMatrix;
}

const Vector3& Camera::GetForwardVector() const
{
	return m_Forward;
}
