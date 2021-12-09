cbuffer WorldMatrix : register(b0)
{
    matrix world;
}

cbuffer ViewMatrix : register(b1)
{
    matrix view;
}

cbuffer ProjectionMatrix : register(b2)
{
    matrix projection;
}

cbuffer MaterialBuffer : register(b3)
{
    float4 ambient;
    float4 diffuse;
    float4 specular;
    float4 emission;
    float shininess;
    float3 dummy;
}

cbuffer Light : register(b4)
{
    float4 light;
}

cbuffer UVOffset : register(b5)
{
    float4 offset;
}

cbuffer AnimMatrix : register(b6)
{
    matrix anim_matrix[32];
}
