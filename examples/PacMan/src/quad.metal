#include <metal_stdlib>
using namespace metal;

struct VertexIn {
    float3 a_position [[attribute(0)]];
    float4 a_color [[attribute(1)]];
    uint a_texIndex [[attribute(2)]];
    float2 a_texCoord [[attribute(3)]];
};

struct VertexOut {
    float4 position [[position]];
    float4 color;
    float2 texCoord;
    uint texIndex;
};

struct Uniforms {
    float4x4 u_viewProjection [[id(0)]];
};

struct TextureArray {
    array<texture2d<float, access::sample>, 32> textures [[texture(0)]];
};

vertex VertexOut quadVertexShader(VertexIn in [[stage_in]], constant Uniforms* u [[buffer(1)]]) {
    VertexOut out;
    out.position = u->u_viewProjection * float4(in.a_position, 1.0);
    out.color = in.a_color;
    out.texIndex = in.a_texIndex;
    out.texCoord = in.a_texCoord;
    return out;
}

fragment float4 quadFragmentShader(VertexOut in [[stage_in]], TextureArray textures, constant Uniforms* u [[buffer(1)]]) {
    constexpr sampler textureSampler (mag_filter::nearest, min_filter::linear);
    float4 colorSample = textures.textures[in.texIndex].sample(textureSampler, in.texCoord);
    return colorSample * in.color;
}