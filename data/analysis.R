setwd("~/Documents/Projects/UAF//uaf_dendrometer/data/")
getwd()
data = data.frame(read.csv("birch//2.CSV"))
names(data) <- c("sensor_num", "temp", "voltage", "time", "num_attempts", "sensor_val")

attach(data)
detach(data)
data$time_real = as.POSIXct(data$time, origin="1970-01-01")

sens_length <- 50
data$diam_mm <- (sensor_val/32767)*sens_length
plot(diam_mm~time_real, data = data)

min(data$sensor_val)

diff = matrix(NA, nrow = (length(time)-1))
aa = (length(time)-1)
  for(i in 1:aa){
    diff[i] <- time[i+1] - time[i]
  }
plot(diff)
plot(sensor_val)
plot(voltage)
plot(num_attempts)
plot(temp)
plot(diam_mm)
help(par)
par(pch=22, col="red") # plotting symbol and color 
par(mfrow=c(2,1)) # all plots on one page 
plot(temp~time_real, data=data, main="Temp")
plot(diam_mm~time_real, data = data, main="Diameter")
