img <- file.path(
  getwd(),
  "man",
  "figures",
  "razor.svg"
)

hexSticker::sticker(
  img,
  s_x = 1,
  s_width = .5,
  s_height = .5,
  p_size = 20,
  package = "segmentrr",
  p_color = "#4a0093",
  h_size = 2.6,
  h_fill = "#ff58ff",
  h_color = "#930093",
  filename = "man/figures/logo-origin.png"
)

usethis::use_logo("man/figures/logo-origin.png")
pkgdown::build_favicons(overwrite = TRUE)
