// [[Rcpp::depends(RcppArmadillo)]]
// #include <RcppArmadillo.h>
#include <RcppArmadilloExtensions/sample.h>
using namespace Rcpp;
typedef void (*funcPtr)(arma::vec& x);
typedef void (*funcPtrM)(arma::vec& x, double y);

// Bodies of user-accessible functions 
//
#define RAGGED_SAPPLY(workerFunName) return(XPtr<funcPtr>(new funcPtr(&workerFunName)))
#define RAGGED_MAPPLY(workerFunName) return(XPtr<funcPtrM>(new funcPtrM(&workerFunName)))

// worker function to modify x in-place
void workerTimes10(arma::vec& x) {
    x = 10*x;
}
// followed by exported function to return external pointer
// [[Rcpp::export]]
XPtr<funcPtr> userFunTimes10() {
    RAGGED_SAPPLY(workerTimes10);
    //return(XPtr<funcPtr>(new funcPtr(&workerFunTimes10)));
}


// [[Rcpp::export]]
NumericVector userFunTimes10R(NumericVector& x) {
    x = 10*x;
    return x;
}

void workerSum(arma::vec& x) {
    x = arma::sum(x);
}

// [[Rcpp::export]]
XPtr<funcPtr> userFunSum() {
    RAGGED_SAPPLY(workerSum);
}

void workerSample(arma::vec& x) {    
    //RNGScope scope;
    x = RcppArmadillo::sample(x, x.size(), true);
}
// [[Rcpp::export]]
XPtr<funcPtr> userFunSample() {
    RAGGED_SAPPLY(workerSample);
}

void workerSampleBig(arma::vec& x) {    
    //RNGScope scope;
    x = RcppArmadillo::sample(x, 1e7, true);
}
// [[Rcpp::export]]
XPtr<funcPtr> userFunSampleBig() {
    RAGGED_SAPPLY(workerSampleBig);
}

// grow x
void workerSample1(arma::vec& x) {
    //RNGScope scope;
    x = RcppArmadillo::sample(x, x.size()+1, true);
}
// [[Rcpp::export]]
XPtr<funcPtr> userFunSample1() {
    RAGGED_SAPPLY(workerSample1);
}

// mapply version of sample
void workerSampleM(arma::vec& x, double y) {
    //RNGScope scope;
    int iy = y;
    x = RcppArmadillo::sample(x, iy, true);
}
// NOTE - return value of userFun has changed!
// [[Rcpp::export]]
XPtr<funcPtrM> userFunSampleM() {
    RAGGED_MAPPLY(workerSampleM);
}
