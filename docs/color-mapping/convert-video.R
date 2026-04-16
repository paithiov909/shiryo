out_dir <- tempdir()

imgs <- purrr::imap_chr(fs::dir_ls("colormap"), purrr::in_parallel(\(f, i) {
  library(tidymodels)

  pict <- aznyan::read_still(f)
  rgb <-
    farver::decode_native(pict) |>
    farver::decode_colour(to = "rgb")
  dat <-
    dplyr::tibble(
      icol = as.integer(pict),
      r = rgb[, 1],
      g = rgb[, 2],
      b = rgb[, 3]
    )

  pred <-
    augment(
      model,
      new_data = prep(recipe) |> bake(new_data = dat)
    ) |>
    dplyr::select(.pred, icol)

  predicted_img <- as.integer(pred$.pred)
  dim(predicted_img) <- dim(pict)
  class(predicted_img) <- "nativeRaster"

  fp <- file.path(out_dir, basename(f))
  aznyan::write_still(predicted_img, fp)
  fp
}, model = extract_fit_parsnip(color_wf), recipe = color_rec), .progress = TRUE)

gifski::gifski(
  imgs,
  "lm-predicted.gif",
  width = 1280,
  height = 720,
  delay = 1 / 24
)
