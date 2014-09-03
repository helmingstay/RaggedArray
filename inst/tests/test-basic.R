## nvec, allocLen, growBy
test.obj = new(RaggedArray, 3, 10, 5)
test.list <- list(1:5, 1:6, 1:7)
test.obj$append(test.list)
test.obj$append(test.list)

expect_equal( test.obj$nvec, 3)
expect_equal( test.obj$growBy, 5)
expect_equal( test.obj$lengths, c( 10, 12, 14))
expect_output( str(test.obj), "Rcpp_RaggedArray")

## modify in-place
test.obj$sapply(function(x) {2*x})
expect_equal( test.obj$data[10,], c( 10, 8, 6))

## test incorrect realloc: error
expect_error( 
    test.obj$sapply(function(x) {c(mean(x), sd(x))}), 
    info="sapply not valid with function that changes x dimensions"
)
expect_error(
    test.obj$sapplyC(userFunSum),
    info="sapplyC not valid with function that changes x dimensions"
)

## function shrinks obj
test.obj$sapplyAlloc(function(x) {c(mean(x), sd(x))})
expect_equal( test.obj$data[1,], 6:8)
expect_true( all(test.obj$data[-1:-2,] ==0))
expect_equal( test.obj$data[3,], c(0,0,0))
## function grows obj
test.obj$sapplyAlloc(function(x) {sample(x, 22, replace=T)})
expect_equal( test.obj$lengths, c(22, 22, 22))
## pass C++ function exported to vanilla R function
test.obj$sapply(userFunTimes10R)

## user-defined C++ functions
test.obj$sapplyC(userFunTimes10)
expect_true( 600 %in% test.obj$data[,1])
expect_true( 700 %in% test.obj$data[,2])
expect_true( 800 %in% test.obj$data[,3])
test.obj$append(test.list)
test.obj$sapplyAllocC(userFunSum)
expect_equal( test.obj$lengths, c(1,1,1))
expect_true( all(test.obj$data[1,] >100))
expect_true( all(test.obj$data[2,] ==0))
expect_true( all(test.obj$data[-1,] ==0))

test.obj$append(test.list)
test.obj$sapplyAllocC(userFunSample)
## segfault
#test.obj$sapplyAllocC(userFunSampleBig)

test.obj1 = new(RaggedArray, 3, 10, 5)
test.obj1$append(test.list)
## mapply
test.obj1$mapply(function(x, y) sample(x, y), test.obj1$lengths)
expect_true( all(sapply(test.list, sum) == apply(test.obj1$data, 2, sum)))

test.obj1$mapplyAlloc(function(x, y) sample(x, y, replace=T), 1:test.obj$nvec)
expect_true( all(test.obj1$lengths == 1:3))
test.obj1$append(test.list)
test.obj1$mapplyAllocC(userFunSampleM, 2*(test.obj$nvec:1))
expect_true( all(test.obj1$lengths == 2*(3:1)))
expect_error( 
    test.obj1$mapplyAllocC(userFunSampleM, 10),
    info="mapply arg must have length == nvec"
)
.n <- 1e5
.lengths <- .n:(.n+2)
test.obj1$mapplyAllocC(userFunSampleM, .lengths)
expect_true( all(test.obj1$lengths == .lengths))

## segfault??
#.n <- 1e7
#.lengths <- .n:(.n+2)
#test.obj1$mapplyAllocC(userFunSampleM, .lengths)
#expect_true( all(test.obj1$lengths == .lengths))

## save object to R List
save.obj <- test.obj$serialize()
new.obj <- new(RaggedArray, save.obj)
expect_equal( new.obj, test.obj)
save.obj$growBy <- NULL
expect_error( 
    new(RaggedArray, save.obj),
    info="Trying to construct from incomplete list object"
)
