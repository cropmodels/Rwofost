\name{wofost_soil}

\alias{wofost_soil}

\title{
WOFOST soil parameters
}

\description{
This function returns a list with soil parameters for the WOFOST model.
See this manual \url{https://www.wur.nl/en/show/WOFOST-7.1-User-Manual.htm} for their interpretation.

}

\usage{
wofost_soil(name='')
}

\arguments{
\item{name}{character. Either the name of a soil that comes with the package (see examples) or a filename of a similarly formatted file}
}

\value{ list }


\seealso{ \code{\link{wofost}} }

\examples{
wofost_soil('')
soil <- wofost_soil('ec1')
str(soil)
}
