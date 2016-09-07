setwd("~/Documents/Projects/UAF/dendrometer/data/20160902/")
##--Load In Data--##
data = data.frame(read.csv("100_12.csv"))
#Sensor Address, Unix Time Stamp, ADC Value, Temperature, Battery Voltage, Excitation Voltage, Number of send attamps
names(data) <- c("sensor_num", "time", "val", "temp", "batt_v", "Ev", "n_at") 
data$time_real = as.POSIXct(data$time, origin="1970-01-01") #Convert time to POSIX
data$time_real[length(data$time_real)]
plot(data$batt_v)


for (i in 1:length(data$temp)) {
  data$log_interval[i] = data$time[i+1] - data$time[i]
}
plot(data$log_interval)
plot(data$Ev)
plot(data$val)
plot(data$time)
plot(data$temp)
plot(data$n_at)
plot(data$batt_v)

data$time_real

data$time_real[length(data$time_real)]
data$time[length(data$time_real)]
