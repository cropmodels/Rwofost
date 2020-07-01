

setMethod("predict", signature("Rcpp_WofostModel"), 
function(object, x, mstart, dates=NULL, filename="", overwrite=FALSE, wopt=list())  {

	stopifnot(inherits(x, "SpatDataSet"))
	
	needed <- c("tmin", "tmax", "srad", "prec", "vapr", "wind")
	nms <- names(x)
	if (!(all(nms %in% needed))) {
		stop(paste("missing these variables:", needed[!(nms %in% needed)]))
	}
	x <- x[needed]
	nss <- nlyr(x)
	if (!all(nss == nss[1])) stop("all subdatasets must have the same number of layers")
	
	if (is.null(dates)) {
		if (!x$tmin@ptr$hasTime) {
			stop("no dates supplied as argument or with tmin")
		}
		dates <- time(x$tmin)
	}
	if (any(is.na(dates))) {stop("NA in dates not allowed")}
	out <- rast(x, nlyr=length(mstart))
	
	use_raster <- FALSE
	if (substr(unlist(sources(x[1])[1]),1,6) == "NETCDF") {
		use_raster <- TRUE
		p <- unlist(sources(x[1])[1])
		f <- unlist(strsplit(gsub("NETCDF:\"", "", p), "\""))[1]
		x <- lapply(needed, function(i) raster::brick(f, varname=i))
		names(x) <- needed
	}

	nc <- ncol(out)
	if (!use_raster) {
		terra::readStart(x)
		if (is.null(wopt$names)) wopt$names <- as.character(mstart)
	}
	b <- terra::writeStart(out, filename, overwrite, wopt)
	for (i in 1:b$n) {
		if (use_raster) {
			tmin <- as.vector(t(raster::getValues(x$tmin, b$row[i], b$nrows[i])))
			tmax <- as.vector(t(raster::getValues(x$tmax, b$row[i], b$nrows[i])))
			srad <- as.vector(t(raster::getValues(x$srad, b$row[i], b$nrows[i])))
			prec <- as.vector(t(raster::getValues(x$prec, b$row[i], b$nrows[i])))
			vapr <- as.vector(t(raster::getValues(x$vapr, b$row[i], b$nrows[i])))
			wind <- as.vector(t(raster::getValues(x$wind, b$row[i], b$nrows[i])))
		} else {
			tmin <- as.vector(t(terra::readValues(x$tmin, b$row[i], b$nrows[i], 1, nc, mat=TRUE)))
			tmax <- as.vector(t(terra::readValues(x$tmax, b$row[i], b$nrows[i], 1, nc, mat=TRUE)))
			srad <- as.vector(t(terra::readValues(x$srad, b$row[i], b$nrows[i], 1, nc, mat=TRUE)))
			prec <- as.vector(t(terra::readValues(x$prec, b$row[i], b$nrows[i], 1, nc, mat=TRUE)))
			vapr <- as.vector(t(terra::readValues(x$vapr, b$row[i], b$nrows[i], 1, nc, mat=TRUE)))
			wind <- as.vector(t(terra::readValues(x$wind, b$row[i], b$nrows[i], 1, nc, mat=TRUE)))
		}
		wof <- object$run_batch(tmin, tmax, srad, prec, vapr, wind, dates, mstart)
		terra::writeValues(out, wof, b$row[i], b$nrows[i])
	}
	if (!use_raster) {
		terra::readStop(x)
	}
	out <- terra::writeStop(out)
	return(out)
}
)

# SpatRaster::setCategories(unsigned layer, std::vector<std::string> labs, std::vector<double> levs
