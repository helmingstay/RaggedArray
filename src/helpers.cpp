// [[Rcpp::depends(RcppArmadillo)]]
#include <Rmath.h>
#include <RcppArmadillo.h>

using namespace Rcpp ;
// pointer to user-supplied function
typedef void (*funcPtr)(arma::vec& x);

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


    //Function ApplyFunction;
    void FunFromR(Function fun, arma::vec &arg) {
        arg =  as<arma::vec>(fun(arg));
    }


    void sapply_master(Function infun, bool realloc) {
    // apply R function to each vector, update in place
    // realloc=T: allow vector size to change
        std::size_t icol, thisLen;
        for ( icol = 0; icol < nvec; icol++){
            thisLen = lengths[icol];
            NumericMatrix::iterator colStart = data.begin() + (icol * this->allocLen);
            // copy_aux_mem=false, reuse existing memory
            // implies strict=true, arma enforces dim match
            if ( !realloc ) {
                // modifies data in-place
                // no size change allowed 
                // all the casting seems excessive, 
                // but avoids memory copies / realloc
                arma::vec dataVec(colStart, thisLen, false);
                FunFromR(infun, dataVec);
                //dataVec = as<arma::vec>(fun(dataVec));
            } else {
                // realloc
                // cast to arma to allow in-place memory
                arma::vec dataVec(colStart, thisLen, false, false);
                //dataVec = as<arma::vec>(fun(dataVec));
                FunFromR(infun, dataVec);
                if ( dataVec.size() != thisLen) {
                    grow_assign_sapply(icol, thisLen, dataVec, colStart);
                }
            }
        }
    }

    void sapply_cpp_master( Function infun, bool realloc=false  ) {
    //void sapply_cpp_master( SEXP funPtrfun, bool realloc=false  ) {
    // call user-defined c++ function that returns external pointer to function that modifies arma::vec
    // specialized into functions for realloc=T/F
        std::size_t icol, thisLen;
        SEXP funPtrfun = infun();
        XPtr<funcPtr> xpfun(funPtrfun);
        funcPtr fun = *xpfun;
        for ( icol = 0; icol < nvec; icol++){
            thisLen = lengths[icol];
            NumericMatrix::iterator colStart = data.begin() + (icol * allocLen);
            if (!realloc) {
                // grab vec to operate on
                // enforce no resize
                arma::vec dataVec(colStart, thisLen, false);
                // function assigns results
                fun(dataVec);
            } else {
                // allow resize
                arma::vec dataVec(colStart, thisLen, false, false);
                fun(dataVec);
                if ( dataVec.size() != thisLen) {
                    grow_assign_sapply(icol, thisLen, dataVec, colStart);
                }
                //grow_assign_sapply(icol, thisLen, dataVec, colStart);
            }
        }
    }
            
public:
    // specialize the above for export 
    void sapply_alloc( Function fun ) {
        // realloc=true
        sapply_master(fun, true);
    }
    void sapply( Function fun ) {
        // realloc=false
        sapply_master(fun, false);
    }
    void sapply_cpp_alloc( Function funPtrfun) {
        // realloc=true
        sapply_cpp_master(funPtrfun, true);
    }
    void sapply_cpp( Function funPtrfun) {
        // realloc=false
        sapply_cpp_master(funPtrfun, false);
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
};

