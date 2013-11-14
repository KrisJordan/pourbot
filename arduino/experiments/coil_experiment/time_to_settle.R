# Time to Impact

data <- read.table("./input.csv", header=TRUE, sep=",")

for (i in 10:1) {
    focus   <- subset(data, experiment==i)
    if(nrow(focus) > 0) {
        v_line  <- focus[focus$v == max(focus$v),]
        print(v_line)
    }
}
