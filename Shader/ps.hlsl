#include "io.hlsli"

Texture2D texture0 : register(t0);
SamplerState sampler0 : register(s0);

float4 ps_main(const vs_out input) : SV_TARGET
{
    return texture0.Sample(sampler0, input.texel);
}
