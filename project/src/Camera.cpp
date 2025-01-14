#include "Camera.h"

void Camera::Initialize(float aspect, float _fovAngle, Vector3 _origin, float _nearPlane, float _farPlane)
{
	fovAngle = _fovAngle;
	fov = tanf((fovAngle * Utils::TO_RADIANS) / 2.f);
	nearPlane = _nearPlane;
	farPlane = _farPlane;
	origin = _origin;


	projectionMatrix = Matrix<float>::CreatePerspectiveFovLH(fov, aspect, nearPlane, farPlane);
	
}

void Camera::CalculateViewMatrix()
{
	right = Vector3::Cross(Vector3::UnitY, forward).Normalized();
	up = Vector3::Cross(forward, right).Normalized();

	viewMatrix = Matrix<float>{
			{right.x,   right.y,   right.z,   0},
			{up.x,	    up.y,      up.z,      0},
			{forward.x, forward.y, forward.z, 0},
			{origin.x,  origin.y,   origin.z, 1}
	};

	invViewMatrix = viewMatrix.Inverse();
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
		input.z += speed;
	}
	if(pKeyboardState[SDL_SCANCODE_S])
	{
		input.z -= speed;
	}
	if(pKeyboardState[SDL_SCANCODE_A])
	{
		input.x -= speed;
	}
	if(pKeyboardState[SDL_SCANCODE_D])
	{
		input.x += speed;
	}

	if(mouseState == SDL_BUTTON_RMASK)
	{
		totalPitch += -static_cast<float>(mouseY) * 0.01f;
		totalYaw += static_cast<float>(mouseX) * 0.01f;
	}
	else if(mouseState == SDL_BUTTON_LMASK)
	{
		input.z += -static_cast<float>(mouseY) * 0.3f;

		totalYaw += static_cast<float>(mouseX) * 0.01f;
	}


	forward = Matrix<float>::CreateRotation(totalPitch, totalYaw, 0).TransformVector(Vector3::UnitZ);
	forward.Normalize();
	origin += (forward * input.z + right * input.x) * 5 * deltaTime;
	
	CalculateViewMatrix();

	projectionViewMatrix = invViewMatrix * projectionMatrix;
}

const Matrix<float>& Camera::GetViewProjectionMatrix() const
{
	return projectionViewMatrix;
}

const float* Camera::GetViewProjectionMatrixAsFloatArray() const
{
	return reinterpret_cast<const float*>(&projectionViewMatrix);
}

const Matrix<float>& Camera::GetViewMatrix() const
{
	return viewMatrix;
}

const Matrix<float>& Camera::GetProjectionMatrix() const
{
	return projectionMatrix;
}
