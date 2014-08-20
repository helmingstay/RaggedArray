// [[Rcpp::depends(RcppArmadillo)]]
#include <Rmath.h>
#include <RcppArmadillo.h>

using namespace Rcpp ;
// for user-supplied functions called by Xptr
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

    void sapply_cpp( SEXP funPtrfun ) {
        // call user-defined c++ function that returns external pointer to function that modifies arma::vec
        // requires function arg and ret dimensions match
        int icol, thisLen;
        // grab the external pointer
        XPtr<funcPtr> xpfun(funPtrfun);
        funcPtr fun = *xpfun;
        for ( icol = 0; icol < nvec; icol++){
            thisLen = lengths[icol];
            // column proxy
            NumericMatrix::Column dataCol = data(_, icol);
            arma::vec dataVec(dataCol.begin(), thisLen,  false);
            // apply function, cast back to arma 
            // modifies in-place
            fun(dataVec);
        }
    }
    void sapply_cpp_alloc( SEXP funPtrfun ) {
        // call user-defined c++ function that returns external pointer to function that modifies arma::vec
        // reallocate version
        int icol, thisLen, sizeNew;
        XPtr<funcPtr> xpfun(funPtrfun);
        funcPtr fun = *xpfun;
        for ( icol = 0; icol < nvec; icol++){
            thisLen = lengths[icol];
            //NumericMatrix::Column dataCol = data(_, icol);
            NumericMatrix::iterator colStart = data.begin() + (icol * allocLen);
            // grab the range of data to operate on
            // we allow strict = false, resize is ok
            arma::vec dataNew(colStart, thisLen,  false, false);
            // assign results to new vector and copy back
            fun(dataNew);
            // check size, grow as needed
            sizeNew = dataNew.size();
            if ( sizeNew > allocLen) {
                grow(sizeNew);
                // recompute offsets
                colStart = data.begin() + (icol * allocLen);
            }
            if ( sizeNew < thisLen) {
                // if results are shorter, zero out this row
                // not necessary, but prevents confusion viewing $data
                std::fill( colStart, colStart + allocLen, 0);
            }
            // fill row of return matrix, starting at first non-zero elem
            std::copy( dataNew.begin(), dataNew.end(), colStart);
            lengths[icol] = sizeNew;
        }
    }
    void sapply( Function fun ) {
        // call R function on each col
        int icol, thisLen;
        for ( icol = 0; icol < nvec; icol++){
            thisLen = lengths[icol];
            // column proxy
            NumericMatrix::Column dataCol = data(_, icol);
            arma::vec dataVec(dataCol.begin(), thisLen,  false);
            // apply function, cast back to arma 
            // modifies in-place
            dataVec = as<arma::vec>(fun(dataVec));
            // verify return dim
            // arma takes care of this.
            /* 
            if ( dataVec.size() != thisLen ) {
                throw std::range_error("In sapply(fun): function argument and return value must have equal lengths");
            }
            */
        }
    }

    void sapply_alloc( Function fun ) {
        // call R function on each col/vec
        int icol, thisLen, sizeNew;
        for ( icol = 0; icol < nvec; icol++){
            thisLen = lengths[icol];
            NumericMatrix::iterator colStart = data.begin() + (icol * this->allocLen);
            // grab the range of data for fun to operate on
            arma::vec dataVec(colStart, thisLen,  false);
            NumericVector dataNew = fun(dataVec);
            //Rf_PrintValue(wrap(dataVec));
            //Rf_PrintValue(dataNew);
            //Rf_PrintValue(wrap(allocLen));
            //Rf_PrintValue(wrap(this->allocLen));
            // check size, grow as needed
            sizeNew = dataNew.size();
            if ( sizeNew > this->allocLen) {
                grow(sizeNew);
                // recompute offsets
                colStart = data.begin() + (icol * this->allocLen);
            }
            if ( sizeNew < thisLen) {
                // if results are shorter, zero out this row
                // not necessary, but prevents confusion viewing $data
                std::fill( colStart, colStart + this->allocLen, 0);
            }
            // fill row of return matrix, starting at first non-zero elem
            std::copy( dataNew.begin(), dataNew.end(), colStart);
            lengths[icol] = sizeNew;
        }
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
private:
    void grow(int minLen) {
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
};

