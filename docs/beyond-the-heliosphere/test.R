setwd("docs/beyond-the-heliosphere")
Rcpp::sourceCpp("shader/test.cpp")

gifski::save_gif(
  {
    for (time in seq(0, 9 * pi, length = 200)) {
      img <- test_raytrace(nara::nr_new(768, 432), list(iTime = time))
      grid::grid.newpage()
      grid::grid.raster(
        nara::nr_flipv(aznyan::blend_ghosting(nara::nr_new(768, 432, "gray20"), img)),
        interpolate = FALSE
      )
    }
  },
  gif_file = "tcsyDj.gif",
  width = 768,
  height = 432,
  delay = 1 / 15,
  progress = TRUE
)
