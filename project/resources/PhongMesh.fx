struct VS_INPUT
{
    float3 position: POSITION;
    float2 uv : TEXCOORD0; // Does order matter with the attributes?????
    float3 normal : NORMAL0;
    float3 tangent : TANGENT0;
};

struct VS_OUTPUT
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 viewDirection : VIEWDIRECTION;
};

float4x4 gWorldViewProj : WorldViewProjection;
float4x4 gWorldMatrix : WorldMatrix;

Texture2D gDiffuseMap : DiffuseMap;
float3 gWorldPosition: WorldPosition;
float3 gLightDirection: LightDirection;

SamplerState samplePoint
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = Wrap;
    AddressV = Wrap;
};


VS_OUTPUT VS(VS_INPUT input)
{
    VS_OUTPUT output = (VS_OUTPUT)0;
    output.position = mul(float4(input.position, 1.f), gWorldViewProj);
    output.uv = input.uv;
    output.normal = mul(float4(input.normal, 0.f), gWorldMatrix).xyz;
    output.tangent = mul(float4(input.tangent, 0.f), gWorldMatrix).xyz;
    output.viewDirection = normalize((gWorldPosition - mul(float4(input.position, 1.f), gWorldMatrix)).xyz);
    
    return output;
};

float4 PS(VS_OUTPUT input) : SV_TARGET
{
    //return float4(input.Uv.x, input.Uv.y, 0.0, 1.0);
    float observableArea = max(0.0f, dot(-gLightDirection, normalize(input.normal)));


    // float3 rgbColor = gDiffuseMap.Sample(samplePoint, input.uv).xyz;
    return float4(observableArea, observableArea, observableArea, 1.0f);
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