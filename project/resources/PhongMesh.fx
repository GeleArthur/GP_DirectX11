#define PI 3.14159265358979323846

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
    float3 binormal : BINORMAL;

    float3 viewDirection : VIEWDIRECTION;
};

float4x4 gWorldViewProj : WorldViewProjection;
float4x4 gWorldMatrix : WorldMatrix;

Texture2D gDiffuseMap : DiffuseMap;
Texture2D gNormalMap : NormalMap;
Texture2D gGlossMap : GlossMap;
Texture2D gSpecularMap : SpecularMap;

float3 gWorldPosition: WorldPosition;
float3 gLightDirection: LightDirection;

float gShininess : Shininess;
float gDiffuseReflectance : DiffuseReflectance;


SamplerState samplePoint
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = Wrap;
    AddressV = Wrap;
};

float3 MaxToOne(float3 color) {
    float maxValue = max(max(max(color.r, color.g), color.b), 1.0f);
    
    return color / maxValue;
};


VS_OUTPUT VS(VS_INPUT input)
{
    VS_OUTPUT output = (VS_OUTPUT)0;
    output.position = mul(float4(input.position, 1.f), gWorldViewProj);
    output.uv = input.uv;
    output.normal = mul(input.normal, (float3x3)gWorldMatrix);
    output.tangent = mul(input.tangent, (float3x3)gWorldMatrix);
    output.binormal = cross(output.normal, output.tangent);
    output.viewDirection = normalize((gWorldPosition - mul(float4(input.position, 1.f), gWorldMatrix)).xyz);
    
    return output;
};

float4 PS(VS_OUTPUT input) : SV_TARGET
{
    float3 finalColor = 0;

    float3 normalizeNormal = normalize(input.normal);
    float3 normalizeTangent = normalize(input.tangent);
    float3 normalizeBinormal = normalize(input.binormal);
    float3x3 tangentSpaceAxis = {normalizeTangent, normalizeBinormal, normalizeNormal};
    
    float3 normalMap = gNormalMap.Sample(samplePoint, input.uv).rgb; // Incress range
    normalMap = (2.0f * normalMap) - 1.0f;
    float3 normal = mul(normalMap, tangentSpaceAxis);

    // finalColor = normal;


	// const Vector3 lightDirection = scene.GetLights().at(0).Normalized(); // TODO: Multilight
    float3 albedoTexture = gDiffuseMap.Sample(samplePoint, input.uv).rgb;

    // lambert
    float observableArea = max(0.0f, dot(-gLightDirection, normalize(normal)));
    float3 lambert = (gDiffuseReflectance * albedoTexture) / PI;

    // Phong
    float specularReflectance = gSpecularMap.Sample(samplePoint, input.uv).r;
    float gloss = gGlossMap.Sample(samplePoint, input.uv).r * gShininess;

    float3 reflect1 = reflect(-gLightDirection, normalize(normal));
    float cosAngle = max(0.0f, dot(reflect1, -input.viewDirection));

    float phong = specularReflectance * pow(cosAngle, gloss);

    finalColor = (albedoTexture * 0.3f) + phong + lambert * observableArea;



    return float4(MaxToOne(finalColor), 1.0f);
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