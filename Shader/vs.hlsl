#include "io.hlsli"
#include "buffer.hlsli"

vs_out vs_main(const vs_in input)
{
    vs_out output;
    matrix wvp = mul(world, view);
    wvp = mul(wvp, projection);

    output.pos = mul(input.pos, wvp);
    output.normal = input.normal;
    output.color = input.color * diffuse;
    output.texel = input.texel + offset.xy;

    return output;
}
