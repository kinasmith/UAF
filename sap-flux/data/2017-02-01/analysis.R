setwd("~/Documents/Projects/UAF/sap-flux/data/2017-02-01/")
d = data.frame(read.csv("105_27.csv"))
names(d) <- c("id", "time", "t1", "t2", "t3", "t_int", "batV", "cnt")
d$time_real = as.POSIXct(d$time, origin="1970-01-01", "America/Anchorage") #Convert time to POSIX
plot(d$t1~d$time_real)
plot(d$t2~d$time_real)
plot(d$t3~d$time_real)
plot(d$t_int~d$time_real)

plot(d$cnt~d$time_real)

plot(d$time)
plot(d$cnt~d$time_real)
plot(d$batV)

dateTime1 <- strptime("9:00 2017/1/31", format="%H:%M %Y/%m/%d", tz="America/Anchorage")
dateTime2 <- strptime("9:30 2017/1/31", format="%H:%M %Y/%m/%d", tz="America/Anchorage")
sub <- d[d$time_real < dateTime2 & d$time_real > dateTime1, ]
plot(sub$t1~sub$time_real)
plot(sub$t2~sub$time_real)
plot(sub$t3~sub$time_real)
