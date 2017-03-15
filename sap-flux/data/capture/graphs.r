setwd("~/Documents/Projects/UAF/sap-flux/data/capture/")
d = data.frame(read.csv("2017-03-10 15-05-11 CC_junction_taped_to_amp.csv"))
g = data.frame(read.csv("2017-03-10 15-05-11 CC_junction_superglue_to_amp.csv"))
r = data.frame(read.csv("2017-03-10 15-05-11 CC_junction_superglue_to_amp_routed.csv"))
names(d) <- c("tc1", "tc2", "tc3", "int1", "int2", "int3")
names(g) <- c("tc1", "tc2", "tc3", "int1", "int2", "int3")
names(r) <- c("tc1", "tc2", "tc3", "int1", "int2", "int3")
attach(r)
par(mfrow=c(1,1))
##Temperature readings graphed together
plot(tc1, col="yellow", ylim=c(20, 22), type = "l", lwd=2)
lines(tc2, col="purple", lwd=2)
lines(tc3, col="orange", lwd=2)

## In proximity heater, the TC Numbers are: 2 is closest, 1 is furthest, 3 is in the middle
plot(int1, col="yellow", ylim=c(22, 27), type = "l", lwd=2)
lines(int2, col="purple", lwd=2)
lines(int3, col="orange", lwd=2)


tc1_plus = tc1+4.5
tc2_plus = tc2+4.5
tc3_plus = tc3+4.5

par(
  mfrow=c(1,1),
  cex=0.5,
  lwd=2,
  pch=8
  )
plot(tc1_plus, type="p", ylim=c(22.5, 25), col="green")
points(int1, col="purple")
plot(int2, type="p", lwd=2, ylim=c(22, 27), col="purple")
points(tc2_plus, col="green", lwd=4)
plot(int3, type="p", lwd=2, ylim=c(22, 27), col="purple")
points(tc3_plus, col="green", lwd=4)


plot(tc1, col="orange")
plot(tc2, col="orange")
plot(tc3, col="orange")
detach(g)
