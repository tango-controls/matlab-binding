// ============================================================================
//
// = CONTEXT
//   TANGO binbing for Matlab
//
// = FILENAME
//   main.cpp
//
// = AUTHOR
//   Nicolas Leclercq - SOLEIL
//
// ============================================================================

//=============================================================================
// DEPENDENCIEs
//=============================================================================
#include "StandardHeader.h"

// ============================================================================
// mexFunction (the mex-file gateway routine)
// ----------------------------------------------------------------------------
// nlhs...Number of left hand side (output) arguments
// plhs...Array of left hand side arguments
// nrhs...Number of right hand side (input) arguments
// prhs...Array of right hand side arguments
// ============================================================================

extern "C" {
 void mexFunction (int nlhs, mxArray **, int nrhs, const mxArray **);
}

void mexFunction (int nlhs, mxArray ** plhs, int nrhs, const mxArray ** prhs)
{
  MexFile::exec(nlhs, plhs, nrhs, const_cast<mxArray**>(prhs));     
}




  
