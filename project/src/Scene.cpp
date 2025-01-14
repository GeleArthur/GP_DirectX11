//
// Created by a on 23/12/2024.
//

#include "Scene.h"

#include "BaseMeshEffect.h"

void Scene::SetupCamera(float aspect, float _fovAngle, Vector3 _origin, float _nearPlane, float _farPlane)
{
    m_camera.Initialize(aspect, _fovAngle, _origin, _nearPlane, _farPlane);
}

void Scene::AddMesh(std::unique_ptr<BaseMeshEffect>&& mesh) 
{
    m_meshesToRender.push_back(std::move(mesh));
}

void Scene::SetBackGroundColor(const ColorRGB& color)
{
    m_BackGroundColor = color;
}

const std::vector<std::unique_ptr<BaseMeshEffect>> & Scene::GetAllMeshes() const { return m_meshesToRender; }
const Camera& Scene::GetCamera() const { return m_camera;}
const std::vector<Vector3> & Scene::GetLights() const { return m_directionLights; }

const ColorRGB& Scene::GetBackGroundColor() const
{
    return m_BackGroundColor;
}

void Scene::Update(Timer const& time)
{
    m_camera.Update(time);
}
