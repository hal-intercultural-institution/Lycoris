#include "io.hlsli"
#include "ps_texture.hlsli"

float4 ps_alpha(const vs_out input) : SV_TARGET
{
    float4 output = texture0.Sample(sampler0, input.texel) * input.color;
    clip(output.z - 0.001f);
    return output;
}
