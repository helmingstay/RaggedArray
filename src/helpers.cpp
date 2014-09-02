// [[Rcpp::depends(RcppArmadillo)]]
#include <Rmath.h>
#include <RcppArmadillo.h>

using namespace Rcpp ;
// pointer to user-supplied function
typedef void (*funcPtr)(arma::vec& x);
typedef void (*funcPtrM)(arma::vec& x, const double y);

class RaggedArray {
private:
    std::size_t nvec, allocLen;
    NumericMatrix data;
    IntegerVector lengths;
public:
    std::size_t growBy;
    // getters
    std::size_t get_growBy() { return growBy; }
    std::size_t get_nvec() { return nvec; }
    IntegerVector get_lengths() { return lengths; }
    NumericMatrix get_data() { return data; }
    //
    // Constructors
    // construct empty obj
    RaggedArray(std::size_t  nvec_, std::size_t allocLen_, std::size_t growBy_) : nvec(nvec_), allocLen(allocLen_), data(allocLen, nvec), lengths(nvec), growBy(growBy_){
    }
    // construct obj from R List as returned by method asList()
    RaggedArray (List fromList) {
        // check names of fromList??
        growBy = fromList["growBy"];
        data = as<NumericMatrix>(fromList["data"]);
        lengths = as<IntegerVector>(fromList["lengths"]);
        allocLen = data.nrow();
        nvec = data.ncol();
    }

    List serialize() {
        // return a list that constructor can use to regenerate object
        return List::create(_["data"] = data, _["lengths"] = lengths, _["growBy"] = growBy);
    }

    // specializations of private sapply_master 
    void sapply( Function fun ) {
        // realloc=false, cppfun=false
        sapply_master(fun, false, false);
    }
    void sapplyAlloc( Function fun ) {
        // realloc=true, cppfun=false
        sapply_master(fun, true, false);
    }
    void sapplyC( Function funPtrfun) {
        // realloc=false, cppfun=true
        sapply_master(funPtrfun, false, true);
    }
    void sapplyAllocC( Function funPtrfun) {
        // realloc=true
        sapply_master(funPtrfun, true, true);
    }

    void mapply( Function fun, NumericVector arg ) {
        // realloc=false, cppfun=false
        mapply_master(fun, arg, false, false);
    }
    void mapplyAlloc( Function fun, NumericVector arg ) {
        // realloc=true, cppfun=false
        mapply_master(fun, arg, true, false);
    }
    void mapplyC( Function funPtrfun, NumericVector arg) {
        // realloc=false, cppfun=true
        mapply_master(funPtrfun, arg, false, true);
    }
    void mapplyAllocC( Function funPtrfun, NumericVector arg) {
        // realloc=true
        mapply_master(funPtrfun, arg, true, true);
    }
    void append(  List fillVecs) {
        // "append" fill oldmat w/  
        // we will loop through cols, filling retmat in with the vectors in list
        // then update retmat_size to index the next free
        // newLenths isn't used, added for compatibility
        std::size_t sizeOld, sizeAdd, sizeNew, icol;
        NumericVector fillTmp;
        // check that number of vectors match
        if ( nvec != fillVecs.size()) {
            throw std::range_error("In append(): dimension mismatch");
        }
        for (icol = 0; icol<nvec; icol++){
            // vector to append
            fillTmp = fillVecs[icol];
            // compute lengths
            sizeOld = lengths[icol];
            sizeAdd = fillTmp.size();
            sizeNew = sizeOld + sizeAdd;
            // grow data matrix as needed
            if ( sizeNew > allocLen) {
                grow(sizeNew);
            }
            // iterator for col to fill
            NumericMatrix::Column dataCol = data(_, icol);
            // fill row of return matrix, starting at first non-zero elem
            std::copy( fillTmp.begin(), fillTmp.end(), dataCol.begin() + sizeOld);
            // update size of retmat
            lengths[icol] = sizeNew;
        }
    }


private:
    void grow(std::size_t minLen) {
    // worker function to reallocate data matrix
        // always grow by a multiple of growBy
        // always grow to at least minLen
        std::size_t end, icol;
        std::size_t newLen = ceil( (float)minLen / (float)growBy) * growBy;
        // larger empty object to be filled
        NumericMatrix tmp(newLen, nvec);
        // fill by column
        for (icol = 0; icol<nvec; icol++){
            end = lengths[icol];
            // grab iterators for new and old data structures
            NumericMatrix::Column tmpCol = tmp(_, icol);
            NumericMatrix::Column dataCol = data(_, icol);
            // copy data from beginning to edge onto tmp
            std::copy( dataCol.begin(), dataCol.begin() + end, tmpCol.begin());
        }
        data = tmp;
        allocLen = newLen;
    }
    void grow_assign_sapply(std::size_t icol, std::size_t thisLen, arma::vec& dataVec, NumericMatrix::iterator& colStart) {
    // worker function shared by sapply and sapply_cpp
        // check size, grow as needed
        std::size_t sizeNew = dataVec.size();
        lengths[icol] = sizeNew;
        if ( sizeNew > allocLen) {
            grow(sizeNew);
            // recompute offsets
            colStart = data.begin() + (icol * allocLen);
        }
        if ( sizeNew < thisLen) {
            // if results are shorter, zero out extra items
            // not necessary, but prevents confusion viewing $data
            std::fill( colStart+sizeNew, colStart + thisLen, 0);
        }
        // fill row of return matrix
        std::copy(dataVec.begin(), dataVec.end(), colStart);
    }


    /// Apply R function, treat return value as arma::vec
    /// Arg is arma::vec proxy of R object, update in-place
    void FunFromR(Function fun, arma::vec &arg) {
        arg =  as<arma::vec>(fun(arg));
    }
    void FunFromR(Function fun, arma::vec &arg, double mapply_arg) {
        arg =  as<arma::vec>(fun(arg, mapply_arg));
    }

    void sapply_master(Function infun, bool realloc, bool cppfun) {
    // apply user-supplied function infun to each vector, update in place
    // realloc=true: allow function input and return dim to differ
    // cppfun=false: infun is regular R function 
    // cppfun=true: infun returns XPtr to C++ function,
    //      infun takes arma::vec&, returns void
        std::size_t icol, thisLen;
        funcPtr fun;
        if (cppfun) {
            SEXP funPtrfun = infun();
            XPtr<funcPtr> xpfun(funPtrfun);
            fun = *xpfun;
        }
        for ( icol = 0; icol < nvec; icol++){
            thisLen = lengths[icol];
            NumericMatrix::iterator colStart = data.begin() + (icol * this->allocLen);
            // copy_aux_mem=false: reuse existing memory, modify data in-place
            // strict=true, arma enforces dim match, no size change allowed
            arma::vec dataVec(colStart, thisLen, false, !realloc);
            if(cppfun) {
                fun(dataVec);
            } else {
                FunFromR(infun, dataVec);
            }
            if ( dataVec.size() != thisLen) {
                // won't make it here unless realloc=true
                // change dimension of data matrix, reset colStart
                grow_assign_sapply(icol, thisLen, dataVec, colStart);
            }
        }
    }

    void mapply_master(Function infun, NumericVector arg, bool realloc, bool cppfun) {
    // apply user-supplied function infun to each vector, update in place
    // realloc=true: allow function input and return dim to differ
    // cppfun=false: infun is regular R function 
    // cppfun=true: infun returns XPtr to C++ function,
    //      infun takes arma::vec&, returns void
        std::size_t icol, thisLen;
        double thisArg;
        funcPtrM fun;
        if (cppfun) {
            SEXP funPtrfun = infun();
            XPtr<funcPtrM> xpfun(funPtrfun);
            fun = *xpfun;
        }
        for ( icol = 0; icol < nvec; icol++){
            thisLen = lengths[icol];
            thisArg = arg[icol];
            NumericMatrix::iterator colStart = data.begin() + (icol * this->allocLen);
            // copy_aux_mem=false: reuse existing memory, modify data in-place
            // strict=true, arma enforces dim match, no size change allowed
            arma::vec dataVec(colStart, thisLen, false, !realloc);
            if(cppfun) {
                fun(dataVec, thisArg);
            } else {
                FunFromR(infun, dataVec, thisArg);
            }
            if ( dataVec.size() != thisLen) {
                // won't make it here unless realloc=true
                // change dimension of data matrix, reset colStart
                grow_assign_sapply(icol, thisLen, dataVec, colStart);
            }
        }
    }

};
