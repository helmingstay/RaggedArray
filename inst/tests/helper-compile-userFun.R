## see https://github.com/hadley/devtools/blob/master/R/R.r
library(RaggedArray)
library(Rcpp)
library(RcppArmadillo)
## change to package root directory,
## compile test examples of user Cpp functions
sourceCpp(
    system.file( 'tests', 'userFun.cpp', package='RaggedArray')
)
