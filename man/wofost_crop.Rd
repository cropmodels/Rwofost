\name{wofost_crop}

\alias{wofost_crop}

\title{
WOFOST crop parameters
}

\description{
This functions returns a list with parameters describing a crop for the WOFOST model
}

\usage{
wofost_crop(name='')
}

\arguments{
\item{name}{character. Either the name of a crop that comes with the package (see examples) or a filename of a similarly formatted file}
}

\value{ list }

\references{
van Heemst
}

\seealso{ \code{\link{wofost}} }


\examples{
wofost_crop('')
crop <- wofost_crop('barley')
str(crop)
}
