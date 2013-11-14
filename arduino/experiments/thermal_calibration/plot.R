# The purpose of this script is to figure out the regression line for
# mapping voltage to temperature from the kettle.
#
# This file reads in a CSV of voltage (x) and temperature in c (y) values.
# It produces a plot with a second order polynomial regression line. In 
# the text output the results of the regression line are printed.
data <- read.table("./thermal.csv", header=TRUE, sep=",")
plot(data$x, data$y)
fit <- lm(data$y ~ data$x+I(data$x^2))
summary(fit)
xx <- seq(min(data$x),max(data$x),len=100)
yy <- fit$coef %*% rbind(1,xx,xx^2)
lines(xx,yy,lwd=2,col=3)
