.context <- 'RaggedArray dimension-changing sapply methods'
context(.context)
test_that(.context, {
    ## initialize array with data
    ragged <- RaggedArrayNew(list(1:5, 1:6, 1:7))
    ## can't use sapply to change dimensions
    expect_error( 
        ragged$sapply(function(x) {c(mean(x), sd(x))}), 
        info="sapply not valid with function that changes x dimensions"
    )
    ## function shrinks obj
    ragged$sapplyAlloc(function(x) {c(mean(x), sd(x))})
    expect_equal( ragged$data[1,], c(3,3.5,4))
    ## all else should equal zero
    expect_true( all(ragged$data[-1:-2,] ==0))
    expect_equal( ragged$data[3,], c(0,0,0))
    ## function grows obj
    ragged$sapplyAlloc(function(x) {sample(x, 22, replace=T)})
    expect_equal( ragged$lengths, c(22, 22, 22))
})
