/**
 * Based on <https://github.com/prontopablo/FIP/blob/main/data/ripple.glsl>
 */
// [[Rcpp::depends(RcppParallel, nativeshadr)]]
#include <nativeshadr.h>

float4 frag(int2 wh, RMatrix<int> nr, float freq, float amp, float2 offset) {
  float2 iResolution = float2(nr.ncol(), nr.nrow());
  float2 uv = float2(wh) / iResolution;

  // Center coordinates of the screen with offset
  float2 center = (iResolution / 2.0 + offset) / iResolution;

  // Calculate the distance from the current pixel to the center
  float distance = length(uv - center);

  // Calculate the ripple effect using sine function with parameters
  float ripple = sin(distance * freq) * amp;

  // Offset the texture coordinate based on the ripple effect
  float2 tc = (uv + ripple) * iResolution;

  if (tc.x > iResolution.x || tc.y > iResolution.y || tc.x < 0.0 || tc.y < 0.0) {
    return float4(0.0, 0.0, 0.0, 0.0);
  }
  float4 color = float4(texture_eval(nr, tc)) / 255.0;
  return color;
}

uint32_t shader(int2 wh, RMatrix<int> nr, const vvd& uniforms) {
  return int4_to_icol(frag(wh, nr, uniforms[0][0], uniforms[1][0], float2(uniforms[2][0], uniforms[2][1])) * 255.0);
}

// [[Rcpp::export]]
Rcpp::IntegerVector test_ripple(Rcpp::IntegerMatrix nr, Rcpp::List uni) {
  const std::vector<double>& freq = uni["freq"];
  const std::vector<double>& amp = uni["amp"];
  const std::vector<double>& offset = uni["offset"];

  const vvd uniforms = {freq, amp, offset};
  return vectorize_shader(shader)(nr, uniforms);
}
