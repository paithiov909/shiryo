/**
 * 次の記事で書かれているHLSLシェーダーを元に作成した
 * <https://sayachang-bot.hateblo.jp/entry/2019/12/11/231351>
 */
// [[Rcpp::depends(RcppParallel, nativeshadr)]]
#include <nativeshadr.h>

float2 barrel(float2 uv) {
  float s1 = .99, s2 = .125;
  float2 centre = 2. * uv - 1.;
  float barrel = min(1.0 - length(centre) * s1, float1(1.0)) * s2;
  return uv - centre * barrel;
}

float2 CRT(float2 uv) {
  float2 nu = uv * 2. - 1.;
  float2 offset = abs(nu.yx) / float2(6., 4.);
  nu += nu * offset * offset;
  return nu;
}

float Scanline(float2 uv, double iTime) {
  float scanline =
      clamp(0.95 + 0.05 * cos(3.14 * (uv.y + 0.008 * floor(iTime * 15.) / 15.) *
                              240.0 * 1.0),
            0.0, 1.0);
  float grille =
      0.85 + 0.15 * clamp(1.5 * cos(3.14 * uv.x * 640.0 * 1.0), 0.0, 1.0);
  return scanline * grille * 1.2;
}

float4 frag(int2 wh, RMatrix<int> nr, const vvd& uniforms) {
  const std::vector<double>& iTime = uniforms[0];

  float2 i = float2(wh) / float2(nr.ncol(), nr.nrow());

  // barrel distortion
  float2 p = barrel(i.xy);
  float4 col = float4(texture_eval(nr, wh)) / 255.0;

  // color grading
  col.rgb *= float3(1.25, 0.95, 0.7);
  col.rgb = clamp(col.rgb, 0.0, 1.0);
  col.rgb = col.rgb * col.rgb * (3.0 - 2.0 * col.rgb);
  col.rgb = 0.5 + 0.5 * col.rgb;

  // scanline
  col.rgb *= Scanline(i.xy, iTime[0]);

  // crt monitor
  float2 crt = CRT(i.xy);
  crt = abs(crt);
  crt = pow(crt, 15.);
  col.rgb = lerp(col.rgb, float1(.0).xxx, (crt.x + crt.y).xxx);

  // gammma correction
  col.rgb = pow(col.rgb, float1(.4545).xxx);

  return col;
}

uint32_t shader(int2 wh, RMatrix<int> nr, const vvd& uniforms) {
  return int4_to_icol(frag(wh, nr, uniforms) * 255.0);
}

// [[Rcpp::export]]
Rcpp::IntegerVector test_retro(Rcpp::IntegerMatrix nr, Rcpp::List uni) {
  const std::vector<double>& iTime = uni["iTime"];
  const vvd uniforms = {iTime};
  return vectorize_shader(shader)(nr, uniforms);
}
