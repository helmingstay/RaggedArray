#include <Rcpp.h> 
using namespace Rcpp;

// export a vanilla R function
// Nothing fancy, for timing comparison 
// w/XPtr functions

// [[Rcpp::export]]
NumericVector FunTimes10R(NumericVector& x) {
    x = 10*x;
    return x;
}
