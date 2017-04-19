setwd("~/Documents/Projects/UAF/sap-flux/data/2017-04-18 Capture//")
d1 = data.frame(read.csv("CoolTerm Capture 2017-04-18 12-09-49.csv"))
d2 = data.frame(read.csv("CoolTerm Capture 2017-04-18 12-23-04.csv"))
names(d1) <- c("int", "int1", "int2", "int3", "ext", "t1", "t2", "t3", "heater State")
names(d2) <- c("int", "int1", "int2", "int3", "ext", "t1", "t2", "t3", "heater State")

plot(d2$int1, type='p', pch=1, cex=.5, ylim=c(21, 23))
points(d2$int2, pch=1, cex=0.5, col="orange")
points(d2$int3, pch=1, cex=0.5, col="purple")

plot(d2$t1)
plot(d2$t1, type='p', pch=1, cex=.5, ylim=c(19, 21))
points(d2$t2, pch=1, cex=0.5, col="orange")
points(d2$t3, pch=1, cex=0.5, col="purple")

plot(d1$t1, type='p', pch=1, cex=.5, ylim=c(19, 25))
points(d1$t2, pch=1, cex=0.5, col="orange")
points(d1$t3, pch=1, cex=0.5, col="purple")
