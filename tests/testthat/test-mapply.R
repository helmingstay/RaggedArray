.context <- 'RaggedArray mapply methods'
context(.context)
test_that(.context, {
    ## initialize array with data
    ragged <- RaggedArrayNew(list(1:5, 1:6, 1:7))
    listdat <- list(1:5, 1:6, 1:7)
    res <- sourceCpp('cpp/userMapplyFun.cpp'); 
    expect_named(res, expect=c('functions', 'modules'))
    ## mapply
    ragged$mapply(function(x, y) sample(x, y), ragged$lengths)
    expect_true( all(sapply(listdat, sum) == apply(ragged$data, 2, sum)))

    ragged$mapplyAlloc(function(x, y) sample(x, y, replace=T), 1:ragged$nvec)
    expect_true( all(ragged$lengths == 1:3))
    ragged$append(listdat)
    ragged$mapplyAllocC(FunSampleM, 2*(ragged$nvec:1))
    expect_true( all(ragged$lengths == 2*(3:1)))
    expect_error( 
        ragged$mapplyAllocC(FunSampleM, 10),
        info="mapply arg must have length == nvec"
    )
    .n <- 1e5
    .lengths <- .n:(.n+2)
    ragged$mapplyAllocC(FunSampleM, .lengths)
    expect_true( all(ragged$lengths == .lengths))
})
