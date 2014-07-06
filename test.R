library(RaggedArray)
test.obj = new(RaggedArray, 3, 10, 5)
test.list <- list(1:5, 1:6, 1:7)
test.obj$append(test.list)
test.obj$append(test.list)
str(test.obj)

save.obj <- test.obj$serialize()
new.obj <- new(RaggedArray, save.obj)
