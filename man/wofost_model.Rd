\name{wofost_model}

\alias{wofost_model}
\alias{run}
\alias{crop<-}
\alias{soil<-}
\alias{control<-}
\alias{weather<-}
\alias{force<-}
\alias{crop<-,Rcpp_WofostModel,list-method}
\alias{soil<-,Rcpp_WofostModel,list-method}
\alias{control<-,Rcpp_WofostModel,list-method}
\alias{weather<-,Rcpp_WofostModel,data.frame-method}
\alias{force<-,Rcpp_WofostModel,data.frame-method}
\alias{run,Rcpp_WofostModel-method}


\title{
WOFOST crop growth model
}

\description{
Create a WOFOST crop growth model object. Trhough this interface, you first create a model object and then you run it. The advantage is that you can easily change single parameters of the model and run the model again.
}

\usage{
wofost_model(crop, weather, soil, control)
run(x, ...)
crop(x) <- value
soil(x) <- value
control(x) <- value
weather(x) <- value
force(x) <- value
}


\arguments{
\item{crop}{list. Crop parameters}
\item{weather}{data.frame with weather data. See Details}
\item{soil}{list. Soil parameters}
\item{control}{list. Model control options}
\item{value}{crop, weather, soil, or control data, as above; or a data.frame for "force"}
\item{x}{WOFOST model}
\item{\dots}{additiontal arguments. \code{stopError}(logica). If an error occurs and \code{stopError} is \code{TRUE}, and error message is given. Otherwise, a warning is given and some simulation data (up till when the error occurred) may be returned}
}

\value{
WofostModel object
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

crop <- wofost_crop("barley")
soil <- wofost_soil("ec1")
contr <- wofost_control()

contr$modelstart <- as.Date("1980-02-06")
contr$latitude=52.57
contr$elevation=50

# create model
m <- wofost_model(crop, w, soil, contr)

# run model
x <- run(m)
plot(x[,"date"], x[, "LAI"], cex=.5)

# make a change
m$control$modelstart = as.Date("1980-02-20")

# run model again
y <- run(m)
lines(y[,"date"], y[, "LAI"], col="red")

# change the crop 
crop(m) <- wofost_crop("potato_704") 
p <- run(m)
lines(p[,"date"], p[, "LAI"], col="blue")
}


