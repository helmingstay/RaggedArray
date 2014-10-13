context('Constructors')
test_that('RaggedArray Constructors', {
    with_data <- RaggedArrayNew( list( 1:5, 1:6, 1:7))
    without_data <- RaggedArrayNew( nvec=3, allocLen=20)
    without_data_alt <- RaggedArrayNew( nvec=3, allocLen=20, growBy=50)
    ## 
    expect_equivalent( class(with_data), "Rcpp_RaggedArray")
    expect_equivalent( class(without_data), "Rcpp_RaggedArray")
    expect_equivalent( class(without_data_alt), "Rcpp_RaggedArray")
})
