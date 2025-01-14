﻿#ifndef SCENE_H
#define SCENE_H
#include <memory>
#include <vector>

#include "BaseMeshEffect.h"
#include "Camera.h"


class Scene {
public:
    void SetupCamera(float aspect, float _fovAngle = 90.f, Vector3 _origin = {0.f,0.f,0.f}, float _nearPlane = 1.0f, float _farPlane = 1000.f);
    void AddMesh(std::unique_ptr<BaseMeshEffect>&& mesh);

    const std::vector<std::unique_ptr<BaseMeshEffect>>& GetAllMeshes() const; // could I return an iterator?
    const Camera& GetCamera() const;
    const std::vector<Vector3>& GetLights() const;
    void Update(Timer const & time);

private:
    std::vector<std::unique_ptr<BaseMeshEffect>> m_meshesToRender{};
    Camera m_camera;
    std::vector<Vector3> m_directionLights{};
};

#endif //SCENE_H
