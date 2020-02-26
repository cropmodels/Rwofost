# Author: Robert J. Hijmans
# 2008
# License GPL3


.writeFSE <- function(x, filename, ... ) {
	f <- file(filename, "w")
	nms <- names(x)
	z <- sapply(1:length(x), function(i) {
			if (length(x[[i]]) > 1) {
				txt <- apply(t(x[[i]]), 1, function(i) paste(i, collapse=", "))
				txt = paste(txt, collapse="\n")
			} else {
				txt <- x[[i]]
			}
			txt <- trimws(paste(nms[i], "=", txt, collapse=" "))
			cat(txt, "\n", file=f)
		})
	close(f)
}



.writeFSEwth <- function(w, country='AAA', station=1, lon=0, lat=0, elev=0, path, ... ) {
	
	if (isTRUE(list(...)$dotsboundlat)) {
	  if ( lat > 60) { lat <- 59 }
	  if ( lat < -60 ) { lat <- -59 }
	}
	
  w$year <- yearFromDate(w$date)
  years <- unique(w$year)

  for (yr in years) {
		fname <- paste(path, '/', country, station, '.', substr(yr, 2, 4), sep="")
		thefile <- file(fname, "w")
		
		cat("*-----------------------------------------------------------", "\n", file = thefile)
		cat("*  Created by the R package 'weather'\n", file = thefile)
		cat("*", "\n", file = thefile)
		cat("*  Column    Daily Value\n", file = thefile)
		cat("*     1      Station number\n", file = thefile)
		cat("*     2      Year\n", file = thefile)
		cat("*     3      Day\n", file = thefile)
		cat("*     4      irradiance         KJ m-2 d-1\n", file = thefile)
		cat("*     5      min temperature            oC\n", file = thefile)
		cat("*     6      max temperature            oC\n", file = thefile)
		cat("*     7      vapor pressure            kPa\n", file = thefile)
		cat("*     8      mean wind speed         m s-1\n", file = thefile)
		cat("*     9      precipitation          mm d-1\n", file = thefile)
		cat("*\n", file = thefile)
		cat("** WCCDESCRIPTION=gizmo\n", file = thefile)
		cat("** WCCFORMAT=2\n", file = thefile)
		cat("** WCCYEARNR=", yr, "\n", file = thefile)
		cat("*-----------------------------------------------------------", "\n", file = thefile)

		cat(lon, lat, elev, '  0.00  0.00 \n', file = thefile)

		yw <- w[w$year==yr, ]
		
		yw[is.na(yw)] <- -9999
		for (d in 1:nrow(yw)) {
			cat("1  ", sprintf("%6.0f", yr), sprintf("%5.0f", d), sprintf("%10.0f", yw$srad[d]), sprintf("%8.1f", yw$tmin[d]), sprintf("%8.1f", yw$tmax[d]), sprintf("%8.1f", yw$vapr[d]), sprintf("%8.1f", yw$wind[d]), sprintf("%8.1f", yw$prec[d]), "\n", file=thefile)
		}
		close(thefile)
    }
	return(invisible(fname))
}		
#wth$srad = wth$srad * 1000
#writeFSEwth(wth, 'NLD', 3, 5.67,51.97, 7 )



.readFortranOutput <- function(f, wlim=FALSE) {

	.trim2 <- function(x) return(gsub("^ *|(?<= ) | *$", "", x, perl=TRUE))

	r <- .trim2(readLines(f));
	hdr <- grep("YEAR DAY", r)
	i <- substr(r,1,4) %in% 1970:2016
	x <- r[i]
	x <- strsplit(x, ' ')
	n <- length(x) / 2

	if (!wlim) {
		pot <- t(sapply(x[1:n], rbind))
		pot <- matrix(as.numeric(pot), ncol=ncol(pot))
		colnames(pot) <- unlist(strsplit(r[hdr[1]], ' '))
		return(pot)
	} else {
		wlm <- t(sapply(x[(n+1):length(x)], rbind))
		wlm <- matrix(as.numeric(wlm), ncol=ncol(wlm))
		colnames(wlm) <- unlist(strsplit(r[hdr[2]], ' '))
		return(wlm)
	}
}
