\name{wofost}

\alias{wofost}

\title{
WOFOST crop growth model
}

\description{
Run the WOFOST crop growth model. Through this interface, you provide weather data, and crop, soil and control parameters to run the model once. For multiple runs it might be preferable to use \code{\link{wofost_model}} instead.
}

\usage{
wofost(crop, weather, soil, control)
}

\arguments{
\item{crop}{list. Crop parameters}
\item{weather}{data.frame with weather data}
\item{soil}{list. Soil parameters}
\item{control}{list. Model control options}
}

\value{
matrix
}

\seealso{
\code{\link{wofost_model}}
}

\references{
Van Diepen, C.A., J. Wolf, and H van Keulen, 1989. WOFOST: a simulation model of crop production. Soil Use and Management, 5: 16-24

Van Keulen, H. and J. Wolf, 1986. Modelling of agricultural production : weather, soils and crops. \url{http://edepot.wur.nl/168025}
}


\examples{

library(Rwofost)
# weather data
f <- system.file("extdata/18419.rds", package="meteor")
w <- readRDS(f)
w$srad <- w$srad * 1000

crop <- wofost_crop("barley")
soil <- wofost_soil("ec1")

contr <- wofost_control()
contr$modelstart <- as.Date("2012-02-06")
contr$latitude=52.57
contr$elevation=50

d <- wofost(crop, w, soil, contr)

# output
head(d)
tail(d)
plot(d[,"step"], d[, "LAI"])


## Anther example
crop <- wofost_crop("rapeseed_1001")
f <- system.file("extdata/Netherlands_Swifterbant.csv", package="meteor")
wth_n <- read.csv(f)
wth_n$date <- as.Date(wth_n$date)

cont <- wofost_control()
cont$IPRODL <- 0
soil <- wofost_soil("soil_5")
cont$modelstart <- as.Date("1977-01-01")

rp <- wofost(crop, wth_n, soil, cont)
plot(rp[,"step"], rp[, "LAI"])

# yield
plot(rp[, 1], rp[,"WSO"])


## water limited
cont <- wofost_control()
cont$water_limited <- TRUE
cont$modelstart <- as.Date("1985-01-01")

crop <- wofost_crop("maize_1")
f <- system.file("extdata/Philippines_IRRI.csv", package="meteor")
wth_n <- read.csv(f)
wth_n$date <- as.Date(wth_n$date)
soil <- wofost_soil("soil_5")
cont$elevation <- 21
cont$latitude <- 14.18
rp <- wofost(crop, wth_n, soil, cont)

plot(rp[,"step"], rp[, "LAI"])
}
