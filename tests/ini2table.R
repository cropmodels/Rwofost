

int2df <- function(f) {
	x  <- trimws(readLines(f))
	x <- x[x!= ""]
	i  <- grep("##", x)
	if (!is.null(i)) x <- x[-i]
	i  <- grep("CRPNAM", x)
	if (!is.null(i)) x <- x[-i]
	
	 
	j  <- grep("#", x)
	v  <- trimws(sapply(strsplit(x[j+1], "="), function(i)i[1]))
	d  <- gsub("\\# ", "", x[j])
	d <- gsub("\\[", "(", d)
	d <- gsub("\\]", ")", d)
	d <- gsub("-; kg kg-1", "", d)

	g <- gregexpr("\\(", d)
	gg <- sapply(g, max)
	dd <- lapply(1:length(gg), function(i) {
		if (gg[i]>0) { 
			c(substr(d[i], 1, gg[i]-1), substr(d[i], gg[i], nchar(d[i])))
		} else {
			c(d[i], "")
		} } )
	dd <- do.call(rbind, dd)
	dd[,2] <- gsub("(cel)", "°C", dd[,2])
	dd[,2] <- gsub("\\(", "", dd[,2])
	dd[,2] <- gsub("\\)", "", dd[,2])
	
	k  <- grep("^\\[", x)
	g <- x[k]
	s  <- (diff(c(k, length(x)+1)) - 1) / 2
	group <- unlist(sapply(1:length(s), function(i) rep(g[i], s[i])))
	group <- gsub("\\[", "", group)
	group <- gsub("\\]", "", group)
	vd <- cbind(group=group, variable=v, description=trimws(dd[,1]), unit=dd[,2])
}

f  <- "C:\\github\\cropmodels\\Rwofost\\inst\\wofost\\crop\\barley.ini"
r <- int2df(f)
r <- r[r[,1] != "nutrient use", ]


paste(as.vector(t(r)), collapse=', ')


params <- gsub("(-; kg kg-1)", "(-)", params)

