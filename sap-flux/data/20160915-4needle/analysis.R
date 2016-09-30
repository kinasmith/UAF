setwd("~/Documents/Projects/UAF/sap-flux/data/20160915-4needle/")
##--Load In Data--##
data = data.frame(read.csv("105_2.csv"))
#check to make sure this is correct
names(data) <- c("sensor_num", "time", "tc_1", "tc_2", "tc_3", "tc_h") 
data$time_real = as.POSIXct(data$time, origin="1970-01-01") #Convert time to POSIX
for (i in 1:length(data$time)) {
  data$log_interval[i] = data$time[i+1] - data$time[i]
}
dataTrim = data[5:length(data$time),]
plot(dataTrim$time)
plot(dataTrim$tc_h~dataTrim$time_real)
plot(dataTrim$tc_1~dataTrim$time_real)
plot(dataTrim$tc_2~dataTrim$time_real)
plot(dataTrim$tc_3~dataTrim$time_real)
