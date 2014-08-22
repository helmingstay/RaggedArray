library(RaggedArray)
## compile user function
sourceCpp('userFun.cpp')
## nvec, allocLen, growBy
test.obj = new(RaggedArray, 3, 10, 5)
test.list <- list(1:5, 1:6, 1:7)
test.obj$append(test.list)
test.obj$append(test.list)

expect_equal( test.obj$lengths, c( 10, 12, 14))
expect_output( str(test.obj), "Rcpp_RaggedArray")

## modify in-place
test.obj$sapply(function(x) {2*x})

expect_equal( test.obj$data[10,], c( 10, 8, 6))
## error
expect_error( 
    test.obj$sapply(function(x) {c(mean(x), sd(x))}), 
    info="sapply only valid with function that preserve data dimensions"
)
test.obj$sapply_alloc(function(x) {c(mean(x), sd(x))})
expect_equal( test.obj$data[1,], 6:8)
expect_equal( test.obj$data[3,], c(0,0,0))
#test.obj$sapply_alloc(function(x) {rnorm(x)})
test.obj$sapply_alloc(function(x) {sample(x, 22, replace=T)})
#test.obj$sapply_alloc(function(x) {sample(x, 22, replace=T)})
expect_equal( test.obj$lengths, c(22, 22, 22))

test.obj$sapply_cpp(myfun_times10())
#browser()
expect_true( 60 %in% test.obj$data[,1])
expect_true( 70 %in% test.obj$data[,2])
expect_true( 80 %in% test.obj$data[,3])
test.obj$append(test.list)
test.obj$sapply_cpp_alloc(myfun1_sum())
expect_equal( test.obj$lengths, c(1,1,1))
expect_true( all(test.obj$data[1,] >100))
expect_true( all(test.obj$data[2,] ==0))
#test.obj$sapply_cpp(myfun1())

save.obj <- test.obj$serialize()
new.obj <- new(RaggedArray, save.obj)
expect_equal( new.obj, test.obj)
