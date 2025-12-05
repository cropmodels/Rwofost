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

\details{
The weather data must be passed as a data.frame with the following variables and units.

\tabular{llll}{
	\tab \bold{variable} \tab \bold{description} \tab \bold{class / unit} \cr
	\tab \code{date} \tab "Date" class variable \tab - \cr
	\tab \code{srad} \tab Solar radiation  \tab kJ m-2 day-1 \cr
	\tab \code{tmin} \tab Minimum temperature \tab degrees C \cr
	\tab \code{tmax} \tab Maximum temperature \tab degrees C \cr
	\tab \code{vapr} \tab Vapor pressure \tab kPa \cr
	\tab \code{wind} \tab Wind speed \tab m s-1 \cr
	\tab \code{prec} \tab Precipitation \tab mm day-1 \cr
}

Note that there should not be any time gaps between the days in the data.frame 

}

\references{
Van Diepen, C.A., J. Wolf, and H van Keulen, 1989. WOFOST: a simulation model of crop production. Soil Use and Management, 5: 16-24

Van Keulen, H. and J. Wolf, 1986. Modelling of agricultural production : weather, soils and crops. \url{https://edepot.wur.nl/168025}
}


\examples{

# weather data
f <- system.file("extdata/Netherlands_Swifterbant.csv", package="meteor")
w <- read.csv(f)
w$date <- as.Date(w$date)
head(w)

# crop and soil parameters
crop <- wofost_crop("barley")
soil <- wofost_soil("ec1")

# "control" parameters
contr <- wofost_control()
contr$modelstart <- as.Date("1980-02-06")
contr$latitude=52.57
contr$elevation=50


# run model
d <- wofost(crop, w, soil, contr)

# output
head(d)
tail(d)
plot(d[,"step"], d[, "LAI"])


## Another example
crop <- wofost_crop("rapeseed_1001")
soil <- wofost_soil("soil_5")
contr$modelstart <- as.Date("1977-01-01")

rp <- wofost(crop, w, soil, contr)
plot(rp[,"step"], rp[, "LAI"])

# yield
plot(rp[, 1], rp[,"WSO"])


## water limited
contr$water_limited <- TRUE
contr$modelstart <- as.Date("1985-01-01")

crop <- wofost_crop("maize_1")
f <- system.file("extdata/Philippines_IRRI.csv", package="meteor")
wth <- read.csv(f)
wth$date <- as.Date(wth$date)
contr$elevation <- 21
contr$latitude <- 14.18

ma <- wofost(crop, wth, soil, contr)
plot(ma[,"step"], ma[, "LAI"])

}
