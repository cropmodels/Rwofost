

example_weather <- function() {
	f <- system.file("extdata/18419.rds", package="meteor")
	readRDS(f)
}


.trim2 <- function(x) return(gsub("^ *|(?<= ) | *$", "", x, perl=TRUE))


wofost_model <- function(crop, weather, soil, control) {
	m <- WofostModel$new()
	if (!missing(crop)) { crop(m) <- crop }
	if (!missing(soil)) { soil(m) <- soil }
	if (!missing(control)) { control(m) <- control }
	if (!missing(weather)) { weather(m) <- weather }
	return(m)
}

setMethod("run", signature('Rcpp_WofostModel'), 
	function(x, ...) {
		x$run()
		out <- matrix(x$output$values, ncol=length(x$output$names), byrow=TRUE)
		colnames(out) <- x$output$names
		out <- data.frame(out)
		date <- as.Date(x$control$modelstart, origin="1970-01-01") + out$step
		out <- data.frame(date, out)
#		out$Wtot <- out$WRT + out$WLV + out$WST + out$WSO
		out
	}
)


setMethod("crop<-", signature('Rcpp_WofostModel', 'list'), 
	function(x, value) {
		parameters <- c("TBASEM", "TEFFMX", "TSUMEM", "IDSL", "DLO", "DLC", "TSUM1", "TSUM2", "DTSMTB", "DVSI", "DVSEND", "TDWI", "LAIEM", "RGRLAI", "SLATB", "SPA", "SSATB", "SPAN", "TBASE", "CVL", "CVO", "CVR", "CVS", "Q10", "RML", "RMO", "RMR", "RMS", "RFSETB", "FRTB", "FLTB", "FSTB", "FOTB", "PERDL", "RDRRTB", "RDRSTB", "CFET", "DEPNR", "RDI", "RRI", "RDMCR", "IAIRDU", "KDIFTB", "EFFTB", "AMAXTB", "TMPFTB", "TMNFTB", "CO2AMAXTB", "CO2EFFTB", "CO2TRATB")
		if (is.null(value$IARDU)) {
			value$IAIRDU = 0
		}
		nms <- names(value)
		if (!all(parameters %in% nms)) stop(paste("parameters missing:", paste(parameters[!(parameters %in% nms)], collapse=", ")))
		value <- value[parameters]
		nms <- names(value)
		lapply(1:length(value), function(i) eval(parse(text = paste0("x$crop$p$", nms[i], " <- ", value[i]))))
		return(x)
	}
)

setMethod("soil<-", signature('Rcpp_WofostModel', 'list'), 
	function(x, value) {
		parameters <- c("SMTAB", "SMW", "SMFCF", "SM0", "CRAIRC", "CONTAB", "K0", "SOPE", "KSUB", "SPADS", "SPASS", "SPODS", "SPOSS", "DEFLIM", "IZT", "IFUNRN", "WAV", "ZTI", "DD", "IDRAIN", "NOTINF", "SSMAX", "SMLIM", "SSI", "RDMSOL")
		nms <- names(value)
		if (!all(parameters %in% nms)) stop(paste("parameters missing:", paste(parameters[!(parameters %in% nms)], collapse=", ")))
		value <- value[parameters]
		nms <- names(value)
		lapply(1:length(value), function(i) eval(parse(text = paste0("x$soil$p$", nms[i], " <- ", value[i]))))
		return(x)
	}
)


setMethod("weather<-", signature('Rcpp_WofostModel', 'list'), 
	function(x, value) {
		parameters <- c("date", "srad", "tmin", "tmax", "prec", "wind", "vapr")
		nms <- names(value)
		if (!all(parameters %in% nms)) stop(paste("parameters missing:", paste(parameters[!(parameters %in% nms)], collapse=", ")))
		x$setWeather(value$date, value$tmin, value$tmax, value$srad, value$prec, value$wind, value$vapr)
		return(x)
	}
)



setMethod("weather<-", signature("Rcpp_WofostModel", "list"), 
	function(x, value) {
		parameters <- c("date", "srad", "tmin", "tmax", "prec", "wind", "vapr")
		nms <- colnames(value)
		if (!all(parameters %in% nms)) stop(paste("weather variables missing:", paste(parameters[!(parameters %in% nms)], collapse=", ")))

		w <- DailyWeather$new()
		w$date <- as.integer(value$date)
		w$srad <- value$srad
		w$tmin <- value$tmin
		w$tmax <- value$tmax
		w$prec <- value$prec
		w$wind <- value$wind
		w$vapr <- value$vapr
		
		x$weather <- w
		return(x)
	}
)



setMethod("control<-", signature('Rcpp_WofostModel', 'list'), 
	function(x, value) {
		parameters <- c("modelstart", "cropstart", "IPRODL", "IOXWL", "ISTCHO", "IDESOW", "IDLSOW", "IENCHO", "IDAYEN", "IDURMX")
		nms <- names(value)
		if (!all(parameters %in% nms)) stop(paste("parameters missing:", paste(parameters[!(parameters %in% nms)], collapse=", ")))
		value <- value[parameters]
		nms <- names(value)
		lapply(1:length(value), function(i) eval(parse(text = paste0("x$control$", nms[i], " <- ", value[i]))))
		return(x)
	}
)


wofost_control <- function(filename='') {
	x <- list()
	if (filename != '') {
		ini <- .readIniFile(filename)
		s <- which(ini[,2] == "modelstart")
		if (length(s) > 0){
			startdate <- as.Date(ini[s[1], 3])
			ini <- ini[-s,]
			x$modelstart <- startdate
		}
		s <- which(ini[,2] == "IRRdates")
		if (length(s) > 0){
			IRRdates <- ini[s[1], 3]
			IRRdates <- (strsplit(IRRdates, ","))[[1]]
			IRRdates <- as.Date(IRRdates)
			ini <- ini[-s,]
			x$IRRdates <- IRRdates
		}
		s <- which(ini[,2] == "NPKdates")
		if (length(s) > 0){
			NPKdates <- ini[s[1], 3]
			NPKdates <- (strsplit(NPKdates, ","))[[1]]
			NPKdates <- as.Date(NPKdates)
			ini <- ini[-s,]
			x$NPKdates <- NPKdates
		}
		x <- append(x, .getNumLst(ini))

		#if (length(s) > 0) {
		#	startdate <- as.Date(ini[s[1], 3])
		#	ini <- ini[-s,]
		#	x <- .getNumLst(ini)
		#	x$modelstart <- startdate
		#} else {
		#	x <- .getNumLst(ini)
		#}
		return(x)
	}

	f <- system.file("wofost/control.ini", package="Rwofost")
	ini <- .readIniFile(f)
	s <- which(ini[,2] == "modelstart")
	if (length(s) > 0){
		startdate <- as.Date(ini[s[1], 3])
		ini <- ini[-s,]
		x$modelstart <- startdate
	}
	s <- which(ini[,2] == "IRRdates")
	if (length(s) > 0){
		IRRdates <- ini[s[1], 3]
		IRRdates <- (strsplit(IRRdates, ","))[[1]]
		IRRdates <- as.Date(IRRdates)
		ini <- ini[-s,]
		x$IRRdates <- IRRdates
	}
	s <- which(ini[,2] == "NPKdates")
	if (length(s) > 0){
		NPKdates <- ini[s[1], 3]
		NPKdates <- (strsplit(NPKdates, ","))[[1]]
		NPKdates <- as.Date(NPKdates)
		ini <- ini[-s,]
		x$NPKdates <- NPKdates
	}
	x <- append(x, .getNumLst(ini))
	x
}


wofost_soil <- function(name='') {

	if (file.exists(name)) {
		ini <- .readIniFile(name)
		lst <- .getNumLst(ini)
		return(lst)
	}

	f <- list.files(system.file("wofost/soil", package="Rwofost"), full.names=TRUE)
	soils <- gsub('.ini', '', basename(f))
	if (name %in% soils) {
		i <- which (name == soils)
		ini <- .readIniFile(f[i])
		lst <- .getNumLst(ini)
		return(lst)
  } else if (name == "") {
		message('Choose one of:',paste(soils, collapse=', '))
	} else {
		stop(paste('not available. Provide a valid filename or choose one of:', paste(soils, collapse=', ')))
	}
}



wofost_crop <- function(name='') {
	if (missing(name)) {
		f <- list.files(system.file("wofost/crop", package="Rwofost"), full.names=TRUE)
		crops <- gsub('.ini', '', basename(f))
		stop(paste('not available. Choose one of:', paste(crops, collapse=', ')))
	}

	if (file.exists(name)) {
		ini <- .readIniFile(name)
		lst <- .getNumLst(ini)
		return(lst)
	}

	f <- list.files(system.file("wofost/crop", package="Rwofost"), full.names=TRUE)
    crops <- gsub('.ini', '', basename(f))
    if (name %in% crops) {
		i <- which (name == crops)
		ini <- .readIniFile(f[i])
		lst <- .getNumLst(ini)
		return(lst)
		} else if (name == "") {
			message('Choose one of:', paste(crops, collapse=', '))
		} else {
			stop(paste('not available. Choose a valid filename or one of:', paste(crops, collapse=', ')))
		}
}



readWofostOutput <- function(f, wlim=FALSE) {

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


.getNumLst <- function(ini) {
	v <- ini[,3]
	vv <- sapply(v, function(i) strsplit(i, ','), USE.NAMES = FALSE)
	vv <- sapply(vv, as.numeric)
	lst <- lapply(vv, function(i) {
			if(length(i) > 1) { 
				#matrix(i, ncol=2, byrow=TRUE) 
				matrix(i, nrow=2) 
			} else {
				i
			}
		}
	)
	names(lst) <- ini[,2]
	lst
}

