//
// Created by a on 23/12/2024.
//

#include "Scene.h"

#include "BaseMeshEffect.h"

void Scene::AddMesh(std::unique_ptr<BaseMeshEffect>&& mesh) // DOES THIS DO WHAT I THINK IT DOES?????
{
    m_meshesToRender.push_back(std::move(mesh));
}
const std::vector<std::unique_ptr<BaseMeshEffect>> & Scene::GetAllMeshes() const { return m_meshesToRender; }
const Camera& Scene::GetCamera() const { return m_camera;}
const std::vector<Vector3> & Scene::GetLights() const { return m_directionLights; }
