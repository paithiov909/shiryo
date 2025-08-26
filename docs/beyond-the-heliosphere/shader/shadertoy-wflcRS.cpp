/**
 * 次のShadertoy作品からの移植
 * [Surf Space](https://www.shadertoy.com/view/wflcRS)
 */
// [[Rcpp::depends(RcppParallel, nativeshadr)]]
#include <nativeshadr.h>

// @Shane
// Commutative smooth minimum function. Provided by Tomkh, and taken
// from Alex Evans's (aka Statix) talk:
// http://media.lolrus.mediamolecule.com/AlexEvans_SIGGRAPH-2015.pdf
// Credited to Dave Smith @media molecule.
float smin(float a, float b, float k) {
  float f = max(0., 1. - abs(b - a) / k);
  return min(a, b) - k * .25 * f * f;
}

float4 mainImage(float2 u, float iTime, float2 iResolution) {
  float4 o = float4(0);

  float d, a, e, i, s, t = iTime;
  float3 p = float3(iResolution, 1.);

  // scale coords
  u = (u + u - p.xy) / p.y;

  // cinema bars
  if (abs(u.y) > float1(.8)) {
    return o;
  }

  // sun
  float sun = max(length(u - float2(1.3, .65)), float1(.001));

  // clear o, march 140 steps
  for (int j = floor(i); j++ < 140;

       // accumulate distance
       d += s = min(.005 + abs(s), e = max(.2 * e, .001)),

           // brightness, orb light,
       o += 2. * float4(1, 2, 4, 0) * 1. / (s + e * 1e2),
           o += float4(5, 2, 1, 0) * .001 / sun) {
    // noise loop start, march
    for (p = float3(u * d, d),  // p = ro + rd *d;
                                // entity (orb)
         e = length(p - float3(sin(sin(t * .3) + t * .4) * 16.,
                               2. + sin(sin(t * .5) + t * .2) * 6. + 6.,
                               64. + cos(t * .7) * 24.)) -
             .1,

        // angled plane and horizontal plane
         s = smin(4. + p.y, 7. + p.y + (p.x) * .6, 15.),

        // move the blue stuff
         p.x += t * 7.,

        // noise starts at .5 up to 16., grow by a+=a
         a = .5;
         a < 16.; a += a) {
      // apply noise
      s += abs(dot(sin(p.z + t + p * a), .1 + p - p)) / a;
    }
  }
  // tanh tonemap, brightness
  o = tanh(o / 2.);

  return o;
}

uint32_t shader(int2 wh, RMatrix<int> nr, const vvd& uniforms) {
  const std::vector<double>& iTime = uniforms[0];
  float2 iResolution = float2(nr.ncol(), nr.nrow());
  return int4_to_icol(
      clamp(mainImage(float2(wh), (float)iTime[0], iResolution), 0.0, 1.0) *
      255.0);
}

// [[Rcpp::export]]
Rcpp::IntegerVector test_shadertoy(Rcpp::IntegerMatrix nr, Rcpp::List uni) {
  const std::vector<double>& iTime = uni["iTime"];
  const vvd uniforms = {iTime};
  return vectorize_shader(shader)(nr, uniforms);
}
