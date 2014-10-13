context('dataList')
test_that('RaggedArray dataList', {
    ragged <- RaggedArrayNew(list(1:5, 1:6, 1:7))
    ## save data to R List
    saved <- ragged$dataList()
    expect_equal( ragged$lengths, sapply(saved, length))
})
