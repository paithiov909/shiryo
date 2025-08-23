library(nativeshadr)

setwd("docs/beyond-the-heliosphere")
Rcpp::sourceCpp("shader/traverse.cpp")

gifski::save_gif(
  {
    for (time in seq(3 * pi, 8 * pi, by = .0314)) {
      img <- test_traverse(nr_new(640, 360, "white"), list(iTime = time))
      grid::grid.newpage()
      grid::grid.raster(img, interpolate = FALSE)
    }
  },
  gif_file = "anim-traverse.gif",
  width = 640,
  height = 360,
  delay = 1 / 30,
  progress = TRUE
)
