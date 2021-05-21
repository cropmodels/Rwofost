\name{predict}

\alias{predict}
\alias{predict,Rcpp_WofostModel-method}


\title{
Spatial WOFOST model predictions
}

\description{
Make spatial predictions with a WOFOST model. First create a model, then use the model with a SpatDataSet of weather and a SpatRaster of soil properties to make spatial predictions.
}


\usage{
\S4method{predict}{Rcpp_WofostModel}(object, weather, mstart, soilindex=NULL,
     soils=NULL, filename="", overwrite=FALSE, ...)
}


\arguments{
  \item{object}{WOFOST model}
  \item{weather}{SpatRasterDataset with weather data. The must be six sub-datasets with daily weather data for the same days and these names: tmin, tmax, prec, srad, wind and vapr}  
  \item{mstart}{dates to start the model}  
  \item{soilindex}{SpatRaster. postive integer with the ID for the soil type to use}  
  \item{soils}{list of wofost soil types}
  \item{filename}{character. Output filename. Optional}
  \item{overwrite}{logical. If \code{TRUE}, \code{filename} is overwritten}
  \item{...}{list. Options for writing files as in \code{\link[terra]{writeRaster}}}
}


\value{
SpatRaster
}


