datafram_data$time = as.POSIXct(datafram_data$X1457737135, origin="1970-01-01")
datafram_data$time

setwd("~/Documents/Projects/UAF//uaf_dendrometer/data/test/")
getwd()
data = data.frame(read.csv("2016.03.17/14.CSV"))
names(data) <- c("sensor", "temp", "voltage", "time")
attach(data)
diff = matrix(NA, nrow = (length(time)-1))
aa = (length(time)-1)
  for(i in 1:aa){
    diff[i] <- time[i+1] - time[i]
  }

plot(diff)
