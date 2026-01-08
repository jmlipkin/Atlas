#include <metal_stdlib>
using namespace metal;

struct VertexIn {
    float3 position [[attribute(0)]];
    float4 color [[attribute(1)]];
};

struct VertexOut {
    float4 position [[position]];
    float4 color;
};

struct Uniforms {
    int3 position [[id(0)]];
    float4x4 bigData [[id(1)]];
    float4 color [[id(2)]];
};

vertex VertexOut vertexShader(VertexIn in [[stage_in]], constant Uniforms* u [[buffer(1)]]) {
    VertexOut out;
    out.position = float4(in.position, 1.0);
    out.color = in.color;
    return out;
}

fragment float4 fragmentShader(VertexOut in [[stage_in]], constant Uniforms* u [[buffer(1)]]) {
    return u->color;
}