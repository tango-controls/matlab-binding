// ============================================================================
//
// = CONTEXT
//   Tango Generic Client for Matlab
//
// = FILENAME
//   MexArgs.h
//
// = AUTHOR
//   Nicolas Leclercq - SOLEIL
//
// ============================================================================

#ifndef _MEX_ARGS_H_
#define _MEX_ARGS_H_

//=============================================================================
// ARG-ID
//=============================================================================
enum {
  k1ST_ARG = 0,
  k2ND_ARG,
  k3RD_ARG,
  k4TH_ARG,
  k5TH_ARG,
  k6TH_ARG,
  k7TH_ARG,
  k8TH_ARG,
  k9TH_ARG
};

//=============================================================================
// MISC. #DEFINEs
//=============================================================================
#define kMX_ANY -1

//=============================================================================
// CLASS : MexArgs (SINGLETON)
//=============================================================================
class MexArgs
{
  friend class MexFile;

public: 
  MexArgs();
  // Ctor.

  virtual ~MexArgs();
  // Dtor.

  int nlhs (void) const;
  // Returns the number of output args of the mex function.

  mxArray ** plhs (void) const;
  // Returns the output args of the mex function.

  int nrhs (void) const;
  // Returns the number of input args of the mex function.

  mxArray ** prhs (void) const;
  // Returns the input args of the mex function.

  int is_string (int arg_pos, int set_error = 1) const;
  // Returns 1 if the <arg_pos>th input argument of the mex-file
  // is a string, 0 otherwise.  If <set_error> is set to 1 (the 
  // default) a Tango::DevError is pushed into the global error 
  // stack (see MexUtils.h).

  int get_input_string (int arg_pos, std::string& str, int set_error = 1) const;
  // Uppon return, <str> contains the string passed as the [arg_pos]th 
  // input argument of the mex-file. This function returns -1 on error 
  // (in this case <str> is invalid/undefined), 0 otherwise. <arg_pos> 
  // is the position of the string (starting from 0) in the mex-file 
  // argument list. If <set_error> is set to 1 (the default) a 
  // Tango::DevError is pushed into the global error stack (see 
  // MexUtils.h).

  mxArray * get_input_array (int arg_pos,
                             int mx_class = kMX_ANY,
                             int mx_m = kMX_ANY,
                             int mx_n = kMX_ANY,
                             int set_error = 1) const;
  // Returns the <arg_pos>th input argument of the mex-file. If 
  // <set_error> is set to 1 (the default) a Tango::DevError is 
  // pushed into the global error stack (see MexUtils.h).

  int set_output_array (int arg_pos, mxArray * array, int set_error = 1);
  // Set the <arg_pos>th output argument of the mex-file. If <set_error> 
  // is set to 1 (the default) a Tango::DevError is pushed into the global 
  // error stack (see MexUtils.h). This function returns -1 on error, 0 
  // otherwise. 

  mxArray * default_prhs (double content = -1) const;
  // Returns the <default plhs>. 

private:
  void set (int nlhs, mxArray * plhs[], int nrhs, mxArray * prhs[]);
  // Store all mex-file args.

  int nlhs_;
  // Number of output args. 

  mxArray ** plhs_;
  // Output args.

  int nrhs_;
  // Number of input args. 

  mxArray ** prhs_;
  // Input args.

  // = Disallow these operations (except for friend function).
  //---------------------------------------------------------
  MexArgs (const MexArgs&);
  void operator= (const MexArgs&); 
};

//=============================================================================
// INLINED CODE
//=============================================================================
#if defined (_MEX_INLINE_)
  #include "MexArgs.i"
#endif 

#endif // _MEX_ARGS_H_

   
   
