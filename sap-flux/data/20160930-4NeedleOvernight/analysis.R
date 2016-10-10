setwd("~/Documents/Projects/UAF/sap-flux/data/20160930-4NeedleOvernight/")
##--Load In Data--##
## 343 mA current
## 10.2 ohms
## >50% moisture content
data = data.frame(read.csv("Data05.csv"))
#check to make sure this is correct
names(data) <- c("time", "tc_1", "tc_2", "tc_3", "h_status") 
data$time_real = as.POSIXct(data$time, origin="1970-01-01") #Convert time to POSIX
for (i in 1:length(data$time)) {
  data$log_interval[i] = data$time[i+1] - data$time[i]
}
par(mfrow=c(1,1))
plot(data$tc_1)
start = (30*60)*30
end = start+800
subData = data[start:end,]
min = min(subData$tc_1) - 0.1
max = max(subData$tc_1) + 0.1
par(mfrow=c(1,1))
plot(subData$tc_1)

par(mfrow=c(2,2))
plot(subData$tc_1~subData$time_real, type='p', pch=1, cex=.5, ylim=c(min, max), main="downstream")
plot(subData$tc_2~subData$time_real, type='p', pch=1, cex=.5, ylim=c(min, max), main="side")
plot(subData$tc_3~subData$time_real, type='p', pch=1, cex=.5, ylim=c(min, max), main="upstream")
plot(subData$h_status~subData$time_real, type='p', pch=1, cex=.5, ylim=c(0, 1), main="heater")

