Vref = 2.048 #ADC Voltage Reference
N = 16 #ADC Set to 16 bit resolution
LSB = ((2*Vref)/2^N) #Least Significant Bit (in volts)
PGA = 1 #Programmable Gain Amplifier (Set to 1)
L = 50 #Length of Linear Sensor

setwd("~/Documents/Projects/UAF/dendrometer/data/2017-02-03//")
##--Load In Data--##
data = data.frame(read.csv("100_3.csv"))
#Sensor Address, Unix Time Stamp, ADC Value, Temperature, Battery Voltage, Excitation Voltage, Number of send attamps
names(data) <- c("sensor_num", "time", "val", "temp", "batt_v", "Ev", "n_at") 
data$time_real = as.POSIXct(data$time, origin="1970-01-01") #Convert time to POSIX

dateTime1 <- strptime("0:00 2016/1/01", format="%H:%M %Y/%m/%d", tz="America/Anchorage")
dateTime2 <- strptime("0:00 2017/12/01", format="%H:%M %Y/%m/%d", tz="America/Anchorage")
subSample <- data[data$time_real < dateTime2 & data$time_real > dateTime1, ]

subSample$Iv = subSample$val*(LSB/PGA) #Caluclate Input Voltage (Iv)
subSample$position = ((subSample$Iv/subSample$Ev)*L) #Calculate Position of Slider

subSample$change = subSample[,10] - subSample[1,10] #calculate the change in diameter of the tree
dataTrim = subSample[0:(nrow(subSample)-1),0:11]
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


plot(subSample$Ev~subSample$time_real, type = "l", ylim=c(1, 2.2))
plot(subSample$temp~subSample$time_real, type = "l", ylim=c(-40, 0))
plot(subSample$batt_v~subSample$time_real, type = "l")
plot(subSample$time~subSample$time_real, type = "l")




plot(data$val)
plot(data$time_real[1:100],)
plot(n_at~time, data = data, main="n_at")
plot(val~time_real, data = dataTrim, main="val")
plot(Ev~time_real, data = dataTrim, main = "exciitation Voltage")

plot(temp~time_real, data=data, main="Temp")
plot(val~time_real, data = data, main="val")
plot(batt_v~time_real, data = data, main = "battery")


ggplot(BOD, aes(x=data$time, y=data$position)) + geom_line()

install.packages("ggplot2")
library("ggplot2", lib.loc="/Library/Frameworks/R.framework/Versions/3.1/Resources/library")
