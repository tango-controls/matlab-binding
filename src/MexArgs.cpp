// ============================================================================
//
// = CONTEXT
//   Tango Generic Client for Matlab
//
// = FILENAME
//   MexArgs.cpp
//
// = AUTHOR
//   Nicolas Leclercq - SOLEIL 
//
// ============================================================================

//=============================================================================
// DEPENDENCIES
//=============================================================================
#include "StandardHeader.h"
#include "MexArgs.h"

//=============================================================================
// INLINE CODE
//=============================================================================
#if !defined (_MEX_INLINE_)
  #include "MexArgs.i"
#endif 

//=============================================================================
// LOCAL CONSTs
//=============================================================================
const char* lk_class_names[] = 
  {
    "unknown",
    "cell",
    "struct",
    "object",
    "char",
    "sparse",
    "double",
    "single",
    "int8",
    "uint8", 
    "int16",
    "uint16",
    "int32",
    "uint32",
    "int64"
    "uint64"
    "function",
    "opaque"
};

//=============================================================================
// MexArgs::MexArgs 
//=============================================================================
MexArgs::MexArgs (void) 
  : nlhs_(0), plhs_(0), nrhs_(0), prhs_(0)
{
  //- noop
}

//=============================================================================
// MexArgs::~MexArgs 
//=============================================================================
MexArgs::~MexArgs (void)
{
  //- noop
}

//=============================================================================
// MexArgs::default_prhs
//=============================================================================
mxArray * MexArgs::default_prhs (double content) const
{
  mxArray * default_prhs = ::mxCreateDoubleMatrix(1, 1, mxREAL);

  if (default_prhs)
    ::mxGetPr(default_prhs)[0] = content;

  return default_prhs; 
}

//=============================================================================
// MexArgs::is_string 
//=============================================================================
int MexArgs::is_string (int _arg_pos, int _set_error) const
{
  // check input.
  if (_arg_pos < 0 || _arg_pos >= this->nrhs_) 
  {
    if (_set_error) 
    {
      MEX_UTILS->set_error("internal error", 
                           "invalid argument position specified", 
                           "MexArgs::is_string");
    }
    return 0;
  }

  // is the second input arg a string?
  if (::mxIsChar(this->prhs_[_arg_pos]) == false) 
  {
    if (_set_error) 
    {
      TangoSys_OMemStream o;
      o << "input argument #" << _arg_pos << " must be a string" << std::ends;
      MEX_UTILS->set_error("invalid argument specified", 
                           o.str(), 
                           "MexArgs::is_string");
    }
    return 0;
  }

  // ok, input arg is a string.
  return 1;
}

//=============================================================================
// MexArgs::get_input_string
//=============================================================================
int MexArgs::get_input_string (int _arg_pos, std::string& str_, int _set_error) const
{
  // is the second input arg a string?
  if (this->is_string(_arg_pos, _set_error) == 0)
    return kError;

  // get c-string from matlab array
  char * cstr = ::mxArrayToString(this->prhs_[_arg_pos]);
  if (cstr == 0)
  {
    if (_set_error) 
    {
      MEX_UTILS->set_error("internal error", 
                           "out of memory", 
                           "MexArgs::get_input_string");
    }
    return kError;
  }

  // set output value.
  str_ = cstr;

  // release cstr
  ::mxFree(cstr);

  return kNoError;
}

//=============================================================================
// MexArgs::get_input_array
//=============================================================================
const mxArray* MexArgs::get_input_array(int _arg_pos, 
                                         int _mx_class, 
                                         int _mx_m, 
                                         int _mx_n, 
                                         int _set_error) const
{
  if (_arg_pos < 0 || _arg_pos >= this->nrhs_) 
  {
    if (_set_error) 
    {
      MEX_UTILS->set_error("internal error", 
                           "invalid argument position specified", 
                           "MexArgs::get_input_array");
    }
    return 0;
  }

  const mxArray* mx_array = this->prhs_[_arg_pos];

  if (   mx_array == 0
      ||
         (_mx_class != kMX_ANY && ::mxGetClassID(mx_array) != _mx_class)
      ||
         (_mx_m != kMX_ANY && ::mxGetM(mx_array) != _mx_m)
      ||
         (_mx_n != kMX_ANY && ::mxGetN(mx_array) != _mx_n)
    )
  {
    if (_set_error) 
    {
      TangoSys_OMemStream o;

      if (_mx_m == kMX_ANY)
        o << "m";
      else
        o << _mx_m;
      o << "-by-";

      if (_mx_n == kMX_ANY)
        o << "n";
      else 
        o << _mx_n;

      o << " " 
        << ((_mx_class != kMX_ANY) ? lk_class_names[_mx_class] : "")
        << " array expected" 
        << std::ends;

      MEX_UTILS->set_error("invalid argument specified", 
                           o.str(), 
                           "MexArgs::get_input_array");
    }

    return 0;
  }

  return mx_array;
}

//=============================================================================
// MexArgs::set_output_array
//=============================================================================
int MexArgs::set_output_array (int _arg_pos, mxArray* _array, int _set_error)
{
  if (_arg_pos < 0 || _arg_pos >= this->nlhs_) 
  {
    if (_set_error) 
    {
      MEX_UTILS->set_error("internal error", 
                           "invalid argument position specified", 
                           "MexArgs::set_output_array");
    }
    return kError;
  }

  this->plhs_[_arg_pos] = _array;

  return kNoError;
}
