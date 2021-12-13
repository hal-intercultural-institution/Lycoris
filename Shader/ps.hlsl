#include "io.hlsli"
#include "ps_texture.hlsli"

float4 ps_main(const vs_out input) : SV_TARGET
{
    return texture0.Sample(sampler0, input.texel) * input.color;
}
