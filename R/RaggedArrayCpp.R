#' Writing user-supplied C++ functions for \code{sapply*C} and \code{mapply*C} (\code{\link{RaggedArrayClass}}).
#'
#' @description This page describes how to write user-supplied C++ functions
#' for use by \code{RaggedArray} objects. 
#' The easiest way is to follow and extend the examples provided in the \code{userFun.cpp} file.
#' Also see the unit tests, contained in the \code{./inst/tests} directory.
#'
#' @details Users can provide R functions to the \code{sapply} and \code{mapply} methods of \code{\link{RaggedArrayClass}} objects, but R functions require non-trivial overhead to access from C++.  Alternately, users can write their own C++ functions.  In order for a \code{\link{RaggedArrayClass}} object to access user-supplied C++ functions, a second wrapper function must return an external pointer to the C++ function.
#' 
#' Most users should simply modify and extend the examples provided in the \code{userFun.cpp} file. A call to \code{\link[Rcpp]{sourceCpp("userFun.cpp")}} will automatically compile and link the user functions. Compilation only occurs as needed, so extra calls to \code{sourceCpp()} incur no cost, and can be safely added to R scripts.  Note that this \code{sourceCpp()} mechanism is not appropriate for packages that extend \code{RaggedArray}.
#'
#'@section Gory Details:
#'   To use \code{sapplyC*}, two functions are required.  First, the desired computation is conducted by a worker function with signature \code{void workerFun(arma::vec& x)}.  This function should assign its results to x (modifying x in-place).  A second function \code{userFun} returns an appropriately-typed XPtr to \code{workerFun}.  \code{userFun} is exported via \pkg{Rcpp} attributes and passed by the user to \code{sapplyC} or \code{sapplyAllocC}.  Internally, \code{sapply*C} calls this function, dereferences the resulting pointer, and calls the appropriate workerFun on each vector in \code{RaggedArray}. 
#'
#' The example code in the \code{userFun.cpp} file includes two typedefs for use with \code{sapplyC} and \code{mapplyC}, respectively:
#' \itemize{ 
#'      \item \code{typedef void (*funcPtr)(arma::vec& x);} 
#'      \item \code{typedef void (*funcPtrM)(arma::vec& x, double y);}
#' }
#' 
#' The signatures of the exported C++ functions \code{userFunSapply} \code{userFunMapply} (for use the respective \code{*applyC} methods) are then:
#' \itemize{ 
#'      \item \code{XPtr<funcPtr> userFunSapply()} 
#'      \item \code{XPtr<funcPtrM> userFunMapply()} 
#' }
#' 
#' A full example for use with \code{sapplyC} thus includes 2 C++ functions and an R call.
#' The C++ functions are:
#' \preformatted{
#' // worker function, modify x in-place
#' void workerMult(arma::vec& x) {
#'    x = 10*x;
#'}
#'// return external pointer to workerFun, export with Attributes
#'// [[Rcpp::export]]
#'XPtr<funcPtr> userFunSapplyMult() {
#'    return(XPtr<funcPtr>(new funcPtr(&workerMult)));
#'}
#'}
#' The final R call is:
#' \code{object$sapplyC(userFunSapplyMult)}.  
#' 
#' @name RaggedArrayCpp
#' @family RaggedArray Docs
#' @seealso \code{\link[Rcpp]{sourceCpp}} \code{\link{sapply}} \code{\link{mapply}}
NULL
