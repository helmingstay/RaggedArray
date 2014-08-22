// [[Rcpp::depends(RcppArmadillo)]]
// #include <RcppArmadillo.h>
#include <RcppArmadilloExtensions/sample.h>
using namespace Rcpp;
typedef NumericVector (*funcPtr)(arma::vec& x);

// worker function to modify x in-place
NumericVector myfunDefn(arma::vec& x) {
    x = 10*x;
}
// followed by exported function to return external pointer
// [[Rcpp::export]]
XPtr<funcPtr> myfun_times10() {
    return(XPtr<funcPtr>(new funcPtr(&myfunDefn)));
}

// [[Rcpp::export]]
NumericVector myfun_times10R(NumericVector& x) {
    x = 10*x;
    return x;
}

NumericVector myfun1Defn(arma::vec& x) {    
    x = arma::sum(x);
}

// [[Rcpp::export]]
XPtr<funcPtr> myfun1_sum() {
    return(XPtr<funcPtr>(new funcPtr(&myfun1Defn)));
}


NumericVector defnSample(arma::vec& x) {    
    RNGScope scope;
    x = RcppArmadillo::sample(x, x.size(), true);
}

// [[Rcpp::export]]
XPtr<funcPtr> myfun_sample() {
    return(XPtr<funcPtr>(new funcPtr(&defnSample)));
}


// grow x
NumericVector defnSample1(arma::vec& x) {    
    RNGScope scope;
    x = RcppArmadillo::sample(x, x.size()+1, true);
}
// [[Rcpp::export]]
XPtr<funcPtr> myfun_sample1() {
    return(XPtr<funcPtr>(new funcPtr(&defnSample1)));
}
