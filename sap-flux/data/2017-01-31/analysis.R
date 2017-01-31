setwd("~/Documents/Projects/UAF/sap-flux/data/2017-01-30///")
long = data.frame(read.csv("105_58.csv"))
names(long) <- c("id", "time", "t1", "t2", "t3", "t_int", "batV", "cnt")
long$time_real = as.POSIXct(long$time, origin="1970-01-01", "America/Anchorage") #Convert time to POSIX
plot(long$t1~long$time_real, ylim=c(0, 25))
plot(long$t2~long$time_real, ylim=c(0, 25))
plot(long$t3~long$time_real, ylim=c(0, 25))
plot(long$t_int~long$time_real, ylim=c(0, 28))

lines(long$cnt~long$time_real)

plot(long$time)
