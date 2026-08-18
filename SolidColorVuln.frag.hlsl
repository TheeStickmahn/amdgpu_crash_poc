// This is just SolidColor.frag.hlsl from the SDLGPU examples, but I've added an invalid UBO that gets read from during main()

cbuffer UBO : register(b0, space2)
{
    float invalidUBO : packoffset(c0);
};

float4 main(float4 Color : TEXCOORD0) : SV_Target0
{
    return Color * invalidUBO;
}
