#°C  x  9/5 + 32 = °F
data1$temp = data1$temp * (9/5)+32
data9$temp = data9$temp * (9/5)+32
data7$temp = data7$temp * (9/5)+32
setwd("~/Documents/Projects/PERSONAL/Home Automation/data/2017-01-24/")
data1 = data.frame(read.csv("101_1.csv"))
data9 = data.frame(read.csv("101_9.csv"))
data7 = data.frame(read.csv("101_7.csv"))
#check to make sure this is correct
names(data1) <- c("id", "time", "temp", "humidity", "voltage", "cnt") 
names(data9) <- c("id", "time", "temp", "humidity", "voltage", "cnt") 
names(data7) <- c("id", "time", "temp", "humidity", "voltage", "cnt") 
data1$time_real = as.POSIXct(data1$time, origin="1970-01-01", "America/Anchorage") #Convert time to POSIX
data9$time_real = as.POSIXct(data9$time, origin="1970-01-01", "America/Anchorage") #Convert time to POSIX
data7$time_real = as.POSIXct(data7$time, origin="1970-01-01", "America/Anchorage") #Convert time to POSIX


dateTime1 <- strptime("0:00 2016/12/20", format="%H:%M %Y/%m/%d", tz="America/Anchorage")
dateTime2 <- strptime("0:00 2017/01/25", format="%H:%M %Y/%m/%d", tz="America/Anchorage")
upstairs <- data1[data1$time_real < dateTime2 & data1$time_real > dateTime1, ]
downstairs <- data9[data9$time_real < dateTime2 & data9$time_real > dateTime1, ]
outside <- data7[data7$time_real < dateTime2 & data7$time_real > dateTime1, ]

min = min(outside$temp) - 1
max = max(downstairs$temp) + 1

plot(upstairs$time_real,upstairs$temp,col="red", type='p', pch=1, cex=.5, ylim=c(min, max))
lines(downstairs$time_real,downstairs$temp,col="green", type='p', pch=1, cex=.5)
lines(outside$time_real,outside$temp,col="blue", type='p', pch=1, cex=.5)
abline(h = 18)
abline(h=15)
#plot(subData1$time_real,subData1$humidity,col="red", type='p', pch=1, cex=.5, ylim=c(min, max))
plot(upstairs$time_real,upstairs$humidity,col="red", type='p', pch=1, cex=.5, ylim=c(0, 80))
lines(downstairs$time_real,downstairs$humidity,col="green", type='p', pch=1, cex=.5)
lines(outside$time_real,outside$humidity,col="blue", type='p', pch=1, cex=.5)

plot(downstairs$humidity, downstairs$temp)
plot(outside$humidity, outside$temp)
#min = 8
#max = 20

plot(data1$time_real, data1$voltage)
plot(data7$time_real, data7$voltage)
plot(data9$time_real, data9$voltage)
min = min(subData$temp) - 0.1
max = max(subData$temp) + 0.1
par(mfrow=c(1,1))
plot(subData$temp~subData$time_real)

attach(data7)
for (i in 1:length(time)) {
  data7$log_interval[i] = time[i+1] - time[i]
}
plot(log_interval)
detach(data7)
par(mfrow=c(1,1))
plot(data$temp~data$time_real)

plot(subData7$temp)
plot(subData7$humidity)
plot(data9$cnt)