data <- read.table("./input.csv", header=TRUE, sep=",")

data$t <- (data$t / 1000) - (data$experiment*2)
#data$v <- (5 / 1024 * data$v)
#data$v <- (0.3611) + (49.3436 * data$v) + -5.5215*(data$v^2)
#data$v <- data$v * 9/5 + 32
colors <- c(
            'violet',
            'blueviolet',
            'blue',
            'seagreen',
            'green',
            'greenyellow',
            'yellow',
            'orange',
            'orangered',
            'red',
            'darkred'
           )

has_not_plotted <- TRUE
for (i in 10:1) {
    focus   <- subset(data, experiment==i)
    max     <- focus[focus$v == max(focus$v), ] 
    #focus   <- subset(focus, t < max$t)

    if(has_not_plotted) {
        plot(focus$t, focus$v, col=colors[i])
        has_not_plotted <- FALSE
    } else {
        points(focus$t, focus$v, col=colors[i])
    }

    #max     <- focus[focus$v == max(focus$v), ] 
    #cool    <- subset(focus, t > max$t)
    #fit     <- lm(cool$v ~ cool$t + I(cool$t^2))
    #xx      <- seq(0,max(cool$t),len=100)
    #yy      <- fit$coef %*% rbind(1,xx,xx^2)
    #lines(xx,yy,lwd=2,col=colors[i])

    focus   <- subset(focus, t >= 0)
    fit     <- lm(focus$v ~ focus$t + I(focus$t^2))
    fit     <- lm(focus$v ~ focus$t)
    xx      <- seq(0,max(focus$t),len=100)
    yy      <- fit$coef %*% rbind(1,xx)#,xx^2)
    lines(xx,yy,lwd=2,col=colors[i])
}
