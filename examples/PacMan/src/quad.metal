#include <metal_stdlib>
using namespace metal;

struct VertexIn {
    float3 position [[attribute(0)]];
};

struct VertexOut {
    float4 position [[position]];
    float4 color;
};

struct Uniforms {
    float4x4 u_transform [[id(0)]];
    float4x4 u_viewProjection [[id(1)]];
    float4 u_color [[id(2)]];
};

vertex VertexOut quadVertexShader(VertexIn in [[stage_in]], constant Uniforms* u [[buffer(1)]]) {
    VertexOut out;
    out.position = u->u_viewProjection * u->u_transform * float4(in.position, 1.0);
    out.color = u->u_color;
    // out.color.rg = in.position.xy;
    // out.color.ba = float2(0, 1);
    return out;
}

fragment float4 quadFragmentShader(VertexOut in [[stage_in]], constant Uniforms* u [[buffer(1)]]) {
    return in.color;
}