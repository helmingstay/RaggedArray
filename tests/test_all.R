library(testthat)
library(Rcpp)  ## for sourceCpp tests
#load_all("RaggedArray") # or library(pkg) in case your package is
                # already built and installed
test_package("RaggedArray")
