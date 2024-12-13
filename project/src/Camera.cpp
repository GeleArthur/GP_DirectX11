#include "Camera.h"

void dae::Camera::Initialize(float aspect, float _fovAngle, Vector<3,float> _origin, float _nearPlane, float _farPlane)
{
	fovAngle = _fovAngle;
	fov = tanf((fovAngle * TO_RADIANS) / 2.f);
	nearPlane = _nearPlane;
	farPlane = _farPlane;
	origin = _origin;


	projectionMatrix = Matrix<float>::CreatePerspectiveFovLH(fov, aspect, nearPlane, farPlane);
	
}

void dae::Camera::CalculateViewMatrix()
{
	right = Vector<3,float>::Cross(Vector<3,float>::UnitY, forward).Normalized();
	up = Vector<3,float>::Cross(forward, right).Normalized();

	viewMatrix = Matrix<float>{
			{right.x,   right.y,   right.z,   0},
			{up.x,	    up.y,      up.z,      0},
			{forward.x, forward.y, forward.z, 0},
			{origin.x,  origin.y,   origin.z, 1}
	};

	invViewMatrix = viewMatrix.Inverse();
}

void dae::Camera::Update(const Timer* pTimer)
{
	const float deltaTime = pTimer->GetElapsed();

	//Keyboard Input
	const uint8_t* pKeyboardState = SDL_GetKeyboardState(nullptr);

	//Mouse Input
	int mouseX{}, mouseY{};
	const uint32_t mouseState = SDL_GetRelativeMouseState(&mouseX, &mouseY);

	Vector<3,float> input{};
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


	forward = Matrix<float>::CreateRotation(totalPitch, totalYaw, 0).TransformVector(Vector<3,float>::UnitZ);
	forward.Normalize();
	origin += (forward * input.z + right * input.x) * 5 * deltaTime;
	
	CalculateViewMatrix();

	projectionViewMatrix = invViewMatrix * projectionMatrix;
}

const dae::Matrix<float>& dae::Camera::GetViewProjectionMatrix() const
{
	return projectionViewMatrix;
}

const float* dae::Camera::GetViewProjectionMatrixAsFloatArray() const
{
	return reinterpret_cast<const float*>(&projectionViewMatrix);
}

const dae::Matrix<float>& dae::Camera::GetViewMatrix() const
{
	return viewMatrix;
}

const dae::Matrix<float>& dae::Camera::GetProjectionMatrix() const
{
	return projectionMatrix;
}
