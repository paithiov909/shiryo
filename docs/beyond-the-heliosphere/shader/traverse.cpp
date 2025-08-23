/**
 * 次のGLSLSandboxからの移植
 * <https://glslsandbox.com/e#109628.0>
 */
// [[Rcpp::depends(RcppParallel, nativeshadr)]]
#include <nativeshadr.h>

static const int iterations = 14;
static const float formuparam2 = 0.79;
static const int volsteps = 5;
static const float stepsize = 0.390;
static const float zoom = 0.900;
static const float tile = 0.850;
static const float speed2 = 0.0;
static const float brightness = 0.003;
static const float darkmatter = 0.400;
static const float distfading = 0.560;
static const float saturation = 0.800;
static const float transverseSpeed = zoom * 2.0;
static const float cloud = 0.11;

float triangle(float x, float a) {
  float output2 = 2.0 * abs(2.0 * ((x / a) - floor((x / a) + 0.5))) - 1.0;
  return output2;
}

float field(float3 p, float time) {
  float strength = 7. + .03 * log(1.e-6 + frac(sin(time) * float1(4373.11)));
  float accum = 0.;
  float prev = 0.;
  float tw = 0.;

  // for (int i = 0; i < 1; ++i) {
  float mag = dot(p, p);
  p = abs(p) / mag + float3(-.5, -.8 + 0.1 * sin(time * 0.7 + 2.0),
                            -1.1 + 0.3 * cos(time * 0.3));
  float w = exp(-float(0) / 7.);
  accum += w * exp(-strength * pow(abs(mag - prev), 2.3));
  tw += w;
  prev = mag;
  // }
  return max(0., 5. * accum / tw - .7);
}

float4 traverse(int2 wh, RMatrix<int> nr, const vvd& uniforms) {
  float2 uv2 = 2. * float2(wh) / float2(512) - 1.;
  float2 uvs = uv2 * float2(512) / 512.;

  const std::vector<double>& iTime = uniforms[0];
  float time = iTime[0];
  float time2 = time;
  float speed = speed2;
  speed = .01 * cos(time2 * 0.02 + 3.1415926 / 4.0);

  float formuparam = formuparam2;

  float2 uv = uvs;

  float a_xz = 0.9;
  float a_yz = -.6;
  float a_xy = 0.9 + time * 0.08;

  float2x2 rot_xz = float2x2(cos(a_xz), sin(a_xz), -sin(a_xz), cos(a_xz));
  float2x2 rot_yz = float2x2(cos(a_yz), sin(a_yz), -sin(a_yz), cos(a_yz));
  float2x2 rot_xy = float2x2(cos(a_xy), sin(a_xy), -sin(a_xy), cos(a_xy));

  float v2 = 1.0;
  float2 mouse = float2(sin(time) / 48., cos(time) / 48.);
  float3 dir = float3(uv * zoom, 1.);
  float3 from = float3(0.0, 0.0, 0.0);
  from.x -= 5.0 * (mouse.x - 0.5);
  from.y -= 5.0 * (mouse.y - 0.5);

  float3 forward = float3(0., 0., 1.);
  from.x += transverseSpeed * (1.0) * cos(0.01 * time) + 0.001 * time;
  from.y += transverseSpeed * (1.0) * sin(0.01 * time) + 0.001 * time;
  from.z += 0.003 * time;

  dir.xy = mul(dir.xy, rot_xy);          // dir.xy *= rot_xy;
  forward.xy = mul(forward.xy, rot_xy);  // forward.xy *= rot_xy;
  dir.xz = mul(dir.xz, rot_xz);          // dir.xz *= rot_xz;
  forward.xz = mul(forward.xz, rot_xz);  // forward.xz *= rot_xz;
  dir.yz = mul(dir.yz, rot_yz);          // dir.yz *= rot_yz;
  forward.yz = mul(forward.yz, rot_yz);  // forward.yz *= rot_yz;

  from.xy = mul(from.xy, -rot_xy);  // from.xy *= -rot_xy;
  from.xz = mul(from.xz, rot_xz);   // from.xz *= rot_xz;
  from.yz = mul(from.yz, rot_yz);   // from.yz *= rot_yz;

  float zooom = (time2 - 3311.) * speed;
  from += forward * zooom;
  float sampleShift = fmod(zooom, stepsize);

  float zoffset = -sampleShift;
  sampleShift /= stepsize;

  float s = 0.24;
  float s3 = s + stepsize / 2.0;
  float3 v = float3(0.);
  float t3 = 0.0;

  float3 backCol2 = float3(0.);
  for (int r = 0; r < volsteps; r++) {
    float3 p2 = from + (s + zoffset) * dir;
    float3 p3 = from + (s3 + zoffset) * dir;

    p2 = abs(float3(tile) - fmod(p2, float3(tile * 2.)));
    p3 = abs(float3(tile) - fmod(p3, float3(tile * 2.)));
    // #ifdef cloud
    t3 = field(p3, time);
    // #endif

    float pa, a = pa = 0.;
    for (int i = 0; i < iterations; i++) {
      p2 = abs(p2) / dot(p2, p2) - formuparam;

      float D = abs(length(p2) - pa);
      a += i > 7 ? min(12., D) : D;
      pa = length(p2);
    }

    a *= a * a;

    float s1 = s + zoffset;

    float fade = pow(distfading, max(0., float(r) - sampleShift));

    v += fade;

    if (r == 0) {
      fade *= (1. - (sampleShift));
    }
    if (r == volsteps - 1) {
      fade *= sampleShift;
    }

    v += float3(s1, s1 * s1, s1 * s1 * s1 * s1) * a * brightness * fade;

    backCol2 += lerp(float1(.11), float1(1.), v2) *
                float3(1.8 * t3 * t3 * t3, 1.4 * t3 * t3, t3) * fade;

    s += stepsize;
    s3 += stepsize;
  }

  v = lerp(float3(length(v)), v, saturation);

  float4 forCol2 = float4(v * .01, 1.);
  // #ifdef cloud
  backCol2 *= cloud;
  // #endif
  backCol2.b *= 1.8;
  backCol2.r *= 0.05;

  backCol2.b = 0.5 * lerp(backCol2.g, backCol2.b, 0.8);
  backCol2.g = 0.0;
  backCol2.bg = lerp(backCol2.gb, backCol2.bg, 0.5 * (cos(time * 0.01) + 1.0));
  forCol2 + float4(backCol2, 1.0);
  return forCol2;
}

uint32_t shader(int2 wh, RMatrix<int> nr, const vvd& uniforms) {
  float4 col = clamp(traverse(wh, nr, uniforms), 0.0, 1.0);
  return int4_to_icol(col * 255.0);
}

// [[Rcpp::export]]
Rcpp::IntegerVector test_traverse(Rcpp::IntegerMatrix nr, Rcpp::List uni) {
  const std::vector<double>& iTime = uni["iTime"];
  const vvd uniforms = {iTime};
  return vectorize_shader(shader)(nr, uniforms);
}
