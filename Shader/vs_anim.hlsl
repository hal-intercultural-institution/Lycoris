#include "io.hlsli"
#include "buffer.hlsli"

vs_out vs_anim(const vs_in_anim input)
{
    vs_out output;
    const matrix wvp = mul(mul(mul(anim_matrix[input.anim_index], world), view), projection);

    output.pos = mul(input.pos, wvp);
    output.normal = input.normal;
    output.color = input.color;
    output.texel = input.texel + offset.xy;

    return output;
}
