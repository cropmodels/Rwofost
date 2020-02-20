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
# weather data
f <- system.file("extdata/18419.rds", package="meteor")
w <- readRDS(f)
w$srad <- w$srad * 1000

crop <- wofost_crop('barley')
soil <- wofost_soil('ec1')

contr <- wofost_control()
contr$modelstart <- as.Date("2012-02-06")
loc <- list(latitude=52.57, elevation=50, CO2=360)

# There are two ways to run the model
# A) direct way
d <- wofost(crop, w, soil, contr, loc)

# output
head(d)
tail(d)
plot(d[,'step'], d[, 'LAI'])

# B) indirect way, first create model and then run it
# the advantage is that you can change single parameters of the model and run again

m <- wofost_model(crop, w, soil, contr, loc)
x <- run(m)
plot(x[,'date'], x[, 'LAI'])

m$control$modelstart = as.Date("2012-03-06")
x <- run(m)
lines(x[,'date'], x[, 'LAI'])


## Other examples
crop <- wofost_crop("rapeseed_1001")
f <- system.file("extdata/Netherlands_Swifterbant.csv", package="meteor")
wth_n <- read.csv(f)
wth_n$date <- as.Date(wth_n$date)

cont <- wofost_control()
cont$IPRODL <- 0
soil <- wofost_soil("soil_5")
cont$modelstart <- as.Date('1977-01-01')

rp <- wofost(crop, wth_n, soil, cont, loc)
plot(rp[,'step'], rp[, 'LAI'])

# yield
plot(rp[, 1], rp[,'WSO'])


## water limited
cont <- wofost_control()
cont$IPRODL <- 1
cont$modelstart <- as.Date('1985-01-01')

crop <- wofost_crop("maize_1")
f <- system.file("extdata/Philippines_IRRI.csv", package="meteor")
wth_n <- read.csv(f)
wth_n$date <- as.Date(wth_n$date)
soil <- wofost_soil("soil_5")
loc$elevation <- 21
loc$latitude <- 14.18
loc$ANGSTA <- -0.25
loc$ANGSTB <- -0.45
rp <- wofost(crop, wth_n, soil, cont, loc)

plot(rp[,'step'], rp[, 'LAI'])


## with groundwater
cont$IZT <- 1
rp <- wofost(crop, wth_n, soil, cont)
plot(rp[,'step'], rp[, 'LAI'])
}


