#' @export
#' @title Construct a new RaggedArray object
#'
#' @description \code{RaggedArrayNew} constructs a new RaggedArray object, filling it with the provided data, if any. Either \code{dataList} or \code{nvec & allocLen} are mandatory.  Given one, the other(s) will be inferred. Alternately, if \code{dataList} is a serialized RaggedArray (e.g. is an R list with element \code{SerializedRaggedArray}), a new object will be created using the default List constructor. In this case, specs are silently ignored.
#' 
#' See \code{\link{RaggedArrayClass}} for methods to manipulate object.
#'
#' @param dataList List of (numeric) vectors to fill the new object, or the results of a previous \code{object$serialize()}.
#' @param nvec Number of (column) vectors that object will store. This is constant over the lifespan of the object.  Defaults to \code{length(dataList)}.
#' @param allocLen Number of rows to allocate in underlying data matrix.   Object will grow as needed.  Defaults to \code{max(sapply(dataList, length))}.
#' @param growBy Number of rows to grow object data matrix by when reallocation is needed. Can be changed.  Defaults to \code{allocLen}.
#' @return An object of (S4) class RaggedArray.
#' @examples
#' RaggedArrayNew( list( 1:5, 1:6, 1:7))
#' RaggedArrayNew( nvec=3, allocLen=20)
#' RaggedArrayNew( nvec=3, allocLen=20, growBy=50)
#' RaggedArrayNew( RaggedArrayNew( list(1:2, 1:10) )$serialize() )
#' str(RaggedArrayNew( list( 1:2, 1:10), growBy=50))
#' sourceCpp('inst/tests/userFun.cpp')
#' @family Ragged Array Docs
RaggedArrayNew <- function(dataList=NULL,
    nvec=NULL, allocLen=NULL, growBy=NULL)
{
    ## can provide data, specs, or both
    .no.data <- is.null(dataList) 
    .no.specs <- (is.null(nvec) || is.null(allocLen))
    ## List is a serialized RaggedArray, 
    ## Ignore specs, pass straight to new()
    if (!.no.data && "SerializedRaggedArray" %in% names(dataList)) {
        ret <- new(RaggedArray, dataList)
        return(ret)
    }
    if( .no.data && .no.specs) {
        stop("In RaggedArray: must provide dataList or nvec and allocLen")
    } else if ( .no.specs ){
        ## if not provided, get specs from data
        nvec <- length(dataList)
        allocLen <- max(sapply(dataList, length))
    }
    if (is.null(growBy)) {
        growBy <- allocLen
    }
    ret <- new(RaggedArray, nvec, allocLen, growBy)
    if ( !.no.data ) {
        ret$append(dataList)
    }
    ret
}
