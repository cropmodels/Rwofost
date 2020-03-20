\name{wofost_crop}

\alias{wofost_crop}

\title{
WOFOST crop parameters
}

\description{
This function returns a list of crop parameters for the WOFOST model.
See this manual \url{https://www.wur.nl/en/show/WOFOST-7.1-User-Manual.htm} for their interpretation.
}

\usage{
wofost_crop(name="", describe=FALSE)
}

\arguments{
\item{name}{character. Either the name of a crop that comes with the package (see examples) or a filename of a similarly formatted file}
\item{describe}{logical. If \code{TRUE} avaialble metadata is printed}
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
