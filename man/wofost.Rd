\name{wofost}

\alias{wofost}

\title{
WOFOST crop growth model
}

\description{
This functions runs the WOFOST crop growth model
}

\usage{
wofost(crop, weather, soil, control, loc)
}

\arguments{
\item{crop}{list. Crop parameters}
\item{weather}{data.frame with weather data}
\item{soil}{list. Soil parameters}
\item{control}{list. Model control options}
\item{location}{list. Location parameters}
}

\value{
matrix
}

\references{
Van Diepen, C.A., J. Wolf, and H van Keulen, 1989. WOFOST: a simulation model of crop production. Soil Use and Management, 5: 16-24

Van Keulen, H. and J. Wolf, 1986. Modelling of agricultural production : weather, soils and crops. \url{http://edepot.wur.nl/168025}
}


\examples{
# input data
library(Rwofost)
w <- example_weather()
w$srad <- w$srad * 1000
crop <- wofost_crop('barley')
soil <- wofost_soil('ec1')
contr <- wofost_control()
contr$modelstart <- as.Date("2012-02-06")
loc <- list(latitude=52.57, elevation=50, CO2=360)

# run model
d <- wofost(crop, w, soil, contr, loc)

# output
head(d)
tail(d)
plot(d[,'step'], d[, 'LAI'])

# or 
m <- wofost_model(crop, w, soil, contr, loc)
x <- run(m)
plot(x[,'date'], d[, 'LAI'])




## compare with FORTRAN wofost
## 1
library(Rwofost)
crop <- wofost_crop("rapeseed_1001")
f <- system.file("extdata/Netherlands_Swifterbant.csv", package="meteor")
wth_n <- read.csv(f)
wth_n$date <- as.Date(wth_n$date)

cont <- wofost_control()
cont$IPRODL <- 0
soil <- wofost_soil("soil_5")
cont$modelstart <- as.Date('1977-01-01')

rp <- wofost(crop, wth_n, soil, cont, loc)

#m <- wofost_model(crop, wth_n, soil, cont)
#rp <- run(m)


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
rp <- wofost(crop, wth_n, soil, cont, loc)
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
cont$IPRODL <- 1
loc$elevation <- 21
loc$latitude <- 14.18
loc$ANGSTA <- -0.25
loc$ANGSTB <- -0.45
rp <- wofost(crop, wth_n, soil, cont, loc)

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

rp <- wofost(crop, wth, soil, cont, loc)

plot(c(1:nrow(d)), d[, 'LAI'], type = "l")
points(rp[, 'step'], rp[, 'LAI'])
}
