

setMethod("predict", signature("Rcpp_WofostModel"), 
function(object, weather, mstart, soils=NULL, soiltypes=NULL, filename="", overwrite=FALSE, ...)  {

	stopifnot(inherits(weather, "SpatRasterDataset"))
	stopifnot(inherits(mstart, "Date"))
	if (any(is.na(mstart))) {
		stop("mstart cannot be NA")
	}
	
	watlim <- object$control$water_limited
	if (watlim) {
		needed <- c("tmin", "tmax", "srad", "prec", "vapr", "wind")
		if (is.null(soils)) {
			scol <- Rwofost:::.makeSoilCollection(list( wofost_soil("ec1") ))
		} else {
			stopifnot(inherits(soils, "SpatRaster"))
			if (!("index" %in% names(soils))) {
				stop("no layer called 'index' in soils")
			}	
			stopifnot(inherits(soiltypes, "list"))
			terra::compareGeom(weather[1], soils, lyrs=FALSE)
			scol <- .makeSoilCollection(soiltypes)
		}
	} else {
		needed <- c("tmin", "tmax", "srad")
		scol <- Rwofost:::.makeSoilCollection(list( wofost_soil("ec1") ))
	}
	nms <- names(weather)
	if (!(all(needed %in% nms))) {
		stop(paste("missing these variables:", paste(needed[!(needed %in% nms)], collapse=", ")))
	}
	weather <- weather[needed]
	nss <- terra::nlyr(weather)
	if (!all(nss == nss[1])) stop("all subdatasets must have the same number of layers")
	
	if (!terra::has.time(weather$tmin)) {
		stop("no dates supplied as argument or with tmin")
	}
	dates <- terra::time(weather$tmin)
	stopifnot(length(dates) == terra::nlyr(weather$tmin))
	if (any(is.na(dates))) {stop("NA in dates not allowed")}
	out <- terra::rast(weather)
	terra::nlyr(out) <- length(mstart)
	
	use_raster <- FALSE
	if (substr(unlist(terra::sources(weather[1])[1]),1,6) == "NETCDF") {
		use_raster <- TRUE
		p <- unlist(terra::sources(weather[1])[1])
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
			if (!is.null(soils)) {
				sidx <- as.vector(terra::readValues(soils$index, b$row[i], b$nrows[i], 1, nc))
				sidx[is.na(sidx)] <- -99
				sidx <- as.integer(sidx)
				if ("depth" %in% names(soils)) {
					depth <- as.vector(terra::readValues(soils$depth, b$row[i], b$nrows[i], 1, nc))
					depth[is.na(depth)] <- -99
				} else {
					depth <- rep(-99, length(sidx))
					warning("soils does not have a depth layer")
				}
				wof <- object$run_batch(tmin, tmax, srad, prec, vapr, wind, dates, mstart, sidx, scol, depth)
			} else {
				wof <- object$run_batch(tmin, tmax, srad, prec, vapr, wind, dates, mstart, 0[], scol, 0[])			
			}
		} else {
			wof <- object$run_batch(tmin, tmax, srad, 0, 0, 0, dates, mstart, 0, scol, 0)
		}
		
		terra::writeValues(out, round(wof), b$row[i], b$nrows[i])
	}
	if (!use_raster) terra::readStop(weather)

	out <- terra::writeStop(out)
	return(out)
}
)


