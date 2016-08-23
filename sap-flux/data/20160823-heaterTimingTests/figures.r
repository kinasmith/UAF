#regex: (([^,]+),[0-9]{1,2}.[0-9]{2})(,)
#replace with $1\n

setwd("~/Documents/Projects/UA_FAIRBANKS/sap_flux/data/20160823-heaterTimingTests//data")
data = data.frame(read.csv("Data40.csv"))
names(data) <- c("time","temp") 
data$time = (data[,1]-data[1,1])/1000
plot(data$temp~data$time)

files = list.files(path = "~/Documents/Projects/UA_FAIRBANKS/sap_flux/data/20160823-heaterTimingTests//data")



