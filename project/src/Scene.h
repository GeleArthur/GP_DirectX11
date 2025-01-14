#ifndef SCENE_H
#define SCENE_H
#include <memory>
#include <vector>

#include "BaseMeshEffect.h"
#include "Camera.h"
#include "ColorRGB.h"


class Scene {
public:
    void SetupCamera(float aspect, float _fovAngle = 90.f, Vector3 _origin = {0.f,0.f,0.f}, float _nearPlane = 1.0f, float _farPlane = 1000.f);
    void AddMesh(std::unique_ptr<BaseMeshEffect>&& mesh);
    void SetBackGroundColor(const ColorRGB& color);

    const std::vector<std::unique_ptr<BaseMeshEffect>>& GetAllMeshes() const; // could I return an iterator?
    const Camera& GetCamera() const;
    const std::vector<Vector3>& GetLights() const;
    const ColorRGB& GetBackGroundColor() const;
    void Update(Timer const & time);
    

private:
    std::vector<std::unique_ptr<BaseMeshEffect>> m_meshesToRender{};
    Camera m_camera;
    std::vector<Vector3> m_directionLights{};
    ColorRGB m_BackGroundColor{};

    
};

#endif //SCENE_H
