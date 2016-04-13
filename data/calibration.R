calibration = data.frame(read.csv("temp-cal.CSV"))
names(calibration) <- c("sensor_num", "temp", "voltage", "time", "num_attempts", "sensor_val")
attach(calibration)
detach(calibration)

plot(temp)
plot(sensor_val)
plot(sensor_val, temp)
