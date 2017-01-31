setwd("~/Documents/Projects/UAF/sap-flux/data/2017-01-30///")
long = data.frame(read.csv("105_58.csv"))
names(d) <- c("id", "time", "t1", "t2", "t3", "t_int", "batV", "cnt")
d$time_real = as.POSIXct(d$time, origin="1970-01-01", "America/Anchorage") #Convert time to POSIX
plot(d$t1~d$time_real, ylim=c(0, 25))
plot(d$t2~d$time_real, ylim=c(0, 25))
plot(d$t3~d$time_real, ylim=c(0, 25))
plot(d$t_int~d$time_real, ylim=c(0, 28))

plot(d$cnt~d$time_real)

plot(d$time)
plot(d$cnt)

