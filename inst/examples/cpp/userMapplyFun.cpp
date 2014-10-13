// [[Rcpp::depends(RcppArmadillo)]]
// #include <RcppArmadillo.h>
#include <RcppArmadilloExtensions/sample.h>
using namespace Rcpp;

// a pointer to a function that modifies x in-place
// and an extra arg
typedef void (*funcPtrM)(arma::vec& x, double y);

// Construct user-accessible function 
// for sapply method from worker functions
#define RAGGED_MAPPLY(workerFunName) return(XPtr<funcPtrM>(new funcPtrM(&workerFunName)))

// mapply version of sample, y draws 
// as in userSapplyFun.cpp, 
// with different macro and return value
void workerSampleM(arma::vec& x, double y) {
    //RNGScope scope;
    int iy = y;
    x = RcppArmadillo::sample(x, iy, true);
}
// [[Rcpp::export]]
XPtr<funcPtrM> FunSampleM() {
    RAGGED_MAPPLY(workerSampleM);
}
