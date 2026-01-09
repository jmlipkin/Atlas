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
    float4 u_color [[id(0)]];
};

vertex VertexOut quadVertexShader(VertexIn in [[stage_in]], constant Uniforms* u [[buffer(1)]]) {
    VertexOut out;
    out.position = float4(in.position, 1.0);
    out.color = u->u_color;
    return out;
}

fragment float4 quadFragmentShader(VertexOut in [[stage_in]], constant Uniforms* u [[buffer(1)]]) {
    return in.color;
}