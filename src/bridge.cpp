// [[Rcpp::plugins(cpp11)]]
#define R_NO_REMAP
#include <Rcpp.h>
#include <cstdlib>
#include "../inst/include/libsegmentrr.h"

using namespace Rcpp;

//' Tiny Segmenter
//'
//' This function bridges between Rcpp and libsegmentrr
//'
//' @param v character vector
//' @return character vector
//'
//' @export
//'
// [[Rcpp::interfaces(r, cpp)]]
// [[Rcpp::export]]
Rcpp::CharacterVector segmenter(Rcpp::CharacterVector v) {

  std::function< Rcpp::String(Rcpp::String) > func_obj = [](Rcpp::String x) {
    const char* s = x.get_cstring();
    const std::size_t n = std::strlen(s);
    const std::ptrdiff_t len = n;
    const GoString m = { s, len };

    char* response = segment(m);
    const std::string res = response;

    free(response);

    const Rcpp::String result = res;
    return result;
  };

  const Rcpp::CharacterVector result = sapply(v, func_obj) ;
  return result;
}
