setwd("~/Documents/Projects/UAF/sap-flux/data/2017-02-10/")
d = data.frame(read.csv("100_27.csv"))
names(d) <- c("id", "time", "t1", "t2", "t3", "t_int", "batV", "cnt")
d$time_real = as.POSIXct(d$time, origin="1970-01-01", "America/Anchorage") #Convert time to POSIX

dateTime1 <- strptime("10:00 2017/2/9", format="%H:%M %Y/%m/%d", tz="America/Anchorage")
dateTime2 <- strptime("12:30 2017/2/9", format="%H:%M %Y/%m/%d", tz="America/Anchorage")
sub <- d[d$time_real < dateTime2 & d$time_real > dateTime1, ]
plot(sub$cnt)

subStart = sub[1,2]
sub$time = sub$time - subStart
min = min(sub$t1) - 0.1
max = min + 20
par(mfrow=c(3,1),mar=c(2, 2, 2, 2))
plot(sub$t1~sub$time, type='p', pch=1, cex=0.7, ylim=c(min(sub$t1) - 0.1, min(sub$t1) + 3), main="Above Heater")
plot(sub$t3~sub$time, type='p', pch=1, cex=0.7, ylim=c(min(sub$t3) - 0.1, min(sub$t3) + 3), main="Below Heater")
plot(sub$t2~sub$time, type='p', pch=1, cex=0.7, ylim=c(min(sub$t2) - 0.1, min(sub$t2) + 5), main="Beside Heater")

par(mfrow=c(1,1))
plot(sub$t_int~sub$time, type='p', pch=1, cex=0.7, ylim=c(min(sub$t_int) - 0.1, min(sub$t_int) + 1), main="Internal Temp")

par(mfrow=c(1,1))
plot(d$t1~d$time_real)
plot(d$t2~d$time_real)
plot(d$t3~d$time_real)
plot(d$t_int~d$time_real)
plot(d$cnt~d$time_real)
plot(d$batV~d$time_real, ylim=c(0, 4.2))
min(d$batV)
