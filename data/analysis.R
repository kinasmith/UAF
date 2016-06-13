

setwd("~/Documents/Projects/UAF//uaf_dendrometer/data/2016.05.25_timing test/")
setwd("~/Desktop/tmp/data/untitled folder/")
##--Load In Data--##
data1 = data.frame(read.csv("101_1.CSV"))
names(data1) <- c("sensor_num", "time", "val", "temp", "batt_v", "ex_v", "n_at")
data1$time_real = as.POSIXct(data1$time, origin="1970-01-01")
##--caculate and plot the time interval between sensor readings--##
diff1 = matrix(NA, nrow = (length(data1$time)-1))
aa = (length(data1$time)-1)
for(i in 1:aa){
  diff2[i] <- data1$time[i+1] - data1$time[i]
}
plot(diff1)
plot(data1$n_at)
##--Calculate change in diameter--##
sens_length <- 50
data1$diam_mm <- (data1$val/32767)*sens_length
plot(temp~time_real, data=data1, main="Temp")
plot(diam_mm~time_real, data = data1, main="Diameter")
plot(batt_v~time_real, data = data1, main = "battery")

data2 = data.frame(read.csv("101_3.CSV"))
names(data2) <- c("sensor_num", "time", "val", "temp", "batt_v", "ex_v", "n_at")
data2$time_real = as.POSIXct(data2$time, origin="1970-01-01")
##--caculate and plot the time interval between sensor readings--##
diff2 = matrix(NA, nrow = (length(data2$time)-1))
aa = (length(data2$time)-1)
for(i in 1:aa){
  diff2[i] <- data2$time[i+1] - data2$time[i]
}
plot(diff2)
plot(data2$n_at)
##--Calculate change in diameter--##
sens_length <- 50
data2$diam_mm <- (data2$val/32767)*sens_length
plot(temp~time_real, data=data2, main="Temp")
plot(diam_mm~time_real, data = data2, main="Diameter")
plot(batt_v~time_real, data = data2, main = "battery")

par(mfrow=c(2,1)) # all plots on one page 

