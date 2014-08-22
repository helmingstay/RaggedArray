## time sapply w/and without realloc, XPtr
library(rbenchmark)

## helper function
## prepare a standard RaggedArray for testing
mk.test.arr <- function(.ncol=1e3, .veclen=1e4, .growby=1e3, seed=1){
    ret <- new(RaggedArray, .ncol, .veclen, .growby)
    set.seed(seed)
    .vec <- rnorm(.veclen)
    ret$append(lapply(1:.ncol, function(x) .vec))
    return(ret)
}

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
    R={test.arr.R$sapply(myfun_times10R)},
    C={test.arr.C$sapply_cpp(myfun_times10())},
    mat={test.mat <- test.mat * 10},
    order='relative', replications=.nrep
)

## sample with memory reallocations
timings.sample.alloc <- benchmark(
    sample={set.seed(1); test.arr.sample.alloc$sapply_alloc(function(x) sample(x, length(x)+1, replace=T))},
    sampleC={set.seed(1); test.arr.sample.alloc.C$sapply_cpp_alloc(myfun_sample1())},
    order='relative', replications=.nrep
)

## sample, no memory reallocations
timings.sample <- benchmark(
    sample={set.seed(1); test.arr.sample$sapply(function(x) sample(x, length(x), replace=T))},
    sampleC={set.seed(1); test.arr.sample.C$sapply_cpp(myfun_sample())},
    order='relative', replications=.nrep
)
