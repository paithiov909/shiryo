max_of_3rd_col <-
  \(csv_text = "10,11,12\n7,8,9\n4,5,6\n1,2,3") {
    dat <- read.csv(text = csv_text)
    mx <- max(dat[[3]])
    return(mx)
  }

max_of_3rd_col()

debug(max_of_3rd_col)

max_of_3rd_col()

undebug(max_of_3rd_col)
