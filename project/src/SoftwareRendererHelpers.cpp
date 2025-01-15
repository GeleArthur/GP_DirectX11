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

void SoftwareRendererHelper::SetCullMode(CullMode mode)
{
    m_CullMode = mode;
}
