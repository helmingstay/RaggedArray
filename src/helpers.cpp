// [[Rcpp::depends(RcppArmadillo)]]
#include <Rmath.h>
#include <RcppArmadillo.h>

using namespace Rcpp ;
// pointer to user-supplied function
typedef void (*funcPtr)(arma::vec& x);

class RaggedArray {
private:
    int nvec, allocLen;
    NumericMatrix data;
    IntegerVector lengths;
public:
    int growBy;
    // getters
    int get_growBy() { return growBy; }
    int get_nvec() { return nvec; }
    IntegerVector get_lengths() { return lengths; }
    NumericMatrix get_data() { return data; }
    //
    // Constructors
    // construct empty obj
    RaggedArray(int  nvec_, int allocLen_, int growBy_) : nvec(nvec_), allocLen(allocLen_), data(allocLen, nvec), lengths(nvec), growBy(growBy_){
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
    void grow(int minLen) {
    // worker function to reallocate data matrix
        // always grow by a multiple of growBy
        // always grow to at least minLen
        int end;
        int newLen = ceil( (float)minLen / (float)growBy) * growBy;
        // larger empty object to be filled
        NumericMatrix tmp(newLen, nvec);
        // fill by column
        for (int icol = 0; icol<nvec; icol++){
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
    void grow_assign_sapply(int icol, int thisLen, arma::vec& dataVec, NumericMatrix::iterator& colStart) {
    // worker function shared by sapply and sapply_cpp
        // check size, grow as needed
        int sizeNew = dataVec.size();
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
        std::copy( dataVec.begin(), dataVec.end(), colStart);
    }

    void sapply_master(Function fun, bool realloc) {
    // apply R function to each vector, update in place
    // realloc=T: allow vector size to change
        int icol, thisLen;
        for ( icol = 0; icol < nvec; icol++){
            thisLen = lengths[icol];
            NumericMatrix::iterator colStart = data.begin() + (icol * this->allocLen);
            // copy_aux_mem=false, reuse existing memory
            // implies strict=true, arma enforces dim match
            arma::vec dataVec(colStart, thisLen, false);
            if ( !realloc ) {
                // modifies data in-place
                // no size change allowed 
                // all the casting seems excessive, 
                // but avoids memory copies / realloc
                dataVec = as<arma::vec>(fun(dataVec));
            } else {
                // realloc
                // cast to arma for compatability w/grow_assign
                // not necessary??
                arma::vec dataNew = as<arma::vec>(fun(dataVec));
                grow_assign_sapply(icol, thisLen, dataNew, colStart);
            }
        }
    }

    void sapply_cpp_master( SEXP funPtrfun, bool realloc=false  ) {
    // call user-defined c++ function that returns external pointer to function that modifies arma::vec
    // specialized into functions for realloc=T/F
        int icol, thisLen;
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
                grow_assign_sapply(icol, thisLen, dataVec, colStart);
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
    void sapply_cpp_alloc( SEXP funPtrfun) {
        // realloc=true
        sapply_cpp_master(funPtrfun, true);
    }
    void sapply_cpp( SEXP funPtrfun) {
        // realloc=false
        sapply_cpp_master(funPtrfun, false);
    }

    void append(  List fillVecs) {
        // "append" fill oldmat w/  
        // we will loop through cols, filling retmat in with the vectors in list
        // then update retmat_size to index the next free
        // newLenths isn't used, added for compatibility
        int sizeOld, sizeAdd, sizeNew;
        NumericVector fillTmp;
        // check that number of vectors match
        if ( nvec != fillVecs.size()) {
            throw std::range_error("In append(): dimension mismatch");
        }
        for (int icol = 0; icol<nvec; icol++){
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

