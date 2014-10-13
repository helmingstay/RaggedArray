context('Serialize')
test_that('RaggedArray Serialize and Restore', {
    ragged <- RaggedArrayNew(list(1:5, 1:6, 1:7))
    ## save object to R List
    saved <- ragged$serialize()
    ## restore
    ragged1 <- RaggedArrayNew(saved)
    expect_equal( ragged, ragged1)
    ## if saved object lacks necessary info, don't restore
    saved <- ragged$serialize()
    saved$growBy <- NULL
    expect_error( 
        RaggedArrayNew(saved),
        info="Trying to construct RaggedArray with missing growBy"
    )
    ## same for lengths
    saved <- ragged$serialize()
    saved$lengths <- NULL
    expect_error( 
        RaggedArrayNew(saved),
        info="Trying to construct RaggedArray with missing lengths"
    )
    ## same for lengths
    saved <- ragged$serialize()
    saved$nvec <- NULL
    expect_error(
        RaggedArrayNew(saved),
        info="Trying to construct RaggedArray with missing nvec"
    )
    ## same for data
    saved <- ragged$serialize()
    saved$data <- NULL
    expect_error( 
        RaggedArrayNew(saved),
        info="Trying to construct RaggedArray with missing nvec"
    )
})
