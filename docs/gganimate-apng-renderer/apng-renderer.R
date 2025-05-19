apng_renderer <- function(file = NULL, loop = TRUE) {
  check_installed("apng", "to use the `apng_renderer`")
  function(frames, fps) {
    if (is.null(file)) {
      file <- tempfile(fileext = ".png")
    }
    if (!all(grepl(".png$", frames))) {
      cli::cli_abort("{.pkg apng} only supports png files", call. = FALSE)
    }
    loop <- if (loop) 0 else 1
    apng::apng(frames, output_file = file, num_plays = loop, delay_num = 1, delay_den = fps)
    apng_file(file)
  }
}

apng_file <- function(file) {
  check_string(file, allow_empty = FALSE)
  if (!grepl(".png$", file)) cli::cli_abort("{.arg file} must point to a png file")
  class(file) <- "apng_image"
  file
}

print.apng_image <- function(x, ...) {
  viewer <- getOption("viewer", utils::browseURL)
  if (is_function(viewer) && length(x)) {
    viewer(x)
  }
  invisible(x)
}

knit_print.apng_image <- function(x, options, ...) {
  knitr::knit_print(htmltools::browsable(as_apng_html(x, width = get_chunk_width(options))), options, ...)
}

as_apng_html <- function(x, width = NULL, alt = "") {
  check_installed("base64enc", "for showing apng")
  check_installed("htmltools", "for showing apng")
  if (is.null(width)) width <- "100%"
  image <- base64enc::dataURI(file = x, mime = "image/apng")
  htmltools::tags$img(src = image, alt = alt, width = width)
}

split.apng_image <- function(x, f, drop = FALSE, ...) {
  cli::cli_abort("{.cls apng_image} objects does not support splitting")
}
