
<!-- README.md is generated from README.Rmd. Please edit that file -->

# segmentrr <img src='https://raw.githack.com/paithiov909/segmentrr/master/man/figures/logo.png' align="right" height="139" />

<!-- badges: start -->
<!-- badges: end -->

> ‘Rcpp’ Port of TinySegmenter

## System Requirements

-   GNU make
-   GNU GCC
-   Go
-   C++11

## Usage

``` r
# remotes::install_github("paithiov909/segmentrr")
print(escaped)
#> [1] "耳をすませば"   "聞こえてくるよ" "君のとなり"     "目と鼻の先"
segmentrr::segment(escaped)
#> [[1]]
#> [1] "耳"       "を"       "すませば"
#> 
#> [[2]]
#> [1] "聞こえ" "て"     "くるよ"
#> 
#> [[3]]
#> [1] "君"   "の"   "と"   "なり"
#> 
#> [[4]]
#> [1] "目"   "と"   "鼻"   "の先"
```

## License

MIT license.

This software includes the works licensed under BSD. See ‘Copyrights
Notice’ and ‘Licenses’ section of
[nyarla/go-japanese-segmenter](https://github.com/nyarla/go-japanese-segmenter)
for details.
