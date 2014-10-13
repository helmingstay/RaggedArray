.context <- 'RaggedArray Basic Functionality'
context(.context)
test_that(.context, {
    ## initialize empty array
    ragged <- RaggedArrayNew( nvec=3, allocLen=20, growBy=5)
    ## test data to append
    listdat <- list(1:5, 1:6, 1:7)
    ragged$append(listdat)
    ragged$append(listdat)
    ## access fields directly 
    expect_equal( ragged$nvec, 3)
    expect_equal( ragged$growBy, 5)
    expect_equal( ragged$lengths, c( 10, 12, 14))
    ## modify in-place
    ragged$sapply(function(x) {2*x})
    ## access data directly
    expect_equal( ragged$data[10,], c( 10, 8, 6))
    ## test incorrect realloc: error
})
