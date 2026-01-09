#include <metal_stdlib>
using namespace metal;

struct PointIn {
    float a_size [[attribute(0)]];
};

struct PointOut {
    float4 position [[position]];
    float size [[point_size]];
};

struct PointUniforms {
    float3 u_position [[id(0)]];
    float4 u_color [[id(1)]];
};

vertex PointOut pointVertexShader(PointIn in [[stage_in]], constant PointUniforms* u [[buffer(1)]]) {
    PointOut out;
    out.position = float4(u->u_position, 1.0f);
    out.size = in.a_size;
    return out;
}

fragment float4 pointFragmentShader(PointOut in [[stage_in]], constant PointUniforms* u [[buffer(1)]]) {
    return u->u_color;
}
