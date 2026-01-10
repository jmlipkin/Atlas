#include <metal_stdlib>
using namespace metal;

struct VertexIn {
    float3 a_position [[attribute(0)]];
    float2 a_texCoord [[attribute(1)]];
};

struct VertexOut {
    float4 position [[position]];
    float2 texCoord;
};

struct Uniforms {
    float4x4 u_transform [[id(0)]];
    float4x4 u_viewProjection [[id(1)]];
    float4 u_color [[id(2)]];
};

vertex VertexOut quadVertexShader(VertexIn in [[stage_in]], constant Uniforms* u [[buffer(1)]]) {
    VertexOut out;
    out.position = u->u_viewProjection * u->u_transform * float4(in.a_position, 1.0);
    out.texCoord = in.a_texCoord;
    return out;
}

fragment float4 quadFragmentShader(VertexOut in [[stage_in]], texture2d<float> texture [[texture(0)]], constant Uniforms* u [[buffer(1)]]) {
    constexpr sampler textureSampler (mag_filter::linear, min_filter::linear);
    float4 colorSample = texture.sample(textureSampler, in.texCoord);
    return colorSample * u->u_color;
}