#pragma once
#include <vector>

#include <cmath>
#include <functional>
#include <SDL_keyboard.h>
#include <SDL_mouse.h>
#include <SDL_pixels.h>
#include <SDL_surface.h>
#include "Vector.h"

#include "ColorRGB.h"
#include "CullMode.h"
#include "Utils.h"

struct Camera;

template <typename VertexType>
struct Triangle
{
    VertexType v0;
    VertexType v1;
    VertexType v2;
};

class SoftwareRendererHelper
{
public:
    SoftwareRendererHelper(int width, int height, SDL_Surface* backBuffer);

    void ClearDepthBuffer();
    [[nodiscard]] float GetLastDepth() const;
    const std::vector<float>& GetDepthBuffer();
    void SetCullMode(CullMode mode);
    void ToggleDrawDepthBuffer();
    void ToggleDrawBoundingBox();

    template <typename VertexContainer, typename IndicesContainer, typename VertexType>
    void GetTriangles(IndicesContainer begin, const IndicesContainer end, const VertexContainer& vertices, std::vector<Triangle<VertexType>>& triangles)
    {
        while (begin != end)
        {
            VertexType& v1 = vertices[*begin];
            VertexType& v2 = vertices[*(begin + 1)];
            VertexType& v3 = vertices[*(begin + 2)];


            if (m_CullMode != CullMode::none)
            {
                Vector3 normal = Vector3::Cross(Vector3{v2.position - v1.position}, Vector3{v3.position - v1.position});
                
                const bool lookAtBack = Vector3::Dot(Vector3{0,0,-1}, normal) > 0;
                if ((lookAtBack && m_CullMode == CullMode::front) || (!lookAtBack && m_CullMode == CullMode::back))
                {
                    begin += 3;
                    continue;
                }
            }


            if (!FrustumCulling(v1.position, v2.position, v3.position))
            {
                triangles.push_back({v1, v2, v3});
            }

            begin += 3;
        }
    }

    template <typename VertexType>
    void RasterizeTriangle(const std::vector<Triangle<VertexType>>& triangles, std::function<ColorRGB(VertexType)> fragmentShader)
    {
        for (const auto& triangle : triangles)
        {
            Vector2 v0 = {
                (triangle.v0.position.x + 1) / 2.0f * static_cast<float>(m_Width),
                (1 - triangle.v0.position.y) / 2.0f * static_cast<float>(m_Height)
            };
            Vector2 v1 = {
                (triangle.v1.position.x + 1) / 2.0f * static_cast<float>(m_Width),
                (1 - triangle.v1.position.y) / 2.0f * static_cast<float>(m_Height)
            };
            Vector2 v2 = {
                (triangle.v2.position.x + 1) / 2.0f * static_cast<float>(m_Width),
                (1 - triangle.v2.position.y) / 2.0f * static_cast<float>(m_Height)
            };

            const int minX = std::max<int>(0, static_cast<int>(std::min<float>({v0.x, v1.x, v2.x})));
            const int minY = std::max<int>(0, static_cast<int>(std::min<float>({v0.y, v1.y, v2.y})));
            const int maxX = std::min<int>(m_Width, static_cast<int>(std::ceil(std::max<float>({v0.x, v1.x, v2.x}))));
            const int maxY = std::min<int>(m_Height, static_cast<int>(std::ceil(std::max<float>({v0.y, v1.y, v2.y}))));

            for (int px{minX}; px < maxX; ++px)
            {
                for (int py{minY}; py < maxY; ++py)
                {
                    if (m_DrawBoundingBoxes)
                    {
                        reinterpret_cast<uint32_t*>(m_BackBuffer->pixels)[px + (py * m_Width)] = SDL_MapRGB(m_BackBuffer->format,255, 0,0);
                        continue;
                    }
                    
                    Vector2 pixelLocation = {static_cast<float>(px) + 0.5f, static_cast<float>(py) + 0.5f};

                    float distV2 = Vector2::Cross(v1 - v0, pixelLocation - v0);
                    float distV0 = Vector2::Cross(v2 - v1, pixelLocation - v1);
                    float distV1 = Vector2::Cross(v0 - v2, pixelLocation - v2);

                    if ( !((distV2 >= 0 && distV0 >= 0 && distV1 >= 0) || (distV2 <= 0 && distV0 <= 0 && distV1 <= 0))) continue;

                    const float area = distV2 + distV0 + distV1;
                    distV0 = (distV0 / area);
                    distV1 = (distV1 / area);
                    distV2 = (distV2 / area);

                    const float depth = 1.0f / (1.0f / triangle.v0.position.z * distV0 + 1.0f / triangle.v1.position.z * distV1 + 1.0f / triangle.v2.position.z * distV2);

                    if (depth > 1 || depth < 0)
                    {
                        continue;
                    }

                    if (m_DepthBuffer[px + (py * m_Width)] >= depth)
                    {
                        m_DepthBuffer[px + (py * m_Width)] = depth;
                        m_LastDepth = depth;

                        if (m_DrawDepthBuffer)
                        {
                            float remappedDepth = Utils::Remap01(depth, 0.9f, 1.0f);
                            reinterpret_cast<uint32_t*>(m_BackBuffer->pixels)[px + (py * m_Width)] = SDL_MapRGB(m_BackBuffer->format,
                                                               static_cast<uint8_t>(remappedDepth * 255),
                                                               static_cast<uint8_t>(remappedDepth * 255),
                                                               static_cast<uint8_t>(remappedDepth * 255));
                            continue;
                        }

    #if _DEBUG
                        if (SDL_GetKeyboardState(nullptr)[SDL_SCANCODE_T])
                        {
                            int mousex;
                            int mousey;
                            SDL_GetMouseState(&mousex, &mousey);
                            if (px == 872 && py == 531)
                            {
                                __debugbreak();
                            }
                        }
    #endif


                        auto vertex0Tuple = triangle.v0.AsTuple();
                        auto vertex1Tuple = triangle.v1.AsTuple();
                        auto vertex2Tuple = triangle.v2.AsTuple();
                        auto correctedVertex = CalulateCorrectedDepthBuffer(vertex0Tuple, vertex1Tuple, vertex2Tuple,
                                                                            distV0, distV1, distV2,
                                                                            std::make_index_sequence<std::tuple_size_v<decltype(vertex0Tuple)>>());

                        
                        ColorRGB finalColor = fragmentShader(VertexType::FromTuple(correctedVertex));
                        finalColor.MaxToOne();

                        reinterpret_cast<uint32_t*>(m_BackBuffer->pixels)[px + (py * m_Width)] = SDL_MapRGB(m_BackBuffer->format,
                                                           static_cast<uint8_t>(std::clamp(finalColor.r, 0.f, 1.0f) * 255),
                                                           static_cast<uint8_t>(std::clamp(finalColor.g, 0.0f, 1.0f) * 255),
                                                           static_cast<uint8_t>(std::clamp(finalColor.b, 0.0f, 1.0f) * 255));
                    }
                }
            }
        }
    }

private:

    template<typename VertexTuple, std::size_t... Index>
    VertexTuple CalulateCorrectedDepthBuffer(VertexTuple v0, VertexTuple v1, VertexTuple v2, float distV0, float distV1, float distV2,  std::index_sequence<Index...>)
    {
        float depthW = 1.0f / (1.0f / std::get<0>(v0).w * distV0 + 1.0f / std::get<0>(v1).w * distV1 + 1.0f / std::get<0>(v2).w * distV2);

        // For each element in the tuple we calulate the corrected depth and barcentic coords
        VertexTuple out;
        ((std::get<Index>(out) =
            (((std::get<Index>(v0) / std::get<0>(v0).w) * distV0 +
            (std::get<Index>(v1) / std::get<0>(v1).w) * distV1 +
            (std::get<Index>(v2) / std::get<0>(v2).w) * distV2)) * depthW  ),
            ...);

        return out;
    }
    
    bool CheckInFrustum(auto v)
    {
        if (v.x < -1 || v.x > 1) return true;
        if (v.y < -1 || v.y > 1) return true;
        if (v.z < 0 || v.z > 1) return true;
        return false;
    }

    bool FrustumCulling(auto v1, auto v2, auto v3)
    {
        if (CheckInFrustum(v1)) return true;
        if (CheckInFrustum(v2)) return true;
        if (CheckInFrustum(v3)) return true;
        return false;
    }

    int m_Width;
    int m_Height;
    float m_LastDepth{};
    CullMode m_CullMode{CullMode::back};
    bool m_DrawDepthBuffer{false};
    bool m_DrawBoundingBoxes{false};

    SDL_Surface* m_BackBuffer;
    std::vector<float> m_DepthBuffer{};
};
