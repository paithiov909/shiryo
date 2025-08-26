library(nara)

setwd("docs/beyond-the-heliosphere")
Rcpp::sourceCpp("shader/shadertoy-wflcRS.cpp")

gifski::save_gif(
  {
    for (time in seq(3 * pi, 6 * pi, length.out = 100)) {
      img <- test_shadertoy(nr_new(640, 360, "white"), list(iTime = time))
      grid::grid.newpage()
      grid::grid.raster(
        nr_flipv(
          # Shadertoyは背景が黒いが、Rではデフォルトだと白いので、黒を置く
          aznyan::blend_over(nr_new(640, 360, "black"), img)
        ),
        interpolate = FALSE
      )
    }
  },
  gif_file = "anim-shadertoy.gif",
  width = 640,
  height = 360,
  delay = 1 / 20,
  progress = TRUE
)
