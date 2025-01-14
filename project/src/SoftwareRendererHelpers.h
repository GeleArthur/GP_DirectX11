#pragma once
#include <vector>

#include "Vector.h"
#include <cmath>
#include <functional>
#include <iostream>
#include <SDL_keyboard.h>
#include <SDL_mouse.h>
#include <SDL_pixels.h>
#include <SDL_surface.h>

#include "ColorRGB.h"


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

    template <typename VertexContainer, typename IndicesContainer, typename VertexType>
    void GetTriangles(IndicesContainer begin, const IndicesContainer end, const VertexContainer& vertices,
                      std::vector<Triangle<VertexType>>& triangles)
    {
        while (begin != end)
        {
            VertexType& v1 = vertices[*begin];
            VertexType& v2 = vertices[*(begin + 1)];
            VertexType& v3 = vertices[*(begin + 2)];

            if (!FrustumCulling(v1.position, v2.position, v3.position))
            {
                triangles.push_back({v1, v2, v3});
            }

            begin += 3;
        }
    }

    template <typename VertexType>
    void RasterizeTriangle(const Triangle<VertexType>& triangle, std::function<ColorRGB(VertexType)> fragmentShader)
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

        int minX = std::max<int>(0, static_cast<int>(std::min<float>({v0.x, v1.x, v2.x})));
        int minY = std::max<int>(0, static_cast<int>(std::min<float>({v0.y, v1.y, v2.y})));
        int maxX = std::min<int>(m_Width, static_cast<int>(std::ceil(std::max<float>({v0.x, v1.x, v2.x}))));
        int maxY = std::min<int>(m_Height, static_cast<int>(std::ceil(std::max<float>({v0.y, v1.y, v2.y}))));

        for (int px{minX}; px < maxX; ++px)
        {
            for (int py{minY}; py < maxY; ++py)
            {
                Vector2 pixelLocation = {static_cast<float>(px) + 0.5f, static_cast<float>(py) + 0.5f};

                float distV2 = Vector<2, float>::Cross(v1 - v0, pixelLocation - v0);
                float distV0 = Vector<2, float>::Cross(v2 - v1, pixelLocation - v1);
                float distV1 = Vector<2, float>::Cross(v0 - v2, pixelLocation - v2);

                if (distV2 < 0 || distV0 < 0 || distV1 < 0) continue;

                const float area = distV2 + distV0 + distV1;
                distV0 = (distV0 / area);
                distV1 = (distV1 / area);
                distV2 = (distV2 / area);

                const float depth = 1.0f / (1.0f / triangle.v0.position.z * distV0 + 1.0f / triangle.v1.position.z *
                    distV1 + 1.0f / triangle.v2.position.z * distV2);

                if (depth > 1 || depth < 0)
                {
                    continue;
                }

                if (m_DepthBuffer[px + (py * m_Width)] >= depth)
                {
                    m_DepthBuffer[px + (py * m_Width)] = depth;

#if _DEBUG
                    if (SDL_GetKeyboardState(nullptr)[SDL_SCANCODE_T])
                    {
                        int mousex;
                        int mousey;
                        SDL_GetMouseState(&mousex, &mousey);
                        if (px == mousex && py == mousey)
                        {
                            __debugbreak();
                        }
                    }
#endif
                    
                    
                    float depthW = 1.0f / (1.0f / triangle.v0.position.w * distV0 + 1.0f / triangle.v1.position.w * distV1 + 1.0f / triangle.v2.position.w * distV2);
                    // Vector4 position = (triangle.v0.position / triangle.v0.position.w * distV0 + triangle.v1.position / triangle.v1.position.w * distV1 + triangle.v2.position / triangle.v2.position.w * distV2) * depthW;
                    // Vector<2, float> uv = (triangle.v0.uv / triangle.v0.position.w * distV0 + triangle.v1.uv / triangle.v1.position.w * distV1 + triangle.v2.uv / triangle.v2.position.w * distV2) * depthW;
                    VertexType fragment = VertexType{
                        ((triangle.v0.position / triangle.v0.position.w) * distV0 + (triangle.v1.position/ triangle.v1.position.w) * distV1 + (triangle.v2.position/triangle.v2.position.w) * distV2) * depthW,
                        ((triangle.v0.uv / triangle.v0.position.w) * distV0 + (triangle.v1.uv/triangle.v1.position.w) * distV1 + (triangle.v2.uv/triangle.v2.position.w) * distV2)*depthW ,
                    };
                    
                    // auto vertex0Tuple = triangle.v0.AsTuple();
                    // auto vertex1Tuple = triangle.v1.AsTuple();
                    // auto vertex2Tuple = triangle.v2.AsTuple();
                    //
                    // decltype(vertex0Tuple) result;
                    //
                    // [&]<std::size_t... Is>(std::index_sequence<Is...>) {
                    //     ((std::get<Is>(result) = (std::get<Is>(vertex0Tuple) / std::get<0>(vertex0Tuple).w * distV0  + std::get<Is>(vertex1Tuple) / std::get<0>(vertex0Tuple).w * distV1 + std::get<Is>(vertex2Tuple) / std::get<0>(vertex0Tuple).w * distV2) * depthW ), ...);
                    // }(std::make_index_sequence<std::tuple_size_v<decltype(result)>>());
                    //
                    // VertexType depthCorrectedVertex = VertexType::FromTuple(result);
                    ColorRGB finalColor = fragmentShader(fragment);
                    finalColor.MaxToOne();

                    reinterpret_cast<uint32_t*>(m_BackBuffer->pixels)[px + (py * m_Width)] = SDL_MapRGB(m_BackBuffer->format,
                                                       static_cast<uint8_t>(finalColor.r * 255),
                                                       static_cast<uint8_t>(finalColor.g * 255),
                                                       static_cast<uint8_t>(finalColor.b * 255));

                    // Vector4 position = (triangle.v0.position / triangle.v0.position.w * distV0 + triangle.v1.position / triangle.v1.position.w * distV1 + triangle.v2.position / triangle.v2.position.w * distV2) * depthW;
                    // Vector<2, float> uv = (triangle.v0.uv / triangle.v0.position.w * distV0 + triangle.v1.uv / triangle.v1.position.w * distV1 + triangle.v2.uv / triangle.v2.position.w * distV2) * depthW;
                    // // Vector3 normal = (vertex0.normal / vertex0.position.w * distV0 + vertex1.normal / vertex1.position.w * distV1 + vertex2.normal / vertex2.position.w * distV2) * depthW;
                    // Vector3 tangent = (vertex0.tangent / vertex0.position.w * distV0 + vertex1.tangent / vertex1.position.w * distV1 + vertex2.tangent / vertex2.position.w * distV2) * depthW;
                    // Vector3 viewDirection = (vertex0.viewDirection / vertex0.position.w * distV0 + vertex1.viewDirection / vertex1.position.w * distV1 + vertex2.viewDirection / vertex2.position.w * distV2) * depthW;
                    //
                    //         ColorRGB finalColor = fragmentShader(VertexType{position, uv});
                    //         
                    //         finalColor.MaxToOne();
                    //

                    
                    // reinterpret_cast<uint32_t*>(m_BackBuffer->pixels)[px + (py * m_Width)] = SDL_MapRGB(m_BackBuffer->format,
                    //                                                        static_cast<uint8_t>(depth * 255),
                    //                                                        static_cast<uint8_t>(depth * 255),
                    //                                                        static_cast<uint8_t>(depth * 255));
                }
            }
        }
    }

private:
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

    SDL_Surface* m_BackBuffer;
    std::vector<float> m_DepthBuffer{};
};
