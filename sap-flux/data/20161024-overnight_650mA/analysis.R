setwd("~/Documents/Projects/UAF/sap-flux/data/20161024-overnight_650mA//")
##--Load In Data--##
## 650 mA current
## 7.5 volts feeding
## 10.2 ohms
## 11.53 ohms including switching circuit
## tip120 forward resistance == 1.3 ohms

data = data.frame(read.csv("Data22.csv"))
#check to make sure this is correct
names(data) <- c("time", "tc_1", "tc_2", "tc_3", "h_status") 
##tc_1 is upStream
##tc_2 is sideStream
##tc_3 is downStream
data$time_real = as.POSIXct(data$time, origin="1970-01-01") #Convert time to POSIX
for (i in 1:length(data$time)) {
  data$log_interval[i] = data$time[i+1] - data$time[i]
}
par(mfrow=c(1,1))
plot(data$tc_1)
start = (30*60)*0
end = start+400
subData = data[start:end,]
min = min(subData$tc_1) - 0.1
#max = max(subData$tc_1) + 0.1
max = min + 1.2
par(mfrow=c(1,1))
plot(subData$tc_1)

par(mfrow=c(2,2))
plot(subData$tc_1~subData$time_real, type='p', pch=1, cex=.5, ylim=c(min, max), main="downstream")
plot(subData$tc_2~subData$time_real, type='p', pch=1, cex=.5, ylim=c(min, max), main="side")
plot(subData$tc_3~subData$time_real, type='p', pch=1, cex=.5, ylim=c(min, max), main="upstream")
plot(subData$h_status~subData$time_real, type='p', pch=1, cex=.5, ylim=c(0, 1), main="heater")

max(subData$tc_1)
max(subData$tc_3)
max(subData$tc_2)
