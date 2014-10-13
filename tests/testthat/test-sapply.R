if(T){
.context <- 'RaggedArray compiled sapply methods'
context(.context)
test_that(.context, {
    ## initialize array with data
    ragged <- RaggedArrayNew(list(1:5, 1:6, 1:7))
    listdat <- list(1:5, 1:6, 1:7)
    ## compile/export C++ function
    ## both "vanilla" C++ and XPtr functions 
    res <- sourceCpp('cpp/userVanillaFun.cpp'); 
    res1 <- sourceCpp('cpp/userSapplyFun.cpp'); 
    expect_named(res, expect=c('functions', 'modules'))
    expect_named(res1, expect=c('functions', 'modules'))
    ## user-defined "vanilla" C++ function
    ragged$sapply(FunTimes10R)
    ## user-defined special XPtr C++ function
    ragged$sapplyC(FunTimes10)
    expect_equal( ragged$data[1,], c(100, 100, 100))
    ## grow list, sum
    ragged$append(listdat)
    ragged$sapplyAllocC(FunSum)
    expect_equal( ragged$lengths, c(1,1,1))
    expect_true( all(ragged$data[1,] >100))
    expect_true( all(ragged$data[2,] ==0))
    expect_true( all(ragged$data[-1,] ==0))
    ## grow list, sample
    ragged$append(listdat)
    ragged$sapplyAllocC(FunSample)
    ## segfault fixed??
    #ragged$sapplyAllocC(FunSampleBig)
    expect_error(
        ragged$sapplyC(FunSum),
        info="sapplyC not valid with function that changes x dimensions"
    )
})

}
