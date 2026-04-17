library(obsws)
dotenv::load_dot_env()

data(ReqType)

client <- Client$new(url = paste0("ws://", Sys.getenv("OBS_HOST"), ":4455"))
client$connect(password = Sys.getenv("OBS_PASSWORD"))
on.exit(client$disconnect())

if (client$current_state() == "identified") {
  req_id <- client$emit(ReqType$GetSceneList, NULL)
  resp <- waiter_for_response(10)(client, req_id)
  d <- parse_data(resp, query = "/d/responseData/scenes")[[1]]
  scene_name <- d[[1]]$sceneName

  req_id <- client$emit(ReqType$GetSceneItemList, list(sceneName = "シーン"))
  resp <- waiter_for_response(10)(client, req_id)
  d <- parse_data(resp, query = "/d/responseData/sceneItems")[[1]]
  items <- purrr::list_transpose(d)
  media_item <- items$sourceName[which(items$inputKind == "ffmpeg_source")]
  if (is.null(media_item)) stop("ffmpeg_source not found")
}

gen_url <- local({
  counter <- 0
  function(name = "rtmp://localhost/stream%03d") {
    counter <<- counter + 1
    sprintf(name, counter)
  }
})

url <- gen_url()
job <- callr::r_bg(
  function(url, width, height, fps) {
    library(skiagd)
    png(nullfile(), width = width, height = height)

    n_frames <- 6000
    n_circles <- 70
    radius <- runif(n_circles, min = .25, max = 2) |> sort()
    trans <- matrix(c(60, 0, width / 2, 0, 60, height / 2, 0, 0, 1), ncol = 3)

    circle <- \(amp, freq, phase) {
      amp * 1i^(freq * seq(0, 600, length.out = n_circles) + phase)
    }

    st <-
      periscope::create_stream(
        name = url,
        width = width,
        height = height,
        fps = fps
      )

    on.exit(
      {
        dev.off()
        close(st)
        gc()
        gc()
      },
      add = TRUE
    )

    for (a in seq(0, 24 * pi, length.out = n_frames + 1)[-1]) {
      l <- sin(pi * (2 * a - .5)) + 1
      z <- circle(pi / 6, -pi, 0) +
        circle(l, ceiling(a), -9 * cos(a) + 1) +
        circle(l / 2 - 1, ceiling((-a + (7 / 2)) %% 7) - 7, -7 * cos(a) + 1)

      hue <- (a + (Re(z / pi))) %% 1
      colours <- grDevices::hsv(hue, .66, .75, alpha = 1)

      rast <- canvas("#04010F") |>
        add_circle(
          cbind(Re(z), Im(z), 1) %*% trans,
          radius = log(max(width, height), exp(.5)) * radius,
          color = col2rgba(colours),
          props = paint(
            style = Style$Fill,
            blend_mode = BlendMode$Plus,
          ),
        ) |>
        as_nativeraster() |>
        aznyan::sepia()

      periscope::send_frame(st, rast)
    }
    TRUE
  },
  args = list(url = url, width = 640, height = 480, fps = 25)
)

if (client$current_state() == "identified") {
  req_id <- client$emit(ReqType$SetInputSettings, list(
    inputName = media_item,
    inputSettings = list(
      input = stringr::str_replace(url, "localhost", "diginnos.local")
    )
  ))
  wait_for_messages()
}
