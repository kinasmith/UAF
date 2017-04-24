setwd("~/Documents/Projects/UAF/sap-flux/data/2017-04-20 Local Testing (inside(dough))/")
d = data.frame(read.csv("100_5.csv"))
names(d) <- c("id", "time", "t1", "t2", "t3", "internal", "batV", "heater", "solar", "count")
d$time_real = as.POSIXct(d$time, origin="1970-01-01", tz = "GMT") #Convert time to POSIX
#"America/Anchorage"
plot(d$t2~d$time_real)

dateTime1 <- strptime("10:00 2017/4/21", format="%H:%M %Y/%m/%d", tz="GMT")
dateTime2 <- strptime("10:30 2017/4/21", format="%H:%M %Y/%m/%d", tz="GMT")
sub <- d[d$time_real < dateTime2 & d$time_real > dateTime1, ]

subStart = sub[1,2]
sub$time = sub$time - subStart
min = min(sub$t1) - 0.1
max = min + 20

par(mfrow=c(3,2),mar=c(2, 2, 2, 2))
plot(sub$t1~sub$time, type='p', pch=1, cex=0.7, ylim=c(min(sub$t1) - 0.1, min(sub$t1) + 1.5), main="Above Heater")
plot(sub$heater~sub$time_real, type='p', pch=1, cex=0.7, ylim=c(0,1), main="Heater Status")
plot(sub$t3~sub$time, type='p', pch=1, cex=0.7, ylim=c(min(sub$t3) - 0.1, min(sub$t3) + 1.5), main="Below Heater")
plot(sub$batV~sub$time, type='p', pch=1, cex=0.7, ylim=c(3, 4.5), main="Bat Voltage")
plot(sub$t2~sub$time, type='p', pch=1, cex=0.7, ylim=c(min(sub$t2) - 0.1, min(sub$t2) + 1.5), main="Beside Heater")
plot(sub$internal~sub$time, type='p', pch=1, cex=0.7, ylim=c(min(sub$internal) - 0.1, min(sub$internal) + 1.5), main="Internal")

par(mfrow=c(1,1), mar=c(2,2,2,2))
plot(sub$solar~sub$time, type='p', pch=1, cex=0.7, ylim=c(0,1), main="Solar Good")

mean(sub$solar)

par(mfrow=c(2,1), mar=c(2,2,2,2))
plot(sub$internal~sub$time, type='p', pch=1, cex=0.7, main="internal")
plot(sub$solar)


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

m1 <- max(d$t1)
m2 <- max(d$t2)
m3 <- max(d$t3)


