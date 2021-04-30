

setMethod("predict", signature("Rcpp_WofostModel"), 
function(object, weather, mstart, soilindex=NULL, soils=NULL, dates=NULL, filename="", overwrite=FALSE, ...)  {

	stopifnot(inherits(weather, "SpatRasterDataset"))
	
	watlim <- object$control$water_limited
	if (watlim) {
		stopifnot(inherits(soilindex, "SpatRaster"))
		stopifnot(inherits(soils, "list"))
		compareGeom(weather[1], soilindex, lyrs=FALSE)
		needed <- c("tmin", "tmax", "srad", "prec", "vapr", "wind")
		scol <- .makeSoilCollection(soils)
	} else {
		needed <- c("tmin", "tmax", "srad")
		scol <- .makeSoilCollection(list( wofost_soil("ec1") ))
	}
	nms <- names(weather)
	if (!(all(nms %in% needed))) {
		stop(paste("missing these variables:", needed[!(nms %in% needed)]))
	}
	weather <- weather[needed]
	nss <- nlyr(weather)
	if (!all(nss == nss[1])) stop("all subdatasets must have the same number of layers")
	
	#if (is.null(dates)) {
		if (!weather$tmin@ptr$hasTime) {
			stop("no dates supplied as argument or with tmin")
		}
		dates <- time(weather$tmin)
	#} else {
		stopifnot(length(dates) == nrow(weather$tmin))
	#}
	if (any(is.na(dates))) {stop("NA in dates not allowed")}
	out <- rast(weather, nlyr=length(mstart))
	
	use_raster <- FALSE
	if (substr(unlist(sources(weather[1])[1]),1,6) == "NETCDF") {
		use_raster <- TRUE
		p <- unlist(sources(weather[1])[1])
		f <- unlist(strsplit(gsub("NETCDF:\"", "", p), "\""))[1]
		weather <- lapply(needed, function(i) raster::brick(f, varname=i))
		names(weather) <- needed
	}
	nc <- ncol(out)
	if (!use_raster) terra::readStart(weather)

	wopt=list(...)
	if (is.null(wopt$names)) wopt$names <- as.character(mstart)
	b <- terra::writeStart(out, filename, overwrite, wopt=wopt)
	for (i in 1:b$n) {
		if (use_raster) {
			tmin <- as.vector(t(raster::getValues(weather$tmin, b$row[i], b$nrows[i])))
			tmax <- as.vector(t(raster::getValues(weather$tmax, b$row[i], b$nrows[i])))
			srad <- as.vector(t(raster::getValues(weather$srad, b$row[i], b$nrows[i])))
			if (watlim) {
				prec <- as.vector(t(raster::getValues(weather$prec, b$row[i], b$nrows[i])))
				vapr <- as.vector(t(raster::getValues(weather$vapr, b$row[i], b$nrows[i])))
				wind <- as.vector(t(raster::getValues(weather$wind, b$row[i], b$nrows[i])))
			}
		} else {
			tmin <- as.vector(t(terra::readValues(weather$tmin, b$row[i], b$nrows[i], 1, nc, mat=TRUE)))
			tmax <- as.vector(t(terra::readValues(weather$tmax, b$row[i], b$nrows[i], 1, nc, mat=TRUE)))
			srad <- as.vector(t(terra::readValues(weather$srad, b$row[i], b$nrows[i], 1, nc, mat=TRUE)))
			if (watlim) {
				prec <- as.vector(t(terra::readValues(weather$prec, b$row[i], b$nrows[i], 1, nc, mat=TRUE)))
				vapr <- as.vector(t(terra::readValues(weather$vapr, b$row[i], b$nrows[i], 1, nc, mat=TRUE)))
				wind <- as.vector(t(terra::readValues(weather$wind, b$row[i], b$nrows[i], 1, nc, mat=TRUE)))
			}
		}
		if (watlim) {
			sidx <- as.vector(soilindex)
			sidx[is.na(sidx)] <- -99
			sids <- as.integer(sidx)
			wof <- object$run_batch(tmin, tmax, srad, prec, vapr, wind, dates, mstart, TRUE, sidx, scol)
		} else {
			wof <- object$run_batch(tmin, tmax, srad, 0, 0, 0, dates, mstart, FALSE, 0, scol)
		}
		
		terra::writeValues(out, round(wof), b$row[i], b$nrows[i])
	}
	if (!use_raster) terra::readStop(weather)

	out <- terra::writeStop(out)
	return(out)
}
)


