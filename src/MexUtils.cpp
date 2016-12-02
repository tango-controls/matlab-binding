// ============================================================================
//
// = CONTEXT
//   Tango Generic Client for Matlab
//
// = FILENAME
//   MexUtils.cpp
//
// = AUTHOR
//   Nicolas Leclercq - SOLEIL
//
// ============================================================================

//=============================================================================
// DEPENDENCIEs
//=============================================================================
#include <cstdarg>
#include "StandardHeader.h"

//=============================================================================
// INLINED CODE
//=============================================================================
#if !defined (_MEX_INLINE_)
  #include "MexUtils.i"
#endif 

//=============================================================================
// WORKAROUND FOR OLDER MATLAB VERSION
//=============================================================================
#if defined (_HAS_NO_MWSIZE_TYPE_)
typedef int mwSize; 
#endif 

//=============================================================================
// #DEFINEs                                                 
//=============================================================================
#if defined (_MEX_DEBUG_)
# define INLINE_BUF_SIZE 1024
#else
# define INLINE_BUF_SIZE  512
#endif 

//=============================================================================
// STATIC MEMBERS
//=============================================================================
MexUtils* MexUtils::instance_ = 0;  

//=============================================================================
// MexUtils::init
//=============================================================================
int MexUtils::init (void)
{ 
  if (MexUtils::instance_ != 0)
    return 0;

  MexUtils::instance_ = new MexUtils;

  return (MexUtils::instance_) ? 0 : -1;
}

//=============================================================================
// MexUtils::cleanup
//=============================================================================
void MexUtils::cleanup (void)
{

}

//=============================================================================
// MexUtils::MexUtils                                                
//=============================================================================
MexUtils::MexUtils () : global_err_code_(0)
{

}

//=============================================================================
// MexUtils::~MexUtils                                                
//=============================================================================
MexUtils::~MexUtils (void)
{

}

//=============================================================================
// MexUtils::notify                                                 
//=============================================================================
void MexUtils::notify (const char *format, ...)
{
  static char notify_str[INLINE_BUF_SIZE];

	char *buf = notify_str;

  size_t fmtlen = ::strlen(format) + 1;

  if (fmtlen > INLINE_BUF_SIZE)  
    buf = new char[fmtlen];
    
  va_list argptr;
  va_start(argptr, format);
    int len = ::vsprintf(buf, format, argptr);
  va_end(argptr);

	if (len > 0) {
    ::mexPrintf(buf);
	} 
	
  if (buf != notify_str) {  
    delete[] buf;
  }
}

//=============================================================================
// MexUtils::set_global_error_code                                       
//=============================================================================
int MexUtils::set_global_error_code (int _err_code)
{
  global_err_code_ = _err_code;
  return kNoError;
}

//=============================================================================
// MexUtils::set_error                                       
//=============================================================================
void MexUtils::set_error (const Tango::DevFailed &e)
{
  this->reset_error();

  this->set_global_error_code();

  Tango::DevLong nb_err = e.errors.length();

  this->error_.errors.length(nb_err);

  for (int i = 0; i < nb_err; i++) 
  {
    this->error_.errors[i].severity = e.errors[i].severity;
    this->error_.errors[i].reason   = CORBA::string_dup(e.errors[i].reason.in());
    this->error_.errors[i].desc     = CORBA::string_dup(e.errors[i].desc.in());
    this->error_.errors[i].origin   = CORBA::string_dup(e.errors[i].origin.in());
  }
}

//=============================================================================
// MexUtils::set_error                                       
//=============================================================================
void MexUtils::set_error (const Tango::DevErrorList& e)
{
  this->reset_error();

  this->set_global_error_code();

  Tango::DevLong nb_err = e.length();

  this->error_.errors.length(nb_err);

  for (int i = 0; i < nb_err; i++) 
  {
    this->error_.errors[i].severity = e[i].severity;
    this->error_.errors[i].reason   = CORBA::string_dup(e[i].reason.in());
    this->error_.errors[i].desc     = CORBA::string_dup(e[i].desc.in());
    this->error_.errors[i].origin   = CORBA::string_dup(e[i].origin.in());
  }
}

//=============================================================================
// MexUtils::set_error                                       
//=============================================================================
void MexUtils::set_error (const char *_r, 
                          const char *_d, 
                          const char *_o,
                          Tango::ErrSeverity _s) 
{
  this->reset_error();

  this->set_global_error_code();

  this->error_.errors.length(1);

  this->error_.errors[0].severity = _s;
  this->error_.errors[0].reason   = CORBA::string_dup(_r);
  this->error_.errors[0].desc     = CORBA::string_dup(_d);
  this->error_.errors[0].origin   = CORBA::string_dup(_o);
}

//=============================================================================
// MexUtils::push_error                                       
//=============================================================================
void MexUtils::push_error (const char *_r, 
                           const char *_d, 
                           const char *_o,
                           Tango::ErrSeverity _s) 
{
  this->set_global_error_code();

  Tango::DevLong nerr = this->error_.errors.length();

  this->error_.errors.length(nerr + 1);

  this->error_.errors[nerr].severity = _s;
  this->error_.errors[nerr].reason   = CORBA::string_dup(_r);
  this->error_.errors[nerr].desc     = CORBA::string_dup(_d);
  this->error_.errors[nerr].origin   = CORBA::string_dup(_o);
}

//=============================================================================
// MexUtils::push_error                                       
//=============================================================================
void MexUtils::push_error (const Tango::DevFailed &e)
{
  this->set_global_error_code();

  Tango::DevLong add_nb_err = e.errors.length();

  Tango::DevLong cur_nb_err = this->error_.errors.length();

  Tango::DevLong total_err = cur_nb_err + add_nb_err;

  this->error_.errors.length(total_err);

  int i, j;
  for (i = cur_nb_err, j = 0; i < total_err; i++, j++) 
  {
    this->error_.errors[i].severity = e.errors[j].severity;
    this->error_.errors[i].reason   = CORBA::string_dup(e.errors[j].reason.in());
    this->error_.errors[i].desc     = CORBA::string_dup(e.errors[j].desc.in());
    this->error_.errors[i].origin   = CORBA::string_dup(e.errors[j].origin.in());
  }
}

//=============================================================================
//  MexUtils::push_error                                       
//=============================================================================
void  MexUtils::push_error (const Tango::DevErrorList& e)
{
  this->set_global_error_code();

  Tango::DevLong add_nb_err = e.length();

  Tango::DevLong cur_nb_err = this->error_.errors.length();

  Tango::DevLong total_err = cur_nb_err + add_nb_err;

  this->error_.errors.length(total_err);

  int i, j;
  for (i = cur_nb_err, j = 0; i < total_err; i++, j++)
  {
    this->error_.errors[i].severity = e[j].severity;
    this->error_.errors[i].reason   = CORBA::string_dup(e[j].reason.in());
    this->error_.errors[i].desc     = CORBA::string_dup(e[j].desc.in());
    this->error_.errors[i].origin   = CORBA::string_dup(e[j].origin.in());
  }
}

//=============================================================================
// MexUtils::error_code                                       
//=============================================================================
int MexUtils::error_code (void)
{
  mxArray* mxa = ::mxCreateDoubleMatrix(1, 1, ::mxREAL);
  if (mxa == 0) 
  {
    MEX_ERROR(("mxCreateDoubleMatrix failed - could not export TANGO error code"));
    return kError;
  }

  ::mxGetPr(mxa)[0] = global_err_code_;

  if (MEX_ARGS->set_output_array(0, mxa) == kError)
    return kError;

  return kNoError;
}

//=============================================================================
// MexUtils::error_stack                                    
//=============================================================================
int MexUtils::error_stack (void)
{
  mxArray *struct_array = 0;

  if (error_stack_to_mxarray(this->error_.errors, struct_array) != kNoError)
  {
    MEX_ERROR(("could not export TANGO error stack"));
    return kError;
  }

  if (MEX_ARGS->set_output_array(0, struct_array) == kError)
    return kError;

  return kNoError;
}


//=============================================================================
// MexUtils::error_stack                                    
//=============================================================================
int MexUtils::error_stack_to_mxarray (const Tango::DevErrorList& es, mxArray*& mx_array)
{
  // get number of errors in the stack
  Tango::DevLong nerr = es.length();

  // create a 1-by-n array of structs (each containing 4 fields)
  const mwSize dims[2] = {1, static_cast<mwSize>(nerr)};
  const char *field_names[] = {"reason", "desc", "origin", "severity", "severity_str"};
  mx_array = ::mxCreateStructArray(2, dims, 5, field_names);
  if (mx_array == 0) 
  {
    MEX_ERROR(("mxCreateStructArray failed - could not export TANGO error stack"));
    return kError;
  }

  //- populate the array
  mxArray *severity_value;
  for (int i = 0; i < nerr; i++) 
  {
    // set each string field of the ith struct
    // note: the field number indices are zero based (reason->0, desc->1, ...)
    ::mxSetFieldByNumber(mx_array, i, 0, ::mxCreateString(es[i].reason.in()));
    ::mxSetFieldByNumber(mx_array, i, 1, ::mxCreateString(es[i].desc.in()));
    ::mxSetFieldByNumber(mx_array, i, 2, ::mxCreateString(es[i].origin.in()));
    // set the severity field 
    severity_value = ::mxCreateDoubleMatrix(1,1,mxREAL);
    ::mxGetPr(severity_value)[0] = es[i].severity;
    ::mxSetFieldByNumber(mx_array, i, 3, severity_value);
    // set the severity_str field 
    switch (es[i].severity) 
    {
      case Tango::WARN:
        ::mxSetFieldByNumber(mx_array, i, 4, ::mxCreateString("Warning"));
        break;
      case Tango::ERR:
        ::mxSetFieldByNumber(mx_array, i, 4, ::mxCreateString("Error"));
        break;
      case Tango::PANIC:
        ::mxSetFieldByNumber(mx_array, i, 4, ::mxCreateString("Panic"));
        break;
      default:
        ::mxSetFieldByNumber(mx_array, i, 4, ::mxCreateString("Unknown"));
        break;
    }
  }
  return kNoError;
}
