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

vertex VertexOut textVertexShader(VertexIn in [[stage_in]], constant Uniforms* u [[buffer(2)]]) {
    VertexOut out;
    out.position = u->u_viewProjection * float4(in.a_position, 1.0);
    out.color = in.a_color;
    out.texIndex = in.a_texIndex;
    out.texCoord = in.a_texCoord;
    return out;
}

fragment float4 textFragmentShader(VertexOut in [[stage_in]], TextureArray textures, constant Uniforms* u [[buffer(2)]]) {
    constexpr sampler textureSampler (mag_filter::linear, min_filter::linear);
    float alpha = textures.textures[in.texIndex].sample(textureSampler, in.texCoord).r;
    if(alpha < 0.001)
        discard_fragment();
        
    return float4(in.color.rgb * alpha, in.color.a * alpha);
}