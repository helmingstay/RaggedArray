library(rbenchmark)


## helper function
## prepare a standard RaggedArray for testing
mk.test.arr <- function(.ncol=1e3, .veclen=1e3, .growby=1e3){
    ret <- new(RaggedArray, .ncol, .veclen, .growby)
    .vec <- rnorm(.veclen)
    ret$append(lapply(1:.ncol, function(x) .vec))
    return(ret)
}


test.arr.R <- mk.test.arr()
test.arr.C <- mk.test.arr()

test.nreps <- 1e2
test.ncols <- 1e4
timings <- benchmark(
    R=test.arr.R$sapply(),
    C=test.arr.C$sapply_cpp(),
    replications=1e1
)
