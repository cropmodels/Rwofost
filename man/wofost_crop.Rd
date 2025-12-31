\name{wofost_crop}

\alias{wofost_crop}

\title{
WOFOST crop parameters
}
\description{

This function returns a list of crop parameters for the WOFOST model.
See this manual \url{https://swap.wur.nl/Documents/WCC_WOFOST_717_Manual.pdf} for their interpretation.
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
van Heemst, H., 1988. Plant data values required for simple and universal simulation models: review and bibliography. Simulation reports CABO-TT. Wageningen.
}

\seealso{ \code{\link{wofost}} }


\examples{
wofost_crop('')
crop <- wofost_crop('barley')
str(crop)
}
