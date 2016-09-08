setwd("~/Documents/Projects/UAF/sap-flux/data/20160908-dualProbeTest/")
##--Load In Data--##
data = data.frame(read.csv("105_1.csv"))
#check to make sure this is correct
names(data) <- c("sensor_num", "time", "ref", "ref_internal", "heater", "heater_internal", "n_at") 
data$time_real = as.POSIXct(data$time, origin="1970-01-01") #Convert time to POSIX
for (i in 1:length(data$time)) {
  data$log_interval[i] = data$time[i+1] - data$time[i]
}
plot(data$log_interval)
plot(data$time)
plot(data$heater)
plot(data$ref)
plot(data$heater_internal)
plot(data$ref_internal)
plot(data$n_at)

data$time_real[length(data$time_real)]
data$time[length(data$time_real)]
