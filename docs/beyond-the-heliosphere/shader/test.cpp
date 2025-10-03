/**
 * [Spacefloat](https://www.shadertoy.com/view/tcsyDj)
 **/

// [[Rcpp::depends(RcppParallel, nativeshadr)]]
#include <nativeshadr.h>

float4 mainImage(float2 u, float2 iResolution, float iTime) {
    float d,a,e,i,s,t = iTime * 1e-2;
    float3 p = float3(iResolution, 1.);

    // scale coords
    u = (u+u-p.xy)/p.y;

    // sun
    float sun = max(length(u-float2(1.2, .6)), float1(.001));

    float4 o = float4(i);
    // clear o, march 140 steps
    for(int j = i; j++<40;

        // accumulate distance
        d += s = min(.08+.5*abs(s), e=max(.5*e, .001)),

        // little light + big light
        o += 1./(lerp(s,e,float1(.1))) + float4(5,6,7,0)*.001/sun) {

        // noise loop start, march
        for (p = float3(u*d,d + t * 32.), // p = ro + rd *d;

            // entity (orb), len(p - vec3(wigglez)) - radius
            // p.y is mirrored
            e = length(p
                     - float3(tanh(cos(t*.3)*3.) * 128. - 128.,
                       sin(sin(t)+t*2.) * 64. + 128.,
                       256.+t * 32.+cos(t*.7)*24.))-.01,

            // plane
            s = 4. + p.y + p.x * .3,

            // noise starts at .2 up to 4., grow by a+=a
            a = .2; a < 4.; a += a) {

            // apply turbulence
            p += cos(2.*t*a+p.yzx*.4);
            // apply noise
            s += abs(dot(sin(3.*t + p * a), float3(.2))) / a;
          }
    }
    // tanh tonemap, brightness
    o = tanh(o/1e1);
    return o;
}


uint32_t shader(int2 wh, RMatrix<int> nr, const vvd& uniforms) {
  float2 iResolution = float2(nr.ncol(), nr.nrow());
  float4 col = clamp(mainImage(float2(wh), iResolution, uniforms[0][0]), 0.0, 1.0);
  return int4_to_icol(col * 255.0);
}

// [[Rcpp::export]]
Rcpp::IntegerVector test_raytrace(Rcpp::IntegerMatrix nr, Rcpp::List uni) {
  const std::vector<double>& iTime = uni["iTime"];
  const vvd uniforms = {iTime};
  return vectorize_shader(shader)(nr, uniforms);
}
