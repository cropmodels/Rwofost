

.getNumLst <- function(ini, make_matrix=TRUE) {
	v <- ini[,3]
	vv <- sapply(v, function(i) strsplit(i, ","), USE.NAMES = FALSE)
	vv <- sapply(vv, as.numeric)
	if (make_matrix) {
		vv <- lapply(vv, function(i) {
				if (length(i) > 1) { 
					#matrix(i, ncol=2, byrow=TRUE) 
					matrix(i, nrow=2) 
				} else {
					i
				}
			}
		)
	} 
	names(vv) <- ini[,2]
	vv
}



.example_weather <- function() {
	f <- system.file("extdata/18419.rds", package="meteor")
	readRDS(f)
}


.notavailable <- function(group, error=TRUE) {
	if (group == "crop") {
		f <- list.files(system.file("wofost/crop", package="Rwofost"), full.names=TRUE)
	} else {
		f <- list.files(system.file("wofost/soil", package="Rwofost"), full.names=TRUE)
	}
	x <- gsub(".ini", "", basename(f))
	y <- rep("", 5*ceiling((length(x))/5))
	y[1:length(x)] <- x
	y <- matrix(y, ncol=5, byrow=TRUE)
	y <- apply(y, 1, function(i)paste(i, collapse=", "))
	y <- paste(gsub(", ,", "", y), collapse="\n")
	if (error) {
		stop(paste(group, "not available. Choose one of:\n"), y)
	} 
	return(y)
}


.as.numeric_date <- function(x) {
	if (all(nchar(x) == 10)) {
		if (sapply(strsplit(x, "-"), length) == 3) {
			return(as.Date(x))
		}
	}
	as.numeric(x)
}


wofost_control <- function(filename="") {
	x <- list()
	filename <- trimws(filename)
	if (filename == "") {
		filename <- system.file("wofost/control.ini", package="Rwofost")
	} 
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
		
	x <- append(x, .getNumLst(ini, make_matrix=FALSE))
	x
}


wofost_soil <- function(name="") {

	if (missing(name)) {
		return(.notavailable("soil", FALSE))
	}
	name <- trimws(name)
	if (name == "") {
		return(.notavailable("soil", FALSE))
	}
	
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
		ini <- ini[ini[,2] %in% .soil_pars, ]
		lst <- .getNumLst(ini)
		return(lst)
	} else {
		.notavailable("soil")
	}
}


.makeSoilCollection <- function(soils) {
	scol <- WofostSoilCollection$new()
	for (i in 1:length(soils)) {
		s <- .getsoil(soils[[i]])
		scol$add(s)
	}
	return(scol)
}

.printDescription <- function(f) {
	d <- readLines(f)
	d <- grep("\\#\\#", d, value=TRUE)
	if (length(d) > 0) {
		d <- trimws(gsub("\\#\\#", "", d))
		d <- paste(d, collapse="\n")
		cat(d, "\n")
	}
}


wofost_crop <- function(name="", describe=FALSE) {

	if (missing(name)) {
		return(.notavailable("crop", FALSE))
	}
	name <- trimws(name)
	if (name == "") {
		return(.notavailable("crop", FALSE))
	}
	
	if (file.exists(name)) {
		ini <- .readIniFile(name)
		lst <- .getNumLst(ini)
		if (describe) {
			.printDescription(name)
		}
		return(lst)
	}

	f <- list.files(system.file("wofost/crop", package="Rwofost"), full.names=TRUE)
    crops <- gsub(".ini", "", basename(f))
    if (name %in% crops) {
		i <- which (name == crops)
		ini <- .readIniFile(f[i])
		ini <- ini[ini[,2] %in% .crop_pars, ]
		j <- .crop_pars %in% ini[,2]
		if (!(all(j))) {
			warning(paste("missing parameter(s):", paste(.crop_pars[!j], collapse=", ")))
		}		
		lst <- .getNumLst(ini)
		if (describe) {
			.printDescription(f[i])
		}
		return(lst)
	} else {
		.notavailable("crop")
	}
}


