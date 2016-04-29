setwd("~/Documents/Projects/UAF//uaf_dendrometer/data/")
data1 = data.frame(read.csv("birch/untitled folder/1_mod.CSV"))
data2 = data.frame(read.csv("birch/untitled folder/2.CSV"))
names(data1) <- c("sensor_num", "time", "sensor_val", "temp", "evolt", "bvolt", "attempts")
names(data2) <- c("sensor_num", "time", "sensor_val", "temp", "evolt", "bvolt", "attempts")
data1$time = as.POSIXct(data1$time, origin="1970-01-01")
data2$time = as.POSIXct(data2$time, origin="1970-01-01")

par(mfrow=c(2,2)) # all plots on one page 
par(mfrow=c(1,1)) # all plots on one page 
plot(data1$sensor_val~data1$time)
plot(data1$time)
plot(data2$sensor_val~data2$time)

plot(data1$temp~data1$time)
plot(data2$temp~data2$time)

min(data2$sensor_val)

data <- data1
t1 <- data$temp[1] #store initial temp value
s1 <- data$sensor_val[1]  #store initial sensor value
data$sensor_val_c <- NULL
data$mm <- NULL

for (i in 1:length(data$temp)) {
  data$sensor_val_c[i] <- ((data$sensor_val[i] - s1) - 4.34 * (data$temp[i] - t1))
  data$mm[i] <- ((data$sensor_val[i] + data$sensor_val_c[i]) / 655.34) #should this be + or -?
}
  

par(mfrow=c(2,2)) # all plots on one page 
plot( data$temp ~ data$time )
plot( data$sensor_val ~ data$time)  
plot( data$sensor_val_c ~ data$time)
plot( data$mm ~ data$time)

names(calibration) <- c("sensor_num", "temp", "voltage", "time", "num_attempts", "sensor_val")
attach(calibration)
detach(calibration)
detach(data)
plot(temp)
plot(sensor_val)
plot(sensor_val, temp)
par(mfrow=c(1,1)) # all plots on one page 

plot(sensor_val ~ temp)
fit <- lm(sensor_val ~ temp, data = calibration) #create a linear line corresponding to data
summary(fit) #get intercept, slope, other model parameters

abline(fit) #add the line from the fit above to your plot

