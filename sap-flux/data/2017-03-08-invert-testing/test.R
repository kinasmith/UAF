setwd("~/Documents/Projects/UAF/sap-flux/data/2017-03-08-invert-testing/")
d = data.frame(read.csv("test_10ohm_air_noRes_inWater_typeT_onChip_Route_probes.csv"))
names(d) <- c("tc1", "tc2", "tc3", "int1", "int2", "int3")
par(mfrow=c(1,1))
plot(d$tc1, col="red", ylim=c(16, 27))
points(d$int1, col="green")
par(mfrow=c(3,1))
plot(d$int1)
plot(d$int2)
plot(d$int3)

par(mfrow=c(2,1))
plot(d$int1)
plot(d$tc1)
plot(d$int2)
plot(d$tc2)
plot(d$int3)
plot(d$tc3)

d$int2_mod = d$int2-5
par(mfrow=c(1,1))
plot(d$tc2, col="red", ylim=c(20, 23))
points(d$int2_mod, col="blue")


trim = d[280:380,]

pdf("RedFishJumping.pdf", useDingbats = FALSE, paper = "USr")
#d[1:4,]
par(ann=FALSE,
    bg="pink",
    bty="u",
    col.axis = "yellow",
    crt=23,
    family="mono",
    las=3)
plot(trim$tc2, col="red", type="l", ylim=c(20, 23), xlim=c(4,75), lwd=30, lty=33)
lines(trim$int2_mod, col="blue", lwd=70)
dev.off()

plot(trim$tc2, col="red", type="p", ylim=c(20, 23), xlim=c(0,75), cex=4, pch=107)
points(trim$int2_mod, col="blue")


