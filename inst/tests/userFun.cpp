// [[Rcpp::depends(RcppArmadillo)]]
#include <RcppArmadillo.h>
#include <RcppArmadilloExtensions/sample.h>
using namespace Rcpp;
typedef void (*funcPtr_armavec)(arma::vec& x);

// worker function to modify x in-place
void myfunDefn(arma::vec& x) {    
    x = 10*x;
}
// followed by exported function to return external pointer
// [[Rcpp::export]]
XPtr<funcPtr_armavec> myfun_times10() {
    return(XPtr<funcPtr_armavec>(new funcPtr_armavec(&myfunDefn)));
}

// [[Rcpp::export]]
NumericVector myfun_times10R(NumericVector& x) {
    x = 10*x;
    return x;
}

void myfun1Defn(arma::vec& x) {    
    x = arma::sum(x);
}

// [[Rcpp::export]]
XPtr<funcPtr_armavec> myfun1_sum() {
    return(XPtr<funcPtr_armavec>(new funcPtr_armavec(&myfun1Defn)));
}


void defnSample(arma::vec& x) {    
    RNGScope scope;
    x = RcppArmadillo::sample(x, x.size(), true);
}

// [[Rcpp::export]]
XPtr<funcPtr_armavec> myfun_sample() {
    return(XPtr<funcPtr_armavec>(new funcPtr_armavec(&defnSample)));
}


// grow x
void defnSample1(arma::vec& x) {    
    RNGScope scope;
    x = RcppArmadillo::sample(x, x.size()+1, true);
}
// [[Rcpp::export]]
XPtr<funcPtr_armavec> myfun_sample1() {
    return(XPtr<funcPtr_armavec>(new funcPtr_armavec(&defnSample1)));
}
