// rcpp_module.cpp: Rcpp R/C++ interface class library -- Rcpp Module examples
// Copyright (C) 2010 - 2012  Dirk Eddelbuettel and Romain Francois
//
// This file is part of Rcpp.
//
// Rcpp is free software: you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// Rcpp is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Rcpp.  If not, see <http://www.gnu.org/licenses/>.

#include <RcppArmadillo.h>
#include "classdef.cpp"

RCPP_MODULE(mod_ragged){
    //using namespace Rcpp ;
    class_<RaggedArray>("RaggedArray")
    // expose the default constructor
    .constructor<int, int, int>()
    // take the list returned by serialize()
    .constructor<List>()
    .field("growBy", &RaggedArray::growBy, "Writable: Number of rows to add when space is required")
    // read only properties
    .property("nvec", &RaggedArray::get_nvec, "Read-only: number of vectors (as columns)")
    .property("lengths", &RaggedArray::get_lengths, "Read-only: Integer vector of lengths")
    .property("data", &RaggedArray::get_data, "Read-only: Numeric matrix of data.")

    .method("append", &RaggedArray::append , "Append a list of data vectors")
    .method("sapply", &RaggedArray::sapply , "Apply function to each col of data, modify in-place.  Dim of function arg and return value must match")
    .method("sapplyAlloc", &RaggedArray::sapplyAlloc , "Apply function to each col of data.  Reallocate, update lengths.")
    .method("sapplyC", &RaggedArray::sapplyC , "Apply user-supplied c++ function to each col of data.  Dim of function arg and return value must match")
    .method("sapplyAllocC", &RaggedArray::sapplyAllocC , "Apply user-supplied c++ function to each col of data.  Reallocate, update lengths")
    .method("mapply", &RaggedArray::mapply , "Apply function to each col of data, modify in-place.  Takes numeric Dim of function arg and return value must match")
    .method("mapplyAlloc", &RaggedArray::mapplyAlloc , "As in sapplyAlloc, multivariate version (see mapply).")
    .method("mapplyC", &RaggedArray::mapplyC , "As in sapplyAlloc, multivariate version (see mapply).")
    .method("mapplyAllocC", &RaggedArray::mapplyAllocC , "As in sapplyAlloc, multivariate version (see mapply).")
    .method("serialize", &RaggedArray::serialize , "Return RaggedArray as an R list.  Can be passed back to constructor.")
    .method("dataList", &RaggedArray::dataList , "Return data field as R list, one element per column of data, with element i having length = obj$lengths[i]. For use with vanilla R sapply.")
    ;
}
