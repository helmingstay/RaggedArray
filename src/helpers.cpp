#include <Rcpp.h>
#include <Rmath.h>

using namespace Rcpp ;

class RaggedArray {
private:
    int nvec, allocLen;
    IntegerVector lengths;
    NumericMatrix data;
public:
    int growBy;
    // getters
    int get_growBy() { return growBy; }
    int get_nvec() { return nvec; }
    IntegerVector get_lengths() { return lengths; }
    NumericMatrix get_data() { return data; }
    // Constructors
    // construct empty obj
    RaggedArray(int  nvec_, int allocLen_, int growBy_) : nvec(nvec_), allocLen(allocLen_), growBy(growBy_), data(allocLen, nvec), lengths(nvec){
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

    void append(  List fillVecs) {
        // "append" fill oldmat w/  
        // we will loop through rows, filling retmat in with the vectors in list
        // then update retmat_size to index the next free
        // newLenths isn't used, added for compatibility
        int sizeOld, sizeAdd, sizeNew;
        NumericVector fillTmp;
        // check that number of vectors match
        if ( nvec != fillVecs.size()) {
            throw std::range_error("In append(): dimension mismatch");
        }
        for (int ii = 0; ii<nvec; ii++){
            // vector to append
            fillTmp = fillVecs[ii];
            // compute lengths
            sizeOld = lengths[ii];
            sizeAdd = fillTmp.size();
            sizeNew = sizeOld + sizeAdd;
            // grow data matrix as needed
            if ( sizeNew >= allocLen) {
                grow();
            }
            // iterator for row to fill
            NumericMatrix::Column dataCol = data(_, ii);
            // fill row of return matrix, starting at first non-zero elem
            std::copy( fillTmp.begin(), fillTmp.end(), dataCol.begin() + sizeOld);
            // update size of retmat
            lengths[ii] = sizeNew;
        }
    }
private:
    void grow() {
        int end;
        int newLen = allocLen+growBy;
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

