#include "SoftwareRendererHelpers.h"

#include <SDL_surface.h>

#include "Camera.h"


SoftwareRendererHelper::SoftwareRendererHelper(const int width, const int height, SDL_Surface* backBuffer):
    m_Width(width),
    m_Height(height),
    m_BackBuffer(backBuffer),
    m_DepthBuffer(static_cast<size_t>(width * height))
{
}

void SoftwareRendererHelper::ClearDepthBuffer()
{
    std::fill(m_DepthBuffer.begin(), m_DepthBuffer.end(), std::numeric_limits<float>::max());
}

float SoftwareRendererHelper::GetLastDepth() const
{
    return m_LastDepth;
}

const std::vector<float>& SoftwareRendererHelper::GetDepthBuffer()
{
    return m_DepthBuffer;
}

void SoftwareRendererHelper::SetCullMode(CullMode mode)
{
    m_CullMode = mode;
}

void SoftwareRendererHelper::ToggleDrawDepthBuffer()
{
    m_DrawDepthBuffer = !m_DrawDepthBuffer;
}

void SoftwareRendererHelper::ToggleDrawBoundingBox()
{
    m_DrawBoundingBoxes = !m_DrawBoundingBoxes;
}
