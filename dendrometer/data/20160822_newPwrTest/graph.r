setwd("~/Documents/Projects/UA_FAIRBANKS/dendrometer/data/20160822_newPwrTest/")
data = data.frame(read.csv("102_1.csv"))
names(data) <- c("sensor_num", "time", "val", "temp", "batt_v", "Ev", "n_at") 
data$time_real = as.POSIXct(data$time, origin="1970-01-01") #Convert time to POSIX

Vref = 2.048 #ADC Voltage Reference
N = 16 #ADC Set to 16 bit resolution
LSB = ((2*Vref)/2^N) #Least Significant Bit (in volts)
PGA = 1 #Programmable Gain Amplifier (Set to 1)
L = 50 #Length of Linear Sensor
data$Iv = data$val*(LSB/PGA) #Caluclate Input Voltage (Iv)
data$position = ((data$Iv/data$Ev)*L) #Calculate Position of Slider

plot(data$batt_v~data$time_real)
plot(data$Ev_volts~data$time_real)
data$Ev_volts = data$Ev * (3.3 / 1023)
