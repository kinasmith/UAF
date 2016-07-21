setwd("~/Documents/Projects/UA_FAIRBANKS/uaf_dendrometer/data/20160708/")
##--Load In Data--##
data = data.frame(read.csv("100_4.csv"))
#Sensor Address, Unix Time Stamp, ADC Value, Temperature, Battery Voltage, Excitation Voltage, Number of send attamps
names(data) <- c("sensor_num", "time", "val", "temp", "batt_v", "Ev", "n_at") 
data$time_real = as.POSIXct(data$time, origin="1970-01-01") #Convert time to POSIX
Vref = 2.048 #ADC Voltage Reference
N = 16 #ADC Set to 16 bit resolution
LSB = ((2*Vref)/2^N) #Least Significant Bit (in volts)
PGA = 1 #Programmable Gain Amplifier (Set to 1)
L = 50 #Length of Linear Sensor
data$Iv = data$val*(LSB/PGA) #Caluclate Input Voltage (Iv)
data$position = ((data$Iv/data$Ev)*L) #Calculate Position of Slider

data$change = data[,10] - data[1,10] #calculate the change in diameter of the tree
dataTrim = data[0:(nrow(data)-1),0:11]
par(
  cex = "1.1",
  cex.sub="0.8",
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

plot(val~time_real, data = dataTrim, main="val")
plot(Ev~time_real, data = dataTrim, main = "exciitation Voltage")

plot(temp~time_real, data=data, main="Temp")
plot(val~time_real, data = data, main="val")
plot(batt_v~time_real, data = data, main = "battery")


ggplot(BOD, aes(x=data$time, y=data$position)) + geom_line()

install.packages("ggplot2")
library("ggplot2", lib.loc="/Library/Frameworks/R.framework/Versions/3.1/Resources/library")
