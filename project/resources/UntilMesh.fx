struct VS_INPUT
{
    float3 Position: POSITION;
    float2 Uv : TEXCOORD;
};

struct VS_OUTPUT
{
    float4 Position : SV_POSITION;
    float2 Uv : TEXCOORD;
};

float4x4 gWorldViewProj : WorldViewProjection;
Texture2D gDiffuseMap : DiffuseMap;

SamplerState samplePoint
{
    Filter = ANISOTROPIC;
    AddressU = Wrap;
    AddressV = Wrap;
};


VS_OUTPUT VS(VS_INPUT input)
{
    VS_OUTPUT output = (VS_OUTPUT)0;
    output.Position = mul(float4(input.Position, 1.f), gWorldViewProj);
    output.Uv = input.Uv;
    return output;
};

float4 PS(VS_OUTPUT input) : SV_TARGET
{
    //return float4(input.Uv.x, input.Uv.y, 0.0, 1.0);
    float3 rgbColor = gDiffuseMap.Sample(samplePoint, input.Uv);
    return float4(rgbColor, 1.0f);
};

technique11 DefaultTechnique
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS()));
    }
};