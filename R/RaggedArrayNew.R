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
#' ragged <- RaggedArrayNew( list( 1:5, 1:6, 1:7))
#' all.equal(ragged, RaggedArrayNew( ragged$serialize() ))
#' str(ragged)
#' ragged$sapply(function(x) x*2)
#' head(ragged$data)
#' sourceCpp(system.file('examples', 'cpp', 'userSapplyFun.cpp', package='RaggedArray'))
#' ragged$sapplyC(FunTimes10)
#' @family Ragged Array Docs
RaggedArrayNew <- function(dataList=NULL,
    nvec=NULL, allocLen=NULL, growBy=NULL)
{
    ## can provide data, specs, or both
    .no.data <- is.null(dataList) 
    .no.specs <- (is.null(nvec) || is.null(allocLen))
    ## dataList is a serialized RaggedArray, 
    ## Ignore specs, pass straight to new()
    if (!.no.data && "SerializedRaggedArray" %in% names(dataList)) {
        ## check that we have a valid object
        .data <- dataList$data
        .lens <- dataList$lengths
        .nvec <- dataList$nvec
        if(!( is.numeric(.data) &&
            is.numeric(.lens) &&
            is.numeric(.nvec) &&
            is.numeric(dataList$growBy) &&
            all( .nvec == c(dim(.data)[2], length(.lens)))
        ))  stop("dataList appears to be a serialized RaggedArray, but fails sanity checks.")
        ret <- new(RaggedArray, dataList)
        return(ret)
    }
    if( .no.data && .no.specs) {
        stop("In RaggedArray: must provide dataList or nvec and allocLen (or both)")
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
