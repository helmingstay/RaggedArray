// [[Rcpp::depends(RcppArmadillo)]]
// #include <RcppArmadillo.h>
#include <RcppArmadilloExtensions/sample.h>
using namespace Rcpp;
// a pointer to a function that modifies x in-place
typedef void (*funcPtr)(arma::vec& x);

// Construct user-accessible function 
// for sapply method from worker functions
#define RAGGED_SAPPLY(workerFunName) return(XPtr<funcPtr>(new funcPtr(&workerFunName)))

// First define a C++ worker function to perform the computation
// modifying x in-place
void workerTimes10(arma::vec& x) {
    x = 10*x;
}
// next use the supplied macro and Rcpp::export
// to make a user-accessible special function for use by sapplyC
// [[Rcpp::export]]
XPtr<funcPtr> FunTimes10() {
    RAGGED_SAPPLY(workerTimes10);
    //return(XPtr<funcPtr>(new funcPtr(&workerFunTimes10)));
}

// as above, a pair of functions: worker, exported
// in this example, the dimension of x changes
void workerSum(arma::vec& x) {
    x = arma::sum(x);
}
// [[Rcpp::export]]
XPtr<funcPtr> FunSum() {
    RAGGED_SAPPLY(workerSum);
}

// resample, same size
void workerSample(arma::vec& x) {    
    //RNGScope scope;
    x = RcppArmadillo::sample(x, x.size(), true);
}
// [[Rcpp::export]]
XPtr<funcPtr> FunSample() {
    RAGGED_SAPPLY(workerSample);
}

// resample a lot
void workerSampleBig(arma::vec& x) {    
    //RNGScope scope;
    x = RcppArmadillo::sample(x, 1e7, true);
}
// [[Rcpp::export]]
XPtr<funcPtr> FunSampleBig() {
    RAGGED_SAPPLY(workerSampleBig);
}

// resample w/resizing (grow x by 1)
void workerSample1(arma::vec& x) {
    //RNGScope scope;
    x = RcppArmadillo::sample(x, x.size()+1, true);
}
// [[Rcpp::export]]
XPtr<funcPtr> FunSample1() {
    RAGGED_SAPPLY(workerSample1);
}
