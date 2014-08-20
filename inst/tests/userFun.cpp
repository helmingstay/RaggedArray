// [[Rcpp::depends(RcppArmadillo)]]
#include <RcppArmadillo.h>
using namespace Rcpp;
typedef void (*funcPtr)(arma::vec& x);

void myfunDefn(arma::vec& x) {    // and a second function
    x = 10*x;
}

// [[Rcpp::export]]
XPtr<funcPtr> myfun_times10() {
    return(XPtr<funcPtr>(new funcPtr(&myfunDefn)));
}


void myfun1Defn(arma::vec& x) {    // and a second function
    x = arma::sum(x);
    //return (x);
}

// [[Rcpp::export]]
XPtr<funcPtr> myfun1_sum() {
    return(XPtr<funcPtr>(new funcPtr(&myfun1Defn)));
}
