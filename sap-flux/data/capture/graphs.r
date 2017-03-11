setwd("~/Documents/Projects/UAF/sap-flux/data/capture/")
d = data.frame(read.csv("2017-03-10 15-05-11 CC_junction_taped_to_amp.csv"))
g = data.frame(read.csv("2017-03-10 15-05-11 CC_junction_superglue_to_amp.csv"))
names(d) <- c("tc1", "tc2", "tc3", "int1", "int2", "int3")
names(g) <- c("tc1", "tc2", "tc3", "int1", "int2", "int3")
attach(g)
par(mfrow=c(1,1))
plot(tc1, ylim=c(16, 19))
plot(tc1, col="red", ylim=c(17, 23), type = "l", lwd=2)
lines(int1, col="green", lwd=4)

tc1_plus = tc1+4.5
tc2_plus = tc2+4.5
tc3_plus = tc3+4.5

plot(tc1_plus)
points(int1)
plot(tc2_plus)
points(int2)
plot(tc3_plus)
points(int3)

par(mfrow=c(1,1))
plot(tc1_plus, type="p", lwd=2, ylim=c(23, 25), col="purple")
points(int1, col="green", lwd=2)

plot(int2, type="l", lwd=2, ylim=c(23, 25), col="purple")
lines(tc2_plus, col="green", lwd=4)
plot(int3, type="l", lwd=2, ylim=c(23, 25), col="purple")
lines(tc3_plus, col="green", lwd=4)


plot(tc1, col="purple")
detach(g)
