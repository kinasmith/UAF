setwd("~/Documents/Projects/UAF/sap-flux/data/2017-01-25 testing//")
long = data.frame(read.csv("105_21.csv"))
names(long) <- c("id", "time", "t1", "t2", "t3", "t_int", "batV", "cnt")
long$time_real = as.POSIXct(long$time, origin="1970-01-01", "America/Anchorage") #Convert time to POSIX
plot(long$t1~long$time_real, ylim=c(0, 20))
lines(long$cnt~long$time_real)
