library(nativeshadr)

setwd("docs/beyond-the-heliosphere")
Rcpp::sourceCpp("shader/retro.cpp")

## テスト
# img <- fastpng::read_png(
#   system.file("images/river.png", package = "nativeshadr"),
#   type = "nativeraster"
# )

# res <- test_retro(img, list(iTime = 120))

# grid::grid.newpage()
# grid::grid.raster(
#   aznyan::blend_hardlight(img, res),
#   interpolate = FALSE
# )

## アニメーション
library(ggplot2)

cap <- ragg::agg_capture(width = 480, height = 360)

gp <-
  ggplot(mtcars, aes(x = wt, y = mpg, colour = cyl)) +
  geom_point(size = 8) +
  theme_dark() +
  scale_color_viridis_c(option = "viridis")
print(gp)

rast <- cap(native = TRUE)
dev.off()

gifski::save_gif(
  {
    for (time in seq_len(200)) {
      fd <- test_retro(rast, list(iTime = time))
      grid::grid.newpage()
      grid::grid.raster(aznyan::blend_hardlight(fd, rast), interpolate = FALSE)
    }
  },
  gif_file = "anim-retro-ggplot.gif",
  width = 480,
  height = 360,
  delay = 1 / 25,
  progress = TRUE
)
