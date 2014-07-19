library(RaggedArray)
test.obj = new(RaggedArray, 3, 10, 5)
test.list <- list(1:5, 1:6, 1:7)
test.obj$append(test.list)
test.obj$append(test.list)
str(test.obj)
## modify in-place
test.obj$sapply(function(x) {2*x})
## error
# test.obj$sapply(function(x) {c(mean(x), sd(x))})
test.obj$sapply_alloc(function(x) {c(mean(x), sd(x))})
#test.obj$sapply_alloc(function(x) {rnorm(x)})
test.obj$sapply_alloc(function(x) {sample(x, 22, replace=T)})
#test.obj$sapply_alloc(function(x) {sample(x, 22, replace=T)})
sourceCpp('userFun.cpp')
test.obj$sapply_cpp(myfun())
test.obj$append(test.list)
test.obj$sapply_cpp_alloc(myfun1())
test.obj$append(test.list)
## error
#test.obj$sapply_cpp(myfun1())

save.obj <- test.obj$serialize()
new.obj <- new(RaggedArray, save.obj)
