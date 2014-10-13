## time sapply w/and without realloc, XPtr
library(rbenchmark)
library(Rcpp)
sourceCpp(system.file('examples', 'cpp', 'userSapplyFun.cpp', package='RaggedArray'))
sourceCpp(system.file('examples', 'cpp', 'userVanillaFun.cpp', package='RaggedArray'))

## helper function
## prepare a standard RaggedArray for testing
mk.test.arr <- function(.ncol=1e3, .veclen=2e3, .growby=1e3, seed=1){
    set.seed(seed)
    .dat <- lapply(1:.ncol, function(x) rnorm(.veclen))
    ret <- RaggedArrayNew(.dat)
    return(ret)
}

## modify in-place means one obj per test
## be careful not to make copies...
test.arr.R <- mk.test.arr()
test.mat <- test.arr.R$data
test.arr.C <- mk.test.arr()
test.arr.sample <- mk.test.arr()
test.arr.sample.C <- mk.test.arr()
test.arr.sample.alloc <- mk.test.arr()
test.arr.sample.alloc.C <- mk.test.arr()

.nrep=1e2
## multiply by 10, also check vanilla matrix
timings.times10 <- benchmark(
    R={test.arr.R$sapply(FunTimes10R)},
    C={test.arr.C$sapplyC(FunTimes10)},
    mat={test.mat <- test.mat * 10},
    order='relative', replications=.nrep
)

## sample with memory reallocations
timings.sample.alloc <- benchmark(
    sample={set.seed(1); test.arr.sample.alloc$sapplyAlloc(function(x) sample(x, length(x)+1, replace=T))},
    sampleC={set.seed(1); test.arr.sample.alloc.C$sapplyAllocC(FunSample1)},
    order='relative', replications=.nrep
)

## sample, no memory reallocations
timings.sample <- benchmark(
    sample={set.seed(1); test.arr.sample$sapply(function(x) sample(x, length(x), replace=T))},
    sampleC={set.seed(1); test.arr.sample.C$sapplyC(FunSample)},
    order='relative', replications=.nrep
)
