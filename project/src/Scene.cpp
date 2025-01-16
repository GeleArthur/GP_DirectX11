#include "Scene.h"

#include "BaseMeshEffect.h"
#include "PhongMesh.h"
#include <ranges>

void Scene::SetupCamera(float aspect, float _fovAngle, Vector3 _origin, float _nearPlane, float _farPlane)
{
    m_Camera.Initialize(aspect, _fovAngle, _origin, _nearPlane, _farPlane);
}

void Scene::AddMesh(std::unique_ptr<BaseMeshEffect>&& mesh) 
{
    m_MeshesToRender.push_back(std::move(mesh));
}

void Scene::AddLight(const Vector3& direction)
{
    m_DirectionLights.push_back(direction);
}

void Scene::SetBackGroundColor(const ColorRGB& color)
{
    m_BackGroundColor = color;
}

const ColorRGB& Scene::GetBackGroundColor() const
{
    return m_BackGroundColor;
}

void Scene::Update(Timer const& time)
{
    m_Camera.Update(time);

    if (m_Rotating)
    {
        m_RotatedAmount += time.GetElapsed();
        for (auto& mesh : m_MeshesToRender)
        {
            mesh->SetWorldMatrix(Matrix<float>::CreateRotationY(m_RotatedAmount));
        }
    }
}

void Scene::ToggleRotation()
{
    m_Rotating = !m_Rotating;
}

const std::vector<std::unique_ptr<BaseMeshEffect>> & Scene::GetAllMeshes() const
{
    return m_MeshesToRender;
}

const Camera& Scene::GetCamera() const { return m_Camera;}

const std::vector<Vector3> & Scene::GetLights() const { return m_DirectionLights; }
