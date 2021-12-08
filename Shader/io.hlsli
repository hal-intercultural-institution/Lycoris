struct vs_in
{
    float4 pos : POSITION0;
    float4 normal : NORMAL0;
    float4 color : COLOR0;
    float2 texel : TEXCOORD0;
};

struct vs_in_anim : vs_in
{
    uint anim_index : BLENDINDICES0;
};

struct vs_out
{
    float4 pos : SV_POSITION;
    float4 normal : NORMAL0;
    float4 color : COLOR0;
    float2 texel : TEXCOORD0;
};
