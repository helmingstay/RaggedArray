#' Wrapper class for R/C++ fast manipulation of ragged arrays
#'
#' @description This page describes the behavior of the reference class 
#' \code{RaggedArray}, which wraps
#' a C++ class that allows for fast, in-place manipulation of ragged arrays.
#'
#' @details The S4 class RaggedArray implements runtime-efficient 
#' manipulation of ragged arrays at the espense of storage space. 
#' This is implemented as a C++ class and \pkg{Rcpp} module that employs
#' pass-by-reference semantics.  A RaggedArray object contains a numeric 
#' data matrix that stores each element
#' of the ragged array as (column) vectors, and an integer vector 
#' that stores the length of each element. Vectorized functions can 
#' operate on the RaggedArray in-place to prevent expensive memory 
#' reallocations.
#' 
#' @section Methods:
#' Instead of separate functions, a RaggedArray \code{object} contains 
#' a number of member functions that are documented here. All methods
#' modify \code{object} in-place, and silently return NULL.
#' See unit tests in directory \code{file.path("/inst/tests")} for usage examples.
#'
#' \describe{
#'      \item{\code{serialize}:}{ Return a native R list representation of 
#'          object.  Suitable for persistence across cycles of 
#'          \code{save(); quit(); load()}. 
#'      }
#'      \item{\code{append}:}{ Takes a list of numeric of numeric vectors  
#'          appends each vector in list to the column vectors in 
#'          object$data. Required: length(list) == object$nvec
#'       } 
#'      \item{\code{sapply}:}{ Takes an (vectorized) R function \code{fun} that 
#'          will be applied to each element of \code{object}. 
#'          Required: \code{length(fun(x)) == length(x)}
#'       } 
#'      \item{\code{sapplyAlloc}:}{ As for \code{sapply}, but 
#'          \code{length(fun(x)) != length(x)} is allowed.  \code{object}
#'          will grow as needed (but will *not* shrink). Extra elements
#'          of \code{object$data} are set to zero.
#'       } 
#'      \item{\code{sapplyC}:}{As for \code{sapply}, where \code{fun} is a 
#'          user-specified
#'          C++ function.  See \code{\link{RaggedArrayCpp}} for details.
#'       } 
#'      \item{\code{sapplyAllocC}:}{A combination of \code{sapplyAlloc} and 
#'          \code{sapplyC} 
#'       } 
#'      \item{\code{mapply*}:}{\code{mapply} versions of \code{sapply}, \code{sapplyAlloc}, \code{sapplyC}, \code{sapplyAllocC}.  The supplied function should take a single numeric value as its second argument.  For the \code{mapplyC*} versions, the signature is \code{arma::vec x, double y}, where x is the object's vector, and y is the (parallel) second argument.
#'       } 
#' }
#'
#' @name RaggedArrayClass
#' @family RaggedArray Docs
#' @seealso \code{\link{Rcpp}} \code{\link{sapply}} \code{\link{mapply}} 
NULL
