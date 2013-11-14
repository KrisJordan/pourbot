# Time to Impact

data <- read.table("./input.csv", header=TRUE, sep=",")

for (i in 10:1) {
    focus   <- subset(data, experiment==i)
    focus   <- focus[focus$v > 515,]
    if(nrow(focus) > 0) {
        t_line  <- focus[focus$t == min(focus$t),]
        print(t_line)
    }
}
