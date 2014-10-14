context('dataList')
test_that('RaggedArray dataList', {
    ragged <- RaggedArrayNew(list(1:5, 1:6, 1:7))
    ## save data to R List
    saved <- ragged$dataList()
    expect_equal( ragged$lengths, sapply(saved, length))
    expect_equal( ragged$data[,1], saved[[1]])
    expect_equal( ragged$data[,2], saved[[2]])
    expect_equal( ragged$data[,3], saved[[3]])
})
