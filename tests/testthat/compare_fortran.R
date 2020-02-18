
## compare with FORTRAN wofost
## 1
crop <- wofost_crop("rapeseed_1001")
f <- system.file("extdata/Netherlands_Swifterbant.csv", package="meteor")
wth_n <- read.csv(f)
wth_n$date <- as.Date(wth_n$date)

cont <- wofost_control()
cont$IPRODL <- 0
soil <- wofost_soil("soil_5")
#wth_n <- wth_n[wth_n$date >= '1977-01-01', ]
cont$modelstart <- as.Date('1977-01-01')
cont$latitude <- 52.57

rp <- wofost(crop, wth_n, soil, cont)

f <- system.file("test/1/wofost.out", package="Rwofost")
d <- readWofostOutput(f)
plot(d[,'DAY'], d[,'LAI'], type='l')
points(rp[,'step'], rp[, 'LAI'])

# yield
plot(d[,'DAY'], d[,'WSO'], type='l')
points(rp[, 1], rp[,'WSO'])


## 2
# no photoperiod effect
crop$IDSL <- 0
rp <- wofost(crop, wth_n, soil, cont)
f <- system.file("test/2/wofost.out", package="Rwofost")
d <- readWofostOutput(f)
plot(d[,'DAY'], d[,'LAI'], type='l')
points(rp[,'step'], rp[, 'LAI'])



## 3 water limited
library(Rwofost)
crop <- wofost_crop("maize_1")
f <- system.file("extdata/Philippines_IRRI.csv", package="meteor")
wth_n <- read.csv(f)
wth_n$date <- as.Date(wth_n$date)
cont <- wofost_control()
soil <- wofost_soil("soil_5")
cont$modelstart <- as.Date('1985-01-01')
cont$elevation <- 21
cont$latitude <- 14.18
cont$ANGSTA <- -0.25
cont$ANGSTB <- -0.45
cont$IPRODL <- 1
rp <- wofost(crop, wth_n, soil, cont)

f <- system.file("test/3/wofost.out", package="Rwofost")
d <- readWofostOutput(f, TRUE)
plot(d[,'DAY'], d[,'LAI'], type='l')
points(rp[,'step'], rp[, 'LAI'])




## 4 water limited with groundwater
library(Rwofost)
crop <- wofost_crop("maize_1")
f <- system.file("extdata/Philippines_IRRI.csv", package="meteor")
wth_n <- read.csv(f)
wth_n$date <- as.Date(wth_n$date)
cont <- wofost_control()
soil <- wofost_soil("ec1")
cont$modelstart <- as.Date("1985-01-01")
cont$elevation <- 21
cont$latitude <- 14.18
cont$ANGSTA <- -0.25
cont$ANGSTB <- -0.45
cont$IPRODL <- 1
cont$IZT <- 1
rp <- wofost(crop, wth_n, soil, cont)

f <- system.file("test/4/wofost.out", package="Rwofost")
d <- readWofostOutput(f, TRUE)
plot(d[,'DAY'], d[,'LAI'], type='l')
points(rp[,'step'], rp[, 'LAI'])



## 5 npk mode
library(Rwofost)
f <- system.file("test/5/wofost_npk_reference_results.csv", package="Rwofost")
d = read.csv(f)
soil <- wofost_soil("ec4")
crop <- wofost_crop("winterwheat_102")
cont <- wofost_control()
cont$IPRODL <- 0
f <- system.file("extdata/Netherlands_Wageningen.csv", package="meteor")
wth <- read.csv(f)
wth$date = as.Date(wth$date)
wth = wth[wth$date >= "1976-01-01", ]
cont$modelstart <- as.Date("1976-01-01")

cont$npk_model = 1
cont$latitude <- 51.58


rp <- wofost(crop, wth, soil, cont)

plot(c(1:nrow(d)), d[, 'LAI'], type = "l")
points(rp[, 'step'], rp[, 'LAI'])
