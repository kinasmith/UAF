setwd("~/Documents/Projects/UAF/sap-flux/code/fourNeedle_test_display/")
data = data.frame(read.csv("data_2016923-1257.txt")) #200mA current input
data = data.frame(read.csv("data_2016923-138.txt")) #300mA current input
data = data.frame(read.csv("data_2016928-1516.txt")) #300mA current input
names(data) <- c("sec", "tc_1", "tc_2", "tc_3", "h_state") 
par(mfrow= c(1, 1))
plot(data$tc_1)
plot(data$h_state)
start = 1798 -30
end = start+200
subData = data[start:end,]
plot(subData$tc_1~subData$sec)
plot(subData$h_state)
subData$sec = subData$sec - start
min = min(subData$tc_1) - 0.1
max = max(subData$tc_1) + 0.1
par(mfrow=c(2,2))
plot(subData$tc_1~subData$sec, type='p', pch=1, cex=.5, ylim=c(min, max), main="downstream")
plot(subData$tc_2~subData$sec, type='p', pch=1, cex=.5, ylim=c(min, max), main="side")
plot(subData$tc_3~subData$sec, type='p', pch=1, cex=.5, ylim=c(min, max), main="upstream")
plot(subData$h_state~subData$sec, type='p', pch=1, cex=.5, ylim=c(0, 1), main="heater")
# plot(subData$h_state~subData$sec, type='p', pch=1, cex=.5, ylim=c(0, 1.2), main="heater")
