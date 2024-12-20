#pragma once
#include "BaseMesh.h"

class DiffuseMesh : BaseMesh
{
public:
    DiffuseMesh();
    
    ~DiffuseMesh() override;
    ID3DX11Effect* GetEffect() const override;
    void DrawIndexed() const override;

    
};