

.getNumLst <- function(ini) {
	v <- ini[,3]
	vv <- sapply(v, function(i) strsplit(i, ","), USE.NAMES = FALSE)
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



.example_weather <- function() {
	f <- system.file("extdata/18419.rds", package="meteor")
	readRDS(f)
}


wofost_control <- function(filename="") {
	x <- list()
	if (filename != "") {
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


wofost_soil <- function(name="") {

	if (file.exists(name)) {
		ini <- .readIniFile(name)
		lst <- .getNumLst(ini)
		return(lst)
	}

	f <- list.files(system.file("wofost/soil", package="Rwofost"), full.names=TRUE)
	soils <- gsub(".ini", "", basename(f))
	if (name %in% soils) {
		i <- which (name == soils)
		ini <- .readIniFile(f[i])
		lst <- .getNumLst(ini)
		return(lst)
  } else if (name == "") {
		message("Choose one of:",paste(soils, collapse=", "))
	} else {
		stop(paste("not available. Provide a valid filename or choose one of:", paste(soils, collapse=", ")))
	}
}



wofost_crop <- function(name="") {
	if (missing(name)) {
		f <- list.files(system.file("wofost/crop", package="Rwofost"), full.names=TRUE)
		crops <- gsub(".ini", "", basename(f))
		stop(paste("not available. Choose one of:", paste(crops, collapse=", ")))
	}

	if (file.exists(name)) {
		ini <- .readIniFile(name)
		lst <- .getNumLst(ini)
		return(lst)
	}

	f <- list.files(system.file("wofost/crop", package="Rwofost"), full.names=TRUE)
    crops <- gsub(".ini", "", basename(f))
    if (name %in% crops) {
		i <- which (name == crops)
		ini <- .readIniFile(f[i])
		lst <- .getNumLst(ini)
		return(lst)
		} else if (name == "") {
			message("Choose one of:", paste(crops, collapse=", "))
		} else {
			stop(paste("not available. Choose a valid filename or one of:", paste(crops, collapse=", ")))
		}
}

