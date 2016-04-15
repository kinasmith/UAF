calibration = data.frame(read.csv("1.CSV"))
names(calibration) <- c("sensor_num", "temp", "voltage", "time", "num_attempts", "sensor_val")
attach(calibration)
detach(calibration)
detach(data)
plot(temp)
plot(sensor_val)
plot(sensor_val, temp)
par(mfrow=c(1,1)) # all plots on one page 
