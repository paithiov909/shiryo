library(skiagd)

W <- 720L
N <- W / 2 - 40  # 320
tau <- 2 * pi

# キャンバスのサイズを固定するためにグラフィックデバイスを開いておく
cap <- ragg::agg_capture(width = W, height = W)

# レンダリングループに相当するもの
imgs <- purrr::imap_chr(seq(0, 4 * pi, by = 0.02), \(t, idx) {

  # 点列を作成する
  x <- y <- 0
  r <- tau / N
  dat <-
    tidyr::expand_grid(i = seq(N), j = seq(N)) |>
    dplyr::group_by(i) |>
    dplyr::group_modify(~ {
      xs <- ys <- double(N)
      for (j in .x$j) {
        u <- sin(.y$i + y) + sin(r * .y$i + x)
        v <- cos(.y$i + y) + cos(r * .y$i + x)
        x <<- u + t
        y <<- v
        xs[j] <- u
        ys[j] <- y
      }
      data.frame(
        x = xs,
        y = ys,
        z = 1,
        col = rgb(.y$i, .x$j, (y + 2) * N / 4, maxColorValue = N)
      )
    }) |>
    dplyr::ungroup()

  # PNG画像をつくる
  png <-
    canvas("black") |>
    add_point(
      # 作成した点列をアフィン変換してキャンバスの中央に配置する
      dat |>
        dplyr::select(x, y, z) |>
        as.matrix() %*%
        affiner::transform2d() %*%
        affiner::scale2d(N / 2) %*%
        affiner::translate2d(W / 2, W / 2),
      group = seq_len(nrow(dat)),
      color = dplyr::pull(dat, col) |>
        col2rgba(),
      props = paint(
        width = 2,
        blend_mode = BlendMode$Plus,
      )
    ) |>
    as_png()

  # PNG画像を保存する
  fp <- file.path(tempdir(), sprintf("%04d.png", idx))
  writeBin(png, fp)

  fp
}, .progress = TRUE)

dev.off()

# WebPアニメーションとして保存する
aznyan::write_animation(
  imgs,
  filename = "bubble-universe.webp",
  delay = 1 / 15
)
