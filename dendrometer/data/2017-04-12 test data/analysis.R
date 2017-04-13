setwd("~/Documents/Projects/UAF/dendrometer/data/2017-04-12 test data//")
##--Load In Data--##
#1,6,12,5,7,3,8
data = data.frame(read.csv("100_8.csv"))
#Sensor Address, Unix Time Stamp, ADC Value, Temperature, Battery Voltage, Excitation Voltage, Number of send attamps
names(data) <- c("address", "time", "val", "temp", "batt_v", "ref_v", "count") 
data$time_real = as.POSIXct(data$time, origin="1970-01-01") #Convert time to POSIX
par(mfrow=c(1,1))

plot(data$val)
plot(data$temp)
plot(data$batt_v)
plot(data$ref_v)
plot(data$count)

plot(data$val~data$temp)

names(data) <- c("address", "time", "adc", "temp", "batt_v","count") #name data
data$time_real = as.POSIXct(data$time, origin="1970-01-01") #Convert the UNIX time stamp to POSIX time format

Vref = 2.048 #ADC Voltage Reference
N = 16 #ADC Set to 16 bit resolution
LSB = ((2*Vref)/2^N) #Least Significant Bit (in volts)
PGA = 1 #Programmable Gain Amplifier (Set to 1)
L = 50 #Length of Linear Sensor
data$Iv = data$adc*(LSB/PGA) #Caluclate Input Voltage (Iv)
data$position = ((data$Iv/Vref)*L) #Calculate Position of Slider

initPosition = data$position[1]
data$dia_change = (data$position - initPosition)/pi

plot(data$position)
plot(data$Iv)
plot(data$dia_change)

par(
  cex = ".7",
  cex.sub="0.5",
  lwd = "2"
  )
plot(
  change~time_real, 
  data = dataTrim, 
  main="Change in Diameter",
  sub="dendro_100.4",
  xlab="Time",
  ylab="Millimeters",
  type = "l"
  )

help(plot)
help(par)

plot(data$time_real[1:100],)
plot(n_at~time, data = data, main="n_at")
plot(val~time_real, data = dataTrim, main="val")
plot(Ev~time_real, data = dataTrim, main = "exciitation Voltage")

plot(temp~time_real, data=data, main="Temp")
plot(val~time_real, data = data, main="val")
plot(batt_v~time_real, data = data, main = "battery")

