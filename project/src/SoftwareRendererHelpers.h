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


                    auto vertex0Tuple = triangle.v0.AsTuple();
                    auto vertex1Tuple = triangle.v1.AsTuple();
                    auto vertex2Tuple = triangle.v2.AsTuple();
                    auto correctedVertex = CalulateCorrectedDepthBuffer(vertex0Tuple, vertex1Tuple, vertex2Tuple,
                                                                        distV0, distV1, distV2,
                                                                        std::make_index_sequence<std::tuple_size_v<decltype(vertex0Tuple)>>());
                    
                    ColorRGB finalColor = fragmentShader(VertexType::FromTuple(correctedVertex));
                    finalColor.MaxToOne();

                    reinterpret_cast<uint32_t*>(m_BackBuffer->pixels)[px + (py * m_Width)] = SDL_MapRGB(m_BackBuffer->format,
                                                       static_cast<uint8_t>(finalColor.r * 255),
                                                       static_cast<uint8_t>(finalColor.g * 255),
                                                       static_cast<uint8_t>(finalColor.b * 255));
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

    SDL_Surface* m_BackBuffer;
    std::vector<float> m_DepthBuffer{};
};
