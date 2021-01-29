#' @noRd
#' @keywords internal
.onUnload <- function(libpath) {
  library.dynam.unload("segmentrr", libpath)
}
