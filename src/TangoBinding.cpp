//- ============================================================================
//
//- = CONTEXT
//-   Tango Generic Client for Matlab
//
//- = FILENAME
//-   TangoBinding.cpp
//
//- = AUTHOR
//-   Nicolas Leclercq - SOLEIL
//
//- ============================================================================

//-----------------------------------------------------------------------------
//- WINDOWS PRAGMA
//-----------------------------------------------------------------------------
#if defined(_WINDOWS)
# pragma warning(push)
# pragma warning(disable:4786)
# pragma warning(disable:4800)
#endif

//=============================================================================
//- DEPENDENCIES
//=============================================================================
#include <algorithm>
#include <math.h>
#include "StandardHeader.h"
#include "GroupRepository.h"
#include "DevRepository.h"
#include "DataAdapter.h"
#include "TangoBinding.h"

#if !defined (_MEX_INLINE_)
  #include "TangoBinding.i" 
#endif 

//=============================================================================
//- WORKAROUND FOR OLDER MATLAB VERSION
//=============================================================================
#if defined (_HAS_NO_MWSIZE_TYPE_)
typedef int mwSize; 
#endif 

//=============================================================================
//- STATIC MEMBERS
//=============================================================================
TangoBinding * TangoBinding::instance_ = 0;  

//=============================================================================
//- LOCAL CONST
//=============================================================================
//- time reference: datenum(1970,1,1,0,0,0) 
const double lk_time_reference = 719529; 

//- time factor: datenum(0,0,0,0,0,1)    
const double lk_time_factor = 1.157407407407407E-5; 

//- TANGO types <-> Matlab types 
const char * const lk_matlab_type_name[] = 
{
  "-",              //- DEV_VOID
#if defined (MAP_DEV_BOOLEAN_TO_MATLAB_LOGICAL)
  "1-by-1 logical", //- DEVVAR_BOOLEANARRAY
#else
  "1-by-1 uint8",   //- DEVVAR_BOOLEANARRAY
#endif
  "1-by-1 int16",   //- DEV_SHORT
  "1-by-1 int32",   //- DEV_LONG
  "1-by-1 single",  //- DEV_FLOAT
  "1-by-1 double",  //- DEV_DOUBLE
  "1-by-1 uint16",  //- DEV_USHORT
  "1-by-1 uint32",  //- DEV_ULONG
  "1-by-n char",    //- DEV_STRING
  "1-by-n char",    //- DEVVAR_CHARARRAY
  "1-by-n int16",   //- DEVVAR_SHORTARRAY
  "1-by-n int32",   //- DEVVAR_LONGARRAY
  "1-by-n single",  //- DEVVAR_FLOATARRAY
  "1-by-n double",  //- DEVVAR_DOUBLEARRAY
  "1-by-n uint16",  //- DEVVAR_USHORTARRAY
  "1-by-n uint32",  //- DEVVAR_ULONGARRAY
  "1-by-n cell{1-by-n char}", //- DEVVAR_STRINGARRAY
  "struct[lvalue: 1-by-n int32,  svalue: 1-by-n cell{1-by-n char}]", //- DEVVAR_LONGSTRINGARRAY
  "struct[lvalue: 1-by-n int32,  svalue: 1-by-n cell{1-by-n char}]", //- DEVVAR_DOUBLESTRINGARRAY
  "1-by-n char",    //- DEV_STATE
  "1-by-n char",    //- CONST_DEV_STRING
#if defined (MAP_DEV_BOOLEAN_TO_MATLAB_LOGICAL)
  "1-by-n logical", //- DEVVAR_BOOLEANARRAY
#else
  "1-by-n uint8",   //- DEVVAR_BOOLEANARRAY
#endif
  "1-by-1 uchar",   //- DEV_UCHAR
  "1-by-1 int64",   //- DEV_LONG64
  "1-by-1 uint64",  //- DEV_ULONG64
  "1-by-n int64",   //- DEVVAR_LONG64ARRAY,
  "1-by-n uint64",  //- DEVVAR_ULONG64ARRAY
  "1-by-1 int32",   //- DEV_INT
  "not supported"   //- UNKNOWN/UNSUPPORTED TYPE
};

//=============================================================================
//- MACROS
//=============================================================================
//-- BEGIN _TRY MACRO ---------------------------------------
#define _TRY(_invoke, _dev_or_group, _cmd) \
  try { \
     _invoke; \
  } \
  catch (const Tango::AsynReplyNotArrived &ae) { \
    MEX_UTILS->set_error(ae); \
    std::string r = "no reply for asynchronous request";\
    std::string d = "no reply received for the specified asynchronous request";\
    std::string o = "TangoBinding::" + std::string(_cmd); \
    MEX_UTILS->push_error(r.c_str(), d.c_str(), o.c_str()); \
                SET_DEFAULT_PRHS_THEN_RETURN(kError); \
        } \
  catch (const Tango::DevFailed &dv) { \
    MEX_UTILS->set_error(dv); \
    std::string r = std::string(_cmd) + " failed"; \
    std::string d = "failed to execute " + std::string(_cmd); \
    d += " on " + _dev_or_group; \
    std::string o = "TangoBinding::" + std::string(_cmd); \
    MEX_UTILS->push_error(r.c_str(), d.c_str(), o.c_str()); \
                SET_DEFAULT_PRHS_THEN_RETURN(kError); \
        } \
  catch (...) { \
    std::string o = "TangoBinding::" + std::string(_cmd); \
    MEX_UTILS->set_error("unknown error", \
                         "unknown exception caught", \
                         o.c_str()); \
    SET_DEFAULT_PRHS_THEN_RETURN(kError); \
  } 
//-- END _TRY MACRO -----------------------------------------

//-- BEGIN _TRY_DELETE MACRO --------------------------------
#define _TRY_DELETE(_invoke, _dev_or_group, _cmd, _obj) \
  try { \
     _invoke; \
  } \
  catch (const Tango::AsynReplyNotArrived &ae) { \
    MEX_UTILS->set_error(ae); \
    std::string r = "no reply for asynchronous request";\
    std::string d = "no reply received for the specified asynchronous request";\
    std::string o = "TangoBinding::" + std::string(_cmd); \
    MEX_UTILS->push_error(r.c_str(), d.c_str(), o.c_str()); \
    if (_obj) delete _obj; \
                SET_DEFAULT_PRHS_THEN_RETURN(kError); \
        } \
  catch (const Tango::DevFailed &dv) { \
    MEX_UTILS->set_error(dv); \
    std::string r = std::string(_cmd) + " failed"; \
    std::string d = "failed to execute " + std::string(_cmd); \
    d += " on " + _dev_or_group; \
    std::string o = "TangoBinding::" + std::string(_cmd); \
    MEX_UTILS->push_error(r.c_str(), d.c_str(), o.c_str()); \
    if (_obj) delete _obj; \
                SET_DEFAULT_PRHS_THEN_RETURN(kError); \
        } \
  catch (...) { \
    std::string o = "TangoBinding::" + std::string(_cmd); \
    MEX_UTILS->set_error("unknown error", \
                         "unknown exception caught", \
                         o.c_str()); \
    if (_obj) delete _obj; \
    SET_DEFAULT_PRHS_THEN_RETURN(kError); \
  }
//-- END _TRY_DELETE MACRO ----------------------------------

//=============================================================================
//- TangoBinding::init
//=============================================================================
int TangoBinding::init (void)
{
  if (TangoBinding::instance_)
    return kNoError;

  if (DevRepository::init() == kError)
    return kError;

  if (GroupRepository::init() == kError)
    return kError;

  if (DataAdapter::init() == kError)
    return kError;

  TangoBinding::instance_ = new TangoBinding;

  return (TangoBinding::instance_) ? kNoError : kError;
}

//=============================================================================
//- TangoBinding::cleanup
//=============================================================================
void TangoBinding::cleanup (void)
{
  DataAdapter::cleanup();

  GroupRepository::cleanup();

  DevRepository::cleanup();

  if (TangoBinding::instance_) 
  {
    delete TangoBinding::instance_;
    TangoBinding::instance_ = 0;
  }  
}

//=============================================================================
//- TangoBinding::TangoBinding 
//=============================================================================
TangoBinding::TangoBinding (void)
  : v1_compatibility_enabled_(true)
{
 //- noop ctor
}

//=============================================================================
//- TangoBinding::~TangoBinding 
//=============================================================================
TangoBinding::~TangoBinding (void)
{
 //- noop dtor
}

//=============================================================================
//- TangoBinding::tango_version
//=============================================================================
int TangoBinding::tango_version (void)
{
  std::ostringstream oss;
  oss << TANGO_VERSION_MAJOR
      << "."
      << TANGO_VERSION_MINOR
      << "."
      << TANGO_VERSION_PATCH;
  
  //- create a 1-by-n char array
  mxArray * mx_tango_version = ::mxCreateString(oss.str().c_str());
  if (mx_tango_version == 0)
  {
    MEX_UTILS->set_error("out of memory",
                         "mxCreateString failed",
                         "TangoBinding::tango_version");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }

  //- set output arg
  if (MEX_ARGS->set_output_array(k1ST_ARG, mx_tango_version) == kError) 
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 

  return kNoError;
}

//=============================================================================
//- TangoBinding::open_device 
//=============================================================================
int TangoBinding::open_device (void)
{
  //- get device name from the 2nd input arg
  std::string dev;
  if (MEX_ARGS->get_input_string(k2ND_ARG, dev) == kError)
    SET_DEFAULT_PRHS_THEN_RETURN(kError);

  //- get device descriptor (proxy)
  DeviceDesc * ddesc = DEV_REP->device_desc(dev);
  if (! ddesc) 
  {
    std::string r = "failed to open device " + dev;
    MEX_UTILS->push_error(r.c_str(),
                          "could not obtain a valid device reference",
                          "TangoBinding::open_device");
    SET_DEFAULT_PRHS_THEN_RETURN(kError);
  }

  return kNoError;
}

//=============================================================================
//- TangoBinding::close_device 
//=============================================================================
int TangoBinding::close_device (void)
{
  //- get device name from the 2nd input arg
  std::string dev;
  if (MEX_ARGS->get_input_string(k2ND_ARG, dev) == kError)
    SET_DEFAULT_PRHS_THEN_RETURN(kError);
 
  //- release device descriptor
  if (DEV_REP->remove_device(dev) == kError) 
  {
    std::string d = "failed to close device " + dev;
    MEX_UTILS->push_error("device not found",
                          d.c_str(),
                          "TangoBinding::close_device");
    SET_DEFAULT_PRHS_THEN_RETURN(kError);
  }

  return kNoError;
}

//=============================================================================
//- TangoBinding::command_inout 
//=============================================================================
int TangoBinding::command_inout (void)
{
  //- get device name from the 2nd input arg
  std::string dev;
  if (MEX_ARGS->get_input_string(k2ND_ARG, dev) == kError)
    SET_DEFAULT_PRHS_THEN_RETURN(kError);

  //- get command name from the 3rd input arg
  std::string cmd;
  if (MEX_ARGS->get_input_string(k3RD_ARG, cmd) == kError)
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 

  //- get device descriptor
  DeviceDesc * ddesc = DEV_REP->device_desc(dev);
  if (! ddesc) 
  {
    std::string r = "failed to execute " + cmd + " on " + dev;
    MEX_UTILS->push_error(r.c_str(),
                          "could not obtain a valid device reference",
                          "TangoBinding::command_inout");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }

  //- is <cmd> a valid command for <dev>
  int cmd_id = ddesc->cmd_exists(cmd);
  if (cmd_id == kError) 
  {
    std::string d = cmd + " is not a valid " + dev + " command";
    MEX_UTILS->set_error("command not found",
                         d.c_str(),
                         "TangoBinding::command_inout");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }

  //- get <argin> type for <cmd>
  int arg_in_type = (ddesc->cmd_list())[cmd_id].in_type;

  //- <cmd> result  
  Tango::DeviceData dd_out;

  //- exec <cmd> on <dev>
  if (arg_in_type != Tango::DEV_VOID) 
  {
    //- get <argin> from the 4th input arg
    const mxArray* arg_in = MEX_ARGS->get_input_array(k4TH_ARG);
    if (arg_in == 0) 
    {
      std::string r = "failed to execute " + cmd + " on " + dev;
      MEX_UTILS->push_error(r.c_str(),
                            "argin is missing",
                            "TangoBinding::command_inout");
      SET_DEFAULT_PRHS_THEN_RETURN(kError); 
    }

    //- encode argin (convert from Matlab to TANGO type)
    Tango::DeviceData dd_in;
    if (DATA_ADAPTER->encode_argin(ddesc, cmd_id, arg_in, dd_in) == kError)
      SET_DEFAULT_PRHS_THEN_RETURN(kError); 

    //- exec command
    _TRY(dd_out = ddesc->proxy()->command_inout(cmd, dd_in), dev, "command_inout");
  }
  else 
  {
    //- argin = void
    _TRY(dd_out = ddesc->proxy()->command_inout(cmd), dev, "command_inout");
  }

  //- get <argout> type for <cmd>
  if ((ddesc->cmd_list())[cmd_id].out_type != Tango::DEV_VOID) 
  {
    //- decode argout
    mxArray * argout = 0;
    if (DATA_ADAPTER->decode_argout(ddesc, cmd_id, dd_out, argout) == kError)
    {
      //- release allocated memory
      if (argout) ::mxDestroyArray(argout);
      SET_DEFAULT_PRHS_THEN_RETURN(kError); 
    }

    //- set output arg
    if (MEX_ARGS->set_output_array(k1ST_ARG, argout) == kError)
    {
      //- release allocated memory
      ::mxDestroyArray(argout);
      SET_DEFAULT_PRHS_THEN_RETURN(kError); 
    }
  }
  else 
  {
    //- argout = void
    SET_DEFAULT_PRHS(0);  
  }

  return kNoError;
}

//=============================================================================
//- TangoBinding::command_inout_asynch 
//=============================================================================
int TangoBinding::command_inout_asynch (void)
{
  //- get <device> name from the 2nd input arg
  std::string dev;
  if (MEX_ARGS->get_input_string(k2ND_ARG, dev) == kError)
    SET_DEFAULT_PRHS_THEN_RETURN(kError);

  //- get <command name> from the 3rd input arg
  std::string cmd;
  if (MEX_ARGS->get_input_string(k3RD_ARG, cmd) == kError)
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 

  //- get <forget> option from the 4th input arg
  const mxArray* mx_forget = MEX_ARGS->get_input_array(k4TH_ARG, kMX_ANY, 1, 1);
  if (mx_forget == 0) 
  {
    std::string r = "failed to execute " + cmd + " on " + dev;
    MEX_UTILS->push_error(r.c_str(),
                          "<forget> option is either missing or has an invalid format",
                          "TangoBinding::command_inout_asynch");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  bool forget = ::mxGetPr(mx_forget)[0] ? true : false;

  //- get device descriptor
  DeviceDesc * ddesc = DEV_REP->device_desc(dev);
  if (! ddesc) 
  {
    std::string r = "failed to execute " + cmd + " on " + dev;
    MEX_UTILS->push_error(r.c_str(),
                          "could not obtain a valid device reference",
                          "TangoBinding::command_inout_asynch");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }

  //- is <cmd> a valid command for <dev>
  int cmd_id = ddesc->cmd_exists(cmd);
  if (cmd_id == kError) 
  {
    std::string d = cmd + " is not a valid " + dev + " command";
    MEX_UTILS->set_error("command not found",
                         d.c_str(),
                         "TangoBinding::command_inout_asynch");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }

  //- get <argin> type for <cmd>
  int arg_in_type = (ddesc->cmd_list())[cmd_id].in_type;

  //- asynch request identifier 
  Tango::DevLong req_id;

  //- exec the command 
  if (arg_in_type != Tango::DEV_VOID) 
  {
    //- get <argin> from the 5th input arg
    const mxArray* arg_in = MEX_ARGS->get_input_array(k5TH_ARG);
    if (arg_in == 0) 
    {
      std::string r = "failed to execute " + cmd + " on " + dev;
      MEX_UTILS->push_error(r.c_str(),
                            "argin is missing",
                            "TangoBinding::command_inout_asynch");
      SET_DEFAULT_PRHS_THEN_RETURN(kError); 
    }

    //- encode argin (convert from Matlab to TANGO type)
    Tango::DeviceData dd_in;
    if (DATA_ADAPTER->encode_argin(ddesc, cmd_id, arg_in, dd_in))
      SET_DEFAULT_PRHS_THEN_RETURN(kError); 

    //- exec command
    _TRY(req_id = ddesc->proxy()->command_inout_asynch(cmd, dd_in, forget), dev, "command_inout_asynch");
  }
  else 
  {
    //- argin = void
    _TRY(req_id = ddesc->proxy()->command_inout_asynch(cmd, forget), dev, "command_inout_asynch");
  }

  //- no reply expected if forget is set to true
  if (forget == true) 
  {
    SET_DEFAULT_PRHS(0); 
    return kNoError;
  }

  //- create a 1-by-1 struct array for <asynch request descriptor>
  const mwSize dims[2] = {1, 1};
  const char * field_names[] = {"dev_name", "cmd_id", "req_id"};
  mxArray * argout = ::mxCreateStructArray(2, dims, 3, field_names);
  if (! argout) 
  {
    MEX_UTILS->set_error("out of memory",
                         "mxCreateStructArray failed",
                         "TangoBinding::command_inout_asynch");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }

  //- argout.dev
  mxArray * mx_array = ::mxCreateString(dev.c_str());
  if (! mx_array) 
  {
    MEX_UTILS->set_error("out of memory",
                         "mxCreateString failed",
                         "TangoBinding::command_inout_asynch");
    //- release allocated memory then return error
    ::mxDestroyArray(argout);
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  ::mxSetFieldByNumber(argout, 0, 0, mx_array);

  //- argout.cmd_id
  mx_array = ::mxCreateNumericArray(2, dims, ::mxINT32_CLASS, ::mxREAL);
  if (! mx_array) 
  {
    MEX_UTILS->set_error("out of memory",
                         "mxCreateNumericArray failed",
                         "TangoBinding::command_inout_asynch");
    //- release allocated memory then return error
    ::mxDestroyArray(argout);
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }

  reinterpret_cast<Tango::DevLong*>(::mxGetPr(mx_array))[0] = cmd_id;
  ::mxSetFieldByNumber(argout, 0, 1, mx_array);

  //- argout.req_id
  mx_array = ::mxCreateNumericArray(2, dims, ::mxINT32_CLASS, ::mxREAL);
  if (! mx_array) 
  {
    MEX_UTILS->set_error("out of memory",
                         "mxCreateNumericArray failed",
                         "TangoBinding::command_inout_asynch");
    //- release allocated memory then return error
    ::mxDestroyArray(argout);
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  reinterpret_cast<Tango::DevLong*>(::mxGetPr(mx_array))[0] = req_id;
  ::mxSetFieldByNumber(argout, 0, 2, mx_array);

  //- set argout
  if (MEX_ARGS->set_output_array(k1ST_ARG, argout) == kError)
  {
    //- release allocated memory then return error
    ::mxDestroyArray(argout);
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }

  return kNoError;
}

//=============================================================================
//- TangoBinding::command_inout_reply
//=============================================================================
int TangoBinding::command_inout_reply (void)
{
  //- get <asynch request descriptor> from 2nd input arg
  const mxArray* mx_rdesc = MEX_ARGS->get_input_array(k2ND_ARG, ::mxSTRUCT_CLASS, 1, 1);
  if (mx_rdesc == 0) 
  {
    MEX_UTILS->push_error("invalid request descriptor specified",
                          "request descriptor is either missing or has an invalid format",
                          "TangoBinding::command_inout_reply");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }

  //- mx_rdesc struct should contain a <dev_name> field
  mxArray * mx_array = ::mxGetField(mx_rdesc, 0, "dev_name");
  if (! mx_array) 
  {
    MEX_UTILS->set_error("invalid argin specified",
                         "struct should contain a field named 'dev_name'",
                         "TangoBinding::command_inout_reply");
    SET_DEFAULT_PRHS_THEN_RETURN(kError);                 
  }

  //- <dev_name> must be 1-by-n char array
  if (::mxIsChar(mx_array) == false || ::mxGetM(mx_array) != 1) 
  {
    MEX_UTILS->set_error("invalid request descriptor specified",
                         "1-by-n char array expected for field 'dev_name'",
                         "TangoBinding::command_inout_reply");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }

  //- extract <dev_name> from mxArray
  char * dev_name_cstr = ::mxArrayToString(mx_array);
  if (dev_name_cstr == 0) 
  {
    MEX_UTILS->set_error("invalid request descriptor specified",
                         "could not extract string from field",
                         "TangoBinding::command_inout_reply");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }

  //- convert from C to C++ string then release memory
  std::string dev_name(dev_name_cstr);
  ::mxFree(dev_name_cstr);

  //- mx_rdesc struct should contain a <cmd_id> field
  mx_array = ::mxGetField(mx_rdesc, 0, "cmd_id");
  if (! mx_array) 
  {
    MEX_UTILS->set_error("invalid request descriptor specified",
                         "struct should contain a field named 'cmd_id'",
                         "TangoBinding::command_inout_reply");
    SET_DEFAULT_PRHS_THEN_RETURN(kError);                 
  }

  //- cmd_id must be 1-by-1 int32 char array
  if (::mxIsInt32(mx_array) == false || ::mxGetM(mx_array) != 1 || ::mxGetN(mx_array) != 1) 
  {
    MEX_UTILS->set_error("invalid request descriptor specified",
                         "1-by-1 int32 array expected for field 'cmd_id'",
                         "TangoBinding::command_inout_reply");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }

  //- extract cmd_id from mxArray
  Tango::DevLong cmd_id = reinterpret_cast<Tango::DevLong*>(::mxGetPr(mx_array))[0];

  //- mx_rdesc struct should contain a <req_id> field
  mx_array = ::mxGetField(mx_rdesc, 0, "req_id");
  if (! mx_array) 
  {
    MEX_UTILS->set_error("invalid asynch. request descriptor specified",
                         "struct should contain a field named 'req_id'",
                         "TangoBinding::command_inout_reply");
    SET_DEFAULT_PRHS_THEN_RETURN(kError);                 
  }

  //- req_id must be 1-by-1 int32 char array
  if (::mxIsInt32(mx_array) == false || ::mxGetM(mx_array) != 1 || ::mxGetN(mx_array) != 1) 
  {
    MEX_UTILS->set_error("invalid argin specified",
                         "1-by-1 int32 array expected for field 'req_id'",
                         "TangoBinding::command_inout_reply");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }

  //- extract asynch. request identifier from mxArray
  Tango::DevLong req_id = reinterpret_cast<Tango::DevLong*>(::mxGetPr(mx_array))[0];

  //- get timeout option the 3rd input arg
  const mxArray* mx_tmo = MEX_ARGS->get_input_array(k3RD_ARG, kMX_ANY, 1, 1);
  if (mx_tmo == 0) 
  {
    MEX_UTILS->push_error("invalid argin specified",
                          "timeout parameter is either missing or has an invalid format",
                          "TangoBinding::command_inout_reply");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }

  //- extract timeout from mxArray
  Tango::DevLong tmo = (Tango::DevLong)::mxGetPr(mx_tmo)[0];

  //- get device descriptor
  DeviceDesc * ddesc = DEV_REP->device_desc(dev_name);
  if (! ddesc) 
  {
    MEX_UTILS->push_error("failed to execute command_inout_reply",
                          "could not obtain a valid device reference",
                          "TangoBinding::command_inout_reply");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }

  //- asynch. cmd response
  Tango::DeviceData dd_out;

  //- try to get request response
  _TRY(dd_out = ddesc->proxy()->command_inout_reply(req_id, tmo), dev_name, "command_inout_reply");

  //- decode cmd response
  mxArray * argout = 0; 
  if (DATA_ADAPTER->decode_argout(ddesc, cmd_id, dd_out, argout) == kError)
  {
    //- release allocated memory then return error
    if (argout) ::mxDestroyArray(argout);
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }

  //- set output arg
  if (MEX_ARGS->set_output_array(k1ST_ARG, argout) == kError)
  {
    //- release allocated memory then return error
    ::mxDestroyArray(argout);
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }

  return kNoError;
}

//=============================================================================
//- TangoBinding::command_history
//=============================================================================
int TangoBinding::command_history (void)
{
  //- get <device name> from the 2nd input arg
  std::string dev;
  if (MEX_ARGS->get_input_string(k2ND_ARG, dev) == kError)
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 

  //- get <command name> from the 3rd input arg
  std::string cmd;
  if (MEX_ARGS->get_input_string(k3RD_ARG, cmd) == kError)
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 

  //- get command history depth from the 4th input arg
  const mxArray* mx_depth = MEX_ARGS->get_input_array(k4TH_ARG, ::mxDOUBLE_CLASS, 1, 1);
  if (mx_depth == 0) 
  {
    MEX_UTILS->push_error("invalid argin specified",
                          "invalid argument specified (wrong type or dims)",
                          "TangoBinding::command_history");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  
  //- extract history depth value from mxArray
  int depth = (int)::mxGetPr(mx_depth)[0];

  //- get device descriptor
  DeviceDesc * ddesc = DEV_REP->device_desc(dev);
  if (! ddesc) 
  {
    std::string r = "failed to execute command_history on " + dev;
    MEX_UTILS->push_error(r.c_str(),
                          "could not obtain a valid device reference",
                          "TangoBinding::command_history");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }

  //- is <cmd> a valid command for <dev>
  int cmd_id = ddesc->cmd_exists(cmd);
  if (cmd_id == kError) 
  {
    std::string d = cmd + " is not a valid " + dev + " command";
    MEX_UTILS->set_error("command not found",
                         d.c_str(),
                         "TangoBinding::command_history");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }

  //- get command history
  Tango::DeviceDataHistoryList * ddhl = 0;
  _TRY(ddhl = ddesc->proxy()->command_history(cmd, depth), dev, "command_history");

  //- check result
  if (ddhl == 0) 
  {
    MEX_UTILS->set_error("internal error",
                         "command_history failed - got unexpected NULL response",
                         "TangoBinding::command_history");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }

  //- build argout: 1-by-[ddhl->size] struct array
  int len = ddhl->size();
  mwSize dims[2] = {1, static_cast<mwSize>(len)};
  const char * field_names[] = {"time", "value", "has_failed", "error"};
  mxArray * argout = ::mxCreateStructArray(2, dims, 4, field_names);
  if (! argout) 
  {
    MEX_UTILS->set_error("out of memory",
                         "mxCreateStructArray failed",
                         "TangoBinding::command_history");
    delete ddhl;
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }

  //- fill each struct in the array (one by history entry)
  Tango::TimeVal time;
  mxArray * mx_array = 0;
  for (int i = 0; i < len; i++) 
  {
    //- argout[i].time
    time = ((*ddhl)[i]).get_date();
    mx_array = ::mxCreateDoubleMatrix(1,1,mxREAL);
    if (mx_array)
    {
      //- convert time to matlab serial number format
      ::mxGetPr(mx_array)[0] = lk_time_reference
                             + lk_time_factor
                             * (time.tv_sec + 1.E-6 * time.tv_usec + 1.E-9 * time.tv_nsec);
    }
    ::mxSetFieldByNumber(argout, i, 0, mx_array);

    //- cmd failed?
    bool has_failed = (*ddhl)[i].has_failed();

    //- argout[i].value
    mx_array = 0;
    if (! has_failed) 
    {
      if (DATA_ADAPTER->decode_argout(ddesc, cmd_id, (*ddhl)[i], mx_array) == kError) 
        has_failed = true;
    }
    ::mxSetFieldByNumber(argout, i, 1, mx_array);

    //- argout[i].failed
    mx_array = ::mxCreateDoubleMatrix(1,1,mxREAL);
    ::mxGetPr(mx_array)[0] = has_failed ? 1 : 0;
    ::mxSetFieldByNumber(argout, i, 2, mx_array);

    //- argout[i].errors
    if (! has_failed) 
    {
      //- no error: argout[i].errors = NULL
      ::mxSetFieldByNumber(argout, i, 3, 0);
    }
    else 
    {
      const char * errors_field_names[] = {"reason", "desc", "origin", "severity"};
      const Tango::DevErrorList & errors = (*ddhl)[i].get_err_stack();
      int errors_len = errors.length();
      if (errors_len)
      {
        //- error comes from Tango side
        //-----------------------------------------------------------------------------
        dims[1] = errors_len;
        mx_array = ::mxCreateStructArray(2, dims, 4, errors_field_names);
        if (mx_array  == 0) 
        {
          MEX_UTILS->set_error("out of memory",
                               "mxCreateStructArray failed",
                               "TangoBinding::command_history");
          //- release allocated memory then return error
          delete ddhl;
          ::mxDestroyArray(argout);
          SET_DEFAULT_PRHS_THEN_RETURN(kError);
        }
        //- populate the error struct array
        for (int j = 0; j < errors_len; j++) 
        {
          //- set each string field of the ith struct
          ::mxSetFieldByNumber(mx_array , j, 0, ::mxCreateString(errors[j].reason.in()));
          ::mxSetFieldByNumber(mx_array , j, 1, ::mxCreateString(errors[j].desc.in()));
          ::mxSetFieldByNumber(mx_array , j, 2, ::mxCreateString(errors[j].origin.in()));
          //- set the severity field
          mxArray * severity_value = ::mxCreateDoubleMatrix(1, 1, mxREAL);
          if (severity_value  == 0) 
          {
            MEX_UTILS->set_error("out of memory",
                                 "mxCreateDoubleMatrix failed",
                                 "TangoBinding::command_history");
            //- release allocated memory then return error
            delete ddhl;
            ::mxDestroyArray(argout);
            SET_DEFAULT_PRHS_THEN_RETURN(kError);
          }
          ::mxGetPr(severity_value)[0] = errors[j].severity;
          ::mxSetFieldByNumber(mx_array, j, 3, severity_value);
        }
      }
      else
      {
        //- error comes from DataAdapter::decode_argout
        //-----------------------------------------------------------------------------
        dims[1] = 1;
        mx_array = ::mxCreateStructArray(2, dims, 4, errors_field_names);
        if (mx_array  == 0) 
        {
          MEX_UTILS->set_error("out of memory",
                               "mxCreateStructArray failed",
                               "TangoBinding::command_history");
          //- release allocated memory then return error
          delete ddhl;
          ::mxDestroyArray(argout);
          SET_DEFAULT_PRHS_THEN_RETURN(kError);
        }
        //- set each string field of the ith struct
        const char * r = "internal error";
        ::mxSetFieldByNumber(mx_array , 0, 0, ::mxCreateString(r));
        const char * d = "TANGO to Matlab data conversion failed";
        ::mxSetFieldByNumber(mx_array , 0, 1, ::mxCreateString(d));
        const char * o = "TangoBinding::command_history";
        ::mxSetFieldByNumber(mx_array , 0, 2, ::mxCreateString(o));
        //- set the severity field 
        mxArray * severity_value = ::mxCreateDoubleMatrix(1, 1, mxREAL);
        if (severity_value  == 0) 
        {
          MEX_UTILS->set_error("out of memory",
                               "mxCreateDoubleMatrix failed",
                               "TangoBinding::command_history");
          //- release allocated memory then return error
          delete ddhl;
          ::mxDestroyArray(argout);
          SET_DEFAULT_PRHS_THEN_RETURN(kError);
        }
        ::mxGetPr(severity_value)[0] = Tango::ERR;
        ::mxSetFieldByNumber(mx_array , 0, 3, severity_value);      
      }
      //- set argout[i].errors
      ::mxSetFieldByNumber(argout, i, 3, mx_array);
    }
  } //- for each history entry

  //- release memory (allocated by TANGO)
  delete ddhl;

  //- set output arg
  if (MEX_ARGS->set_output_array(k1ST_ARG, argout) == kError)
  {
    //- release allocated memory then return error
    ::mxDestroyArray(argout);
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }

  return kNoError;
}

//=============================================================================
//- TangoBinding::read_attribute 
//=============================================================================
int TangoBinding::read_attribute (void)
{
  //- get <device name> from the 2nd MEX-file input arg
  std::string dev_name;
  if (MEX_ARGS->get_input_string(k2ND_ARG, dev_name) == kError)
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 

  //- get <attribute name> from the 3rd MEX-file input arg
  std::string attr_name;
  if (MEX_ARGS->get_input_string(k3RD_ARG, attr_name) == kError)
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 

  //- get device descriptor
  DeviceDesc * ddesc = DEV_REP->device_desc(dev_name);
  if (! ddesc) 
  {
    std::string r = "failed to read attribute " + attr_name + " on " + dev_name;
    MEX_UTILS->push_error(r.c_str(),
                          "could not obtain a valid device reference",
                          "TangoBinding::read_attribute");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }

  //- is <attr_name> a valid attribute for <dev>
  int attr_id = ddesc->attr_exists(attr_name);
  if (attr_id == kError) 
  {
    std::string d = attr_name + " is not a valid " + dev_name + " attribute";
    MEX_UTILS->set_error("API_AttrNotFound",
                         d.c_str(),
                         "TangoBinding::read_attribute");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }

  //- read attribute 
  Tango::DeviceAttribute attr_value;
  _TRY(attr_value = ddesc->proxy()->read_attribute(attr_name), dev_name, "read_attribute");

  //- build argout: 1-by-1 struct array
  const mwSize dims[2] = {1, 1};
  const char *field_names[] = {
                                "name",
                                "has_failed",
                                "error",
                                "m",
                                "n",
                                "quality",
                                "quality_str",
                                "time",
                                "value",
                                "setpoint"
                              };
  mxArray * argout = ::mxCreateStructArray(2, dims, 10, field_names);
  if (! argout) 
  {
    MEX_UTILS->set_error("out of memory",
                         "mxCreateStructArray failed",
                         "TangoBinding::read_attribute");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  std::vector<mxArray *> output(10);
  if (this->mxattribute(ddesc,attr_value, argout, 0, true) != kNoError) {
    ::mxDestroyArray(argout);
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }

  //- set output arg
  if (MEX_ARGS->set_output_array(k1ST_ARG, argout) == kError)
  {
    //- release allocated memory then return error
    ::mxDestroyArray(argout);
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }

  return kNoError;
}

//=============================================================================
//- TangoBinding::read_attribute_asynch 
//=============================================================================
int TangoBinding::read_attribute_asynch (void)
{
  //- get <device name> from the 2nd MEX-file input arg
  std::string dev;
  if (MEX_ARGS->get_input_string(k2ND_ARG, dev) == kError)
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 

  //- get <attribute name> from the 3rd MEX-file input arg
  std::string attr_name;
  if (MEX_ARGS->get_input_string(k3RD_ARG, attr_name) == kError)
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 

  //- get device descriptor
  DeviceDesc * ddesc = DEV_REP->device_desc(dev);
  if (! ddesc) 
  {
    std::string r = "failed to read attribute " + attr_name + " on " + dev;
    MEX_UTILS->push_error(r.c_str(),
                          "could not obtain a valid device reference",
                          "TangoBinding::read_attribute_asynch");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }

  //- is <attr_name> a valid attribute of <dev>
  int attr_id = ddesc->attr_exists(attr_name);
  if (attr_id == kError) 
  {
    std::string d = attr_name + " is not a valid " + dev + " attribute";
    MEX_UTILS->set_error("API_AttrNotFound",
                         d.c_str(),
                         "TangoBinding::read_attribute_asynch");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }

  //- is <attr_name> readable?
  if (ddesc->is_attr_readable(attr_id) == false) 
  {
    std::string d = "attribute " + attr_name + " of " + dev + " is not readable";
    MEX_UTILS->set_error("failed to read attribute write only attribute",
                         d.c_str(),
                         "TangoBinding::read_attribute_asynch");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }

  //- read attribute 
  Tango::DevLong req_id;
  _TRY(req_id = ddesc->proxy()->read_attribute_asynch(attr_name), dev, "read_attribute_asynch");

  //- create a 1-by-1 struct array for result
  const mwSize dims[2] = {1, 1};
  const char * field_names[] = {"dev_name", "attr_name", "attr_id", "req_id"};
  mxArray * argout = ::mxCreateStructArray(2, dims, 4, field_names);
  if (! argout) 
  {
    MEX_UTILS->set_error("out of memory",
                         "mxCreateStructArray failed",
                         "TangoBinding::read_attribute_asynch");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }

  //- argout.dev_name
  mxArray * mx_array = ::mxCreateString(dev.c_str());
  if (! mx_array) 
  {
    MEX_UTILS->set_error("out of memory",
                         "mxCreateString failed",
                         "TangoBinding::read_attribute_asynch");
    //- release allocated memory then return error
    ::mxDestroyArray(argout);
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  ::mxSetFieldByNumber(argout, 0, 0, mx_array);

  //- argout.attr_name
  mx_array = ::mxCreateString(attr_name.c_str());
  if (! mx_array) 
  {
    MEX_UTILS->set_error("out of memory",
                         "mxCreateString failed",
                         "TangoBinding::read_attribute_asynch");
    //- release allocated memory then return error
    ::mxDestroyArray(argout);
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  ::mxSetFieldByNumber(argout, 0, 1, mx_array);

  //- argout.attr_id
  mx_array = ::mxCreateNumericArray(2, dims, ::mxINT32_CLASS, ::mxREAL);
  if (! mx_array) 
  {
    MEX_UTILS->set_error("out of memory",
                         "mxCreateNumericArray failed",
                         "TangoBinding::read_attribute_asynch");
    //- release allocated memory then return error
    ::mxDestroyArray(argout);
    SET_DEFAULT_PRHS_THEN_RETURN(kError);  
  }
  reinterpret_cast<Tango::DevLong*>(::mxGetPr(mx_array))[0] = attr_id;
  ::mxSetFieldByNumber(argout, 0, 2, mx_array);

  //- argout.req_id
  mx_array = ::mxCreateNumericArray(2, dims, ::mxINT32_CLASS, ::mxREAL);
  if (! mx_array) 
  {
    MEX_UTILS->set_error("out of memory",
                         "mxCreateNumericArray failed",
                         "TangoBinding::read_attribute_asynch");
    //- release allocated memory then return error
    ::mxDestroyArray(argout);
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  reinterpret_cast<Tango::DevLong*>(::mxGetPr(mx_array))[0] = req_id;
  ::mxSetFieldByNumber(argout, 0, 3, mx_array);

  //- set output arg
  if (MEX_ARGS->set_output_array(k1ST_ARG, argout) == kError)
  {
    //- release allocated memory then return error
    ::mxDestroyArray(argout);
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }

  return kNoError;
}

//=============================================================================
//- TangoBinding::read_attribute_reply
//=============================================================================
int TangoBinding::read_attribute_reply (void)
{
  //- get <asynch. request descriptor> from 2nd input arg
  const mxArray* mx_rdesc = MEX_ARGS->get_input_array(k2ND_ARG, ::mxSTRUCT_CLASS, 1, 1);
  if (mx_rdesc == 0) 
  {
    MEX_UTILS->push_error("invalid request descriptor specified",
                          "request descriptor is either missing or has an invalid format",
                          "TangoBinding::read_attribute_reply");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }

  //- mx_rdesc struct should contain a <dev_name> field
  mxArray * mx_array = ::mxGetField(mx_rdesc, 0, "dev_name");
  if (! mx_array) 
  {
    MEX_UTILS->set_error("invalid argin specified",
                         "struct should contain a field named 'dev_name'",
                         "TangoBinding::read_attribute_reply");
    SET_DEFAULT_PRHS_THEN_RETURN(kError);                 
  }

  //- <dev_name> must be 1-by-n char array
  if (::mxIsChar(mx_array) == false || ::mxGetM(mx_array) != 1) 
  {
    MEX_UTILS->set_error("invalid request descriptor specified",
                         "1-by-n char array expected for field 'dev_name'",
                         "TangoBinding::read_attribute_reply");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }

  //- extract <dev_name> string from mxArray
  char * dev_name_cstr = ::mxArrayToString(mx_array);
  if (dev_name_cstr == 0) 
  {
    MEX_UTILS->set_error("invalid request descriptor specified",
                         "could not extract string from field",
                         "TangoBinding::read_attribute_reply");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  std::string dev_name(dev_name_cstr);
  ::mxFree(dev_name_cstr);

  //- mx_rdesc struct should contain a <attr_name> field
  mx_array = ::mxGetField(mx_rdesc, 0, "attr_name");
  if (! mx_array) 
  {
    MEX_UTILS->set_error("invalid argin specified",
                         "struct should contain a field named 'attr_name'",
                         "TangoBinding::read_attribute_reply");
    SET_DEFAULT_PRHS_THEN_RETURN(kError);                 
  }

  //- attr_name must be 1-by-n char array
  if (::mxIsChar(mx_array) == false || ::mxGetM(mx_array) != 1) 
  {
    MEX_UTILS->set_error("invalid request descriptor specified",
                         "1-by-n char array expected for field 'attr_name'",
                         "TangoBinding::read_attribute_reply");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }

  //- extract attr_name string from mxArray
  char * attr_name_cstr = ::mxArrayToString(mx_array);
  if (attr_name_cstr == 0) 
  {
    MEX_UTILS->set_error("invalid request descriptor specified",
                         "could not extract string from field",
                         "TangoBinding::read_attribute_reply");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  std::string attr_name(attr_name_cstr);
  ::mxFree(attr_name_cstr);

  //- mx_rdesc struct should contain an <attr_id> field
  mx_array = ::mxGetField(mx_rdesc, 0, "attr_id");
  if (! mx_array) 
  {
    MEX_UTILS->set_error("invalid request descriptor specified",
                         "struct should contain a field named 'attr_id'",
                         "TangoBinding::read_attribute_reply");
    SET_DEFAULT_PRHS_THEN_RETURN(kError);                 
  }

  //- <attr_id> must be 1-by-1 int32 char array
  if (::mxIsInt32(mx_array) == false || ::mxGetM(mx_array) != 1 || ::mxGetN(mx_array) != 1) 
  {
    MEX_UTILS->set_error("invalid request descriptor specified",
                         "1-by-1 int32 array expected for field 'attr_id'",
                         "TangoBinding::read_attribute_reply");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  //- extract <attr_id>
  Tango::DevLong attr_id = reinterpret_cast<Tango::DevLong*>(::mxGetPr(mx_array))[0];

  //- mx_rdesc struct should contain a <req_id> field
  mx_array = ::mxGetField(mx_rdesc, 0, "req_id");
  if (! mx_array) 
  {
    MEX_UTILS->set_error("invalid request descriptor specified",
                         "struct should contain a field named 'req_id'",
                         "TangoBinding::read_attribute_reply");
    SET_DEFAULT_PRHS_THEN_RETURN(kError);                 
  }
  //- <req_id> must be 1-by-1 int32 char array
  if (::mxIsInt32(mx_array) == false || ::mxGetM(mx_array) != 1 || ::mxGetN(mx_array) != 1) 
  {
    MEX_UTILS->set_error("invalid argin specified",
                         "1-by-1 int32 array expected for field 'req_id'",
                         "TangoBinding::read_attribute_reply");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  //- extract asynch request id
  Tango::DevLong req_id = reinterpret_cast<Tango::DevLong*>(::mxGetPr(mx_array))[0];

  //- get timeout option the 3rd input arg
  const mxArray* mx_tmo = MEX_ARGS->get_input_array(k3RD_ARG, kMX_ANY, 1, 1);
  if (mx_tmo == 0) 
  {
    MEX_UTILS->push_error("invalid argin specified",
                          "timeout parameter is either missing or has an invalid format",
                          "TangoBinding::read_attribute_reply");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  Tango::DevLong tmo = (Tango::DevLong)::mxGetPr(mx_tmo)[0];

  //- get device descriptor
  DeviceDesc * ddesc = DEV_REP->device_desc(dev_name);
  if (! ddesc) 
  {
    MEX_UTILS->push_error("failed to execute read_attribute_reply",
                          "could not obtain a valid device reference",
                          "TangoBinding::read_attribute_reply");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }

  //- get asynch. reply
  Tango::DeviceAttribute * attr_value;
  _TRY(attr_value = ddesc->proxy()->read_attribute_reply(req_id, tmo), dev_name, "read_attribute_reply");

  //- build argout: 1-by-1 struct array
  const mwSize dims[2] = {1, 1};
  const char *field_names[] = {
                                "name",
                                "has_failed",
                                "error",
                                "m",
                                "n",
                                "quality",
                                "quality_str",
                                "time",
                                "value",
                                "setpoint"
                              };
  mxArray * argout = ::mxCreateStructArray(2, dims, 10, field_names);
  if (! argout) 
  {
    MEX_UTILS->set_error("out of memory",
                         "mxCreateStructArray failed",
                         "TangoBinding::read_attribute");
    delete attr_value;
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  std::vector<mxArray *> output(10);
  if (this->mxattribute(ddesc, *attr_value, argout, 0, true) != kNoError) {
    ::mxDestroyArray(argout);
    delete attr_value;
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }

  delete attr_value;
  //- set output arg
  if (MEX_ARGS->set_output_array(k1ST_ARG, argout) == kError)
  {
    //- release allocated memory then return error
    ::mxDestroyArray(argout);
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }

  return kNoError;
}

int TangoBinding::mxattribute (DeviceDesc * ddesc,
	Tango::DeviceAttribute& tgattr,
	mxArray *mxattr,
	mwIndex index,
	bool exit_on_error)
// name		0
// has_failed	1
// error	2
// m		3
// n		4
// quality	5
// quality_str	6
// time		7
// value	8
// setpoint	9
{  
    //- reset error flag
    bool has_failed = false;
    mxArray *mx_array, *mx_value = 0, *mx_setpoint = 0;

    //- be sure we have some valid data to decode
    if (tgattr.has_failed()) {
      //- abort on first error?
      if (exit_on_error) {
        MEX_UTILS->set_error(tgattr.get_err_stack());
        return kError;
      }
      //- no valid data for this attribute
      MEX_UTILS->error_stack_to_mxarray(tgattr.get_err_stack(), mx_array);
      ::mxSetFieldByNumber(mxattr, index, 2, mx_array);
      has_failed = true;
    }
    else if (tgattr.quality == Tango::ATTR_INVALID) {
      //- abort on first error?
      if (exit_on_error) {
        std::string r = "could not read attribute " + tgattr.name + " on " + ddesc->name();
        MEX_UTILS->set_error(r.c_str(),
                              "got invalid data from device",
                              "TangoBinding::read_attributes");
        return kError;
      }
      //- no valid data for this attribute
      Tango::DevErrorList errors(1);
      errors.length(1);
      errors[0].desc = CORBA::string_dup("could not read attribute");
      errors[0].severity = Tango::ERR;
      errors[0].reason = CORBA::string_dup("got invalid data from device");
      errors[0].origin = CORBA::string_dup("TangoBinding::read_attributes");
      MEX_UTILS->error_stack_to_mxarray(errors, mx_array);
      ::mxSetFieldByNumber(mxattr, index, 2, mx_array);
      has_failed = true;
    }
    //- decode attribute value
    else if (DATA_ADAPTER->decode_attr(ddesc, ddesc->attr_exists(tgattr.name), tgattr, mx_value, mx_setpoint) == kError) {
      //- abort on first error?
      if (exit_on_error) {
        std::string r = "could not read attribute " + tgattr.name + " on " + ddesc->name();
        MEX_UTILS->set_error(r.c_str(),
                              "TANGO to Matlab data conversion failed",
                              "TangoBinding::read_attributes");
        return kError;
      }
      //- force attribute value quality to INVALID
      tgattr.quality = Tango::ATTR_INVALID;
      Tango::DevErrorList errors(1);
      errors.length(1);		// Absolutely necessary
      errors[0].desc = CORBA::string_dup("TANGO to Matlab data conversion failed");
      errors[0].severity = Tango::ERR;
      errors[0].reason = CORBA::string_dup("data conversion failed");
      errors[0].origin = CORBA::string_dup("TangoBinding::decode_attribute");
     MEX_UTILS->error_stack_to_mxarray(errors, mx_array);
      ::mxSetFieldByNumber(mxattr, index, 2, mx_array);
      has_failed = true;
    }
    else {
      ::mxSetFieldByNumber(mxattr, index, 8, mx_value);
      ::mxSetFieldByNumber(mxattr, index, 9, mx_setpoint);
    }

    //- argout[i].name
    ::mxSetFieldByNumber(mxattr, index, 0, ::mxCreateString(tgattr.name.c_str()));

    //- argout[i].has_failed
    ::mxSetFieldByNumber(mxattr, index, 1, ::mxCreateDoubleScalar(has_failed ? 1. : 0.));

    //- argout[i].m
    ::mxSetFieldByNumber(mxattr, index, 3, ::mxCreateDoubleScalar(mx_value ? ::mxGetM(mx_value) : 0));

    //- argout[i].n
    ::mxSetFieldByNumber(mxattr, index, 4, ::mxCreateDoubleScalar(mx_value ? ::mxGetN(mx_value) : 0));

    //- argout[i].quality
    switch (tgattr.quality) {
      case Tango::ATTR_VALID:
	::mxSetFieldByNumber(mxattr, index, 5, ::mxCreateDoubleScalar(0));
        ::mxSetFieldByNumber(mxattr, index, 6, ::mxCreateString("VALID"));
        break;
      case Tango::ATTR_INVALID:
	::mxSetFieldByNumber(mxattr, index, 5, ::mxCreateDoubleScalar(1));
        ::mxSetFieldByNumber(mxattr, index, 6, ::mxCreateString("INVALID"));
        break;
      case Tango::ATTR_ALARM:
	::mxSetFieldByNumber(mxattr, index, 5, ::mxCreateDoubleScalar(2));
        ::mxSetFieldByNumber(mxattr, index, 6, ::mxCreateString("ALARM"));
        break;
      case Tango::ATTR_CHANGING:
	::mxSetFieldByNumber(mxattr, index, 5, ::mxCreateDoubleScalar(3));
        ::mxSetFieldByNumber(mxattr, index, 6, ::mxCreateString("CHANGING"));
       break;
      case Tango::ATTR_WARNING:
	::mxSetFieldByNumber(mxattr, index, 5, ::mxCreateDoubleScalar(4));
        ::mxSetFieldByNumber(mxattr, index, 6, ::mxCreateString("WARNING"));
        break;
    }

    //- argout[i].time
    //- convert time to matlab serial number format
    ::mxSetFieldByNumber(mxattr, index, 7, ::mxCreateDoubleScalar(lk_time_reference 
              + lk_time_factor 
              * (
                  tgattr.time.tv_sec  + 
                  1.E-6 * tgattr.time.tv_usec + 
                  1.E-9 * tgattr.time.tv_nsec
                )));

  return kNoError;
}

//=============================================================================
//- standalone template function: extract_attribute_values 
//=============================================================================
template <typename T> 
int TangoBinding::extract_attribute_values (DeviceDesc * ddesc, 
                                            std::vector<T>* avs, 
                                            mxArray*& argout)
{
  //- check input
  if (! ddesc || ! avs) {
    MEX_UTILS->set_error("internal error",
                         "unexpected NULL input argument",
                         "TangoBinding::extract_attribute_values");
    return kError;
  }
  
  //- build argout: 1-by-n_attrs struct array
  mwSize n_attrs = avs->size();
  const mwSize dims[2] = {1, n_attrs};
  const char * field_names[] = {
                                "name",
                                "has_failed",
                                "error",
                                "m",
                                "n",
                                "quality",
                                "quality_str",
                                "time",
                                "value",
                                "setpoint"
                               };
  argout = ::mxCreateStructArray(2, dims, 10, field_names);
  if (!argout) {
    MEX_UTILS->set_error("out of memory",
                         "mxCreateStructArray failed",
                         "TangoBinding::extract_attribute_values");
    delete avs;
    return kError;
  }

  //- fill each struct in the array
  for (mwSize i = 0; i < n_attrs; i++) {
     if (this->mxattribute(ddesc,(*avs)[i],argout, i, this->v1_compatibility_enabled()) == kError) {
        ::mxDestroyArray(argout);
	argout = 0;
        delete avs;
        return kError;
     }
  }

  delete avs;	//- release memory (allocated by Tango)
  return kNoError;
}

//=============================================================================
//- TangoBinding::read_attributes 
//=============================================================================
int TangoBinding::read_attributes (void)
{
  //- get <device name> from the 2nd MEX-file input arg
  std::string dev;
  if (MEX_ARGS->get_input_string(k2ND_ARG, dev) == kError)
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 

  //- get <attributes list> from the 3rd MEX-file input arg
  const mxArray* attr_list = MEX_ARGS->get_input_array(k3RD_ARG, ::mxCELL_CLASS, 1);
  if (attr_list == 0)
  {
    MEX_UTILS->push_error("invalid argin specified",
                          "invalid attribute list specified (wrong type or dims)",
                          "TangoBinding::read_attributes");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }

  //- get device descriptor
  DeviceDesc * ddesc = DEV_REP->device_desc(dev);
  if (! ddesc) 
  {
    std::string r = "failed to read attributes on " + dev;
    MEX_UTILS->push_error(r.c_str(),
                          "could not obtain a valid device reference",
                          "TangoBinding::read_attributes");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }

  //- get num of attributes to read
  int n_attrs = ::mxGetN(attr_list);

  //- attributes names
  std::vector<std::string> attr_names;
  attr_names.resize(n_attrs);

  //- attributes ids
  std::vector<int> attr_ids;
  attr_ids.resize(n_attrs);

  //- fill attr_names  
  int i = 0;
  char * cstr = 0;
  mxArray * cell = 0;
  for (i = 0; i < n_attrs; i++) 
  {
    //- get <i>th cell of the input array
    cell = ::mxGetCell(attr_list, i);
    if (! cell) 
    {
      MEX_UTILS->set_error("internal error",
                           "unexpected NULL cell",
                           "TangoBinding::read_attributes");
      SET_DEFAULT_PRHS_THEN_RETURN(kError); 
    }
    //- cell should contain a string
    if (::mxIsChar(cell) == false) 
    {
      MEX_UTILS->set_error("internal error",
                           "unexpected cell content (string expected)",
                           "TangoBinding::read_attributes");
      SET_DEFAULT_PRHS_THEN_RETURN(kError); 
    }
    //- get c-string from matlab char array
    cstr = ::mxArrayToString(cell);
    if (! cstr) 
    {
      MEX_UTILS->set_error("internal error", 
                           "could not extract string from cell",
                           "TangoBinding::read_attributes");
      SET_DEFAULT_PRHS_THEN_RETURN(kError); 
    }
    //- store <i>-th attribute name
    attr_names[i] = cstr;
    //- release cstr
    ::mxFree(cstr);
    //- is <attr_names[i]> a valid attribute for <dev>?
    attr_ids[i] = ddesc->attr_exists(attr_names[i]);
    if (attr_ids[i] == kError) 
    {
      std::string d = attr_names[i] + " is not a valid " + dev + " attribute";
      MEX_UTILS->set_error("API_AttrNotFound",
                           d.c_str(),
                           "TangoBinding::read_attributes");
      SET_DEFAULT_PRHS_THEN_RETURN(kError); 
    }
  } 

  //- read attributes
  std::vector<Tango::DeviceAttribute> * attr_values = 0;
  _TRY(attr_values = ddesc->proxy()->read_attributes(attr_names), dev, "read_attributes");
  if (attr_values == 0) 
  {
    MEX_UTILS->set_error("com. error",
                         "read_attributes failed",
                         "TangoBinding::read_attributes");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }

  //- decode data 
  mxArray * argout = 0;
  if (extract_attribute_values(ddesc, attr_values, argout) == kError)
  {
    MEX_UTILS->push_error("failed to execute read_attributes",
                          "could extract data from device answers [extract_attribute_values failed]",
                          "TangoBinding::read_attributes");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
 
  //- set output arg
  if (MEX_ARGS->set_output_array(k1ST_ARG, argout) == kError)
  {
    ::mxDestroyArray(argout);
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }

  return kNoError;
}

//=============================================================================
//- TangoBinding::read_attributes_asynch 
//=============================================================================
int TangoBinding::read_attributes_asynch (void)
{
  //- get <device name> from the 2nd MEX-file input arg
  std::string dev;
  if (MEX_ARGS->get_input_string(k2ND_ARG, dev) == kError)
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 

  //- get <attributes list> from the 3rd MEX-file input arg
  const mxArray* attr_list = MEX_ARGS->get_input_array(k3RD_ARG, ::mxCELL_CLASS, 1);
  if (attr_list == 0) 
  {
    MEX_UTILS->push_error("invalid argin specified",
                          "invalid attribute list specified (wrong type or dims)",
                          "TangoBinding::read_attributes_asynch");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }

  //- get device descriptor
  DeviceDesc * ddesc = DEV_REP->device_desc(dev);
  if (! ddesc) 
  {
    std::string r = "failed to read attributes on " + dev;
    MEX_UTILS->push_error(r.c_str(),
                          "could not obtain a valid device reference",
                          "TangoBinding::read_attributes_asynch");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }

  //- get num of attrs to read
  int n_attrs = ::mxGetN(attr_list);

  //- attr-names
  std::vector<std::string> attr_names;
  attr_names.resize(n_attrs);

  //- fill attr-names
  for (int i = 0; i < n_attrs; i++) 
  {
    //- get <i>th cell of the array
    mxArray * cell = ::mxGetCell(attr_list, i);
    if (! cell) 
    {
      MEX_UTILS->set_error("internal error",
                            "unexpected NULL cell",
                            "TangoBinding::read_attributes_asynch");
      SET_DEFAULT_PRHS_THEN_RETURN(kError); 
    }
    //- cell should contain a string
    if (::mxIsChar(cell) == false) 
    {
      MEX_UTILS->set_error("internal error",
                           "unexpected cell content (string expected)",
                           "TangoBinding::read_attributes_asynch");
      SET_DEFAULT_PRHS_THEN_RETURN(kError); 
    }
    //- get c-string from matlab char array
    char * cstr = ::mxArrayToString(cell);
    if (! cstr) 
    {
      MEX_UTILS->set_error("internal error", 
                           "could not extract string from cell",
                           "TangoBinding::read_attributes_asynch");
      SET_DEFAULT_PRHS_THEN_RETURN(kError); 
    }
    //- store attr name
    attr_names[i] = cstr;
    //- release cstr
    ::mxFree(cstr);
    //- is <attr_names[i]> a valid attribute for <dev>?
    int attr_id = ddesc->attr_exists(attr_names[i]);
    if (attr_id == kError) 
    {
      std::string d = attr_names[i] + " is not a valid " + dev + " attribute";
      MEX_UTILS->set_error("API_AttrNotFound",
                           d.c_str(),
                           "TangoBinding::read_attributes_asynch");
      SET_DEFAULT_PRHS_THEN_RETURN(kError); 
    }
  }

  //- read attributes (obtain asynch. request identifier as result)
  Tango::DevLong req_id = 0;
  _TRY(req_id = ddesc->proxy()->read_attributes_asynch(attr_names), dev, "read_attributes"); 

  //- create a 1-by-1 struct array for asynch. request identifier 
  const mwSize dims[2] = {1, 1};
  const char *field_names[] = {"dev_name", "req_id"};
  mxArray * argout = ::mxCreateStructArray(2, dims, 2, field_names);
  if (! argout) 
  {
    MEX_UTILS->set_error("out of memory",
                         "mxCreateStructArray failed",
                         "TangoBinding::read_attributes_asynch");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }

  //- set field argout.dev_name
  mxArray * mx_array = ::mxCreateString(dev.c_str());
  if (! mx_array) 
  {
    MEX_UTILS->set_error("out of memory",
                         "mxCreateString failed",
                         "TangoBinding::read_attributes_asynch");
    ::mxDestroyArray(argout);
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  ::mxSetFieldByNumber(argout, 0, 0, mx_array);

  //- set field argout.req_id
  mx_array = ::mxCreateNumericArray(2, dims, ::mxINT32_CLASS, ::mxREAL);
  if (! mx_array) 
  {
    MEX_UTILS->set_error("out of memory",
                         "mxCreateNumericArray failed",
                         "TangoBinding::read_attributes_asynch");
    ::mxDestroyArray(argout);
    SET_DEFAULT_PRHS_THEN_RETURN(kError);
  }
  reinterpret_cast<Tango::DevLong*>(::mxGetPr(mx_array))[0] = req_id; 
  ::mxSetFieldByNumber(argout, 0, 1, mx_array);

  //- set output arg
  if (MEX_ARGS->set_output_array(k1ST_ARG, argout) == kError) 
  {
    ::mxDestroyArray(argout);
    SET_DEFAULT_PRHS_THEN_RETURN(kError);
  }

  return kNoError;
}

//=============================================================================
//- TangoBinding::read_attributes_reply 
//=============================================================================
int TangoBinding::read_attributes_reply (void)
{
  //- get asynch request descriptor from 2nd input arg
  const mxArray* mx_rdesc = MEX_ARGS->get_input_array(k2ND_ARG, ::mxSTRUCT_CLASS, 1, 1);
  if (! mx_rdesc) 
  {
    MEX_UTILS->push_error("invalid request descriptor specified",
                          "request descriptor is either missing or has an invalid format",
                          "TangoBinding::read_attributes_reply");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }

  //- mx_rdesc struct should contain a <dev_name> field
  mxArray * mx_array = ::mxGetField(mx_rdesc, 0, "dev_name");
  if (! mx_array) 
  {
    MEX_UTILS->set_error("invalid argin specified",
                         "struct should contain a field named 'dev_name'",
                         "TangoBinding::read_attributes_reply");
    SET_DEFAULT_PRHS_THEN_RETURN(kError);                 
  }

  //- dev_name must be 1-by-n char array
  if (::mxIsChar(mx_array) == false || ::mxGetM(mx_array) != 1) 
  {
    MEX_UTILS->set_error("invalid request descriptor specified",
                         "1-by-n char array expected for field 'dev_name'",
                         "TangoBinding::read_attributes_reply");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }

  //- extract dev_name string from mxArray
  char * dev_name_cstr = ::mxArrayToString(mx_array);
  if (dev_name_cstr == 0) 
  {
    MEX_UTILS->set_error("invalid request descriptor specified",
                         "could not extract string from field",
                         "TangoBinding::read_attributes_reply");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  std::string dev_name(dev_name_cstr);
  ::mxFree(dev_name_cstr);

  //- mx_rdesc struct should contain a <req_id> field
  mx_array = ::mxGetField(mx_rdesc, 0, "req_id");
  if (! mx_array) 
  {
    MEX_UTILS->set_error("invalid request descriptor specified",
                         "struct should contain a field named 'req_id'",
                         "TangoBinding::read_attributes_reply");
    SET_DEFAULT_PRHS_THEN_RETURN(kError);                 
  }

  //- req_id must be 1-by-1 int32 array
  if (::mxIsInt32(mx_array) == false || ::mxGetM(mx_array) != 1 || ::mxGetN(mx_array) != 1) 
  {
    MEX_UTILS->set_error("invalid argin specified",
                         "1-by-1 int32 array expected for field 'req_id'",
                         "TangoBinding::read_attributes_reply");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  Tango::DevLong req_id = reinterpret_cast<Tango::DevLong*>(::mxGetPr(mx_array))[0];

  //- get <timeout> option the 3rd input arg
  const mxArray* mx_tmo = MEX_ARGS->get_input_array(k3RD_ARG, kMX_ANY, 1, 1);
  if (! mx_tmo) 
  {
    MEX_UTILS->push_error("invalid argin specified",
                          "timeout parameter is either missing or has an invalid format",
                          "TangoBinding::read_attributes_reply");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  Tango::DevLong tmo = (Tango::DevLong)::mxGetPr(mx_tmo)[0];

  //- get device descriptor
  DeviceDesc * ddesc = DEV_REP->device_desc(dev_name);
  if (! ddesc) 
  {
    MEX_UTILS->push_error("failed to execute command_inout_reply",
                          "could not obtain a valid device reference",
                          "TangoBinding::read_attributes_reply");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }

  //- get asynch. request reply
  std::vector<Tango::DeviceAttribute> * attr_values;
  _TRY(attr_values = ddesc->proxy()->read_attributes_reply(req_id, tmo), dev_name, "read_attributes_reply");
  if (! attr_values) 
  {
    MEX_UTILS->set_error("internal error",
                         "read_attributes_reply failed (unexpected NULL reply)",
                         "TangoBinding::read_attributes_reply");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }

  //- decode data 
  mxArray * argout = 0;
  if (extract_attribute_values(ddesc, attr_values, argout) == kError)
  {
    MEX_UTILS->push_error("failed to execute command_inout_reply",
                          "could extract data from device answers [extract_attribute_values failed]",
                          "TangoBinding::read_attributes_reply");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }

  //- set output arg
  if (MEX_ARGS->set_output_array(k1ST_ARG, argout) == kError)
  {
    ::mxDestroyArray(argout);
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }

  return kNoError;
}

//=============================================================================
//- TangoBinding::write_attribute 
//=============================================================================
int TangoBinding::write_attribute (void)
{
  //- get <device name> from the 2nd MEX-file input arg
  std::string dev_name;
  if (MEX_ARGS->get_input_string(k2ND_ARG, dev_name) == kError)
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 

  //- get <attribute name> from the 3rd MEX-file input arg
  std::string attr_name;
  if (MEX_ARGS->get_input_string(k3RD_ARG, attr_name) == kError)
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 

  //- get device descriptor
  DeviceDesc * ddesc = DEV_REP->device_desc(dev_name);
  if (! ddesc) 
  {
    std::string r = "failed to write attribute " + attr_name + " on " + dev_name;
    MEX_UTILS->push_error(r.c_str(),
                          "could not obtain a valid device reference",
                          "TangoBinding::write_attribute");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }

  //- is <attr_name> a valid attribute for <dev_name>?
  int attr_id = ddesc->attr_exists(attr_name);
  if (attr_id == kError) 
  {
    std::string d = attr_name + " is not a valid " + dev_name + " attribute";
    MEX_UTILS->set_error("API_AttrNotFound",
                         d.c_str(),
                         "TangoBinding::write_attribute");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }

  //- is <attr_name> writable?
  if (ddesc->is_attr_writable(attr_id) == false) 
  {
    std::string d = "attribute " + attr_name + " of " + dev_name + " is not writable";
    MEX_UTILS->set_error("API_AttrNotWritable",
                         d.c_str(),
                         "TangoBinding::write_attribute");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }

  //- get value to write from the 4th MEX-file input arg
  const mxArray* arg_in = MEX_ARGS->get_input_array(k4TH_ARG);
  if (! arg_in) 
  {
    std::string r = "failed to write attribute " + attr_name + " on " + dev_name;
    MEX_UTILS->push_error(r.c_str(),
                          "argin is missing",
                          "TangoBinding::write_attribute");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }

  //- encode argin
  Tango::DeviceAttribute value;
  if (DATA_ADAPTER->encode_attr(ddesc, attr_id, arg_in, value)) 
  {
    std::string r = "could not write attribute " + attr_name + " on device " + dev_name;
    MEX_UTILS->push_error(r.c_str(),
                          "failed to extract data from argin",
                          "TangoBinding::write_attribute");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  
  //- write attribute 
  _TRY(ddesc->proxy()->write_attribute(value), dev_name, "write_attribute");

  return kNoError;
}

//=============================================================================
//- TangoBinding::write_attribute_asynch 
//=============================================================================
int TangoBinding::write_attribute_asynch (void)
{
  //- get <device name> from the 2nd MEX-file input arg
  std::string dev_name;
  if (MEX_ARGS->get_input_string(k2ND_ARG, dev_name) == kError)
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 

  //- get <attribute name> from the 3rd MEX-file input arg
  std::string attr_name;
  if (MEX_ARGS->get_input_string(k3RD_ARG, attr_name) == kError)
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 

  //- get device descriptor
  DeviceDesc * ddesc = DEV_REP->device_desc(dev_name);
  if (! ddesc) 
  {
    std::string r = "failed to write attribute " + attr_name + " on " + dev_name;
    MEX_UTILS->push_error(r.c_str(),
                          "could not obtain a valid device reference",
                          "TangoBinding::write_attribute_asynch");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }

  //- is <attr_name> a valid attribute of <dev_name>?
  int attr_id = ddesc->attr_exists(attr_name);
  if (attr_id == kError) 
  {
    std::string d = attr_name + " is not a valid " + dev_name + " attribute";
    MEX_UTILS->set_error("API_AttrNotFound",
                          d.c_str(),
                          "TangoBinding::write_attribute_asynch");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }

  //- is <attr_name> writable?
  if (ddesc->is_attr_writable(attr_id) == false) 
  {
    std::string d = "attribute " + attr_name + " of " + dev_name + " is not writable";
    MEX_UTILS->set_error("API_AttrNotWritable",
                         d.c_str(),
                         "TangoBinding::write_attribute");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }

  //- get value to write from the 4th MEX-file input arg
  const mxArray* arg_in = MEX_ARGS->get_input_array(k4TH_ARG);
  if (arg_in == 0) 
  {
    std::string r = "failed to write attribute " + attr_name + " on " + dev_name;
    MEX_UTILS->push_error(r.c_str(),
                          "argin is missing",
                          "TangoBinding::write_attribute_asynch");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }

  //- encode argin
  Tango::DeviceAttribute value;
  if (DATA_ADAPTER->encode_attr(ddesc, attr_id, arg_in, value)) 
  {
    std::string r = "could not write attribute " + attr_name + " on device " + dev_name;
    MEX_UTILS->push_error(r.c_str(),
                          "failed to extract data from argin",
                          "TangoBinding::write_attribute_asynch");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }

  //- write attribute asynchronously
  Tango::DevLong req_id;
  _TRY(req_id = ddesc->proxy()->write_attribute_asynch(value), dev_name, "write_attribute_asynch");

  //- create a 1-by-1 struct array for result 
  const mwSize dims[2] = {1, 1};
  const char *field_names[] = {"dev_name", "req_id"};
  mxArray * argout = ::mxCreateStructArray(2, dims, 2, field_names);
  if (! argout) 
  {
    MEX_UTILS->set_error("out of memory",
                         "mxCreateStructArray failed",
                         "TangoBinding::write_attribute_asynch");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }

  //- set field argout.dev_name
  mxArray * mx_array = ::mxCreateString(dev_name.c_str());
  if (! mx_array) 
  {
    MEX_UTILS->set_error("out of memory",
                         "mxCreateString failed",
                         "TangoBinding::write_attribute_asynch");
    ::mxDestroyArray(argout);
    SET_DEFAULT_PRHS_THEN_RETURN(kError);
  }
  ::mxSetFieldByNumber(argout, 0, 0, mx_array);

  //- set field argout.req_id
  mx_array = ::mxCreateNumericArray(2, dims, ::mxINT32_CLASS, ::mxREAL);
  if (! mx_array) 
  {
    MEX_UTILS->set_error("out of memory",
                         "mxCreateNumericArray failed",
                         "TangoBinding::write_attribute_asynch");
    ::mxDestroyArray(argout);
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  reinterpret_cast<Tango::DevLong*>(::mxGetPr(mx_array))[0] = req_id;
  ::mxSetFieldByNumber(argout, 0, 1, mx_array);

  //- set output arg
  if (MEX_ARGS->set_output_array(k1ST_ARG, argout) == kError) 
  {
    ::mxDestroyArray(argout);
    SET_DEFAULT_PRHS_THEN_RETURN(kError);
  }
  
  return kNoError;
}

//=============================================================================
//- TangoBinding::write_attribute_reply 
//=============================================================================
int TangoBinding::write_attribute_reply (void)
{
  //- get asynch. request descriptor from 2nd input arg
  const mxArray* mx_rdesc = MEX_ARGS->get_input_array(k2ND_ARG, ::mxSTRUCT_CLASS, 1, 1);
  if (! mx_rdesc) 
  {
    MEX_UTILS->push_error("invalid request descriptor specified",
                          "request descriptor is either missing or has an invalid format",
                          "TangoBinding::write_attribute_reply");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }

  //- mx_rdesc struct should contain a <dev_name> field
  mxArray * mx_array = ::mxGetField(mx_rdesc, 0, "dev_name");
  if (! mx_array) 
  {
    MEX_UTILS->set_error("invalid argin specified",
                         "struct should contain a field named 'dev_name'",
                         "TangoBinding::write_attribute_reply");
    SET_DEFAULT_PRHS_THEN_RETURN(kError);                 
  }

  //- dev_name must be 1-by-n char array
  if (::mxIsChar(mx_array) == false || ::mxGetM(mx_array) != 1) 
  {
    MEX_UTILS->set_error("invalid request descriptor specified",
                         "1-by-n char array expected for field 'dev_name'",
                         "TangoBinding::write_attribute_reply");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  
  //- extract dev_name string from mxArray
  char * dev_name_cstr = ::mxArrayToString(mx_array);
  if (! dev_name_cstr) 
  {
    MEX_UTILS->set_error("invalid request descriptor specified",
                         "could not extract string from field",
                         "TangoBinding::write_attribute_reply");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }  
  std::string dev_name(dev_name_cstr);
  ::mxFree(dev_name_cstr);

  //- mx_rdesc struct should contain a <req_id> field
  mx_array = ::mxGetField(mx_rdesc, 0, "req_id");
  if (! mx_array) 
  {
    MEX_UTILS->set_error("invalid request descriptor specified",
                         "struct should contain a field named 'req_id'",
                         "TangoBinding::write_attribute_reply");
    SET_DEFAULT_PRHS_THEN_RETURN(kError);                 
  }
  
  //- req_id must be 1-by-1 int32 char array
  if (::mxIsInt32(mx_array) == false || ::mxGetM(mx_array) != 1 || ::mxGetN(mx_array) != 1) 
  {
    MEX_UTILS->set_error("invalid argin specified",
                         "1-by-1 int32 array expected for field 'req_id'",
                         "TangoBinding::write_attribute_reply");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  
  //- extract request identifier from mxArray
  Tango::DevLong req_id = reinterpret_cast<Tango::DevLong*>(::mxGetPr(mx_array))[0];

  //- get <timeout> option the 3rd input arg
  const mxArray* mx_tmo = MEX_ARGS->get_input_array(k3RD_ARG, kMX_ANY, 1, 1);
  if (! mx_tmo) 
  {
    MEX_UTILS->push_error("invalid argin specified",
                          "timeout parameter is either missing or has an invalid format",
                          "TangoBinding::write_attribute_reply");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }

  //- extract timeout from mxArray
  Tango::DevLong tmo = (Tango::DevLong)::mxGetPr(mx_tmo)[0];

  //- get device descriptor
  DeviceDesc * ddesc = DEV_REP->device_desc(dev_name);
  if (! ddesc) 
  {
    MEX_UTILS->push_error("failed to execute write_attribute_reply",
                          "could not obtain a valid device reference",
                          "TangoBinding::write_attribute_reply");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  
  //- try to get write acknowledge
  _TRY(ddesc->proxy()->write_attribute_reply(req_id, tmo), dev_name, "write_attribute_reply");
  
  return kNoError;
}

//=============================================================================
//- TangoBinding::write_attributes 
//=============================================================================
int TangoBinding::write_attributes (void)
{
  //- get <device name> from the 2nd MEX-file input arg
  std::string dev_name;
  if (MEX_ARGS->get_input_string(k2ND_ARG, dev_name) == kError)
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 

  //- get <attribute value list> from the 3rd MEX-file input arg
  const mxArray* av_list = MEX_ARGS->get_input_array(k3RD_ARG, ::mxSTRUCT_CLASS, 1);
  if (! av_list) 
  {
    MEX_UTILS->push_error("invalid argin specified",
                          "invalid attribute list specified (wrong type or dims)",
                          "TangoBinding::write_attributes");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }

  //- get device descriptor
  DeviceDesc * ddesc = DEV_REP->device_desc(dev_name);
  if (! ddesc) 
  {
    std::string r = "failed to write attributes on " + dev_name;
    MEX_UTILS->push_error(r.c_str(),
                          "could not obtain a valid device reference",
                          "TangoBinding::write_attributes");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }

  //- create and resize a vector of Tango::DeviceAttribute
  std::vector<Tango::DeviceAttribute> da_list;
  int n_attrs = ::mxGetN(av_list);
  da_list.resize(n_attrs);

  //- get attr/val for each entry in the array
  for (int i = 0; i < n_attrs; i++) 
  {
    //- each attr. val. struct should contain a <name> field
    mxArray * mx_attr_name = ::mxGetField(av_list, i, "name");
    if (! mx_attr_name) 
    {
      MEX_UTILS->set_error("invalid argin specified",
                           "struct should contain a field named 'name'",
                           "TangoBinding::write_attributes");
      SET_DEFAULT_PRHS_THEN_RETURN(kError); 
    }
    //- this <name> field must be 1-by-n char array
    if (::mxIsChar(mx_attr_name) == false || ::mxGetM(mx_attr_name) != 1) 
    {
      MEX_UTILS->set_error("invalid argin specified",
                           "1-by-n char array expected for field 'name'",
                           "TangoBinding::write_attributes");
      SET_DEFAULT_PRHS_THEN_RETURN(kError); 
    }
    //- get string from mxArray
    char * cstr = ::mxArrayToString(mx_attr_name);
    if (! cstr) 
    {
      MEX_UTILS->set_error("internal error", 
                           "could not extract string from field",
                           "TangoBinding::write_attributes");
      SET_DEFAULT_PRHS_THEN_RETURN(kError);
    }
    std::string attr_name(cstr);
    ::mxFree(cstr);
    //- is <attr_name> a valid attribute of <dev_name>?
    int attr_id = ddesc->attr_exists(attr_name);
    if (attr_id == kError) 
    {
      std::string d = attr_name + " is not a valid " + dev_name + " attribute";
      MEX_UTILS->set_error("API_AttrNotFound",
                           d.c_str(),
                           "TangoBinding::write_attributes");
      SET_DEFAULT_PRHS_THEN_RETURN(kError); 
    }
    //- is <attr_name> writable?
    if (ddesc->is_attr_writable(attr_id) == false) 
    {
      std::string d = "attribute " + dev_name + "/" + attr_name + " is not writable";
      MEX_UTILS->set_error("API_AttrNotWritable",
                           d.c_str(),
                           "TangoBinding::write_attributes");
      SET_DEFAULT_PRHS_THEN_RETURN(kError); 
    }
    //- each struct should contains a <value> field
    mxArray * attr_value = ::mxGetField(av_list, i, "value");
    if (! attr_value) 
    {
      MEX_UTILS->set_error("invalid argin specified",
                           "struct should contain a field named 'value'",
                           "TangoBinding::write_attributes");
      SET_DEFAULT_PRHS_THEN_RETURN(kError); 
    }
    //- encode value for the i-th attrbute to write
    if (DATA_ADAPTER->encode_attr(ddesc, attr_id, attr_value, da_list[i])) 
    {
      std::string r = "could not write attribute " + attr_name + " on device " + dev_name;
      MEX_UTILS->push_error(r.c_str(),
                            "failed to extract data from argin",
                            "TangoBinding::write_attributes");
      SET_DEFAULT_PRHS_THEN_RETURN(kError); 
    }
  }

  //- write attributes 
  _TRY(ddesc->proxy()->write_attributes(da_list), dev_name, "write_attributes");

  return kNoError;
}

//=============================================================================
//- TangoBinding::write_attributes_asynch 
//=============================================================================
int TangoBinding::write_attributes_asynch (void)
{
  //- get <device name> from the 2nd MEX-file input arg
  std::string dev_name;
  if (MEX_ARGS->get_input_string(k2ND_ARG, dev_name) == kError)
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 

  //- get <attribute value list> from the 3rd MEX-file input arg
  const mxArray* av_list = MEX_ARGS->get_input_array(k3RD_ARG, ::mxSTRUCT_CLASS, 1);
  if (! av_list) 
  {
    MEX_UTILS->push_error("invalid argin specified",
                          "invalid attribute list specified (wrong type or dims)",
                          "TangoBinding::write_attributes");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }

  //- get device descriptor
  DeviceDesc * ddesc = DEV_REP->device_desc(dev_name);
  if (! ddesc) 
  {
    std::string r = "failed to write attributes on " + dev_name;
    MEX_UTILS->push_error(r.c_str(),
                          "could not obtain a valid device reference",
                          "TangoBinding::write_attributes");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }

  //- create and resize a vector of Tango::DeviceAttribute
  std::vector<Tango::DeviceAttribute> da_list;
  int n_attrs = ::mxGetN(av_list);
  da_list.resize(n_attrs);

  //- get attr/val for each entry in the array
  for (int i = 0; i < n_attrs; i++) 
  {
    //- each attr. val. struct should contain a <name> field
    mxArray * mx_attr_name = ::mxGetField(av_list, i, "name");
    if (! mx_attr_name) 
    {
      MEX_UTILS->set_error("invalid argin specified",
                           "struct should contain a field named 'name'",
                           "TangoBinding::write_attributes");
      SET_DEFAULT_PRHS_THEN_RETURN(kError); 
    }
    //- this <name> field must be 1-by-n char array
    if (::mxIsChar(mx_attr_name) == false || ::mxGetM(mx_attr_name) != 1) 
    {
      MEX_UTILS->set_error("invalid argin specified",
                           "1-by-n char array expected for field 'name'",
                           "TangoBinding::write_attributes");
      SET_DEFAULT_PRHS_THEN_RETURN(kError); 
    }
    //- get string from mxArray
    char * cstr = ::mxArrayToString(mx_attr_name);
    if (! cstr) 
    {
      MEX_UTILS->set_error("internal error", 
                           "could not extract string from field",
                           "TangoBinding::write_attributes");
      SET_DEFAULT_PRHS_THEN_RETURN(kError);
    }
    std::string attr_name(cstr);
    ::mxFree(cstr);
    //- is <attr_name> a valid attribute of <dev_name>?
    int attr_id = ddesc->attr_exists(attr_name);
    if (attr_id == kError) 
    {
      std::string d = attr_name + " is not a valid " + dev_name + " attribute";
      MEX_UTILS->set_error("API_AttrNotFound",
                           d.c_str(),
                           "TangoBinding::write_attributes");
      SET_DEFAULT_PRHS_THEN_RETURN(kError); 
    }
    //- is <attr_name> writable?
    if (ddesc->is_attr_writable(attr_id) == false) 
    {
      std::string d = "attribute " + dev_name + "/" + attr_name + " is not writable";
      MEX_UTILS->set_error("API_AttrNotWritable",
                           d.c_str(),
                           "TangoBinding::write_attributes");
      SET_DEFAULT_PRHS_THEN_RETURN(kError); 
    }
    //- each struct should contains a <value> field
    mxArray * attr_value = ::mxGetField(av_list, i, "value");
    if (! attr_value) 
    {
      MEX_UTILS->set_error("invalid argin specified",
                           "struct should contain a field named 'value'",
                           "TangoBinding::write_attributes");
      SET_DEFAULT_PRHS_THEN_RETURN(kError); 
    }
    //- encode value for the i-th attrbute to write
    if (DATA_ADAPTER->encode_attr(ddesc, attr_id, attr_value, da_list[i])) 
    {
      std::string r = "could not write attribute " + attr_name + " on device " + dev_name;
      MEX_UTILS->push_error(r.c_str(),
                            "failed to extract data from argin",
                            "TangoBinding::write_attributes");
      SET_DEFAULT_PRHS_THEN_RETURN(kError); 
    }
  }

  //- write attributes asynchronously
  Tango::DevLong req_id;
  _TRY(req_id = ddesc->proxy()->write_attributes_asynch(da_list), dev_name, "write_attributes_asynch");

  //- create a 1-by-1 struct array for result 
  const mwSize dims[2] = {1, 1};
  const char * field_names[] = {"dev_name", "req_id"};
  mxArray * argout = ::mxCreateStructArray(2, dims, 2, field_names);
  if (! argout) 
  {
    MEX_UTILS->set_error("out of memory",
                         "mxCreateStructArray failed",
                         "TangoBinding::write_attributes_asynch");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }

  //- set field argout.dev_name
  mxArray * mx_array = ::mxCreateString(dev_name.c_str());
  if (! mx_array) 
  {
    MEX_UTILS->set_error("out of memory",
                         "mxCreateString failed",
                         "TangoBinding::write_attributes_asynch");
    ::mxDestroyArray(argout);
    SET_DEFAULT_PRHS_THEN_RETURN(kError);
  }
  ::mxSetFieldByNumber(argout, 0, 0, mx_array);

  //- set field argout.req_id
  mx_array = ::mxCreateNumericArray(2, dims, ::mxINT32_CLASS, ::mxREAL);
  if (! mx_array) 
  {
    MEX_UTILS->set_error("out of memory",
                         "mxCreateNumericArray failed",
                         "TangoBinding::write_attributes_asynch");
    ::mxDestroyArray(argout);
    SET_DEFAULT_PRHS_THEN_RETURN(kError);
  }
  reinterpret_cast<Tango::DevLong*>(::mxGetPr(mx_array))[0] = req_id;
  ::mxSetFieldByNumber(argout, 0, 1, mx_array);

  //- set output arg
  if (MEX_ARGS->set_output_array(k1ST_ARG, argout) == kError) 
  {
    ::mxDestroyArray(argout);
    SET_DEFAULT_PRHS_THEN_RETURN(kError);
  }

  return kNoError;
}

//=============================================================================
//- TangoBinding::write_attributes_reply 
//=============================================================================
int TangoBinding::write_attributes_reply (void)
{
  //- get request descriptor from 2nd input arg
  const mxArray* mx_rdesc = MEX_ARGS->get_input_array(k2ND_ARG, ::mxSTRUCT_CLASS, 1, 1);
  if (mx_rdesc == 0) 
  {
    MEX_UTILS->push_error("invalid request descriptor specified",
                          "request descriptor is either missing or has an invalid format",
                          "TangoBinding::write_attributes_reply");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }

  //- mx_rdesc struct should contain a <dev_name> field
  mxArray * mx_array = ::mxGetField(mx_rdesc, 0, "dev_name");
  if (! mx_array) 
  {
    MEX_UTILS->set_error("invalid argin specified",
                         "struct should contain a field named 'dev_name'",
                         "TangoBinding::write_attributes_reply");
    SET_DEFAULT_PRHS_THEN_RETURN(kError);                 
  }
  
  //- dev_name must be 1-by-n char array
  if (::mxIsChar(mx_array) == false || ::mxGetM(mx_array) != 1) 
  {
    MEX_UTILS->set_error("invalid request descriptor specified",
                         "1-by-n char array expected for field 'dev_name'",
                         "TangoBinding::write_attributes_reply");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  
  //- extract dev_name from mxArray
  char * dev_name_cstr = ::mxArrayToString(mx_array);
  if (! dev_name_cstr) 
  {
    MEX_UTILS->set_error("invalid request descriptor specified",
                         "could not extract string from field",
                         "TangoBinding::write_attributes_reply");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  std::string dev_name(dev_name_cstr);
  ::mxFree(dev_name_cstr);
  
  //- mx_rdesc struct should contain a <req_id> field
  mx_array = ::mxGetField(mx_rdesc, 0, "req_id");
  if (! mx_array)
  {
    MEX_UTILS->set_error("invalid request descriptor specified",
                         "struct should contain a field named 'req_id'",
                         "TangoBinding::write_attributes_reply");
    SET_DEFAULT_PRHS_THEN_RETURN(kError);                 
  }

  //- <req_id> must be 1-by-1 int32 char array
  if (::mxIsInt32(mx_array) == false || ::mxGetM(mx_array) != 1 || ::mxGetN(mx_array) != 1) 
  {
    MEX_UTILS->set_error("invalid argin specified",
                         "1-by-1 int32 array expected for field 'req_id'",
                         "TangoBinding::write_attributes_reply");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }

  //- extract <req_id> from mxArray
  Tango::DevLong req_id = reinterpret_cast<Tango::DevLong*>(::mxGetPr(mx_array))[0];

  //- get <timeout> option the 3rd input arg
  const mxArray* mx_tmo = MEX_ARGS->get_input_array(k3RD_ARG, kMX_ANY, 1, 1);
  if (! mx_tmo) 
  {
    MEX_UTILS->push_error("invalid argin specified",
                          "timeout parameter is either missing or has an invalid format",
                          "TangoBinding::write_attributes_reply");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }

  //- extract <timeout> from mxArray
  Tango::DevLong tmo = (Tango::DevLong)::mxGetPr(mx_tmo)[0];

  //- get device descriptor
  DeviceDesc * ddesc = DEV_REP->device_desc(dev_name);
  if (! ddesc) 
  {
    MEX_UTILS->push_error("failed to execute write_attributes_reply",
                          "could not obtain a valid device reference",
                          "TangoBinding::write_attributes_reply");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }

  //- try to get asynch. request acknowledge
  _TRY(ddesc->proxy()->write_attributes_reply(req_id, tmo), dev_name, "write_attributes_reply");

  return kNoError;
}

//=============================================================================
//- TangoBinding::get_attribute_list
//=============================================================================
int TangoBinding::get_attribute_list (void) 
{
  //- get <device name> from the 2nd MEX-file input arg
  std::string dev_name;
  if (MEX_ARGS->get_input_string(k2ND_ARG, dev_name) == kError) 
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
 
  //- get device descriptor
  DeviceDesc * ddesc = DEV_REP->device_desc(dev_name);
  if (! ddesc) 
  {
    std::string r = "failed to write attributes on " + dev_name;
    MEX_UTILS->push_error(r.c_str(),
                          "could not obtain a valid device reference",
                          "TangoBinding::get_attribute_list");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }

  //- get attribute list
  std::vector<std::string> * attr_list;
  _TRY(attr_list = ddesc->proxy()->get_attribute_list(), dev_name, "get_attribute_list");

  //- build argout : 1-by-n cell array containing strings
  int n_attrs = attr_list->size();
  const mwSize dims[2] = {1, static_cast<mwSize>(n_attrs)};
  mxArray * argout = ::mxCreateCellArray(2, dims);
  if (! argout) 
  {
    MEX_UTILS->set_error("out of memory",
                         "mxCreateCellArray failed",
                         "TangoBinding::get_attribute_list");
    delete attr_list;
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }

  //- copy from src to dest
  for (int i = 0; i < n_attrs; i++)
    ::mxSetCell(argout, i, ::mxCreateString((*attr_list)[i].c_str()));        

  //- release memory (allocated by the Tango API)
  delete attr_list;

  //- set output arg
  if (MEX_ARGS->set_output_array(k1ST_ARG, argout) == kError) 
  {
    ::mxDestroyArray(argout);
    SET_DEFAULT_PRHS_THEN_RETURN(kError);
  }
  
  return kNoError;
}

//=============================================================================
//- TangoBinding::attribute_history
//=============================================================================
int TangoBinding::attribute_history (void)
{
  //- get <device name> from the 2nd input arg
  std::string dev_name;
  if (MEX_ARGS->get_input_string(k2ND_ARG, dev_name) == kError)
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 

  //- get <attribute name> from the 3rd input arg
  std::string attr_name;
  if (MEX_ARGS->get_input_string(k3RD_ARG, attr_name) == kError)
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 

  //- get <attribute history depth> from the 4th input arg
  const mxArray* mx_depth = MEX_ARGS->get_input_array(k4TH_ARG, ::mxDOUBLE_CLASS, 1, 1);
  if (mx_depth == 0) 
  {
    MEX_UTILS->push_error("invalid argin specified",
                          "invalid argument specified (wrong type or dims)",
                          "TangoBinding::attribute_history");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  
  //- extract <depth> from mxArray
  int history_depth = (int)::mxGetPr(mx_depth)[0];

  //- get device descriptor
  DeviceDesc * ddesc = DEV_REP->device_desc(dev_name);
  if (! ddesc) 
  {
    std::string r = "failed to execute attribute_history on " + dev_name;
    MEX_UTILS->push_error(r.c_str(),
                          "could not obtain a valid device reference",
                          "TangoBinding::attribute_history");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }

  //- is <attr> a valid attribute for <dev>
  int attr_id = ddesc->attr_exists(attr_name);
  if (attr_id == kError) 
  {
    std::string d = attr_name + " is not a valid " + dev_name + " attribute";
    MEX_UTILS->set_error("API_AttrNotFound",
                         d.c_str(),
                         "TangoBinding::attribute_history");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }

  //- get attribute history
  std::vector<Tango::DeviceAttributeHistory> * attr_values = 0;
  _TRY(attr_values = ddesc->proxy()->attribute_history(attr_name, history_depth), dev_name, "attribute_history");
  if (! attr_values) 
  {
    MEX_UTILS->set_error("internal error",
                         "attribute_history failed",
                         "TangoBinding::attribute_history");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }

  //- decode data 
  mxArray * argout = 0;
  if (extract_attribute_values(ddesc, attr_values, argout) == kError)
  {
    MEX_UTILS->push_error("failed to execute attribute_history",
                          "could extract data from device answers [extract_attribute_values failed]",
                          "TangoBinding::attribute_history");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }

  //- set output arg
  if (MEX_ARGS->set_output_array(k1ST_ARG, argout) == kError) 
  {
    ::mxDestroyArray(argout);
    SET_DEFAULT_PRHS_THEN_RETURN(kError);
  }
  
  return kNoError;
}

//=============================================================================
//- TangoBinding::status
//=============================================================================
int TangoBinding::status (void)
{
  //- get <device name> from the 2nd MEX-file input arg
  std::string dev;
  if (MEX_ARGS->get_input_string(k2ND_ARG, dev) == kError)
  {
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  //- get device descriptor
  DeviceDesc * ddesc = DEV_REP->device_desc(dev);
  if (! ddesc)
  {
    MEX_UTILS->push_error("com. error",
        "could not obtain a valid device reference",
        "TangoBinding::status");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  //- get device status
  std::string dev_status;
  _TRY(dev_status = ddesc->proxy()->status(), dev, "status");
  //- Create a 1-by-n char array
  mxArray * argout = ::mxCreateString(dev_status.c_str());
  if (! argout)
  {
    MEX_UTILS->set_error("out of memory",
       "mxCreateString failed",
       "TangoBinding::status");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  //- Set output arg
  if (MEX_ARGS->set_output_array(k1ST_ARG, argout) == kError)
  {
    ::mxDestroyArray(argout);
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  return kNoError;
}

//=============================================================================
//- TangoBinding::ping
//=============================================================================
int TangoBinding::ping (void)
{
  //- get <device name> from the 2nd MEX-file input arg
  std::string dev;
  if (MEX_ARGS->get_input_string(k2ND_ARG, dev) == kError)
  {
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  //- get device descriptor
  DeviceDesc * ddesc = DEV_REP->device_desc(dev);
  if (! ddesc)
  {
    MEX_UTILS->push_error("com. error",
        "could not obtain a valid device reference",
        "TangoBinding::ping");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  //- Ping device
  int ping_result = 0;
  _TRY(ping_result = ddesc->proxy()->ping(), dev, "ping");
  //- Create a 1-by-1 double array
  const mwSize dims[2] = {1, 1};
  mxArray * mx_ping_result = ::mxCreateNumericArray(2, dims, ::mxDOUBLE_CLASS, ::mxREAL);
  if (mx_ping_result == 0)
  {
    MEX_UTILS->set_error("out of memory",
                         "mxCreateNumericArray failed",
                         "TangoBinding::ping");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  //- copy from src to dest
  *(::mxGetPr(mx_ping_result)) = ping_result;
  //- Set output arg
  if (MEX_ARGS->set_output_array(k1ST_ARG, mx_ping_result) == kError)
  {
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  return kNoError;
}

//=============================================================================
//- TangoBinding::info
//=============================================================================
int TangoBinding::info (void)
{
  //- get <device name> from the 2nd MEX-file input arg
  std::string dev;
  if (MEX_ARGS->get_input_string(k2ND_ARG, dev) == kError)
  {
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  //- get device descriptor
  DeviceDesc * ddesc = DEV_REP->device_desc(dev);
  if (! ddesc)
  {
    MEX_UTILS->push_error("com. error",
        "could not obtain a valid device reference",
        "TangoBinding::info");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  //- get device info
  Tango::DeviceInfo dev_info;
  _TRY(dev_info = ddesc->proxy()->info(), dev, "info");
  //- Build argout: 1-by-1 struct array
  const mwSize dims[2] = {1, 1};
  const char *field_names[] = {"class", "server", "host", "version", "doc_url", "type"};
  mxArray * argout = ::mxCreateStructArray(2, dims, 6, field_names);
  if (! argout)
  {
    MEX_UTILS->set_error("out of memory",
                         "mxCreateStructArray failed",
                         "TangoBinding::info");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  //- Set field argout.class
  ::mxSetFieldByNumber(argout, 0, 0, ::mxCreateString(dev_info.dev_class.c_str()));
  //- Set field argout.server
  ::mxSetFieldByNumber(argout, 0, 1, ::mxCreateString(dev_info.server_id.c_str()));
  //- Set field argout.host
  ::mxSetFieldByNumber(argout, 0, 2, ::mxCreateString(dev_info.server_host.c_str()));
  //- Set field argout.server
  mxArray * version = ::mxCreateDoubleMatrix(1,1,mxREAL);
  ::mxGetPr(version)[0] =  (double)dev_info.server_version;
  ::mxSetFieldByNumber(argout, 0, 3, version);
  //- Set field argout.doc_url
  ::mxSetFieldByNumber(argout, 0, 4, ::mxCreateString(dev_info.doc_url.c_str()));
  //- Set field argout.type
  mxArray * dev_type = ::mxCreateDoubleMatrix(1,1,mxREAL);
  ::mxGetPr(dev_type)[0] = 0; //-TODO: ADD TANGO-5 SUPPORT: (double)dev_info.dev_type;
  ::mxSetFieldByNumber(argout, 0, 5, dev_type);

  //- Set output arg
  if (MEX_ARGS->set_output_array(k1ST_ARG, argout) == kError) 
  {
    ::mxDestroyArray(argout);
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  return kNoError;
}

//=============================================================================
//- TangoBinding::state
//=============================================================================
int TangoBinding::state (void)
{
  //- get <device name> from the 2nd MEX-file input arg
  std::string dev;
  if (MEX_ARGS->get_input_string(k2ND_ARG, dev) == kError)
  {
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  //- get device descriptor
  DeviceDesc * ddesc = DEV_REP->device_desc(dev);
  if (! ddesc)
  {
    MEX_UTILS->push_error("com. error",
                          "could not obtain a valid device reference",
                          "TangoBinding::state");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  //- get device state
  Tango::DevState dev_state;
  _TRY(dev_state = ddesc->proxy()->state(), dev, "state");
  //- Build argout: 1-by-1 struct array
  const mwSize dims[2] = {1, 1};
  const char *field_names[] = {
     "value", 
     "name"
  };
  mxArray * argout = ::mxCreateStructArray(2, dims, 2, field_names);
  if (! argout)
  {
    MEX_UTILS->set_error("out of memory",
                         "mxCreateStructArray failed",
                         "TangoBinding::state");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  //- set field argout.value
  mxArray * mx_value = ::mxCreateDoubleMatrix(1,1,mxREAL);
  ::mxGetPr(mx_value)[0] = dev_state;
  ::mxSetFieldByNumber(argout, 0, 0, mx_value);
  //- set field argout.name
  switch (dev_state)
  {
    case Tango::ON:
      ::mxSetFieldByNumber(argout, 0, 1, ::mxCreateString("ON"));
      break;
    case Tango::OFF:
      ::mxSetFieldByNumber(argout, 0, 1, ::mxCreateString("OFF"));
      break;
    case Tango::CLOSE:
      ::mxSetFieldByNumber(argout, 0, 1, ::mxCreateString("CLOSE"));
      break;
    case Tango::OPEN:
      ::mxSetFieldByNumber(argout, 0, 1, ::mxCreateString("OPEN"));
      break;
    case Tango::INSERT:
      ::mxSetFieldByNumber(argout, 0, 1, ::mxCreateString("INSERT"));
      break;
    case Tango::EXTRACT:
      ::mxSetFieldByNumber(argout, 0, 1, ::mxCreateString("EXTRACT"));
      break;
    case Tango::MOVING:
      ::mxSetFieldByNumber(argout, 0, 1, ::mxCreateString("MOVING"));
      break;
    case Tango::STANDBY:
      ::mxSetFieldByNumber(argout, 0, 1, ::mxCreateString("STANDBY"));
      break;
    case Tango::FAULT:
      ::mxSetFieldByNumber(argout, 0, 1, ::mxCreateString("FAULT"));
      break;
    case Tango::INIT:
      ::mxSetFieldByNumber(argout, 0, 1, ::mxCreateString("INIT"));
      break;
    case Tango::RUNNING:
      ::mxSetFieldByNumber(argout, 0, 1, ::mxCreateString("RUNNING"));
      break;
    case Tango::ALARM:
      ::mxSetFieldByNumber(argout, 0, 1, ::mxCreateString("ALARM"));
      break;
    case Tango::DISABLE:
      ::mxSetFieldByNumber(argout, 0, 1, ::mxCreateString("DISABLE"));
      break;
    default:
      ::mxSetFieldByNumber(argout, 0, 1, ::mxCreateString("UNKNOWN"));
      break;
  }
  //- Set output arg
  if (MEX_ARGS->set_output_array(k1ST_ARG, argout) == kError) 
  {
    ::mxDestroyArray(argout);
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  return kNoError;
}

//=============================================================================
//- TangoBinding::description
//=============================================================================
int TangoBinding::description (void)
{
  //- get <device name> from the 2nd MEX-file input arg
  std::string dev;
  if (MEX_ARGS->get_input_string(k2ND_ARG, dev) == kError)
  {
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  //- get device descriptor
  DeviceDesc * ddesc = DEV_REP->device_desc(dev);
  if (! ddesc)
  {
    MEX_UTILS->push_error("com. error",
                          "could not obtain a valid device reference",
                          "TangoBinding::description");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  //- get device state
  std::string dev_desc;
  _TRY(dev_desc = ddesc->proxy()->description(), dev, "description");
  //- Create a 1-by-n char array
  mxArray * argout = ::mxCreateString(dev_desc.c_str());
  if (! argout)
  {
    MEX_UTILS->set_error("out of memory",
                         "mxCreateString failed",
                         "TangoBinding::description");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  //- Set output arg
  if (MEX_ARGS->set_output_array(k1ST_ARG, argout) == kError)
  {
    ::mxDestroyArray(argout);
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  return kNoError;
}

//=============================================================================
//- TangoBinding::admin_name
//=============================================================================
int TangoBinding::admin_name (void)
{
  //- get <device name> from the 2nd MEX-file input arg
  std::string dev;
  if (MEX_ARGS->get_input_string(k2ND_ARG, dev) == kError)
  {
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  //- get device descriptor
  DeviceDesc * ddesc = DEV_REP->device_desc(dev);
  if (! ddesc)
  {
    MEX_UTILS->push_error("com. error",
        "could not obtain a valid device reference",
        "TangoBinding::admin_name");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  //- get device state
  std::string dev_adm_name;
  _TRY(dev_adm_name = ddesc->proxy()->adm_name(), dev, "admin_name");
  //- Create a 1-by-n char array
  mxArray * argout = ::mxCreateString(dev_adm_name.c_str());
  if (! argout)
  {
    MEX_UTILS->set_error("out of memory",
                         "mxCreateString failed",
                         "TangoBinding::admin_name");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  //- Set output arg
  if (MEX_ARGS->set_output_array(k1ST_ARG, argout) == kError)
  {
    ::mxDestroyArray(argout);
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  return kNoError;
}

//=============================================================================
//- TangoBinding::get_attr_config
//=============================================================================
int TangoBinding::get_attr_config (void)
{
  //- get <device name> from the 2nd MEX-file input arg
  std::string dev;
  if (MEX_ARGS->get_input_string(k2ND_ARG, dev) == kError)
  {
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  //- get attributes list name from the 3rd MEX-file input arg
  const mxArray* attr_list = MEX_ARGS->get_input_array(k3RD_ARG, ::mxCELL_CLASS, 1);
  if (attr_list == 0)
  {
    MEX_UTILS->push_error("invalid argin specified",
                          "invalid attribute list specified (wrong type or dims)",
                          "TangoBinding::get_attr_config");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  //- get device descriptor
  DeviceDesc * ddesc = DEV_REP->device_desc(dev);
  if (! ddesc)
  {
    MEX_UTILS->push_error("com. error",
                          "could not obtain a valid device reference",
                          "TangoBinding::get_attr_config");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  //- get num of attr_config to read
  int len = ::mxGetN(attr_list);
  //- attr-names
  std::vector<std::string> attr_names;
  attr_names.resize(len);
  //- fill attr-names
  int i = 0;
  char* cstr = 0;
  mxArray * cell = 0;
  for (i = 0; i < len; i++)
  {
    //- get <i>th cell of the array
    cell = ::mxGetCell(attr_list, i);
    if (! cell)
    {
      MEX_UTILS->set_error("internal error",
         "unexpected NULL cell",
         "TangoBinding::get_attr_config");
      SET_DEFAULT_PRHS_THEN_RETURN(kError); 
    }
    //- cell should contain a string
    if (::mxIsChar(cell) == false)
    {
      MEX_UTILS->set_error("internal error",
         "unexpected cell content (string expected)",
         "TangoBinding::get_attr_config");
      SET_DEFAULT_PRHS_THEN_RETURN(kError); 
    }
    //- get c-string from matlab char array.
    cstr = ::mxArrayToString(cell);
    if (! cstr)
    {
      MEX_UTILS->set_error("internal error", 
         "could not extract string from cell", 
         "TangoBinding::get_attr_config");
      SET_DEFAULT_PRHS_THEN_RETURN(kError); 
    }
    //- store attr name
    attr_names[i] = cstr;
    //- release cstr
    ::mxFree(cstr);
  }
  //- Read requested attribute-config
  Tango::AttributeInfoList *attr_config = 0;
  _TRY(attr_config = ddesc->proxy()->get_attribute_config(attr_names), dev, "get_attr_config");
  if (attr_config == 0)
  {
    MEX_UTILS->set_error("internal error",
                         "get_attribute_config failed",
                         "TangoBinding::get_attr_config");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }

  //- Build argout: 1-by-len struct array
  len = attr_config->size();
  const mwSize dims[2] = {1, static_cast<mwSize>(len)};
  //- Containing the following fields:
  const char *field_names[] = {
     "name", 
     "writable",
     "writable_str", 
     "data_format", 
     "data_format_str", 
     "data_type", 
     "data_type_str", 
     "max_m", 
     "max_n", 
     "description", 
     "label", 
     "unit", 
     "standard_unit", 
     "display_unit", 
     "format", 
     "min_value", 
     "max_value", 
     "min_alarm", 
     "max_alarm", 
     "writable_attr_name", 
     "extensions", 
     "disp_level"
  };
  mxArray * argout = ::mxCreateStructArray(2, dims, 22, field_names);
  if (! argout)
  {
    MEX_UTILS->set_error("out of memory",
                         "mxCreateStructArray failed",
                         "TangoBinding::get_attr_config");
    delete attr_config;
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  //- Fill each struct in the array
  int ext_len = 0;
  mxArray * mx_extensions = 0;
  for (i = 0; i < len; i++)
  {
    //- set field argout[i].name
    ::mxSetFieldByNumber(argout, i, 0, ::mxCreateString((*attr_config)[i].name.c_str()));
    //- set field argout[i].writable
    mxArray * mx_writable = ::mxCreateDoubleMatrix(1,1,mxREAL);
    ::mxGetPr(mx_writable)[0] =  (double)(*attr_config)[i].writable;
    ::mxSetFieldByNumber(argout, i, 1, mx_writable);
    //- set field argout[i].writable_str
    switch ((*attr_config)[i].writable)
    {
      case Tango::READ:
        ::mxSetFieldByNumber(argout, i, 2, ::mxCreateString("RO"));
        break;
      case Tango::READ_WITH_WRITE:
        ::mxSetFieldByNumber(argout, i, 2, ::mxCreateString("RWW"));
        break;
      case Tango::WRITE:
        ::mxSetFieldByNumber(argout, i, 2, ::mxCreateString("WO"));
        break;
      case Tango::READ_WRITE:
        ::mxSetFieldByNumber(argout, i, 2, ::mxCreateString("RW"));
        break;
      default:
        ::mxSetFieldByNumber(argout, i, 2, ::mxCreateString("Unknown"));
        break;
    }
    //- set field argout[i].data_format
    mxArray * mx_data_format = ::mxCreateDoubleMatrix(1,1,mxREAL);
    ::mxGetPr(mx_data_format)[0] = (double)(*attr_config)[i].data_format;
    ::mxSetFieldByNumber(argout, i, 3, mx_data_format);
    //- set field argout[i].data_format_str
    switch ((*attr_config)[i].data_format)
    {
      case Tango::SCALAR:
        ::mxSetFieldByNumber(argout, i, 4, ::mxCreateString("Scalar"));
        break;
      case Tango::SPECTRUM:
        ::mxSetFieldByNumber(argout, i, 4, ::mxCreateString("Spectrum"));
        break;
      case Tango::IMAGE:
        ::mxSetFieldByNumber(argout, i, 4, ::mxCreateString("Image"));
        break;
      default:
        ::mxSetFieldByNumber(argout, i, 4, ::mxCreateString("Unknown"));
        break;
    }
    //- set field argout[i].data_type
    mxArray * mx_data_type = ::mxCreateDoubleMatrix(1,1,mxREAL);
    ::mxGetPr(mx_data_type)[0] = (double)(*attr_config)[i].data_type;
    ::mxSetFieldByNumber(argout, i, 5, mx_data_type);
    //- set field argout[i].data_type_str
    switch ((*attr_config)[i].data_format)
    {
      case Tango::SCALAR:
        switch ((*attr_config)[i].data_type)
        {
          case Tango::DEV_STRING:
            ::mxSetFieldByNumber(argout, i, 6, ::mxCreateString("1-by-n char"));
            break;
          case Tango::DEV_SHORT:
            ::mxSetFieldByNumber(argout, i, 6, ::mxCreateString("1-by-1 int16"));
            break;
          case Tango::DEV_LONG:
            ::mxSetFieldByNumber(argout, i, 6, ::mxCreateString("1-by-1 int32"));
            break;
          case Tango::DEV_DOUBLE:
            ::mxSetFieldByNumber(argout, i, 6, ::mxCreateString("1-by-1 double"));
            break;
          default:
            ::mxSetFieldByNumber(argout, i, 6, ::mxCreateString("not supported"));
            break;
        }
        break;
      case Tango::SPECTRUM:
        switch ((*attr_config)[i].data_type)
        {
          case Tango::DEV_SHORT:
            ::mxSetFieldByNumber(argout, i, 6, ::mxCreateString("1-by-n int16"));
            break;
          case Tango::DEV_LONG:
            ::mxSetFieldByNumber(argout, i, 6, ::mxCreateString("1-by-n int32"));
            break;
          case Tango::DEV_DOUBLE:
            ::mxSetFieldByNumber(argout, i, 6, ::mxCreateString("1-by-n double"));
            break;
          default:
            ::mxSetFieldByNumber(argout, i, 6, ::mxCreateString("not supported"));
            break;
        }
        break;
      case Tango::IMAGE:
        switch ((*attr_config)[i].data_type)
        {
          case Tango::DEV_SHORT:
            ::mxSetFieldByNumber(argout, i, 6, ::mxCreateString("m-by-n int16"));
            break;
          case Tango::DEV_LONG:
            ::mxSetFieldByNumber(argout, i, 6, ::mxCreateString("m-by-n int32"));
            break;
          case Tango::DEV_DOUBLE:
            ::mxSetFieldByNumber(argout, i, 6, ::mxCreateString("m-by-n double"));
            break;
          default:
            ::mxSetFieldByNumber(argout, i, 6, ::mxCreateString("not supported"));
            break;
        }
        break;
      default:
        ::mxSetFieldByNumber(argout, i, 6, ::mxCreateString("not supported"));
        break;
    }
    //- set fields argout[i].max_m and argout[i].max_n
    mxArray * mx_max_m = ::mxCreateDoubleMatrix(1,1,mxREAL);
    mxArray * mx_max_n = ::mxCreateDoubleMatrix(1,1,mxREAL);
    switch ((*attr_config)[i].data_format)
    {
      case Tango::SCALAR:
        ::mxGetPr(mx_max_m)[0] = 1;
        ::mxGetPr(mx_max_n)[0] = 1;
        break;
      case Tango::SPECTRUM:
        ::mxGetPr(mx_max_m)[0] = 1;
        ::mxGetPr(mx_max_n)[0] = (double)(*attr_config)[i].max_dim_x;
        break;
      case Tango::IMAGE:
        ::mxGetPr(mx_max_m)[0] = (double)(*attr_config)[i].max_dim_x;
        ::mxGetPr(mx_max_n)[0] = (double)(*attr_config)[i].max_dim_y;
        break;
      default:
        ::mxGetPr(mx_max_m)[0] = 0;
        ::mxGetPr(mx_max_n)[0] = 0;
        break;
    }
    ::mxSetFieldByNumber(argout, i, 7, mx_max_m);
    ::mxSetFieldByNumber(argout, i, 8, mx_max_n);
    //- set field argout[i].description
    ::mxSetFieldByNumber(argout, i,  9, ::mxCreateString((*attr_config)[i].description.c_str()));
    //- set field argout[i].label
    ::mxSetFieldByNumber(argout, i, 10, ::mxCreateString((*attr_config)[i].label.c_str()));
    //- set field argout[i].unit
    ::mxSetFieldByNumber(argout, i, 11, ::mxCreateString((*attr_config)[i].unit.c_str()));
    //- set field argout[i].standard_unit
    ::mxSetFieldByNumber(argout, i, 12, ::mxCreateString((*attr_config)[i].standard_unit.c_str()));
    //- set field argout[i].display_unit
    ::mxSetFieldByNumber(argout, i, 13, ::mxCreateString((*attr_config)[i].display_unit.c_str()));
    //- set field argout[i].format
    ::mxSetFieldByNumber(argout, i, 14, ::mxCreateString((*attr_config)[i].format.c_str()));
    //- set field argout[i].min_value
    ::mxSetFieldByNumber(argout, i, 15, ::mxCreateString((*attr_config)[i].min_value.c_str()));
    //- set field argout[i].max_value
    ::mxSetFieldByNumber(argout, i, 16, ::mxCreateString((*attr_config)[i].max_value.c_str()));
    //- set field argout[i].min_alarm
    ::mxSetFieldByNumber(argout, i, 17, ::mxCreateString((*attr_config)[i].min_alarm.c_str()));
    //- set field argout[i].max_alarm
    ::mxSetFieldByNumber(argout, i, 18, ::mxCreateString((*attr_config)[i].max_alarm.c_str()));
    //- set field argout[i].writable_attr_name
    ::mxSetFieldByNumber(argout, i, 19, ::mxCreateString((*attr_config)[i].writable_attr_name.c_str()));
    //- set field argout[i].extensions - create a 1-by-n cell array 
    ext_len = (*attr_config)[i].extensions.size();
    if (ext_len)
    {
      const mwSize dims[2] = {1, static_cast<mwSize>(ext_len)};
      mx_extensions  = ::mxCreateCellArray(2, dims);
      if (mx_extensions == 0)
      {
        MEX_UTILS->set_error("out of memory",
                             "mxCreateCellArray failed",
                             "TangoBinding::get_attr_config");
        delete attr_config;
        ::mxDestroyArray(argout);
        SET_DEFAULT_PRHS_THEN_RETURN(kError); 
      }
      //- copy from src to dest
      for (int j = 0; j < ext_len; j++)
      {
        ::mxSetCell(mx_extensions, i, ::mxCreateString((*attr_config)[i].extensions[j].c_str()));
      }
    }
    else {
      mx_extensions = 0;
    }
    ::mxSetFieldByNumber(argout, i, 20, mx_extensions);
    //- set field argout[i].disp_level
    mxArray * mx_disp_level = ::mxCreateDoubleMatrix(1,1,mxREAL);
    ::mxGetPr(mx_disp_level)[0] = (double)(*attr_config)[i].disp_level;
    ::mxSetFieldByNumber(argout, i, 21, mx_disp_level);
  } //- for
  //- Release memory (allocated by TANGO)
  delete attr_config;
  //- Set output arg
  if (MEX_ARGS->set_output_array(k1ST_ARG, argout) == kError) 
  {
    ::mxDestroyArray(argout);
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  return kNoError;
}

//=============================================================================
//- TangoBinding::set_attr_config
//=============================================================================
int TangoBinding::set_attr_config (void)
{
  //- get device name from the 2nd MEX-file input arg
  std::string dev;
  if (MEX_ARGS->get_input_string(k2ND_ARG, dev) == kError)
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 

  //- get attribute-config list from the 3rd MEX-file input arg
  const mxArray* attr_config = MEX_ARGS->get_input_array(k3RD_ARG, ::mxSTRUCT_CLASS, 1);
  if (attr_config == 0)
  {
    MEX_UTILS->push_error("invalid argin specified",
                          "invalid attribute list specified (wrong type or dims)",
                          "TangoBinding::set_attr_config");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }

  //- get device descriptor
  DeviceDesc * ddesc = DEV_REP->device_desc(dev);
  if (! ddesc) 
  {
    MEX_UTILS->push_error("com. error",
                          "could not obtain a valid device reference",
                          "TangoBinding::set_attr_config");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }

  //- create and resize a TANGO DeviceAttribute vector 
  Tango::AttributeInfoList attrs_config;
  int len = ::mxGetN(attr_config);
  attrs_config.resize(len);
  //- each entry in the input array...
  char * cstr = 0;
  mxArray * mx_array = 0;
  for (int i = 0; i < len; i++) 
  {
    //- FIELD: NAME -------------------------------------------------------
    //- struct attr_config[i] should contains a <name> field
    mx_array = ::mxGetField(attr_config, i, "name");
    if (! mx_array) 
    {
      MEX_UTILS->set_error("invalid argin specified",
                           "struct should contain a field named 'name'",
                           "TangoBinding::set_attr_config");
      SET_DEFAULT_PRHS_THEN_RETURN(kError); 
    }
    //- this field must be 1-by-n char array
    if (::mxIsChar(mx_array) == false || ::mxGetM(mx_array) != 1) 
    {
      MEX_UTILS->set_error("invalid argin specified",
                           "1-by-n char array expected for field 'name'",
                           "TangoBinding::set_attr_config");
      SET_DEFAULT_PRHS_THEN_RETURN(kError); 
    }
    //- get string from mxArray
    cstr = ::mxArrayToString(mx_array);
    if (! cstr) 
    {
      MEX_UTILS->set_error("internal error", 
                           "could not extract string from field 'name'",
                           "TangoBinding::set_attr_config");
      SET_DEFAULT_PRHS_THEN_RETURN(kError); 
    }
    attrs_config[i].name = cstr;
    //- free memory
    ::mxFree(cstr);
    //- FIELD: WRITABLE ---------------------------------------------------
    //- struct attr_config[i] should contains a <writable> field
    mx_array = ::mxGetField(attr_config, i, "writable");
    if (! mx_array) 
    {
      MEX_UTILS->set_error("invalid argin specified",
                           "struct should contain a field named 'writable'",
                           "TangoBinding::set_attr_config");
      SET_DEFAULT_PRHS_THEN_RETURN(kError); 
    }
    //- this field must be 1-by-1 int array
    if (::mxIsNumeric(mx_array) == false || ::mxGetM(mx_array) != 1 || ::mxGetN(mx_array) != 1)
    {
      MEX_UTILS->set_error("invalid argin specified",
                           "1-by-1 int array expected for field 'writable'",
                           "TangoBinding::set_attr_config");
      SET_DEFAULT_PRHS_THEN_RETURN(kError); 
    }
    //- get int from mxArray
    attrs_config[i].writable = static_cast<Tango::AttrWriteType>(::mxGetScalar(mx_array));
    //- FIELD: DATA_FORMAT ------------------------------------------------
    //- struct attr_config[i] should contains a <data_format> field
    mx_array = ::mxGetField(attr_config, i, "data_format");
    if (! mx_array) 
    {
      MEX_UTILS->set_error("invalid argin specified",
                           "struct should contain a field named 'data_format'",
                           "TangoBinding::set_attr_config");
      SET_DEFAULT_PRHS_THEN_RETURN(kError); 
    }
    //- this field must be 1-by-1 int array
    if (::mxIsNumeric(mx_array) == false || ::mxGetM(mx_array) != 1 || ::mxGetN(mx_array) != 1)
    {
      MEX_UTILS->set_error("invalid argin specified",
                           "1-by-1 int array expected for field 'data_format'",
                           "TangoBinding::set_attr_config");
      SET_DEFAULT_PRHS_THEN_RETURN(kError); 
    }
    //- get int from mxArray
    attrs_config[i].data_format = static_cast<Tango::AttrDataFormat>(::mxGetScalar(mx_array));
    //- FIELD: DATA_TYPE --------------------------------------------------
    //- struct attr_config[i] should contains a <data_type> field
    mx_array = ::mxGetField(attr_config, i, "data_type");
    if (! mx_array) 
    {
      MEX_UTILS->set_error("invalid argin specified",
                           "struct should contain a field named 'data_type'",
                           "TangoBinding::set_attr_config");
      SET_DEFAULT_PRHS_THEN_RETURN(kError); 
    }
    //- this field must be 1-by-1 int array
    if (::mxIsNumeric(mx_array) == false || ::mxGetM(mx_array) != 1 || ::mxGetN(mx_array) != 1)
    {
      MEX_UTILS->set_error("invalid argin specified",
                           "1-by-1 int array expected for field 'data_type'",
                           "TangoBinding::set_attr_config");
      SET_DEFAULT_PRHS_THEN_RETURN(kError); 
    }
    //- get int from mxArray
    attrs_config[i].data_type = (int)::mxGetScalar(mx_array);
    //- FIELD: MAX_M ------------------------------------------------------
    //- struct attr_config[i] should contains a <max_m> field
    mx_array = ::mxGetField(attr_config, i, "max_m");
    if (! mx_array) 
    {
      MEX_UTILS->set_error("invalid argin specified",
                           "struct should contain a field named 'max_m'",
                           "TangoBinding::set_attr_config");
      SET_DEFAULT_PRHS_THEN_RETURN(kError); 
    }
    //- this field must be 1-by-1 int array
    if (::mxIsNumeric(mx_array) == false || ::mxGetM(mx_array) != 1 || ::mxGetN(mx_array) != 1)
    {
      MEX_UTILS->set_error("invalid argin specified",
                           "1-by-1 int array expected for field 'max_dim_x'",
                           "TangoBinding::set_attr_config");
      SET_DEFAULT_PRHS_THEN_RETURN(kError); 
    }
    //- get int from mxArray
    int max_m = (int)::mxGetScalar(mx_array);
    //- FIELD: MAX_N ------------------------------------------------------
    //- struct attr_config[i] should contains a <max_n> field
    mx_array = ::mxGetField(attr_config, i, "max_n");
    if (! mx_array) 
    {
      MEX_UTILS->set_error("invalid argin specified",
                           "struct should contain a field named 'max_n'",
                           "TangoBinding::set_attr_config");
      SET_DEFAULT_PRHS_THEN_RETURN(kError); 
    }
    //- this field must be 1-by-1 int array
    if (::mxIsNumeric(mx_array) == false || ::mxGetM(mx_array) != 1 || ::mxGetN(mx_array) != 1)
    {
      MEX_UTILS->set_error("invalid argin specified",
                           "1-by-1 int array expected for field 'max_n'",
                           "TangoBinding::set_attr_config");
      SET_DEFAULT_PRHS_THEN_RETURN(kError); 
    }
    //- get int from mxArray
    int max_n = (int)::mxGetScalar(mx_array);
    //- set max_dim_x and max_dim_y from max_m and max_n
    attrs_config[i].max_dim_x = (int)::mxGetScalar(mx_array);
    attrs_config[i].max_dim_y = (int)::mxGetScalar(mx_array);
    switch (attrs_config[i].data_format)
    {
      case Tango::SCALAR:
        attrs_config[i].max_dim_x = 1;
	attrs_config[i].max_dim_y = 0;
        break;
      case Tango::SPECTRUM:
        attrs_config[i].max_dim_x = max_n;
	attrs_config[i].max_dim_y = 0;
        break;
      case Tango::IMAGE:
        attrs_config[i].max_dim_x = max_m;
	attrs_config[i].max_dim_y = max_n;
        break;
      default:
        attrs_config[i].max_dim_x = 0;
	attrs_config[i].max_dim_y = 0;
        break;
    }
    //- FIELD: DESCRIPTION ------------------------------------------------
    //- struct attr_config[i] should contains a <description> field
    mx_array = ::mxGetField(attr_config, i, "description");
    if (! mx_array) 
    {
      MEX_UTILS->set_error("invalid argin specified",
                           "struct should contain a field named 'description'",
                           "TangoBinding::set_attr_config");
      SET_DEFAULT_PRHS_THEN_RETURN(kError); 
    }
    //- this field must be 1-by-n char array
    if (::mxIsChar(mx_array) == false || ::mxGetM(mx_array) != 1) 
    {
      MEX_UTILS->set_error("invalid argin specified",
                           "1-by-n char array expected for field 'description'",
                           "TangoBinding::set_attr_config");
      SET_DEFAULT_PRHS_THEN_RETURN(kError); 
    }
    //- get string from mxArray
    cstr = ::mxArrayToString(mx_array);
    if (! cstr) 
    {
      MEX_UTILS->set_error("internal error", 
                           "could not extract string from field 'description'",
                           "TangoBinding::set_attr_config");
      SET_DEFAULT_PRHS_THEN_RETURN(kError); 
    }
    attrs_config[i].description = cstr;
    //- free memory
    ::mxFree(cstr);
    //- FIELD: LABEL ------------------------------------------------------
    //- struct attr_config[i] should contains a <label> field
    mx_array = ::mxGetField(attr_config, i, "label");
    if (! mx_array) 
    {
      MEX_UTILS->set_error("invalid argin specified",
                           "struct should contain a field named 'label'",
                           "TangoBinding::set_attr_config");
      SET_DEFAULT_PRHS_THEN_RETURN(kError); 
    }
    //- this field must be 1-by-n char array
    if (::mxIsChar(mx_array) == false || ::mxGetM(mx_array) != 1) 
    {
      MEX_UTILS->set_error("invalid argin specified",
                           "1-by-n char array expected for field 'label'",
                           "TangoBinding::set_attr_config");
      SET_DEFAULT_PRHS_THEN_RETURN(kError); 
    }
    //- get string from mxArray
    cstr = ::mxArrayToString(mx_array);
    if (! cstr) 
    {
      MEX_UTILS->set_error("internal error", 
                           "could not extract string from field 'label'",
                           "TangoBinding::set_attr_config");
      SET_DEFAULT_PRHS_THEN_RETURN(kError); 
    }
    attrs_config[i].label = cstr;
    //- free memory
    ::mxFree(cstr);
    //- FIELD: UNIT -------------------------------------------------------
    //- struct attr_config[i] should contains a <unit> field
    mx_array = ::mxGetField(attr_config, i, "unit");
    if (! mx_array) 
    {
      MEX_UTILS->set_error("invalid argin specified",
                           "struct should contain a field named 'unit'",
                           "TangoBinding::set_attr_config");
      SET_DEFAULT_PRHS_THEN_RETURN(kError); 
    }
    //- this field must be 1-by-n char array
    if (::mxIsChar(mx_array) == false)
    {
      MEX_UTILS->set_error("invalid argin specified",
                           "1-by-n char array expected for field 'unit'",
                           "TangoBinding::set_attr_config");
      SET_DEFAULT_PRHS_THEN_RETURN(kError); 
    }
    //- get string from mxArray
    cstr = ::mxArrayToString(mx_array);
    if (! cstr) 
    {
      MEX_UTILS->set_error("internal error", 
                           "could not extract string from field 'unit'",
                           "TangoBinding::set_attr_config");
      SET_DEFAULT_PRHS_THEN_RETURN(kError); 
    }
    attrs_config[i].unit = cstr;
    //- free memory
    ::mxFree(cstr);
    //- FIELD: STANDARD_UNIT ----------------------------------------------
    //- struct attr_config[i] should contains a <standard_unit> field
    mx_array = ::mxGetField(attr_config, i, "standard_unit");
    if (! mx_array) 
    {
      MEX_UTILS->set_error("invalid argin specified",
                           "struct should contain a field named 'standard_unit'",
                           "TangoBinding::set_attr_config");
      SET_DEFAULT_PRHS_THEN_RETURN(kError); 
    }
    //- this field must be 1-by-n char array
    if (::mxIsChar(mx_array) == false || ::mxGetM(mx_array) != 1) 
    {
      MEX_UTILS->set_error("invalid argin specified",
                           "1-by-n char array expected for field 'standard_unit'",
                           "TangoBinding::set_attr_config");
      SET_DEFAULT_PRHS_THEN_RETURN(kError); 
    }
    //- get string from mxArray
    cstr = ::mxArrayToString(mx_array);
    if (! cstr) 
    {
      MEX_UTILS->set_error("internal error", 
                                 "could not extract string from field 'standard_unit'",
                                 "TangoBinding::set_attr_config");
      SET_DEFAULT_PRHS_THEN_RETURN(kError); 
    }
    attrs_config[i].standard_unit = cstr;
    //- free memory
    ::mxFree(cstr);
    //- FIELD: DISPLAY_UNIT -----------------------------------------------
    //- struct attr_config[i] should contains a <display_unit> field
    mx_array = ::mxGetField(attr_config, i, "display_unit");
    if (! mx_array) 
    {
      MEX_UTILS->set_error("invalid argin specified",
                           "struct should contain a field named 'display_unit'",
                           "TangoBinding::set_attr_config");
      SET_DEFAULT_PRHS_THEN_RETURN(kError); 
    }
    //- this field must be 1-by-n char array
    if (::mxIsChar(mx_array) == false || ::mxGetM(mx_array) != 1) 
    {
      MEX_UTILS->set_error("invalid argin specified",
                           "1-by-n char array expected for field 'display_unit'",
                           "TangoBinding::set_attr_config");
      SET_DEFAULT_PRHS_THEN_RETURN(kError); 
    }
    //- get string from mxArray
    cstr = ::mxArrayToString(mx_array);
    if (! cstr) 
    {
      MEX_UTILS->set_error("internal error", 
                           "could not extract string from field 'display_unit'",
                           "TangoBinding::set_attr_config");
      SET_DEFAULT_PRHS_THEN_RETURN(kError); 
    }
    attrs_config[i].display_unit = cstr;
    //- free memory
    ::mxFree(cstr);
    //- FIELD: FORMAT -----------------------------------------------
    //- struct attr_config[i] should contains a <format> field
    mx_array = ::mxGetField(attr_config, i, "format");
    if (! mx_array) 
    {
      MEX_UTILS->set_error("invalid argin specified",
                           "struct should contain a field named 'format'",
                           "TangoBinding::set_attr_config");
      SET_DEFAULT_PRHS_THEN_RETURN(kError); 
    }
    //- this field must be 1-by-n char array
    if (::mxIsChar(mx_array) == false || ::mxGetM(mx_array) != 1) 
    {
      MEX_UTILS->set_error("invalid argin specified",
                                 "1-by-n char array expected for field 'format'",
                                 "TangoBinding::set_attr_config");
      SET_DEFAULT_PRHS_THEN_RETURN(kError); 
    }
    //- get string from mxArray
    cstr = ::mxArrayToString(mx_array);
    if (! cstr) 
    {
      MEX_UTILS->set_error("internal error", 
                           "could not extract string from field 'format'",
                           "TangoBinding::set_attr_config");
      SET_DEFAULT_PRHS_THEN_RETURN(kError); 
    }
    attrs_config[i].format = cstr;
    //- free memory
    ::mxFree(cstr);
    //- FIELD: MIN_VALUE --------------------------------------------------
    //- struct attr_config[i] should contains a <min_value> field
    mx_array = ::mxGetField(attr_config, i, "min_value");
    if (! mx_array) 
    {
      MEX_UTILS->set_error("invalid argin specified",
                           "struct should contain a field named 'min_value'",
                           "TangoBinding::set_attr_config");
      SET_DEFAULT_PRHS_THEN_RETURN(kError); 
    }
    //- this field must be 1-by-n char array
    if (::mxIsChar(mx_array) == false || ::mxGetM(mx_array) != 1) 
    {
      MEX_UTILS->set_error("invalid argin specified",
                           "1-by-n char array expected for field 'min_value'",
                           "TangoBinding::set_attr_config");
      SET_DEFAULT_PRHS_THEN_RETURN(kError); 
    }
    //- get string from mxArray
    cstr = ::mxArrayToString(mx_array);
    if (! cstr) 
    {
      MEX_UTILS->set_error("internal error", 
                           "could not extract string from field 'min_value'",
                           "TangoBinding::set_attr_config");
      SET_DEFAULT_PRHS_THEN_RETURN(kError); 
    }
    attrs_config[i].min_value = cstr;
    //- free memory
    ::mxFree(cstr);
    //- FIELD: MAX_VALUE --------------------------------------------------
    //- struct attr_config[i] should contains a <max_value> field
    mx_array = ::mxGetField(attr_config, i, "max_value");
    if (! mx_array) 
    {
      MEX_UTILS->set_error("invalid argin specified",
                           "struct should contain a field named 'max_value'",
                           "TangoBinding::set_attr_config");
      SET_DEFAULT_PRHS_THEN_RETURN(kError); 
    }
    //- this field must be 1-by-n char array
    if (::mxIsChar(mx_array) == false || ::mxGetM(mx_array) != 1) 
    {
      MEX_UTILS->set_error("invalid argin specified",
                           "1-by-n char array expected for field 'max_value'",
                           "TangoBinding::set_attr_config");
      SET_DEFAULT_PRHS_THEN_RETURN(kError); 
    }
    //- get string from mxArray
    cstr = ::mxArrayToString(mx_array);
    if (! cstr) 
    {
      MEX_UTILS->set_error("internal error", 
                           "could not extract string from field 'max_value'",
                           "TangoBinding::set_attr_config");
      SET_DEFAULT_PRHS_THEN_RETURN(kError); 
    }
    attrs_config[i].max_value = cstr;
    //- free memory
    ::mxFree(cstr);
    //- FIELD: MIN_ALARM --------------------------------------------------
    //- struct attr_config[i] should contains a <min_alarm> field
    mx_array = ::mxGetField(attr_config, i, "min_alarm");
    if (! mx_array) 
    {
      MEX_UTILS->set_error("invalid argin specified",
                           "struct should contain a field named 'min_alarm'",
                           "TangoBinding::set_attr_config");
      SET_DEFAULT_PRHS_THEN_RETURN(kError); 
    }
    //- this field must be 1-by-n char array
    if (::mxIsChar(mx_array) == false || ::mxGetM(mx_array) != 1) 
    {
      MEX_UTILS->set_error("invalid argin specified",
                           "1-by-n char array expected for field 'min_alarm'",
                           "TangoBinding::set_attr_config");
      SET_DEFAULT_PRHS_THEN_RETURN(kError); 
    }
    //- get string from mxArray
    cstr = ::mxArrayToString(mx_array);
    if (! cstr) 
    {
      MEX_UTILS->set_error("internal error", 
                           "could not extract string from field 'min_alarm'",
                           "TangoBinding::set_attr_config");
      SET_DEFAULT_PRHS_THEN_RETURN(kError); 
    }
    attrs_config[i].min_alarm = cstr;
    //- free memory
    ::mxFree(cstr);
    //- FIELD: MAX_ALARM --------------------------------------------------
    //- struct attr_config[i] should contains a <max_alarm> field
    mx_array = ::mxGetField(attr_config, i, "max_alarm");
    if (! mx_array) 
    {
      MEX_UTILS->set_error("invalid argin specified",
                           "struct should contain a field named 'max_alarm'",
                           "TangoBinding::set_attr_config");
      SET_DEFAULT_PRHS_THEN_RETURN(kError); 
    }
    //- this field must be 1-by-n char array
    if (::mxIsChar(mx_array) == false || ::mxGetM(mx_array) != 1) 
    {
      MEX_UTILS->set_error("invalid argin specified",
                           "1-by-n char array expected for field 'max_alarm'",
                           "TangoBinding::set_attr_config");
      SET_DEFAULT_PRHS_THEN_RETURN(kError); 
    }
    //- get string from mxArray
    cstr = ::mxArrayToString(mx_array);
    if (! cstr) 
    {
      MEX_UTILS->set_error("internal error", 
                           "could not extract string from field 'max_alarm'",
                           "TangoBinding::set_attr_config");
      SET_DEFAULT_PRHS_THEN_RETURN(kError); 
    }
    attrs_config[i].max_alarm = cstr;
    //- free memory
    ::mxFree(cstr);
    //- FIELD: WRITABLE_ATTR_NAME -----------------------------------------
    //- struct attr_config[i] should contains a <writable_attr_name> field
    mx_array = ::mxGetField(attr_config, i, "writable_attr_name");
    if (! mx_array) 
    {
      MEX_UTILS->set_error("invalid argin specified",
                           "struct should contain a field named 'writable_attr_name'",
                           "TangoBinding::set_attr_config");
      SET_DEFAULT_PRHS_THEN_RETURN(kError); 
    }
    //- this field must be 1-by-n char array
    if (::mxIsChar(mx_array) == false || ::mxGetM(mx_array) != 1) 
    {
      MEX_UTILS->set_error("invalid argin specified",
                           "1-by-n char array expected for field 'writable_attr_name'",
                           "TangoBinding::set_attr_config");
      SET_DEFAULT_PRHS_THEN_RETURN(kError); 
    }
    //- get string from mxArray
    cstr = ::mxArrayToString(mx_array);
    if (! cstr) 
    {
      MEX_UTILS->set_error("internal error", 
                           "could not extract string from field 'writable_attr_name'",
                           "TangoBinding::set_attr_config");
      SET_DEFAULT_PRHS_THEN_RETURN(kError); 
    }
    attrs_config[i].writable_attr_name = cstr;
    //- free memory
    ::mxFree(cstr);
    //- FIELD: EXTENSIONS -------------------------------------------------
    //- struct attr_config[i] should contains a <extension> field
    mx_array = ::mxGetField(attr_config, i, "extensions");
    if (mx_array == 0 || ::mxGetN(mx_array) == 0) 
    {
      attrs_config[i].extensions.resize(0);
    }
    else 
    {
      //- this field must be 1-by-n cell array
      if (::mxIsCell(mx_array) == false || ::mxGetM(mx_array) != 1) 
      {
        MEX_UTILS->set_error("invalid argin specified",
                             "1-by-n cell array expected for field 'extensions'",
                             "TangoBinding::set_attr_config");
        SET_DEFAULT_PRHS_THEN_RETURN(kError); 
      }
      //- resize the attrs_config[i].extensions vector
      int len = ::mxGetN(mx_array); 
      attrs_config[i].extensions.resize(len);
      //- transfer strings from cells to std::vector 
      mxArray * cell = 0;
      for (int j = 0; j < len; i++) 
      {
        //- get <i>th cell of the array
        cell = ::mxGetCell(mx_array, j);
        if (! cell) 
        {
          MEX_UTILS->set_error("internal error",
                               "unexpected NULL cell",
                               "TangoBinding::set_attr_config");
          SET_DEFAULT_PRHS_THEN_RETURN(kError); 
        }
        //- cell should contain a string
        if (::mxIsChar(cell) == false) 
        {
          MEX_UTILS->set_error("internal error",
                               "unexpected cell content (string expected)",
                               "TangoBinding::set_attr_config");
          SET_DEFAULT_PRHS_THEN_RETURN(kError); 
        }
        //- get c-string from matlab char array.
        cstr = ::mxArrayToString(cell);
        if (! cstr) 
        {
          MEX_UTILS->set_error("internal error", 
                               "could not extract string from cell",
                               "TangoBinding::set_attr_config");
          SET_DEFAULT_PRHS_THEN_RETURN(kError); 
        }
        //- store the ith extension
        attrs_config[i].extensions[j] = cstr;
        //- free memory
        ::mxFree(cstr);
      } //- for j ...
    } //- if... else ...  
    //- FIELD: DISP_LEVEL -------------------------------------------------
    //- struct attr_config[i] should contains a <disp_level> field
    mx_array = ::mxGetField(attr_config, i, "disp_level");
    if (! mx_array) 
    {
      MEX_UTILS->set_error("invalid argin specified",
                           "struct should contain a field named 'disp_level'",
                           "TangoBinding::set_attr_config");
      SET_DEFAULT_PRHS_THEN_RETURN(kError); 
    }
    //- this field must be 1-by-1 int array
    if (::mxIsNumeric(mx_array) == false || ::mxGetM(mx_array) != 1 || ::mxGetN(mx_array) != 1)
    {
      MEX_UTILS->set_error("invalid argin specified",
                           "1-by-1 int array expected for field 'disp_level'",
                           "TangoBinding::set_attr_config");
      SET_DEFAULT_PRHS_THEN_RETURN(kError); 
    }
    //- get int from mxArray
    attrs_config[i].disp_level = static_cast<Tango::DispLevel>(::mxGetScalar(mx_array));
  } //- for i ...

  _TRY(ddesc->proxy()->set_attribute_config(attrs_config), dev, "set_attr_config");

  return kNoError;
}

//=============================================================================
//- TangoBinding::black_box
//=============================================================================
int TangoBinding::black_box (void)
{
  //- get <device name> from the 2nd MEX-file input arg
  std::string dev;
  if (MEX_ARGS->get_input_string(k2ND_ARG, dev) == kError)
  {
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  //- get black-box len from the 3rd MEX-file input arg
  const mxArray* mx_bb_len = MEX_ARGS->get_input_array(k3RD_ARG, ::mxDOUBLE_CLASS, 1, 1);
  if (mx_bb_len == 0)
  {
    MEX_UTILS->push_error("invalid argin specified",
                          "invalid argument specified (wrong type or dims)",
                          "TangoBinding::black_box");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  //- Store bb_len locally  
  Tango::DevLong bb_len = (Tango::DevLong)::mxGetPr(mx_bb_len)[0];
  //- get device descriptor
  DeviceDesc * ddesc = DEV_REP->device_desc(dev);
  if (! ddesc)
  {
    MEX_UTILS->push_error("com. error",
                          "could not obtain a valid device reference",
                          "TangoBinding::black_box");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  //- get the device black box
  std::vector<std::string>* bb_vector;
  _TRY(bb_vector = ddesc->proxy()->black_box(bb_len), dev, "black_box");
  //- Convert <bb_vector> to cell array
  int len = bb_vector->size();
  const mwSize dims[2] = {1, static_cast<mwSize>(len)};
  mxArray * argout  = ::mxCreateCellArray(2, dims);
  if (! argout)
  {
    MEX_UTILS->set_error("out of memory",
                         "mxCreateCellArray failed",
                         "TangoBinding::black_box");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  for (int i = 0; i < len; i++)
  {
    ::mxSetCell(argout, i, ::mxCreateString((*bb_vector)[i].c_str()));        
  }
  //- Release memory (allocated by TANGO API)
  delete bb_vector;
  //- Set output arg
  if (MEX_ARGS->set_output_array(k1ST_ARG, argout) == kError)
  {
    ::mxDestroyArray(argout);
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  return kNoError;
}

//=============================================================================
//- TangoBinding::command_list_query
//=============================================================================
int TangoBinding::command_list_query (void)
{
  //- get <device name> from the 2nd MEX-file input arg
  std::string dev;
  if (MEX_ARGS->get_input_string(k2ND_ARG, dev) == kError)
  {
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  //- get device descriptor
  DeviceDesc * ddesc = DEV_REP->device_desc(dev);
  if (! ddesc)
  {
    MEX_UTILS->push_error("com. error",
                          "could not obtain a valid device reference",
                          "TangoBinding::command_list_query");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  //- get the device black box
  Tango::CommandInfoList *ci_vector;
  _TRY(ci_vector = ddesc->proxy()->command_list_query(), dev, "command_list_query");
  //- Convert <ci_vector> to struct array
  if (ci_vector == 0)
  {
    MEX_UTILS->set_error("internal error",
                         "command_list_query failed",
                         "TangoBinding::command_list_query");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  //- Build argout: 1-by-len struct array
  int len = ci_vector->size();
  const mwSize dims[2] = {1, static_cast<mwSize>(len)};
  //- Containing the following fields:
  const char *field_names[] =
  {
    "cmd_name",
    "cmd_tag",
    "in_type",
    "in_type_desc",
    "out_type",
    "out_type_desc",
    "disp_level",
    "disp_level_str",
    "devin_type",
    "devout_type"
  };
  mxArray * argout = ::mxCreateStructArray(2, dims, 10, field_names);
  if (! argout)
  {
    MEX_UTILS->set_error("out of memory",
                         "mxCreateStructArray failed",
                         "TangoBinding::command_list_query");
    delete ci_vector;
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  mxArray * mx_array = 0; 
  for (int i = 0; i < len; i++)
  {
    //- set field argout[i].cmd_name
    ::mxSetFieldByNumber(argout, i, 0, ::mxCreateString((*ci_vector)[i].cmd_name.c_str()));
    //- set field argout[i].cmd_tag
    mx_array = ::mxCreateDoubleMatrix(1,1,mxREAL);
    ::mxGetPr(mx_array)[0] = (double)((*ci_vector)[i].cmd_tag);
    ::mxSetFieldByNumber(argout, i, 1, mx_array);
    //- set field argout[i].in_type
    ::mxSetFieldByNumber(argout, i, 2, ::mxCreateString(lk_matlab_type_name[(*ci_vector)[i].in_type]));
    //- set field argout[i].in_type_desc
    ::mxSetFieldByNumber(argout, i, 3, ::mxCreateString((*ci_vector)[i].in_type_desc.c_str()));
    //- set field argout[i].out_type
    ::mxSetFieldByNumber(argout, i, 4, ::mxCreateString(lk_matlab_type_name[(*ci_vector)[i].out_type]));
    //- set field argout[i].out_type_desc
    ::mxSetFieldByNumber(argout, i, 5, ::mxCreateString((*ci_vector)[i].out_type_desc.c_str()));
    //- set field argout[i].disp_level
    mx_array = ::mxCreateDoubleMatrix(1,1,mxREAL);
    ::mxGetPr(mx_array)[0] = (double)((*ci_vector)[i].disp_level);
    ::mxSetFieldByNumber(argout, i, 6, mx_array);
    //- set field argout[i].disp_level_str
    switch ((*ci_vector)[i].disp_level)
    {
      case Tango::OPERATOR:
        ::mxSetFieldByNumber(argout, i, 7, ::mxCreateString("Operator"));
        break;
      case Tango::EXPERT:
        ::mxSetFieldByNumber(argout, i, 7, ::mxCreateString("Expert"));
        break;
      default:
        ::mxSetFieldByNumber(argout, i, 7, ::mxCreateString("Unknown"));
        break;
    }
      mx_array=::mxCreateDoubleMatrix(1, 1, mxREAL);
      ::mxGetPr(mx_array)[0] = (double)((*ci_vector)[i].in_type);
      ::mxSetFieldByNumber(argout, i, 8, mx_array);
      mx_array=::mxCreateDoubleMatrix(1, 1, mxREAL);
      ::mxGetPr(mx_array)[0] = (double)((*ci_vector)[i].out_type);
      ::mxSetFieldByNumber(argout, i, 9, mx_array);
  }
  //- Release memory (allocated by TANGO API)
  delete ci_vector;
  //- Set output arg
  if (MEX_ARGS->set_output_array(k1ST_ARG, argout) == kError)
  {
    ::mxDestroyArray(argout);
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  return kNoError;
}

//=============================================================================
//- TangoBinding::command_query
//=============================================================================
int TangoBinding::command_query (void)
{
  //- get <device name> from the 2nd MEX-file input arg
  std::string dev;
  if (MEX_ARGS->get_input_string(k2ND_ARG, dev) == kError)
  {
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  //- get command name from the 3rd MEX-file input arg
  std::string cmd_name;
  if (MEX_ARGS->get_input_string(k3RD_ARG, cmd_name) == kError)
  {
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  //- get device descriptor
  DeviceDesc * ddesc = DEV_REP->device_desc(dev);
  if (! ddesc)
  {
    MEX_UTILS->push_error("com. error",
                          "could not obtain a valid device reference",
                          "TangoBinding::command_list_query");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  //- get the device black box
  Tango::CommandInfo cmd_info;
  _TRY(cmd_info = ddesc->proxy()->command_query(cmd_name), dev, "command_query");
  //- Build argout: 1-by-1 struct array
  const mwSize dims[2] = {1, 1};
  //- Containing the following fields:
  const char *field_names[] =
  {
    "cmd_name",
    "cmd_tag",
    "in_type",
    "in_type_desc",
    "out_type",
    "out_type_desc",
    "disp_level",
    "disp_level_str",
    "devin_type",
    "devout_type",
  };
  mxArray * argout = ::mxCreateStructArray(2, dims, 10, field_names);
  if (! argout)
  {
    MEX_UTILS->set_error("out of memory",
                         "mxCreateStructArray failed",
                         "TangoBinding::command_query");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  mxArray * mx_array = 0; 
  //- set field argout.cmd_name
  ::mxSetFieldByNumber(argout, 0, 0, ::mxCreateString(cmd_info.cmd_name.c_str()));
  //- set field argout.cmd_tag
  mx_array = ::mxCreateDoubleMatrix(1,1,mxREAL);
  ::mxGetPr(mx_array)[0] = (double)(cmd_info.cmd_tag);
  ::mxSetFieldByNumber(argout, 0, 1, mx_array);
  //- set field argout.in_type
  ::mxSetFieldByNumber(argout, 0, 2, ::mxCreateString(lk_matlab_type_name[cmd_info.in_type]));
  //- set field argout.in_type_desc
  ::mxSetFieldByNumber(argout, 0, 3, ::mxCreateString(cmd_info.in_type_desc.c_str()));
  //- set field argout.out_type
  ::mxSetFieldByNumber(argout, 0, 4, ::mxCreateString(lk_matlab_type_name[cmd_info.out_type]));
  //- set field argout.out_type_desc
  ::mxSetFieldByNumber(argout, 0, 5, ::mxCreateString(cmd_info.out_type_desc.c_str()));
  //- set field argout.disp_level
  mx_array = ::mxCreateDoubleMatrix(1,1,mxREAL);
  ::mxGetPr(mx_array)[0] = (double)(cmd_info.disp_level);
  ::mxSetFieldByNumber(argout, 0, 6, mx_array);
  //- set field argout.disp_level_str
  switch (cmd_info.disp_level)
  {
    case Tango::OPERATOR:
      ::mxSetFieldByNumber(argout, 0, 7, ::mxCreateString("Operator"));
      break;
    case Tango::EXPERT:
      ::mxSetFieldByNumber(argout, 0, 7, ::mxCreateString("Expert"));
      break;
    default:
      ::mxSetFieldByNumber(argout, 0, 7, ::mxCreateString("Unknown"));
      break;
  }
    mx_array=::mxCreateDoubleMatrix(1, 1, mxREAL);
    ::mxGetPr(mx_array)[0] = (double)(cmd_info.in_type);
    ::mxSetFieldByNumber(argout, 0, 8, mx_array);
    mx_array=::mxCreateDoubleMatrix(1, 1, mxREAL);
    ::mxGetPr(mx_array)[0] = (double)(cmd_info.out_type);
    ::mxSetFieldByNumber(argout, 0, 9, mx_array);
    
  //- Set output arg
  if (MEX_ARGS->set_output_array(k1ST_ARG, argout) == kError)
  {
    ::mxDestroyArray(argout);
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  return kNoError;
}

//=============================================================================
//- TangoBinding::set_timeout
//=============================================================================
int TangoBinding::set_timeout (void)
{
  //- get <device name> from the 2nd MEX-file input arg
  std::string dev;
  if (MEX_ARGS->get_input_string(k2ND_ARG, dev) == kError)
  {
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  //- get timeout from the 3rd MEX-file input arg
  const mxArray* mx_tmo = MEX_ARGS->get_input_array(k3RD_ARG, ::mxDOUBLE_CLASS, 1, 1);
  if (mx_tmo == 0)
  {
    MEX_UTILS->push_error("invalid argin specified",
                          "invalid argument specified (wrong type or dims)",
                          "TangoBinding::set_timeout");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  //- Store bb_len locally  
  int tmo = (int)::mxGetPr(mx_tmo)[0];
  //- get device descriptor
  DeviceDesc * ddesc = DEV_REP->device_desc(dev);
  if (! ddesc)
  {
    MEX_UTILS->push_error("com. error",
                          "could not obtain a valid device reference",
                          "TangoBinding::set_timeout");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  //- Change device timeout
  _TRY(ddesc->proxy()->set_timeout_millis(tmo), dev, "set_timeout");

  return kNoError;
}

//=============================================================================
//- TangoBinding::get_timeout
//=============================================================================
int TangoBinding::get_timeout (void)
{
  //- get <device name> from the 2nd MEX-file input arg
  std::string dev;
  if (MEX_ARGS->get_input_string(k2ND_ARG, dev) == kError)
  {
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  //- get device descriptor
  DeviceDesc * ddesc = DEV_REP->device_desc(dev);
  if (! ddesc)
  {
    MEX_UTILS->push_error("com. error",
                          "could not obtain a valid device reference",
                          "TangoBinding::get_timeout");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  //- get device timeout
  int tmo = 0;
  _TRY(tmo = ddesc->proxy()->get_timeout_millis(), dev, "get_timeout");
  //- Create a 1-by-1 double array
  mxArray * argout = ::mxCreateDoubleMatrix(1,1,mxREAL);
  if (! argout)
  {
    MEX_UTILS->set_error("out of memory",
                         "mxCreateDoubleMatrix failed",
                         "TangoBinding::get_timeout");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  //- Store device timeout
  ::mxGetPr(argout)[0] = tmo;
  //- Set output arg
  if (MEX_ARGS->set_output_array(k1ST_ARG, argout) == kError)
  {
    ::mxDestroyArray(argout);
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  return kNoError;
}

//=============================================================================
//- TangoBinding::set_source
//=============================================================================
int TangoBinding::set_source (void)
{
  //- get <device name> from the 2nd MEX-file input arg
  std::string dev;
  if (MEX_ARGS->get_input_string(k2ND_ARG, dev) == kError)
  {
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  //- get source from the 3rd MEX-file input arg
  const mxArray* mx_source = MEX_ARGS->get_input_array(k3RD_ARG);
  if (mx_source == 0)
  {
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  //- <mx_source> can be a 1-by-1 double array
  int is_1x1_db = 0;
  if (::mxIsDouble(mx_source) == true && ::mxGetM(mx_source) == 1 && ::mxGetN(mx_source) == 1)
  {
    is_1x1_db = 1;
  }
  //- or a 1-by-n char array
  int is_1xn_char = 0;
  if (is_1x1_db == 0 && ::mxIsChar(mx_source) == true && ::mxGetM(mx_source) == 1)
  {
    is_1xn_char = 1;
  }
  //- but at leats one of the two allowed types
  if (is_1x1_db == 0 && is_1xn_char == 0)
  {
    MEX_UTILS->set_error("invalid argin specified",
                         "1-by-1 double or 1-by-n char array expected",
                         "TangoBinding::set_source");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }

  //- get source from input arg
  Tango::DevSource dev_source;
  if (is_1x1_db)
  {
    dev_source = (Tango::DevSource)((int)::mxGetPr(mx_source)[0]);
    if (dev_source < Tango::DEV || dev_source > Tango::CACHE_DEV)
    {
      MEX_UTILS->set_error("invalid argin specified",
                           "invalid source specified",
                           "TangoBinding::set_source");
      SET_DEFAULT_PRHS_THEN_RETURN(kError); 
    }
  }
  else
  {
    char* cstr = ::mxArrayToString(mx_source);
    std::string std_str(cstr);
    ::mxFree(cstr);
    std::transform(std_str.begin(), std_str.end(), std_str.begin(), ::tolower);
    do
    {
      if (std_str == "cache-device")
      {
              dev_source = Tango::CACHE_DEV;
              break;
      }
      if (std_str == "cache")
      {
              dev_source = Tango::CACHE;
              break;
      }
      if (std_str == "device")
      {
              dev_source = Tango::DEV;
              break;
      }
      MEX_UTILS->set_error("invalid argin specified",
                           "invalid source specified ('Device', 'Cache' or 'Cache-Device' allowed)",
                           "TangoBinding::set_source");
      SET_DEFAULT_PRHS_THEN_RETURN(kError); 
    }
    while (0);
  }
  //- get device descriptor
  DeviceDesc * ddesc = DEV_REP->device_desc(dev);
  if (! ddesc)
  {
    MEX_UTILS->push_error("com. error",
                          "could not obtain a valid device reference",
                          "TangoBinding::set_source");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  //- Change device source
  _TRY(ddesc->proxy()->set_source(dev_source), dev, "set_source");
  return kNoError;
}

//=============================================================================
//- TangoBinding::get_source
//=============================================================================
int TangoBinding::get_source (void)
{
  //- get <device name> from the 2nd MEX-file input arg
  std::string dev;
  if (MEX_ARGS->get_input_string(k2ND_ARG, dev) == kError)
  {
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  //- get device descriptor
  DeviceDesc * ddesc = DEV_REP->device_desc(dev);
  if (! ddesc)
  {
    MEX_UTILS->push_error("com. error",
                          "could not obtain a valid device reference",
                          "TangoBinding::get_source");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  //- get device timeout
  Tango::DevSource dev_source;
  _TRY(dev_source = ddesc->proxy()->get_source(), dev, "get_source");
  //- Build argout: 1-by-1 struct array
  const mwSize dims[2] = {1, 1};
  //- Containing the following fields:
  const char *field_names[] =
  {
     "value", 
     "name"
  };
  mxArray * argout = ::mxCreateStructArray(2, dims, 2, field_names);
  if (! argout)
  {
    MEX_UTILS->set_error("out of memory",
                         "mxCreateStructArray failed",
                         "TangoBinding::get_source");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  //- set field argout.value
  mxArray * mx_value = ::mxCreateDoubleMatrix(1,1,mxREAL);
  ::mxGetPr(mx_value)[0] = dev_source;
  ::mxSetFieldByNumber(argout, 0, 0, mx_value);
  //- set field argout.name
  switch (dev_source)
  {
    case Tango::DEV:
      ::mxSetFieldByNumber(argout, 0, 1, ::mxCreateString("Device"));
      break;
    case Tango::CACHE:
      ::mxSetFieldByNumber(argout, 0, 1, ::mxCreateString("Cache"));
      break;
    case Tango::CACHE_DEV:
      ::mxSetFieldByNumber(argout, 0, 1, ::mxCreateString("Cache-Device"));
      break;
    default:
      ::mxSetFieldByNumber(argout, 0, 1, ::mxCreateString("Unknown"));
      break;
  };
  //- Set output arg
  if (MEX_ARGS->set_output_array(k1ST_ARG, argout) == kError)
  {
    ::mxDestroyArray(argout);
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  return kNoError;
}

//=============================================================================
//- TangoBinding::polling_status
//=============================================================================
int TangoBinding::polling_status (void)
{
  //- get <device name> from the 2nd MEX-file input arg
  std::string dev;
  if (MEX_ARGS->get_input_string(k2ND_ARG, dev) == kError)
  {
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  //- get device descriptor
  DeviceDesc * ddesc = DEV_REP->device_desc(dev);
  if (! ddesc)
  {
    MEX_UTILS->push_error("com. error",
                          "could not obtain a valid device reference",
                          "TangoBinding::polling_status");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  //- get the polling status
  std::vector<std::string> * polling_status = 0;
  _TRY(polling_status = ddesc->proxy()->polling_status(), dev, "polling_status");
  if (polling_status == 0)
  {
    MEX_UTILS->set_error("internal error",
                         "polling_status failed",
                         "TangoBinding::polling_status");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  //- Convert polling_status to a 1-by-n struct array
  int len = polling_status->size();
  const mwSize dims[2] = {1, static_cast<mwSize>(len)};
  //- Containing the following fields:
  const char *field_names[] =
  {
    "ob",
    "pp",
    "bd",
    "le",
    "lu",
    "dt",
    "ex"
  };
  const int num_fields = 7;
  mxArray * argout = ::mxCreateStructArray(2, dims, num_fields, field_names);
  if (! argout)
  {
    MEX_UTILS->set_error("out of memory",
                         "mxCreateStructArray failed",
                         "TangoBinding::command_query");
    delete polling_status;
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  //- get each sub-string and set each field of argout[i]
  int i, j;
  bool done;
  std::string multi_line_str;
  std::string current_line;
  std::string::size_type last_pos;
  std::string::size_type current_pos;
  for (i = 0; i < len; i++)
  {
    //- reset limits
    last_pos = 0;
    current_pos = 0;
    //- store multi-line string in temp var.
    multi_line_str = (*polling_status)[i];
    j = 0;
    done = false;
    do
    {
      //- get sub-string 
      current_pos = multi_line_str.find('\n', last_pos);
      if (current_pos == std::string::npos)
      {
        current_pos = multi_line_str.size();
        done = true;
      }
      current_line = multi_line_str.substr(last_pos, current_pos - last_pos);
      //- set field of argout[i]
      ::mxSetFieldByNumber(argout, i, j, ::mxCreateString(current_line.c_str()));
      if (done == true)
      {
        for (++j; j < num_fields; j++)
        {
          if (j != num_fields - 1)
          {
            ::mxSetFieldByNumber(argout, i, j, ::mxCreateString("no data"));
          }
          else
          { 
            ::mxSetFieldByNumber(argout, i, j, ::mxCreateString("no error"));
          }
        }
      }
      else
      {
        last_pos = ++current_pos;
        j++;
      }
    } while (j < num_fields);
  }
  //-- Release memory (allocated by the TANGO API)
  delete polling_status;
  //- Set output arg
  if (MEX_ARGS->set_output_array(k1ST_ARG, argout) == kError)
  {
    ::mxDestroyArray(argout);
    SET_DEFAULT_PRHS_THEN_RETURN(kError);
  }
  return kNoError;
} 

//=============================================================================
//- TangoBinding::is_command_polled
//=============================================================================
int TangoBinding::is_command_polled (void)
{
  //- get <device name> from the 2nd input arg
  std::string dev;
  if (MEX_ARGS->get_input_string(k2ND_ARG, dev) == kError)
  {
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  //- get command name from the 3rd input arg
  std::string cmd;
  if (MEX_ARGS->get_input_string(k3RD_ARG, cmd) == kError)
  {
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  //- get device descriptor
  DeviceDesc * ddesc = DEV_REP->device_desc(dev);
  if (! ddesc)
  {
    std::string r = "failed to execute is_command_polled on " + dev;
    MEX_UTILS->push_error(r.c_str(),
                          "could not obtain a valid device reference",
                          "TangoBinding::is_command_polled");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  //- Is command polled?
  bool b;
  _TRY(b = ddesc->proxy()->is_command_polled(cmd), dev, "is_command_polled");
  //- Build argout: 1-by-1 double array
  mxArray * argout = ::mxCreateDoubleMatrix(1,1,mxREAL);
  if (! argout)
  {
    MEX_UTILS->set_error("out of memory",
                         "mxCreateDoubleMatrix failed",
                         "TangoBinding::is_command_polled");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  ::mxGetPr(argout)[0] = (double)b;
#ifdef mxSetLogical
  ::mxSetLogical(argout);
#endif
  //- Set output arg
  if (MEX_ARGS->set_output_array(k1ST_ARG, argout) == kError)
  {
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  return kNoError;
}

//=============================================================================
//- TangoBinding::poll_command
//=============================================================================
int TangoBinding::poll_command (void)
{
  //- get <device name> from the 2nd input arg
  std::string dev;
  if (MEX_ARGS->get_input_string(k2ND_ARG, dev) == kError)
  {
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  //- get command name from the 3rd input arg
  std::string cmd;
  if (MEX_ARGS->get_input_string(k3RD_ARG, cmd) == kError)
  {
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  //- get polling period from the 4th MEX-file input arg
  const mxArray* mx_pp = MEX_ARGS->get_input_array(k4TH_ARG, ::mxDOUBLE_CLASS, 1, 1);
  if (mx_pp == 0)
  {
    MEX_UTILS->push_error("invalid argin specified",
                          "invalid argument specified (wrong type or dims)",
                          "TangoBinding::poll_attribute");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  //- Store polling period locally  
  int pp = (int)::mxGetPr(mx_pp)[0];
  //- get device descriptor
  DeviceDesc * ddesc = DEV_REP->device_desc(dev);
  if (! ddesc)
  {
    std::string r = "failed to execute poll_command on " + dev;
    MEX_UTILS->push_error(r.c_str(),
                          "could not obtain a valid device reference",
                          "TangoBinding::poll_command");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  //- Poll the command
  _TRY(ddesc->proxy()->poll_command(cmd, pp), dev, "poll_command");
  return kNoError;
}

//=============================================================================
//- TangoBinding::get_command_poll_period
//=============================================================================
int TangoBinding::get_command_poll_period (void)
{
  //- get <device name> from the 2nd input arg
  std::string dev;
  if (MEX_ARGS->get_input_string(k2ND_ARG, dev) == kError)
  {
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  //- get command from the 3rd input arg
  std::string cmd;
  if (MEX_ARGS->get_input_string(k3RD_ARG, cmd) == kError)
  {
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  //- get device descriptor
  DeviceDesc * ddesc = DEV_REP->device_desc(dev);
  if (! ddesc)
  {
    std::string r = "failed to execute get_command_poll_period on " + dev;
    MEX_UTILS->push_error(r.c_str(),
                          "could not obtain a valid device reference",
                          "TangoBinding::get_command_poll_period");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  //- get attribute polling period
  int pp = 0;
  _TRY(pp = ddesc->proxy()->get_command_poll_period(cmd), dev, "get_command_poll_period");
  //- Build argout: 1-by-1 double array
  mxArray * argout = ::mxCreateDoubleMatrix(1,1,mxREAL);
  if (! argout)
  {
    MEX_UTILS->set_error("out of memory",
                         "mxCreateDoubleMatrix failed",
                         "TangoBinding::get_command_poll_period");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  ::mxGetPr(argout)[0] = (double)pp;
  //- Set output arg
  if (MEX_ARGS->set_output_array(k1ST_ARG, argout) == kError)
  {
    ::mxDestroyArray(argout);
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  return kNoError;
}

//=============================================================================
//- TangoBinding::stop_poll_command
//=============================================================================
int TangoBinding::stop_poll_command (void)
{
  //- get <device name> from the 2nd input arg
  std::string dev;
  if (MEX_ARGS->get_input_string(k2ND_ARG, dev) == kError)
  {
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  //- get command name from the 3rd input arg
  std::string cmd;
  if (MEX_ARGS->get_input_string(k3RD_ARG, cmd) == kError)
  {
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  //- get device descriptor
  DeviceDesc * ddesc = DEV_REP->device_desc(dev);
  if (! ddesc)
  {
    std::string r = "failed to execute stop_poll_command on " + dev;
    MEX_UTILS->push_error(r.c_str(),
                          "could not obtain a valid device reference",
                          "TangoBinding::stop_poll_command");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  //- Stop polling the command
  _TRY(ddesc->proxy()->stop_poll_command(cmd), dev, "stop_poll_command");
  return kNoError;
}

//=============================================================================
//- TangoBinding::is_attribute_polled
//=============================================================================
int TangoBinding::is_attribute_polled (void)
{
  //- get <device name> from the 2nd input arg
  std::string dev;
  if (MEX_ARGS->get_input_string(k2ND_ARG, dev) == kError)
  {
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  //- get attribute from the 3rd input arg
  std::string attr;
  if (MEX_ARGS->get_input_string(k3RD_ARG, attr) == kError)
  {
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  //- get device descriptor
  DeviceDesc * ddesc = DEV_REP->device_desc(dev);
  if (! ddesc)
  {
    std::string r = "failed to execute is_attribute_polled on " + dev;
    MEX_UTILS->push_error(r.c_str(),
                          "could not obtain a valid device reference",
                          "TangoBinding::is_attribute_polled");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  //- Is attribute polled?
  bool b;
  _TRY(b = ddesc->proxy()->is_attribute_polled(attr), dev, "is_attribute_polled");
  //- Build argout: 1-by-1 double array
  mxArray * argout = ::mxCreateDoubleMatrix(1,1,mxREAL);
  if (! argout)
  {
    MEX_UTILS->set_error("out of memory",
                         "mxCreateDoubleMatrix failed",
                         "TangoBinding::is_attribute_polled");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  ::mxGetPr(argout)[0] = (double)b;
#ifdef mxSetLogical
  ::mxSetLogical(argout);
#endif
  //- Set output arg
  if (MEX_ARGS->set_output_array(k1ST_ARG, argout) == kError)
  {
    ::mxDestroyArray(argout);
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  return kNoError;
}

//=============================================================================
//- TangoBinding::poll_attribute
//=============================================================================
int TangoBinding::poll_attribute (void)
{
  //- get <device name> from the 2nd input arg
  std::string dev;
  if (MEX_ARGS->get_input_string(k2ND_ARG, dev) == kError)
  {
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  //- get attribute from the 3rd input arg
  std::string attr;
  if (MEX_ARGS->get_input_string(k3RD_ARG, attr) == kError)
  {
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  //- get polling period from the 4th MEX-file input arg
  const mxArray* mx_pp = MEX_ARGS->get_input_array(k4TH_ARG, ::mxDOUBLE_CLASS, 1, 1);
  if (mx_pp == 0)
  {
    MEX_UTILS->push_error("invalid argin specified",
                          "invalid argument specified (wrong type or dims)",
                          "TangoBinding::poll_attribute");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  //- Store polling period locally  
  int pp = (int)::mxGetPr(mx_pp)[0];
  //- get device descriptor
  DeviceDesc * ddesc = DEV_REP->device_desc(dev);
  if (! ddesc)
  {
    std::string r = "failed to execute poll_attribute on " + dev;
    MEX_UTILS->push_error(r.c_str(),
                          "could not obtain a valid device reference",
                          "TangoBinding::poll_attribute");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  //- Poll the attribute
  _TRY(ddesc->proxy()->poll_attribute(attr, pp), dev, "poll_attribute");
  return kNoError; 
}

//=============================================================================
//- TangoBinding::get_attribute_poll_period
//=============================================================================
int TangoBinding::get_attribute_poll_period (void)
{
  //- get <device name> from the 2nd input arg
  std::string dev;
  if (MEX_ARGS->get_input_string(k2ND_ARG, dev) == kError)
  {
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  //- get attribute from the 3rd input arg
  std::string attr;
  if (MEX_ARGS->get_input_string(k3RD_ARG, attr) == kError)
  {
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  //- get device descriptor
  DeviceDesc * ddesc = DEV_REP->device_desc(dev);
  if (! ddesc)
  {
    std::string r = "failed to execute get_attribute_poll_period on " + dev;
    MEX_UTILS->push_error(r.c_str(),
                          "could not obtain a valid device reference",
                          "TangoBinding::get_attribute_poll_period");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  //- get attribute polling period
  int pp = 0;
  _TRY(pp = ddesc->proxy()->get_attribute_poll_period(attr), dev, "get_attribute_poll_period");
  //- Build argout: 1-by-1 double array
  mxArray * argout = ::mxCreateDoubleMatrix(1,1,mxREAL);
  if (! argout)
  {
    MEX_UTILS->set_error("out of memory",
                         "mxCreateDoubleMatrix failed",
                         "TangoBinding::get_attribute_poll_period");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  ::mxGetPr(argout)[0] = (double)pp;
  //- Set output arg
  if (MEX_ARGS->set_output_array(k1ST_ARG, argout) == kError)
  {
    ::mxDestroyArray(argout);
    SET_DEFAULT_PRHS_THEN_RETURN(kError);
  }
  return kNoError;
}

//=============================================================================
//- TangoBinding::stop_poll_attribute
//=============================================================================
int TangoBinding::stop_poll_attribute (void)
{
  //- get <device name> from the 2nd input arg
  std::string dev;
  if (MEX_ARGS->get_input_string(k2ND_ARG, dev) == kError)
  {
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  //- get attribute from the 3rd input arg
  std::string attr;
  if (MEX_ARGS->get_input_string(k3RD_ARG, attr) == kError)
  {
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  //- get device descriptor
  DeviceDesc * ddesc = DEV_REP->device_desc(dev);
  if (! ddesc)
  {
    std::string r = "failed to execute stop_poll_attribute on " + dev;
    MEX_UTILS->push_error(r.c_str(),
                          "could not obtain a valid device reference",
                          "TangoBinding::stop_poll_attribute");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  //- Stop polling the attribute
  _TRY(ddesc->proxy()->stop_poll_attribute(attr), dev, "stop_poll_attribute");
  return kNoError;
}

//=============================================================================
//- TangoBinding::get_idl_version
//=============================================================================
int TangoBinding::get_idl_version (void)
{
  //- get <device name> from the 2nd MEX-file input arg
  std::string dev;
  if (MEX_ARGS->get_input_string(k2ND_ARG, dev) == kError)
  {
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  //- get device descriptor
  DeviceDesc * ddesc = DEV_REP->device_desc(dev);
  if (! ddesc)
  {
    MEX_UTILS->push_error("com. error",
                          "could not obtain a valid device reference",
                          "TangoBinding::get_idl_version");
    SET_DEFAULT_PRHS_THEN_RETURN(kError);
  }
  //- get IDL version
  int idl_ver;
  _TRY(idl_ver = ddesc->proxy()->get_idl_version(), dev, "get_idl_version");
  //- Create a 1-by-1 double array
  mxArray * argout = ::mxCreateDoubleMatrix(1,1,mxREAL);
  if (! argout) 
  {
    MEX_UTILS->set_error("out of memory",
                         "mxCreateDoubleMatrix failed",
                         "TangoBinding::get_idl_version");
    ::mxDestroyArray(argout);
    SET_DEFAULT_PRHS_THEN_RETURN(kError);
  }
  //- Store device timeout
  ::mxGetPr(argout)[0] = idl_ver;
  //- Set output arg
  if (MEX_ARGS->set_output_array(k1ST_ARG, argout) == kError) 
  {
    ::mxDestroyArray(argout);
    SET_DEFAULT_PRHS_THEN_RETURN(kError);
  }
  return kNoError;
}

//=============================================================================
//- TangoBinding::get_property
//=============================================================================
int TangoBinding::get_property (void)
{
  //- get <device name> from the 2nd MEX-file input arg
  std::string dev;
  if (MEX_ARGS->get_input_string(k2ND_ARG, dev) == kError)
  {
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  //- get property name from the 3rd MEX-file input arg
  const mxArray* mx_prop_name = MEX_ARGS->get_input_array(k3RD_ARG, ::mxCHAR_CLASS, 1);
  if (mx_prop_name == 0)
  {
    MEX_UTILS->push_error("invalid argin specified",
                          "invalid property name specified (wrong type or dims)",
                          "TangoBinding::get_property");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  //- get device descriptor
  DeviceDesc * ddesc = DEV_REP->device_desc(dev);
  if (! ddesc)
  {
    MEX_UTILS->push_error("com. error",
                          "could not obtain a valid device reference",
                          "TangoBinding::get_property");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  //- get property name as string from mxArray
  char *cstr = ::mxArrayToString(mx_prop_name);
  if (! cstr)
  {
    MEX_UTILS->set_error("internal error", 
                         "could not extract string from char array",
                         "TangoBinding::get_property");
  }
  //- Create and resize  a vector of string to store the property name
  std::vector<std::string> prop_name;
  prop_name.resize(1);
  prop_name[0] = cstr;
  //- Free memory
  ::mxFree(cstr);
  //- get property from database
  Tango::DbData db_data;
  _TRY(ddesc->proxy()->get_property(prop_name, db_data), dev, "get_property");
  //- get len of the returned vector
  int len = db_data.size();
  //- Create a 1-by-len array of struct
  mwSize dims[2] = {1, static_cast<mwSize>(len)};
  //- Containing the following fields:
  const char *field_names[] = {"name", "value"};
  mxArray * argout = ::mxCreateStructArray(2, dims, 2, field_names);
  if (! argout)
  {
    MEX_UTILS->set_error("out of memory",
                         "mxCreateStructArray failed",
                         "TangoBinding::get_properties");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  int n = 0;
  mxArray *mx_array = 0;
  for (int i = 0; i < len; i++)
  {
    //- set field argout[i].name
    ::mxSetFieldByNumber(argout, i, 0, ::mxCreateString(db_data[i].name.c_str()));
    //- set field argout[i].value: create a 1-by-n cell array containing strings
    n = db_data[i].value_string.size();
    if (n > 0)
    {
      dims[1] = n;
      mx_array = ::mxCreateCellArray(2, dims);
      if (! mx_array)
      {
        MEX_UTILS->set_error("out of memory",
                             "mxCreateCellArray failed",
                             "TangoBinding::get_properties");
        ::mxDestroyArray(argout);
        SET_DEFAULT_PRHS_THEN_RETURN(kError);
      }
      //- copy from src to dest
      for (int j = 0; j < n; j++)
      {
        ::mxSetCell(mx_array, j, ::mxCreateString(db_data[i].value_string[j].c_str()));
      }
      //- set field argout[i].name
      ::mxSetFieldByNumber(argout, i, 1, mx_array);
    }
    else
    {
      //- set field argout[i].name
      ::mxSetFieldByNumber(argout, i, 1, 0);
    }
  }
  //- Set output arg
  if (MEX_ARGS->set_output_array(k1ST_ARG, argout) == kError)
  {
    ::mxDestroyArray(argout);
    SET_DEFAULT_PRHS_THEN_RETURN(kError);
  }
  return kNoError;
}

//=============================================================================
//- TangoBinding::get_properties
//=============================================================================
int TangoBinding::get_properties (void)
{
  //- get <device name> from the 2nd MEX-file input arg
  std::string dev;
  if (MEX_ARGS->get_input_string(k2ND_ARG, dev) == kError)
  {
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  //- get property list from the 3rd MEX-file input arg
  const mxArray* prop_list = MEX_ARGS->get_input_array(k3RD_ARG, ::mxCELL_CLASS, 1);
  if (prop_list == 0)
  {
    MEX_UTILS->push_error("invalid argin specified",
                          "invalid property list specified (wrong type or dims)",
                          "TangoBinding::get_properties");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  //- get device descriptor
  DeviceDesc * ddesc = DEV_REP->device_desc(dev);
  if (! ddesc)
  {
    MEX_UTILS->push_error("com. error",
                          "could not obtain a valid device reference",
                          "TangoBinding::get_properties");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  //- get len of the property list
  int len = ::mxGetN(prop_list);
  //- Create and resize  a vector of string to store the property name
  std::vector<std::string> prop_name;
  prop_name.resize(len);
  //- get property names from input array
  int i;
  char* cstr = 0;
  mxArray * mx_array = 0;
  for (i = 0; i < len; i++)
  {
    //- FIELD: NAME -------------------------------------------------------
      //- struct prop_list[i] should contains a <name> field
      mx_array = ::mxGetCell(prop_list, i);
      if (! mx_array)
      {
        MEX_UTILS->set_error("internal error",
                             "mxGetCell failed",
                             "TangoBinding::get_properties");
        SET_DEFAULT_PRHS_THEN_RETURN(kError);
      }
      //- this field must be 1-by-n char array
      if (::mxIsChar(mx_array) == false || ::mxGetM(mx_array) != 1)
      {
        MEX_UTILS->set_error("invalid argin specified",
                             "unexpected cell content (1-by-n char array expected)",
                             "TangoBinding::get_properties");
        SET_DEFAULT_PRHS_THEN_RETURN(kError);
      }
      //- get string from mxArray
      cstr = ::mxArrayToString(mx_array);
      if (! cstr)
      {
        MEX_UTILS->set_error("internal error",
                             "could not extract string from cell",
                             "TangoBinding::get_properties");
        SET_DEFAULT_PRHS_THEN_RETURN(kError); 
      }
      prop_name[i] = cstr;
      //- free memory
      ::mxFree(cstr);
  }
  //- get properties from database
  Tango::DbData db_data;
  _TRY(ddesc->proxy()->get_property(prop_name, db_data), dev, "get_property");
  //- get len of the returned vector
  len = db_data.size();
  //- Create a 1-by-len array of struct
  mwSize dims[2] = {1, static_cast<mwSize>(len)};
  //- Containing the following fields:
  const char *field_names[] = {"name", "value"};
  mxArray * argout = ::mxCreateStructArray(2, dims, 2, field_names);
  if (! argout)
  {
    MEX_UTILS->set_error("out of memory",
                         "mxCreateStructArray failed",
                         "TangoBinding::get_properties");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  int n = 0;
  for (i = 0; i < len; i++)
  {
    //- set field argout[i].name
    ::mxSetFieldByNumber(argout, i, 0, ::mxCreateString(db_data[i].name.c_str()));
    //- set field argout[i].value: create a 1-by-n cell array containing strings
    n = db_data[i].value_string.size();
    if (n > 0)
    {
      dims[1] = n;
      mx_array = ::mxCreateCellArray(2, dims);
      if (! mx_array)
      {
        MEX_UTILS->set_error("out of memory",
                             "mxCreateCellArray failed",
                             "TangoBinding::get_properties");
        ::mxDestroyArray(argout);
        SET_DEFAULT_PRHS_THEN_RETURN(kError);
      }
      //- copy from src to dest
      for (int j = 0; j < n; j++)
      {
        ::mxSetCell(mx_array, j, ::mxCreateString(db_data[i].value_string[j].c_str()));
      }
      //- set field argout[i].name
      ::mxSetFieldByNumber(argout, i, 1, mx_array);
    }
    else
    {
      //- set field argout[i].name
      ::mxSetFieldByNumber(argout, i, 1, 0);
    }
  }
  //- Set output arg
  if (MEX_ARGS->set_output_array(k1ST_ARG, argout) == kError)
  {
    ::mxDestroyArray(argout);
    SET_DEFAULT_PRHS_THEN_RETURN(kError);
  }
  return kNoError;
}

//=============================================================================
//- TangoBinding::put_property
//=============================================================================
int TangoBinding::put_property (void)
{
  //- get <device name> from the 2nd MEX-file input arg
  std::string dev;
  if (MEX_ARGS->get_input_string(k2ND_ARG, dev) == kError)
  {
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  //- get property name from the 3rd MEX-file input arg
  std::string prop_name;
  if (MEX_ARGS->get_input_string(k3RD_ARG, prop_name) == kError)
  {
    MEX_UTILS->push_error("invalid argin specified",
                          "invalid property name specified (wrong type or dims)",
                          "TangoBinding::put_property");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  //- get property value from the 4th MEX-file input arg
  const mxArray* prop_values = MEX_ARGS->get_input_array(k4TH_ARG, ::mxCELL_CLASS, 1);
  if (prop_values == 0)
  {
    MEX_UTILS->push_error("invalid argin specified",
                          "invalid property value(s) specified (wrong type or dims)",
                          "TangoBinding::put_property");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  //- get device descriptor
  DeviceDesc * ddesc = DEV_REP->device_desc(dev);
  if (! ddesc)
  {
    MEX_UTILS->push_error("com. error",
                          "could not obtain a valid device reference",
                          "TangoBinding::put_property");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  //- get len of value array
  int n = ::mxGetN(prop_values);
  //- get string values from cell array and store them in a std::vector 
  std::vector<std::string> str_values;
  str_values.resize(n);
  char *cstr = 0;
  mxArray * mx_cell = 0;
  for (int i = 0; i < n; i++)
  {
    mx_cell = ::mxGetCell(prop_values, i);
    if (mx_cell == 0)
    {
      MEX_UTILS->set_error("internal error", 
                           "mxGetCell failed",
                           "TangoBinding::put_property");
      SET_DEFAULT_PRHS_THEN_RETURN(kError); 
    }
    cstr = ::mxArrayToString(mx_cell);
    if (! cstr)
    {
      MEX_UTILS->set_error("internal error", 
                           "could not extract string from cell",
                           "TangoBinding::put_property");
      SET_DEFAULT_PRHS_THEN_RETURN(kError); 
    }
    str_values[i] = cstr;
    ::mxFree(cstr);
  }
  Tango::DbDatum db_datum(prop_name);
  db_datum << str_values;
  Tango::DbData db_data;
  db_data.push_back(db_datum);
  //- Store property into database
  _TRY(ddesc->proxy()->put_property(db_data), dev, "put_property");
  return kNoError;
}

//=============================================================================
//- TangoBinding::put_properties
//=============================================================================
int TangoBinding::put_properties (void)
{
  //- get device name from the 2nd MEX-file input arg
  std::string dev;
  if (MEX_ARGS->get_input_string(k2ND_ARG, dev) == kError)
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 

  //- get property[name/value] list from the 3rd MEX-file input arg
  const mxArray* prop_list = MEX_ARGS->get_input_array(k3RD_ARG, ::mxSTRUCT_CLASS, 1);
  if (prop_list == 0) 
  {
    MEX_UTILS->push_error("invalid argin specified",
                          "invalid property list specified (wrong type or dims)",
                          "TangoBinding::put_properties");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }

  //- get device descriptor
  DeviceDesc * ddesc = DEV_REP->device_desc(dev);
  if (! ddesc) 
  {
    MEX_UTILS->push_error("com. error",
                          "could not obtain a valid device reference",
                          "TangoBinding::put_properties");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }

  //- get length of the property[name/value] list
  int prop_list_len = ::mxGetN(prop_list);

  //- the Tango::DbData 
  Tango::DbData db_data;

  //- get property[name/value] from input array
  int i, j, n;
  char * cstr = 0;
  mxArray * mx_cell = 0;
  mxArray * mx_array = 0;
  for (i = 0; i < prop_list_len; i++) 
  {
    //- FIELD: NAME -------------------------------------------------------
    //- struct prop_list[i] should contains a <name> field
    mx_array = ::mxGetField(prop_list, i, "name");
    if (! mx_array) 
    {
      MEX_UTILS->set_error("invalid argin specified",
                           "struct should contain a field named 'name'",
                           "TangoBinding::put_properties");
      SET_DEFAULT_PRHS_THEN_RETURN(kError); 
    }
    //- this field must be 1-by-n char array
    if (::mxIsChar(mx_array) == false || ::mxGetM(mx_array) != 1) 
    {
      MEX_UTILS->set_error("invalid argin specified",
                           "1-by-n char array expected for field 'name'",
                           "TangoBinding::put_properties");
      SET_DEFAULT_PRHS_THEN_RETURN(kError); 
    }
    //- get string from mxArray
    cstr = ::mxArrayToString(mx_array);
    if (! cstr) 
    {
      MEX_UTILS->set_error("internal error", 
                           "could not extract string from field 'name'",
                           "TangoBinding::get_properties");
      SET_DEFAULT_PRHS_THEN_RETURN(kError); 
    }
    //- create/init the Tango::DbDatum
    Tango::DbDatum db_datum(cstr);
    //- release memory
    ::mxFree(cstr);
    //- FIELD: VALUE ------------------------------------------------------
    //- struct prop_list[i] should contains a <value> field
    mx_array = ::mxGetField(prop_list, i, "value");
    if (! mx_array) 
    {
      MEX_UTILS->set_error("invalid argin specified",
                           "struct should contain a field named 'value'",
                           "TangoBinding::put_properties");
      SET_DEFAULT_PRHS_THEN_RETURN(kError); 
    }
    //- this field must be 1-by-n cell array
    n = ::mxGetN(mx_array);
    if (::mxIsCell(mx_array) == false || n == 0 || ::mxGetM(mx_array) != 1) 
    {
      MEX_UTILS->set_error("invalid argin specified",
                           "1-by-n cell array expected for field 'value'",
                           "TangoBinding::put_properties");
      SET_DEFAULT_PRHS_THEN_RETURN(kError); 
    }
    //- get string values from cell array and store them into a std::vector 
    std::vector<std::string> str_values(n);
    for (j = 0; j < n; j++) 
    {
      mx_cell = ::mxGetCell(mx_array, j);
      if (! mx_cell) 
      {
        MEX_UTILS->set_error("internal error", 
                             "unexpected NULL cell - mxGetCell failed",
                             "TangoBinding::put_properties");
        SET_DEFAULT_PRHS_THEN_RETURN(kError); 
      }
      cstr = ::mxArrayToString(mx_cell);
      if (! cstr) 
      {
        MEX_UTILS->set_error("internal error", 
                             "could not extract string from cell",
                             "TangoBinding::put_properties");
        SET_DEFAULT_PRHS_THEN_RETURN(kError); 
      }
      str_values[j] = cstr;
      ::mxFree(cstr);
    }
    db_datum << str_values;
    db_data.push_back(db_datum);
  }

  //- store properties into database
  _TRY(ddesc->proxy()->put_property(db_data), dev, "put_property");

  return kNoError;
}

//=============================================================================
//- TangoBinding::del_property
//=============================================================================
int TangoBinding::del_property (void)
{
  //- get <device name> from the 2nd MEX-file input arg
  std::string dev;
  if (MEX_ARGS->get_input_string(k2ND_ARG, dev) == kError)
  {
    SET_DEFAULT_PRHS_THEN_RETURN(kError);
  }
  //- get property name from the 3rd MEX-file input arg
  std::string prop_name;
  if (MEX_ARGS->get_input_string(k3RD_ARG, prop_name) == kError)
  {
    MEX_UTILS->push_error("invalid argin specified",
                          "invalid property name specified (wrong type or dims)",
                          "TangoBinding::del_property");
    SET_DEFAULT_PRHS_THEN_RETURN(kError);
  }
  //- get device descriptor
  DeviceDesc * ddesc = DEV_REP->device_desc(dev);
  if (! ddesc)
  {
    MEX_UTILS->push_error("com. error",
                          "could not obtain a valid device reference",
                          "TangoBinding::del_property");
    SET_DEFAULT_PRHS_THEN_RETURN(kError);
  }
  //- Del property from database
  _TRY(ddesc->proxy()->delete_property(prop_name), dev, "delete_property");
  return kNoError;
}

//=============================================================================
//- TangoBinding::del_properties
//=============================================================================
int TangoBinding::del_properties (void)
{
  //- get <device name> from the 2nd MEX-file input arg
  std::string dev;
  if (MEX_ARGS->get_input_string(k2ND_ARG, dev) == kError)
  {
    SET_DEFAULT_PRHS_THEN_RETURN(kError);
  }
  //- get property list from the 3rd MEX-file input arg
  const mxArray* mx_prop_list = MEX_ARGS->get_input_array(k3RD_ARG, ::mxCELL_CLASS, 1);
  if (mx_prop_list == 0)
  {
    MEX_UTILS->push_error("invalid argin specified",
                          "invalid property list specified (wrong type or dims)",
                          "TangoBinding::del_properties");
    SET_DEFAULT_PRHS_THEN_RETURN(kError);
  }
  //- get device descriptor
  DeviceDesc * ddesc = DEV_REP->device_desc(dev);
  if (! ddesc)
  {
    MEX_UTILS->push_error("com. error",
                          "could not obtain a valid device reference",
                          "TangoBinding::del_properties");
    SET_DEFAULT_PRHS_THEN_RETURN(kError);
  }
  //- get len of the property list
  int len = ::mxGetN(mx_prop_list);
  //- Create a vector of strings
  std::vector<std::string> std_prop_list;
  std_prop_list.resize(len);
  //- get property names from input array
  int i;
  char* cstr = 0;
  mxArray * mx_array = 0;
  for (i = 0; i < len; i++)
  {
    //- FIELD: NAME -------------------------------------------------------
    //- struct prop_list[i] should contains a <name> field
    mx_array = ::mxGetCell(mx_prop_list, i);
    if (! mx_array)
    {
      MEX_UTILS->set_error("internal error",
                           "mxGetCell failed",
                           "TangoBinding::del_properties");
      SET_DEFAULT_PRHS_THEN_RETURN(kError);
    }
    //- this field must be 1-by-n char array
    if (::mxIsChar(mx_array) == false || ::mxGetM(mx_array) != 1)
    {
      MEX_UTILS->set_error("invalid argin specified",
                           "unexpected cell content (1-by-n char array expected)",
                           "TangoBinding::del_properties");
      SET_DEFAULT_PRHS_THEN_RETURN(kError);
    }
    //- get string from mxArray
    cstr = ::mxArrayToString(mx_array);
    if (! cstr)
    {
      MEX_UTILS->set_error("internal error",
                            "could not extract string from cell",
                            "TangoBinding::del_properties");
      SET_DEFAULT_PRHS_THEN_RETURN(kError);
    }
    std_prop_list.push_back(std::string(cstr));
    //- free memory
    ::mxFree(cstr);
  }
  //- Del properties from database
  _TRY(ddesc->proxy()->delete_property(std_prop_list), dev, "delete_property");

  return kNoError;
}

#if !defined (DISABLE_TANGO_GROUP_SUPPORT)
//=============================================================================
//- TangoBinding::group_create
//=============================================================================
int TangoBinding::group_create (void)
{
  //- get group name from the 2nd input arg
  std::string name;
  if (MEX_ARGS->get_input_string(k2ND_ARG, name) == kError)
  {
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  //- Create and add the group to the repository 
  Tango::DevLong grp_id = GRP_REP->add(name);
  if (grp_id == kError)
  {
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  //- Build returned array
  mxArray * argout = ::mxCreateDoubleMatrix(1,1,mxREAL);
  if (! argout)
  {
    MEX_UTILS->set_error("out of memory",
                         "mxCreateDoubleMatrix failed",
                         "TangoBinding::create_group");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  ::mxGetPr(argout)[0] = (double)grp_id;
  //- Set output arg
  if (MEX_ARGS->set_output_array(k1ST_ARG, argout) == kError)
  {
    ::mxDestroyArray(argout);
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  return kNoError;
}

//=============================================================================
//- TangoBinding::group_kill
//=============================================================================
int TangoBinding::group_kill (void)
{
  //- get group id from the 2nd input arg
  const mxArray* mx_grp_id = MEX_ARGS->get_input_array(k2ND_ARG, kMX_ANY, 1, 1);
  if (mx_grp_id == 0 || ::mxIsNumeric(mx_grp_id) == false)
  {
    MEX_UTILS->set_error("invalid argin specified",
                         "group id is either missing or has an invalid format",
                         "TangoBinding::group_kill");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  Tango::DevLong grp_id = (Tango::DevLong)::mxGetPr(mx_grp_id)[0];
  //- get group ref
  Tango::Group* g = GRP_REP->get(grp_id);
  if (g == 0)
  {
    //- Invalid group id specified (error handled by repository)
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  Tango::Group* p = g->get_parent();
  //- Does g have a parent?
  if (p) 
  {
    //- First, remove g from the repository (no delete) ...
    if (GRP_REP->remove(grp_id, false) == kError)
    {
      SET_DEFAULT_PRHS_THEN_RETURN(kError); 
    }
    //- ... then remove <g> from its parent hierarchy (delete)
    p->remove(g->get_fully_qualified_name(), false);
  }
  else
  {
    //- Just remove it from repository (delete)
    if (GRP_REP->remove(grp_id, true) == kError)
    {
      SET_DEFAULT_PRHS_THEN_RETURN(kError); 
    }
  }
  return kNoError;
}

//=============================================================================
//- TangoBinding::group_add
//=============================================================================
int TangoBinding::group_add (void)
{
  //- get parent group id from the 2nd input arg
  const mxArray* mx_parent_grp_id = MEX_ARGS->get_input_array(k2ND_ARG, kMX_ANY, 1, 1);
  if (mx_parent_grp_id == 0 || ::mxIsNumeric(mx_parent_grp_id) == false)
  {
    MEX_UTILS->set_error("invalid argin specified",
                         "group id is either missing or has an invalid format",
                         "TangoBinding::group_add");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  Tango::DevLong parent_grp_id = (Tango::DevLong)::mxGetPr(mx_parent_grp_id)[0];
  //- get parent group from repository
  Tango::Group* parent = GRP_REP->get(parent_grp_id);
  if (parent == 0)
  {
    //- Invalid group id specified (error handled by repository)
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  //- get child group id or device names from the 3rd input arg
  const mxArray* mx_array = MEX_ARGS->get_input_array(k3RD_ARG);
  if (! mx_array)
  {
    MEX_UTILS->set_error("invalid argin specified",
                         "second argument is either missing or has an invalid format",
                         "TangoBinding::group_add");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  //- Indentify 3rd argument type
  if (::mxIsNumeric(mx_array) && ::mxGetM(mx_array) == 1 && ::mxGetM(mx_array) == 1)
  {
    //- numeric value : group id passed 
    Tango::DevLong child_grp_id = (Tango::DevLong)::mxGetPr(mx_array)[0];
    //- get child group from repository
    Tango::Group* child = GRP_REP->get(child_grp_id);
    if (child == 0)
    {
      //- Invalid group id specified (error handled by repository)
      SET_DEFAULT_PRHS_THEN_RETURN(kError); 
    }
    //- Attach child to parent
    parent->add(child);
  }
  else if (::mxIsChar(mx_array))
  {
    //- single string : device name or device name pattern passed
    char* cstr = ::mxArrayToString(mx_array);
    if (! cstr)
    {
      MEX_UTILS->set_error("internal error", 
                           "could not extract string from cell",
                           "TangoBinding::group_add");
      SET_DEFAULT_PRHS_THEN_RETURN(kError); 
    }
    parent->add(cstr);
    ::mxFree(cstr);
  }
  else if (::mxIsCell(mx_array) == true)
  {
    //- cell array of string : array of device name and/or device name pattern passed
    char* cstr = 0;
    mxArray * cell = 0;
    size_t len = ::mxGetN(mx_array);
    std::vector<std::string> patterns(len);
    for (size_t i = 0; i < len; i++)
    {
      //- get <i>th cell of the array
      cell = ::mxGetCell(mx_array, i);
      if (! cell)
      {
        MEX_UTILS->set_error("internal error",
                             "unexpected NULL cell",
                             "TangoBinding::group_add");
        SET_DEFAULT_PRHS_THEN_RETURN(kError); 
      }
      //- cell should contain a string
      if (::mxIsChar(cell) == false)
      {
        MEX_UTILS->set_error("internal error",
                             "unexpected cell content (string expected)",
                             "TangoBinding::group_add");
        SET_DEFAULT_PRHS_THEN_RETURN(kError); 
      }
      //- get c-string from matlab char array.
      cstr = ::mxArrayToString(cell);
      if (! cstr)
      {
        MEX_UTILS->set_error("internal error", 
                             "could not extract string from cell",
                             "TangoBinding::group_add");
        SET_DEFAULT_PRHS_THEN_RETURN(kError); 
      }
      //- assign i-th string
      patterns[i] = cstr;
      //- free temp c-string
      ::mxFree(cstr);
    } //- for i ...
    //- add devices
    parent->add(patterns);
  }
  else
  {
    MEX_UTILS->set_error("invalid argin specified",
                         "second argument has an invalid format",
                         "TangoBinding::group_add");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  return kNoError;
}

//=============================================================================
//- TangoBinding::group_remove
//=============================================================================
int TangoBinding::group_remove (void)
{
  //- get parent group id from the 2nd input arg
  const mxArray* mx_parent_grp_id = MEX_ARGS->get_input_array(k2ND_ARG, kMX_ANY, 1, 1);
  if (mx_parent_grp_id == 0 || ::mxIsNumeric(mx_parent_grp_id) == false)
  {
    MEX_UTILS->set_error("invalid argin specified",
                         "group id is either missing or has an invalid format",
                         "TangoBinding::group_remove");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  Tango::DevLong parent_grp_id = (Tango::DevLong)::mxGetPr(mx_parent_grp_id)[0];
  //- get parent group from repository
  Tango::Group* parent = GRP_REP->get(parent_grp_id);
  if (parent == 0)
  {
     //- Invalid group id specified (error handled by repository)
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  //- get child names or patterns from the 3rd input arg
  const mxArray* mx_array = MEX_ARGS->get_input_array(k3RD_ARG);
  if (! mx_array)
  {
    MEX_UTILS->set_error("invalid argin specified",
                         "second argument is either missing or has an invalid format",
                         "TangoBinding::group_remove");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  //- get forward option from the 4th input arg
  const mxArray* mx_forward = MEX_ARGS->get_input_array(k4TH_ARG, kMX_ANY, 1, 1);
  if (mx_forward == 0 || ::mxIsNumeric(mx_forward) == false)
  {
    MEX_UTILS->set_error("invalid argin specified",
                         "forward option is either missing or has an invalid format",
                         "TangoBinding::group_remove");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  bool forward = ::mxGetPr(mx_forward)[0] ? true : false;
  //- Indentify 3rd argument type
  if (::mxIsCell(mx_array) == true)
  {
    //- Cell array of strings
    char* cstr = 0;
    mxArray * cell = 0;
    size_t len = ::mxGetN(mx_array);
    std::vector<std::string> patterns(len);
    for (size_t i = 0; i < len; i++)
    {
      //- get <i>th cell of the array
      cell = ::mxGetCell(mx_array, i);
      if (! cell)
      {
        MEX_UTILS->set_error("internal error",
                             "unexpected NULL cell",
                             "TangoBinding::group_remove");
        SET_DEFAULT_PRHS_THEN_RETURN(kError); 
      }
      //- cell should contain a string
      if (::mxIsChar(cell) == false)
      {
        MEX_UTILS->set_error("internal error",
                             "unexpected cell content (string expected)",
                             "TangoBinding::group_remove");
        SET_DEFAULT_PRHS_THEN_RETURN(kError); 
      }
      //- get c-string from matlab char array.
      cstr = ::mxArrayToString(cell);
      if (! cstr)
      {
        MEX_UTILS->set_error("internal error", 
                             "could not extract string from cell",
                             "TangoBinding::group_remove");
        SET_DEFAULT_PRHS_THEN_RETURN(kError); 
      }
      //- If it is a group then remove it from the repository (no delete)
      try
      {
        Tango::DeviceProxy * dp = parent->get_device(cstr);
        if (dp == 0)
        {
          //- cstr is the name of a group
          Tango::Group*  g = parent->get_group(cstr);
          if (g && g != parent)
          {
            GRP_REP->remove(g, false);
          }
        }
      }
      catch (...)
      {
        //- cstr is the name of unreachable device
        //- ignore exception
      }
      //- assign i-th string
      patterns[i] = cstr;
      //- free temp c-string
      ::mxFree(cstr);
    } //- for i ...
    //- remove devices
    parent->remove(patterns, forward);
  }
  else if (::mxIsChar(mx_array))
  {
    //- Single string
    char* cstr = ::mxArrayToString(mx_array);
    if (! cstr)
    {
      MEX_UTILS->set_error("internal error", 
                           "could not extract string from char array",
                           "TangoBinding::group_remove");
      SET_DEFAULT_PRHS_THEN_RETURN(kError); 
    }
    //- cstr is a group name then remove this group from the repository (no delete)
    try
    {
      Tango::DeviceProxy * dp = parent->get_device(cstr);
      if (dp == 0)
      {
        //- cstr is the name of a group
        Tango::Group*  g = parent->get_group(cstr);
        if (g && g != parent)
        {
          GRP_REP->remove(g, false);
        }
      }
    }
    catch (...)
    {
      //- cstr is the name of unreachable device
      //- ignore exception
    }
    //- Remove object (i.e. group or devices) from the hierarchy
    parent->remove(cstr, forward);
    ::mxFree(cstr);
  }
  else if (::mxIsNumeric(mx_array) && ::mxGetM(mx_array) == 1 && ::mxGetN(mx_array) == 1)
  {
    //- Group id specified
    Tango::DevLong grp_id = (Tango::DevLong)::mxGetPr(mx_array)[0];
    if (parent_grp_id == grp_id)
    {
      MEX_UTILS->set_error("invalid argin specified",
                           "can't remove self from group",
                           "TangoBinding::group_remove");
      SET_DEFAULT_PRHS_THEN_RETURN(kError); 
    }
    //- get group ref
    Tango::Group* g = GRP_REP->get(grp_id);
    if (g)
    {
      GRP_REP->remove(grp_id, false);
    }
    //- Remove group from parent hierarchy
    parent->remove(g->get_fully_qualified_name(), forward);
  }
  else
  {
    MEX_UTILS->set_error("invalid argin specified",
                         "second argument has an invalid format",
                         "TangoBinding::group_remove");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  return kNoError;
}

//=============================================================================
//- TangoBinding::group_id
//=============================================================================
int TangoBinding::group_id (void)
{
  //- get group name
  std::string group_name;
  if (MEX_ARGS->get_input_string(k2ND_ARG, group_name) == kError)
  {
    SET_DEFAULT_PRHS_THEN_RETURN(kError);
  }
  //- get parent group from repository
  Tango::DevLong gid = GRP_REP->gid(group_name);
  if (gid == -1)
  {
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  //- Create a 1-by-1 double array
  mxArray * argout = ::mxCreateDoubleMatrix(1,1,mxREAL);
  if (! argout)
  {
    MEX_UTILS->set_error("out of memory",
                         "mxCreateDoubleMatrix failed",
                         "TangoBinding::group_id");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  //- Store group gid
  ::mxGetPr(argout)[0] = gid;
  //- Set output arg
  if (MEX_ARGS->set_output_array(k1ST_ARG, argout) == kError)
  {
    ::mxDestroyArray(argout);
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  return kNoError;
}

//=============================================================================
//- TangoBinding::group_dump
//=============================================================================
int TangoBinding::group_dump (void)
{
  //- get group id from the 2nd input arg
  const mxArray* mx_grp_id = MEX_ARGS->get_input_array(k2ND_ARG, kMX_ANY, 1, 1);
  if (mx_grp_id == 0 || ::mxIsNumeric(mx_grp_id) == false)
  {
    MEX_UTILS->set_error("invalid argin specified",
                         "group id is either missing or has an invalid format",
                         "TangoBinding::group_dump");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  Tango::DevLong grp_id = (Tango::DevLong)::mxGetPr(mx_grp_id)[0];
  //- get parent group from repository
  Tango::Group* g = GRP_REP->get(grp_id);
  if (g == 0)
  {
     //- Invalid group id specified (error handled by repository)
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  //- Dump group content
  TangoSys_OMemStream oms;
  g->dump(oms);
  ::mexPrintf("%s", oms.str().c_str()); 
  return kNoError;
}

//=============================================================================
//- TangoBinding::group_device_list
//=============================================================================
int TangoBinding::group_device_list (void)
{
  //- get group id from the 2nd input arg
  const mxArray* mx_grp_id = MEX_ARGS->get_input_array(k2ND_ARG, kMX_ANY, 1, 1);
  if (mx_grp_id == 0 || ::mxIsNumeric(mx_grp_id) == false)
  {
    MEX_UTILS->set_error("invalid argin specified",
                         "group id is either missing or has an invalid format",
                         "TangoBinding::group_device_list");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  Tango::DevLong grp_id = (Tango::DevLong)::mxGetPr(mx_grp_id)[0];
  //- get parent group from repository
  Tango::Group* g = GRP_REP->get(grp_id);
  if (g == 0)
  {
     //- Invalid group id specified (error handled by repository)
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  //- get <forward> option the 3rd input arg
  const mxArray* mx_forward = MEX_ARGS->get_input_array(k3RD_ARG, kMX_ANY, 1, 1);
  if (mx_forward == 0 || ::mxIsNumeric(mx_forward) == false)
  {
    std::string r = "could not obtain size of group " + g->get_name();
    MEX_UTILS->push_error(r.c_str(),
                          "forward option is either missing or has an invalid format",
                          "TangoBinding::group_device_list");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  //- number of device in group
  long num_dev_in_group = g->get_size(mx_forward);
  //- create a 1-by-n cell array
  const mwSize dims[2] = {1, static_cast<mwSize>(num_dev_in_group)};
  mxArray * argout = ::mxCreateCellArray(2, dims);
  if (argout == 0) 
  {
    MEX_UTILS->set_error("out of memory",
                         "mxCreateCellArray failed",
                         "DataAdapter::group_device_list");
    return kError;
  }
  //- populate returned array
  for (long d = 1; d <= num_dev_in_group; d++)
  {
    ::mxSetCell(argout, d, ::mxCreateString(g->get_device(d)->dev_name().c_str()));  
  }
  //- set output arg
  if (MEX_ARGS->set_output_array(k1ST_ARG, argout) == kError)
  {
    ::mxDestroyArray(argout);
    SET_DEFAULT_PRHS_THEN_RETURN(kError);
  }
  return kNoError;
}

//=============================================================================
//- TangoBinding::group_size
//=============================================================================
int TangoBinding::group_size (void)
{
  //- get group id from the 2nd input arg
  const mxArray* mx_grp_id = MEX_ARGS->get_input_array(k2ND_ARG, kMX_ANY, 1, 1);
  if (mx_grp_id == 0 || ::mxIsNumeric(mx_grp_id) == false)
  {
    MEX_UTILS->set_error("invalid argin specified",
                         "group id is either missing or has an invalid format",
                         "TangoBinding::group_size");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  Tango::DevLong grp_id = (Tango::DevLong)::mxGetPr(mx_grp_id)[0];
  //- get parent group from repository
  Tango::Group* g = GRP_REP->get(grp_id);
  if (g == 0)
  {
     //- Invalid group id specified (error handled by repository)
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  //- get <forward> option the 3rd input arg
  const mxArray* mx_forward = MEX_ARGS->get_input_array(k3RD_ARG, kMX_ANY, 1, 1);
  if (mx_forward == 0 || ::mxIsNumeric(mx_forward) == false)
  {
    std::string r = "could not obtain size of group " + g->get_name();
    MEX_UTILS->push_error(r.c_str(),
                          "forward option is either missing or has an invalid format",
                          "TangoBinding::group_size");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  bool forward = ::mxGetPr(mx_forward)[0] ? true : false;
  //- Create a 1-by-1 double array
  mxArray * argout = ::mxCreateDoubleMatrix(1,1,mxREAL);
  if (! argout)
  {
    MEX_UTILS->set_error("out of memory",
                         "mxCreateDoubleMatrix failed",
                         "TangoBinding::group_size");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  //- Store group size
  ::mxGetPr(argout)[0] = g->get_size(forward);
  //- Set output arg
  if (MEX_ARGS->set_output_array(k1ST_ARG, argout) == kError)
  {
    ::mxDestroyArray(argout);
    SET_DEFAULT_PRHS_THEN_RETURN(kError);
  }
  return kNoError;
}

//=============================================================================
//- TangoBinding::group_ping
//=============================================================================
int TangoBinding::group_ping (void)
{
  //- get group id from the 2nd input arg
  const mxArray* mx_grp_id = MEX_ARGS->get_input_array(k2ND_ARG, kMX_ANY, 1, 1);
  if (mx_grp_id == 0 || ::mxIsNumeric(mx_grp_id) == false)
  {
    MEX_UTILS->set_error("invalid argin specified",
                         "group id is either missing or has an invalid format",
                         "TangoBinding::group_size");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  Tango::DevLong grp_id = (Tango::DevLong)::mxGetPr(mx_grp_id)[0];
  //- get parent group from repository
  Tango::Group* g = GRP_REP->get(grp_id);
  if (g == 0)
  {
     //- Invalid group id specified (error handled by repository)
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  //- get <forward> option the 3rd input arg
  const mxArray* mx_forward = MEX_ARGS->get_input_array(k3RD_ARG, kMX_ANY, 1, 1);
  if (mx_forward == 0 || ::mxIsNumeric(mx_forward) == false)
  {
    std::string r = "could not ping group " + g->get_name();
    MEX_UTILS->push_error(r.c_str(),
                          "forward option is either missing or has an invalid format",
                          "TangoBinding::group_ping");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  bool forward = ::mxGetPr(mx_forward)[0] ? true : false;
  //- Create a 1-by-1 double array
  mxArray * argout = ::mxCreateDoubleMatrix(1,1,mxREAL);
  if (! argout)
  {
    MEX_UTILS->set_error("out of memory",
                         "mxCreateDoubleMatrix failed",
                         "TangoBinding::group_ping");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  //- Ping the group
  ::mxGetPr(argout)[0] = g->ping(forward) ? 1 : 0;
  //- Set output arg
  if (MEX_ARGS->set_output_array(k1ST_ARG, argout) == kError)
  {
    ::mxDestroyArray(argout);
    SET_DEFAULT_PRHS_THEN_RETURN(kError);
  }
  return kNoError;
}

//=============================================================================
//- TangoBinding::group_set_timeout_msecs
//=============================================================================
int TangoBinding::group_set_timeout_msecs (void)
{
  //- get group id from the 2nd input arg
  const mxArray* mx_grp_id = MEX_ARGS->get_input_array(k2ND_ARG, kMX_ANY, 1, 1);
  if (mx_grp_id == 0 || ::mxIsNumeric(mx_grp_id) == false)
  {
    MEX_UTILS->set_error("invalid argin specified",
                         "group id is either missing or has an invalid format",
                         "TangoBinding::group_size");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  Tango::DevLong grp_id = (Tango::DevLong)::mxGetPr(mx_grp_id)[0];
  //- get parent group from repository
  Tango::Group* g = GRP_REP->get(grp_id);
  if (g == 0)
  {
     //- Invalid group id specified (error handled by repository)
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  //- get <forward> option the 3rd input arg
  const mxArray* mx_tmo = MEX_ARGS->get_input_array(k3RD_ARG, kMX_ANY, 1, 1);
  if (mx_tmo == 0 || ::mxIsNumeric(mx_tmo) == false)
  {
    std::string r = "could not set timeout on group " + g->get_name();
    MEX_UTILS->push_error(r.c_str(),
                          "<tmo_msecs> value is either missing or has an invalid format",
                          "TangoBinding::group_ping");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  //- set timeout on g
  g->set_timeout_millis((Tango::DevLong)::mxGetPr(mx_tmo)[0]);
  
  return kNoError;
}

//=============================================================================
//- TangoBinding::group_contains
//=============================================================================
int TangoBinding::group_contains (void) 
{
  //- get group id from the 2nd input arg
  const mxArray* mx_grp_id = MEX_ARGS->get_input_array(k2ND_ARG, kMX_ANY, 1, 1);
  if (mx_grp_id == 0 || ::mxIsNumeric(mx_grp_id) == false)
  {
    MEX_UTILS->set_error("invalid argin specified",
                         "group id is either missing or has an invalid format",
                         "TangoBinding::group_contains");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  Tango::DevLong grp_id = (Tango::DevLong)::mxGetPr(mx_grp_id)[0];
  //- get parent group from repository
  Tango::Group* g = GRP_REP->get(grp_id);
  if (g == 0)
  {
     //- Invalid group id specified (error handled by repository)
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  //- get wild card from 3rd input arg
  std::string pattern;
  if (MEX_ARGS->get_input_string(k3RD_ARG, pattern) == kError)
  {
    SET_DEFAULT_PRHS_THEN_RETURN(kError);
  }
  //- get <forward> option from 4th input arg
  const mxArray* mx_forward = MEX_ARGS->get_input_array(k4TH_ARG, kMX_ANY, 1, 1);
  if (mx_forward == 0 || ::mxIsNumeric(mx_forward) == false)
  {
    std::string r = "could not ping group " + g->get_name();
    MEX_UTILS->push_error(r.c_str(),
                          "forward option is either missing or has an invalid format",
                          "TangoBinding::group_contains");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  bool forward = ::mxGetPr(mx_forward)[0] ? true : false;
  //- Create a 1-by-1 double array
  mxArray * argout = ::mxCreateDoubleMatrix(1,1,mxREAL);
  if (! argout)
  {
    MEX_UTILS->set_error("out of memory",
                         "mxCreateDoubleMatrix failed",
                         "TangoBinding::group_contains");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  //- Does the group contain device(s) matching pattern?
  ::mxGetPr(argout)[0] = g->contains(pattern, forward) ? 1 : 0;
  //- Set output arg
  if (MEX_ARGS->set_output_array(k1ST_ARG, argout) == kError)
  {
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  return kNoError;  
}

//=============================================================================
//- TangoBinding::group_command_inout_asynch 
//=============================================================================
int TangoBinding::group_command_inout_asynch (void)
{
  //- get group id from the 2nd input arg
  const mxArray* mx_grp_id = MEX_ARGS->get_input_array(k2ND_ARG, kMX_ANY, 1, 1);
  if (mx_grp_id == 0 || ::mxIsNumeric(mx_grp_id) == false) 
  {
    MEX_UTILS->set_error("invalid argin specified",
                         "group id is either missing or has an invalid format",
                         "TangoBinding::group_command_inout_asynch");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  Tango::DevLong grp_id = (Tango::DevLong)::mxGetPr(mx_grp_id)[0];
  //- get group ref from repository
  Tango::Group* g = GRP_REP->get(grp_id);
  if (g == 0) 
  {
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  //- get command name from the 3rd input arg
  std::string cmd;
  if (MEX_ARGS->get_input_string(k3RD_ARG, cmd) == kError) 
  {
    std::string r = "failed to execute command on group " + g->get_name();
    MEX_UTILS->push_error(r.c_str(),
                          "invalid argument specified for command name",
                          "TangoBinding::group_command_inout_asynch");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  //- get "forget" option the 4th input arg
  const mxArray* mx_forget = MEX_ARGS->get_input_array(k4TH_ARG, kMX_ANY, 1, 1);
  if (mx_forget == 0 || ::mxIsNumeric(mx_forget) == false) 
  {
    std::string r = "failed to execute " + cmd + " on group " + g->get_name();
    MEX_UTILS->push_error(r.c_str(),
                          "forget option is either missing or has an invalid format",
                          "TangoBinding::group_command_inout_asynch");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  bool forget = ::mxGetPr(mx_forget)[0] ? true : false;
  //- get <forward> option the 5th input arg
  const mxArray* mx_forward = MEX_ARGS->get_input_array(k5TH_ARG, kMX_ANY, 1, 1);
  if (mx_forward == 0 || ::mxIsNumeric(mx_forward) == false) 
  {
    std::string r = "failed to execute " + cmd + " on group " + g->get_name();
    MEX_UTILS->push_error(r.c_str(),
                          "forward option is either missing or has an invalid format",
                          "TangoBinding::group_command_inout_asynch");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  bool forward = ::mxGetPr(mx_forward)[0] ? true : false;
  //- com tmo expired for the specified group?
  if (GRP_REP->com_timeout_expired(grp_id))
  {
    try
    {
      //- MEX_UTILS->notify("com. tmo expired for specified group. forcing ping...");
      g->ping();
    }
    catch (...)
    {
      //- ignore any error
    }
  }
  int idx = 1;
  int cmd_id = kError;
  DeviceDesc * ddesc = 0;
  Tango::DeviceProxy* dp = 0;
  Tango::CmdArgType arg_in_type = Tango::DEV_VOID;
  do
  {
    //- get device descriptor of the i-th device in the group
    try 
    {
      dp = g->get_device(idx);
      if (dp) 
      {
        //- get device descriptor of the i-th device in the group
        ddesc = DEV_REP->device_desc(dp, false, false);
        if (ddesc) 
        {
          //- Is <cmd> a valid command for <dev>
          cmd_id = ddesc->cmd_exists(cmd);
          if (cmd_id != kError) 
          {
            arg_in_type = (Tango::CmdArgType)(ddesc->cmd_list())[cmd_id].in_type;
            break;
          }
        }
      }
    }
    catch (...) 
    {
      //- i-th device in the group is not reachable
      //- ignore exception and retry with next device
    }
    //- Next device in the group
    idx++;
  } while (idx <= g->get_size());
  //- Command not found
  if (cmd_id == kError)
  {
    std::string r = "failed to execute " + cmd + " on group " + g->get_name();
    std::string d = "can't find a running device supporting <" + cmd + "> in group " + g->get_name();
    MEX_UTILS->set_error(r.c_str(),
                         d.c_str(),
                         "TangoBinding::group_command_inout_asynch");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  //- Asynch request ID 
  Tango::DevLong req_id = kError;
  //- Exec the command 
  if (arg_in_type != Tango::DEV_VOID) 
  {
    //- get <argin> from the 6th input arg
    const mxArray* arg_in = MEX_ARGS->get_input_array(k6TH_ARG);
    if (arg_in == 0) 
    {
      std::string r = "failed to execute " + cmd + " on group " + g->get_name();
      MEX_UTILS->push_error(r.c_str(),
                            "command argin is missing",
                            "TangoBinding::group_command_inout_asynch");
      SET_DEFAULT_PRHS_THEN_RETURN(kError); 
    }
    //- does user want to pass one specific arg for each device in group?
    //- case 1 : scalar TANGO arg / vector of value passed
    ::mxClassID class_id;
    bool is_aoa = MEX_UTILS->is_array_of_array(arg_in, class_id);
    bool is_aos = is_aoa && (class_id == ::mxCHAR_CLASS);
    if (MEX_UTILS->is_scalar(arg_in_type) && (MEX_UTILS->is_vector(arg_in) || is_aos)) 
    {
      req_id = group_command_inout_asynch_specific_scalar(g, cmd, forget, forward, arg_in); 
      if (req_id == kError) 
      {
        std::string r = "failed to execute " + cmd + " on group " + g->get_name();
        MEX_UTILS->push_error(r.c_str(),
                              "data conversion failed (specific value per device)",
                              "TangoBinding::group_command_inout_asynch");
        SET_DEFAULT_PRHS_THEN_RETURN(kError); 
      } 
    } 
    //- case 2 : sequence TANGO arg / vector of struct passed
    else if (MEX_UTILS->is_struct(arg_in_type) && MEX_UTILS->is_array_of_struct(arg_in)) 
    {
      req_id = group_command_inout_asynch_specific_scalar(g, cmd, forget, forward, arg_in); 
      if (req_id == kError)
      {
        std::string r = "failed to execute " + cmd + " on group " + g->get_name();
        MEX_UTILS->push_error(r.c_str(),
                              "data conversion failed (specific value per device)",
                              "TangoBinding::group_command_inout_asynch");
        SET_DEFAULT_PRHS_THEN_RETURN(kError); 
      }
    }
    //- case 3 : spectrum TANGO arg / vector of vector passed
    else if (MEX_UTILS->is_vector(arg_in_type) && !is_aos && is_aoa) 
    {
      req_id = group_command_inout_asynch_specific_vector(g, cmd, forget, forward, arg_in); 
      if (req_id == kError)
      {
        std::string r = "failed to execute " + cmd + " on group " + g->get_name();
        MEX_UTILS->push_error(r.c_str(),
                              "data conversion failed (specific value per device)",
                              "TangoBinding::group_command_inout_asynch");
        SET_DEFAULT_PRHS_THEN_RETURN(kError); 
      }
    }
    //- single arg for all devices in the group
    else 
    {
      //- encode argin (convert from Matlab to TANGO type)
      Tango::DeviceData dd_in;
      if (DATA_ADAPTER->encode_argin(ddesc, cmd_id, arg_in, dd_in) == kError)
      {
        SET_DEFAULT_PRHS_THEN_RETURN(kError); 
      }
      //- exec command
      _TRY(req_id = g->command_inout_asynch(cmd, dd_in, forget, forward), 
                    g->get_name(),
                    "group_command_inout_asynch");
    }
  } 
  else 
  {
    //- Exec command
    _TRY(req_id = g->command_inout_asynch(cmd, forget, forward), 
                  g->get_name(),
                  "group_command_inout_asynch");
  }
  //- No reply needed if forget is set to true
  if (forget == true) 
  {
    SET_DEFAULT_PRHS(0); 
    return kNoError;
  }
  //- Create a 1-by-1 struct array for result
  const mwSize dims[2] = {1, 1};
  const char *field_names[] = {"grp_id", "dev_id", "cmd_id", "req_id"};
  mxArray * argout = ::mxCreateStructArray(2, dims, 4, field_names);
  if (! argout) 
  {
    MEX_UTILS->set_error("out of memory",
                         "mxCreateStructArray failed",
                         "TangoBinding::group_command_inout_asynch");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  //- set field argout.group_id
  mxArray * mx_array = ::mxCreateNumericArray(2, dims, ::mxINT32_CLASS, ::mxREAL);
  if (! mx_array) 
  {
    MEX_UTILS->set_error("out of memory",
                         "mxCreateNumericArray failed",
                         "TangoBinding::group_command_inout_asynch");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  reinterpret_cast<Tango::DevLong*>(::mxGetPr(mx_array))[0] = grp_id;
  ::mxSetFieldByNumber(argout, 0, 0, mx_array);
  //- set field argout.dev_id
  mx_array = ::mxCreateString(dp->dev_name().c_str());
  if (! mx_array) 
  {
    MEX_UTILS->set_error("out of memory",
                         "mxCreateString failed",
                         "TangoBinding::group_command_inout_asynch");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  ::mxSetFieldByNumber(argout, 0, 1, mx_array);
  //- set field argout.cmd_id
  mx_array = ::mxCreateNumericArray(2, dims, ::mxINT32_CLASS, ::mxREAL);
  if (! mx_array) 
  {
    MEX_UTILS->set_error("out of memory",
                         "mxCreateNumericArray failed",
                         "TangoBinding::group_command_inout_asynch");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  reinterpret_cast<Tango::DevLong*>(::mxGetPr(mx_array))[0] = cmd_id;
  ::mxSetFieldByNumber(argout, 0, 2, mx_array);
  //- set field argout.req_id
  mx_array = ::mxCreateNumericArray(2, dims, ::mxINT32_CLASS, ::mxREAL);
  if (! mx_array) 
  {
    MEX_UTILS->set_error("out of memory",
                         "mxCreateNumericArray failed",
                         "TangoBinding::group_command_inout_asynch");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  reinterpret_cast<Tango::DevLong*>(::mxGetPr(mx_array))[0] = req_id;
  ::mxSetFieldByNumber(argout, 0, 3, mx_array);
  //- Set output arg
  if (MEX_ARGS->set_output_array(k1ST_ARG, argout) == kError) 
  {
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  return kNoError;
}

//=============================================================================
//- TangoBinding::group_command_inout_reply
//=============================================================================
int TangoBinding::group_command_inout_reply (void)
{
  //- get request descriptor from 2nd input arg
  const mxArray* mx_rdesc = MEX_ARGS->get_input_array(k2ND_ARG, ::mxSTRUCT_CLASS, 1, 1);
  if (mx_rdesc == 0)
  {
    MEX_UTILS->push_error("invalid request descriptor specified",
                          "request descriptor is either missing or has an invalid format",
                          "TangoBinding::group_command_inout_reply");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  //- mx_rdesc struct should contain a <grp_id> field
  mxArray * mx_array = ::mxGetField(mx_rdesc, 0, "grp_id");
  if (! mx_array) 
  {
    MEX_UTILS->set_error("invalid argin specified",
                         "struct should contain a field named 'grp_id'",
                         "TangoBinding::group_command_inout_reply");
    SET_DEFAULT_PRHS_THEN_RETURN(kError);                 
  }
  //- grp_id must be 1-by-n int32 array
  if (::mxIsInt32(mx_array) == false || ::mxGetM(mx_array) != 1 || ::mxGetN(mx_array) != 1) 
  {
    MEX_UTILS->set_error("invalid request descriptor specified",
                         "1-by-1 int32 array expected for field 'grp_id'",
                         "TangoBinding::group_command_inout_reply");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  Tango::DevLong grp_id = reinterpret_cast<Tango::DevLong*>(::mxGetPr(mx_array))[0];
  //- mx_rdesc struct should contain a <dev_id> field
  mx_array = ::mxGetField(mx_rdesc, 0, "dev_id");
  if (! mx_array) 
  {
    MEX_UTILS->set_error("invalid argin specified",
                         "struct should contain a field named 'dev_id'",
                         "TangoBinding::group_command_inout_reply");
    SET_DEFAULT_PRHS_THEN_RETURN(kError);                 
  }
  //- dev_id must be 1-by-n char array
  if (::mxIsChar(mx_array) == false || ::mxGetM(mx_array) != 1) 
  {
    MEX_UTILS->set_error("invalid request descriptor specified",
                         "1-by-n char array expected for field 'dev_id'",
                         "TangoBinding::group_command_inout_reply");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  //- extract dev_id string from mxArray
  char* dev_id_cstr = ::mxArrayToString(mx_array);
  if (dev_id_cstr == 0) 
  {
    MEX_UTILS->set_error("invalid request descriptor specified",
                         "could not extract string from field",
                         "TangoBinding::group_command_inout_reply");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  std::string dev_name(dev_id_cstr);
  ::mxFree(dev_id_cstr);
  //- mx_rdesc struct should contain a <cmd_id> field
  mx_array = ::mxGetField(mx_rdesc, 0, "cmd_id");
  if (! mx_array)
  {
    MEX_UTILS->set_error("invalid request descriptor specified",
                         "struct should contain a field named 'cmd_id'",
                         "TangoBinding::group_command_inout_reply");
    SET_DEFAULT_PRHS_THEN_RETURN(kError);                 
  }
  //- cmd_id must be 1-by-1 int32 char array
  if (::mxIsInt32(mx_array) == false || ::mxGetM(mx_array) != 1 || ::mxGetN(mx_array) != 1) 
  {
    MEX_UTILS->set_error("invalid request descriptor specified",
                         "1-by-1 int32 array expected for field 'cmd_id'",
                         "TangoBinding::group_command_inout_reply");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  //- extract cmd_id string from mxArray
  Tango::DevLong cmd_id = reinterpret_cast<Tango::DevLong*>(::mxGetPr(mx_array))[0];
  //- mx_rdesc struct should contain a <req_id> field
  mx_array = ::mxGetField(mx_rdesc, 0, "req_id");
  if (! mx_array)
  {
    MEX_UTILS->set_error("invalid request descriptor specified",
                         "struct should contain a field named 'req_id'",
                         "TangoBinding::group_command_inout_reply");
    SET_DEFAULT_PRHS_THEN_RETURN(kError);                 
  }
  //- req_id must be 1-by-1 int32 char array
  if (::mxIsInt32(mx_array) == false || ::mxGetM(mx_array) != 1 || ::mxGetN(mx_array) != 1) 
  {
    MEX_UTILS->set_error("invalid argin specified",
                         "1-by-1 int32 array expected for field 'req_id'",
                         "TangoBinding::group_command_inout_reply");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  //- extract dev_name string from mxArray
  Tango::DevLong req_id = reinterpret_cast<Tango::DevLong*>(::mxGetPr(mx_array))[0];
  //- get <timeout> option from the 3rd input arg
  const mxArray* mx_tmo = MEX_ARGS->get_input_array(k3RD_ARG, kMX_ANY, 1, 1);
  if (mx_tmo == 0) 
  {
    MEX_UTILS->push_error("invalid argin specified",
                          "timeout parameter is either missing or has an invalid format",
                          "TangoBinding::group_command_inout_reply");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  } 
  Tango::DevLong tmo = (Tango::DevLong)::mxGetPr(mx_tmo)[0];
  //- get group reference
  Tango::Group*  g = GRP_REP->get(grp_id);
  if (g == 0) 
  {
    MEX_UTILS->push_error("failed to execute group_command_inout_reply",
                          "invalid group id - no such group",
                          "TangoBinding::group_command_inout_reply");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  //- Asynch. request responses
  Tango::GroupCmdReplyList crl;
  //- Try to get request responses
  _TRY(crl = g->command_inout_reply(req_id, tmo), g->get_name(), "group_command_inout_reply");
  //- get device descriptor from repository
  DeviceDesc * ddesc = DEV_REP->device_desc(dev_name);
  if (! ddesc) 
  {
    MEX_UTILS->push_error("failed to execute group_command_inout_reply",
                          "invalid device id - no such device",
                          "TangoBinding::group_command_inout_reply");
    SET_DEFAULT_PRHS_THEN_RETURN(kError);   
  }
  //- Create a 1-by-1 struct array for results
  const mwSize dims[2] = {1, 1};
  const char *field_names[] = {"has_failed", "replies"};
  mxArray * main_argout = ::mxCreateStructArray(2, dims, 2, field_names);
  if (main_argout == 0) 
  {
    MEX_UTILS->set_error("out of memory",
                         "mxCreateStructArray failed",
                         "TangoBinding::group_command_inout_reply");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  //- Set main_argout.has_failed() field
  mx_array = ::mxCreateNumericArray(2, dims, ::mxDOUBLE_CLASS, ::mxREAL);
  if (! mx_array) 
  {
    MEX_UTILS->set_error("out of memory",
                         "mxCreateNumericArray failed",
                         "TangoBinding::group_command_inout_reply");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  ::mxGetPr(mx_array)[0] = crl.has_failed() ? 1 : 0;
  ::mxSetFieldByNumber(main_argout, 0, 0, mx_array);
  //- Set main_argout.replies field
  const mwSize replies_dims[2] = {1, static_cast<mwSize>(crl.size())};
  const char *replies_field_names[] = {"is_enabled", "has_failed", "dev_name", "obj_name", "data", "error"};
  mxArray * mx_replies = ::mxCreateStructArray(2, replies_dims, 6, replies_field_names);
  if (mx_replies == 0) 
  {
    MEX_UTILS->set_error("out of memory",
                         "mxCreateStructArray failed",
                         "TangoBinding::group_command_inout_reply");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  //- Decode argout for each reply in le list
  mxArray * argout = 0;
  for (size_t r = 0; r < crl.size(); r++) 
  {
    //- set is_enabled field
    mx_array = ::mxCreateNumericArray(2, dims, ::mxDOUBLE_CLASS, ::mxREAL);
    if (! mx_array) 
    {
      MEX_UTILS->set_error("out of memory",
                           "mxCreateNumericArray failed",
                           "TangoBinding::group_command_inout_reply");
     SET_DEFAULT_PRHS_THEN_RETURN(kError); 
    }
    ::mxGetPr(mx_array)[0] = crl[r].group_element_enabled() ? 1 : 0;
    ::mxSetFieldByNumber(mx_replies, r, 0, mx_array);
    //- set has_failed field
    mx_array = ::mxCreateNumericArray(2, dims, ::mxDOUBLE_CLASS, ::mxREAL);
    if (! mx_array) 
    {
      MEX_UTILS->set_error("out of memory",
                           "mxCreateNumericArray failed",
                           "TangoBinding::group_command_inout_reply");
     SET_DEFAULT_PRHS_THEN_RETURN(kError); 
    }
    ::mxGetPr(mx_array)[0] = crl[r].has_failed() ? 1 : 0;
    ::mxSetFieldByNumber(mx_replies, r, 1, mx_array);
    //- set dev_name field
    mx_array = ::mxCreateString(crl[r].dev_name().c_str());
    if (! mx_array) 
    {
      MEX_UTILS->set_error("out of memory",
                           "mxCreateString failed",
                           "TangoBinding::group_command_inout_reply");
     SET_DEFAULT_PRHS_THEN_RETURN(kError); 
    }
    ::mxSetFieldByNumber(mx_replies, r, 2, mx_array);
    //- set cmd_name field
    mx_array = ::mxCreateString(crl[r].obj_name().c_str());
    if (! mx_array) 
    {
      MEX_UTILS->set_error("out of memory",
                           "mxCreateString failed",
                           "TangoBinding::group_command_inout_reply");
     SET_DEFAULT_PRHS_THEN_RETURN(kError); 
    }
    ::mxSetFieldByNumber(mx_replies, r, 3, mx_array);
    if (crl[r].group_element_enabled() == false)
    {
      //- set undefined fields to NIL
      ::mxSetFieldByNumber(mx_replies, r, 4, 0);
      ::mxSetFieldByNumber(mx_replies, r, 5, 0);
    }
    else 
    {
      //- if r-th device gave error then set error field
      if (crl[r].has_failed() == true) 
      {
        //- set data field to 0
        ::mxSetFieldByNumber(mx_replies, r, 4, 0);
        //- set error field
        MEX_UTILS->error_stack_to_mxarray(crl[r].get_err_stack(), argout);
        ::mxSetFieldByNumber(mx_replies, r, 5, argout);
      } 
      //- otherwise, set data field
      else 
      {
        if (DATA_ADAPTER->decode_argout(ddesc, cmd_id, crl[r].get_data(), argout) == kError) 
        {
          //- reset error (erase error set by decode_argout)
          MEX_UTILS->reset_error();
          //- set error field manually
          Tango::DevErrorList errors(1);
          errors.length(1);
          errors[0].desc = CORBA::string_dup("TANGO to Matlab data conversion failed");
          errors[0].severity = Tango::ERR;
          errors[0].reason = CORBA::string_dup("data conversion failed");
          errors[0].origin = CORBA::string_dup("TangoBinding::group_command_inout_reply");
          MEX_UTILS->error_stack_to_mxarray(errors, argout);
          ::mxSetFieldByNumber(mx_replies, r, 5, argout);
          //- set data field to 0
          ::mxSetFieldByNumber(mx_replies, r, 4, 0);
        }
        else 
        {
          //- set data field
          ::mxSetFieldByNumber(mx_replies, r, 4, argout);
          //- set error field to 0
          ::mxSetFieldByNumber(mx_replies, r, 5, 0);
        }
      }
    }
  }
  //- Attach replies to main argout 
  ::mxSetFieldByNumber(main_argout, 0, 1, mx_replies);
  //- Set output arg
  if (MEX_ARGS->set_output_array(k1ST_ARG, main_argout) == kError) 
  {
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  return kNoError;
}

//=============================================================================
//- TangoBinding::group_command_inout_asynch_specific_scalar
//=============================================================================
Tango::DevLong TangoBinding::group_command_inout_asynch_specific_scalar (Tango::Group*  g, 
                                                                         const std::string& cmd,
                                                                         bool forget, 
                                                                         bool forward, 
                                                                         const mxArray* mx_array) 
{
  Tango::DevLong req_id = kError;

  ::mxClassID class_id = ::mxUNKNOWN_CLASS;

  if (MEX_UTILS->is_vector(mx_array, class_id)) 
  {
    switch(class_id) 
    {
      case mxDOUBLE_CLASS:
        {
          double dummy;
          std::vector<double>* v = DATA_ADAPTER->mxarray_to_vector(mx_array, dummy);
          _TRY_DELETE (req_id = g->command_inout_asynch(cmd, *v, forget, forward),
                       g->get_name(),
                       "group_command_inout_asynch",
                       v);
          delete v;
        }
        break;
      case mxSINGLE_CLASS:
        {
          float dummy;
          std::vector<float>* v = DATA_ADAPTER->mxarray_to_vector(mx_array, dummy);
          _TRY_DELETE (req_id = g->command_inout_asynch(cmd, *v, forget, forward), 
                       g->get_name(), 
                       "group_command_inout_asynch",
                       v);
          delete v;
        }
        break;
      case mxUINT8_CLASS:
        {
          bool dummy;
          std::vector<bool>* v = DATA_ADAPTER->mxarray_to_vector(mx_array, dummy);
          _TRY_DELETE (req_id = g->command_inout_asynch(cmd, *v, forget, forward), 
                       g->get_name(), 
                       "group_command_inout_asynch",
                       v);
          delete v;
        }
        break;
      case mxINT16_CLASS:
        {
          short dummy;
          std::vector<short>* v = DATA_ADAPTER->mxarray_to_vector(mx_array, dummy);
          _TRY_DELETE (req_id = g->command_inout_asynch(cmd, *v, forget, forward), 
                       g->get_name(), 
                       "group_command_inout_asynch",
                       v);
          delete v;
        }
        break;
      case mxUINT16_CLASS:
        {
          unsigned short dummy;
          std::vector<unsigned short>* v = DATA_ADAPTER->mxarray_to_vector(mx_array, dummy);
          _TRY_DELETE (req_id = g->command_inout_asynch(cmd, *v, forget, forward), 
                       g->get_name(), 
                       "group_command_inout_asynch",
                       v);
          delete v;
        }
        break;
      case mxINT32_CLASS:
        {
          Tango::DevLong dummy;
          std::vector<Tango::DevLong>* v = DATA_ADAPTER->mxarray_to_vector(mx_array, dummy);
          _TRY_DELETE (req_id = g->command_inout_asynch(cmd, *v, forget, forward), 
                       g->get_name(), 
                       "group_command_inout_asynch",
                       v);
          delete v;
        }
        break;
      case mxUINT32_CLASS:
        {
          Tango::DevULong dummy;
          std::vector<Tango::DevULong>* v = DATA_ADAPTER->mxarray_to_vector(mx_array, dummy);
          _TRY_DELETE (req_id = g->command_inout_asynch(cmd, *v, forget, forward), 
                       g->get_name(), 
                       "group_command_inout_asynch",
                       v);
          delete v;
        }
        break;
#if ! defined(SCILAB)
      case mxINT64_CLASS:
        {
          Tango::DevLong64 dummy;
          std::vector<Tango::DevLong64>* v = DATA_ADAPTER->mxarray_to_vector(mx_array, dummy);
          _TRY_DELETE (req_id = g->command_inout_asynch(cmd, *v, forget, forward), 
                       g->get_name(), 
                       "group_command_inout_asynch",
                       v);
          delete v;
        }  
        break;
      case mxUINT64_CLASS:
        {
          Tango::DevULong64 dummy;
          std::vector<Tango::DevULong64>* v = DATA_ADAPTER->mxarray_to_vector(mx_array, dummy);
          _TRY_DELETE (req_id = g->command_inout_asynch(cmd, *v, forget, forward), 
                       g->get_name(), 
                       "group_command_inout_asynch",
                       v);
          delete v;
        }
        break;
#endif //- #if ! defined(SCILAB)
      default:
        MEX_UTILS->set_error("invalid argument specified",
                              "unexpected data type",
                              "TangoBinding::group_command_inout_asynch");
        SET_DEFAULT_PRHS_THEN_RETURN(kError);
        break;
    }
  }
  else if (MEX_UTILS->is_array_of_string(mx_array)) 
  {
    std::vector<std::string>* v = DATA_ADAPTER->mxarray_to_vector_of_string(mx_array);
    _TRY_DELETE (req_id = g->command_inout_asynch(cmd, *v, forget, forward), 
                 g->get_name(), 
                 "group_command_inout_asynch",
                 v);
    delete v;
  }
  else if (MEX_UTILS->is_array_of_struct(mx_array, class_id))
  {
    switch(class_id) 
    {
      case mxINT32_CLASS:
        {
          std::vector<Tango::DevVarLongStringArray*>* v = DATA_ADAPTER->mxarray_to_vector_of_dvlsa(mx_array);
          _TRY_DELETE (req_id = g->command_inout_asynch(cmd, *v, forget, forward), 
                       g->get_name(), 
                       "group_command_inout_asynch",
                       v);
          delete v;
        }
        break;
      case mxDOUBLE_CLASS:
        {
          std::vector<Tango::DevVarDoubleStringArray*>* v = DATA_ADAPTER->mxarray_to_vector_of_dvdsa(mx_array);
          _TRY_DELETE (req_id = g->command_inout_asynch(cmd, *v, forget, forward), 
                       g->get_name(), 
                       "group_command_inout_asynch",
                       v);
          delete v;
        }
        break;
      default:
        MEX_UTILS->set_error("invalid argument specified",
                             "unexpected data type",
                             "TangoBinding::group_command_inout_asynch");
        SET_DEFAULT_PRHS_THEN_RETURN(kError); 
        break;
    }
  }
  else
  {
    MEX_UTILS->set_error("invalid argument specified",
                         "expected cell array or simple array",
                         "TangoBinding::group_command_inout_asynch");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }

  return req_id;
}

//=============================================================================
//- TangoBinding::group_command_inout_asynch_specific_vector
//=============================================================================
Tango::DevLong TangoBinding::group_command_inout_asynch_specific_vector (Tango::Group* g, 
                                                                         const std::string& cmd,
                                                                         bool forget,
                                                                         bool forward,
                                                                         const mxArray* mx_array)
  {
  Tango::DevLong req_id = kError;
  ::mxClassID class_id = ::mxUNKNOWN_CLASS;
  if (MEX_UTILS->is_array_of_array_of_string(mx_array)) 
  {
    std::vector<std::vector<std::string> >* v = DATA_ADAPTER->mxarray_to_vector_of_vector_of_string(mx_array);
    _TRY_DELETE (req_id = g->command_inout_asynch(cmd, *v, forget, forward), 
                 g->get_name(), 
                 "group_command_inout_asynch",
                 v);
    delete v;
  }
  else if (MEX_UTILS->is_array_of_array(mx_array, class_id)) 
  {
    switch(class_id) 
    {
      case mxDOUBLE_CLASS:
        {
          double dummy;
          std::vector<std::vector<double> >* v = DATA_ADAPTER->mxarray_to_vector_of_vector(mx_array, dummy);
          _TRY_DELETE (req_id = g->command_inout_asynch(cmd, *v, forget, forward), 
                       g->get_name(), 
                       "group_command_inout_asynch",
                       v);
          delete v;
        }
        break;
      case mxSINGLE_CLASS:
        {
          float dummy;
          std::vector<std::vector<float> >* v = DATA_ADAPTER->mxarray_to_vector_of_vector(mx_array, dummy);
          _TRY_DELETE (req_id = g->command_inout_asynch(cmd, *v, forget, forward), 
                       g->get_name(), 
                       "group_command_inout_asynch",
                       v);
          delete v;
        }
        break;
      case mxINT8_CLASS:
      case mxUINT8_CLASS:
        {
          unsigned char dummy;
          std::vector<std::vector<unsigned char> >* v = DATA_ADAPTER->mxarray_to_vector_of_vector(mx_array, dummy);
          _TRY_DELETE (req_id = g->command_inout_asynch(cmd, *v, forget, forward), 
                       g->get_name(), 
                       "group_command_inout_asynch",
                       v); 
          delete v;
        }
        break;
      case mxINT16_CLASS:
        {
          short dummy;
          std::vector<std::vector<short> >* v = DATA_ADAPTER->mxarray_to_vector_of_vector(mx_array, dummy);
          _TRY_DELETE (req_id = g->command_inout_asynch(cmd, *v, forget, forward), 
                       g->get_name(), 
                       "group_command_inout_asynch",
                       v);
          delete v;
        }
        break;
      case mxUINT16_CLASS:
        {
          unsigned short dummy;
          std::vector<std::vector<unsigned short> >* v = DATA_ADAPTER->mxarray_to_vector_of_vector(mx_array, dummy);
          _TRY_DELETE (req_id = g->command_inout_asynch(cmd, *v, forget, forward), 
                       g->get_name(), 
                       "group_command_inout_asynch",
                       v);
          delete v;
        }
        break;
      case mxINT32_CLASS:
        {
          Tango::DevLong dummy;
          std::vector<std::vector<Tango::DevLong> >* v = DATA_ADAPTER->mxarray_to_vector_of_vector(mx_array, dummy);
          _TRY_DELETE (req_id = g->command_inout_asynch(cmd, *v, forget, forward), 
                       g->get_name(), 
                       "group_command_inout_asynch",
                       v);
          delete v;
        }
        break;
      case mxUINT32_CLASS:
        {
          Tango::DevLong dummy;
          std::vector<std::vector<Tango::DevLong> >* v = DATA_ADAPTER->mxarray_to_vector_of_vector(mx_array, dummy);
          _TRY_DELETE (req_id = g->command_inout_asynch(cmd, *v, forget, forward), 
                       g->get_name(), 
                       "group_command_inout_asynch",
                       v);
          delete v;
        }
        break;
#if ! defined(SCILAB)
        case mxINT64_CLASS:
        {
          Tango::DevLong64 dummy;
          std::vector<std::vector<Tango::DevLong64> >* v = DATA_ADAPTER->mxarray_to_vector_of_vector(mx_array, dummy);
          _TRY_DELETE (req_id = g->command_inout_asynch(cmd, *v, forget, forward), 
                       g->get_name(), 
                       "group_command_inout_asynch",
                       v);
          delete v;
        }
        break;
        case mxUINT64_CLASS:
        {
          Tango::DevULong64 dummy;
          std::vector<std::vector<Tango::DevULong64> >* v = DATA_ADAPTER->mxarray_to_vector_of_vector(mx_array, dummy);
          _TRY_DELETE (req_id = g->command_inout_asynch(cmd, *v, forget, forward), 
                       g->get_name(), 
                       "group_command_inout_asynch",
                       v);
          delete v;
        }
        break;
#endif //- #if ! defined(SCILAB)
      default:
        MEX_UTILS->set_error("invalid argument specified",
                             "unexpected or unsupported data type",
                             "TangoBinding::group_command_inout_asynch");
        SET_DEFAULT_PRHS_THEN_RETURN(kError); 
        break;
    }
  }
  else
  {
    MEX_UTILS->set_error("invalid argument specified",
                         "expected cell array or simple array",
                         "TangoBinding::group_command_inout_asynch");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }

  return req_id;
}

//=============================================================================
//- TangoBinding::group_write_attribute_asynch 
//=============================================================================
int TangoBinding::group_write_attribute_asynch (void)
{
  //- get group id from the 2nd input arg
  const mxArray* mx_grp_id = MEX_ARGS->get_input_array(k2ND_ARG, kMX_ANY, 1, 1);
  if (mx_grp_id == 0 || ::mxIsNumeric(mx_grp_id) == false)
  {
    MEX_UTILS->set_error("invalid argin specified",
                         "group id is either missing or has an invalid format",
                         "TangoBinding::group_write_attribute_asynch");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  Tango::DevLong grp_id = (Tango::DevLong)::mxGetPr(mx_grp_id)[0];
  //- get group ref from repository
  Tango::Group* g = GRP_REP->get(grp_id);
  if (g == 0)
  {
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  //- get attr name from the 3rd input arg
  std::string attr;
  if (MEX_ARGS->get_input_string(k3RD_ARG, attr) == kError)
  {
    std::string r = "failed to write attribute on group " + g->get_name();
    MEX_UTILS->push_error(r.c_str(),
                          "invalid argument specified for attribute name",
                          "TangoBinding::group_write_attribute_asynch");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  //- get <forward> option the 4th input arg
  const mxArray* mx_forward = MEX_ARGS->get_input_array(k4TH_ARG, kMX_ANY, 1, 1);
  if (mx_forward == 0 || ::mxIsNumeric(mx_forward) == false)
  {
    std::string r = "failed to write attribute " + attr + " on group " + g->get_name();
    MEX_UTILS->push_error(r.c_str(),
                          "forward option is either missing or has an invalid format",
                          "TangoBinding::group_write_attribute_asynch");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  bool forward = ::mxGetPr(mx_forward)[0] ? true : false;
  //- com tmo expired for the specified group?
  if (GRP_REP->com_timeout_expired(grp_id))
  {
    try
    {
      //- MEX_UTILS->notify("com. tmo expired for specified group. forcing ping...");
      g->ping();
    }
    catch (...)
    {
      //- ignore any error
    }
  }
  int idx = 1;
  int attr_id = kError;
  DeviceDesc * ddesc = 0;
  Tango::DeviceProxy* dp = 0;
  int attr_type = kError;
  int attr_format = kError;
  do
  {
    try
    {
      //- get device descriptor of the i-th device in the group
      dp = g->get_device(idx);
      if (dp)
      {
        //- get device descriptor of the i-th device in the group
        ddesc = DEV_REP->device_desc(dp, false, false);
        if (ddesc)
        {
          //- Is <attr> a valid attribute for <dev>
          attr_id = ddesc->attr_exists(attr);
          if (attr_id != kError)
          {
             attr_format = (ddesc->attr_list())[attr_id].data_format;
             attr_type = (ddesc->attr_list())[attr_id].data_type;
            break;
          }
        }
      }
    }
    catch (...)
    {
      //- i-th device in the group is not reachable
      //- ignore exception and retry with next device
    }
    //- Next device in the group
    idx++;
  } while (idx <= g->get_size());
  //- Attribute not found
  if (attr_id == kError)
  {
    std::string r = "failed to write attribute <" + attr + "> on group " + g->get_name();
    std::string d = "can't find a running device with attribute <" + attr + "> in group " + g->get_name();
    MEX_UTILS->set_error(r.c_str(),
                         d.c_str(),
                         "TangoBinding::group_write_attribute_asynch");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  //- get value to write from the 5th MEX-file input arg
  const mxArray* mx_value = MEX_ARGS->get_input_array(k5TH_ARG);
  if (mx_value == 0)
  {
    std::string r = "failed to write attribute <" + attr + "> on group " + g->get_name();
    MEX_UTILS->push_error(r.c_str(),
                          "value is missing",
                          "TangoBinding::group_write_attribute_asynch");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  //- da request id
  Tango::DevLong req_id = kError;
  //- does user want to pass one specific arg for each device in group?
  //- case 1 : scalar TANGO arg / vector of value passed
  ::mxClassID class_id;
  bool is_aoa = MEX_UTILS->is_array_of_array(mx_value, class_id);
  bool is_aos = is_aoa && (class_id == ::mxCHAR_CLASS);
  if (attr_format == Tango::SCALAR && (MEX_UTILS->is_vector(mx_value) || is_aos))
  {
    req_id = group_write_attr_asynch_specific_scalar(g, attr, forward, mx_value); 
    if (req_id == kError)
    {
      std::string r = "failed to write attribute <" + attr + "> on group " + g->get_name();
      MEX_UTILS->push_error(r.c_str(),
                            "data conversion failed (specific value per device)",
                            "TangoBinding::group_write_attribute_asynch");
      SET_DEFAULT_PRHS_THEN_RETURN(kError); 
    }
  } 
  //- simple arg for all devices in the group
  else
  {
    //- Encode the value
    Tango::DeviceAttribute value;
    if (DATA_ADAPTER->encode_attr(ddesc, attr_id, mx_value, value) == kError)
    {
      //- reset error (erase error set by encode_attr)
      MEX_UTILS->reset_error();
      std::string r = "failed to write attribute <" + attr + "> on group " + g->get_name();
      MEX_UTILS->push_error(r.c_str(),
                            "Matlab to TANGO data conversion failed",
                            "TangoBinding::group_write_attribute_asynch");
      SET_DEFAULT_PRHS_THEN_RETURN(kError); 
    }
    //- Write attribute 
    _TRY(req_id = g->write_attribute_asynch(value, forward), 
         g->get_name() , 
         "TangoBinding::group_write_attribute_asynch");
  }
  //- Create a 1-by-1 struct array for result (i.e. req_desc)
  const mwSize dims[2] = {1, 1};
  const char *field_names[] = {"grp_id", "dev_id", "attr_id", "req_id"};
  mxArray * argout = ::mxCreateStructArray(2, dims, 4, field_names);
  if (! argout)
  {
    MEX_UTILS->set_error("out of memory",
                         "mxCreateStructArray failed",
                         "TangoBinding::group_write_attribute_asynch");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  //- set field argout.group_id
  mxArray * mx_array = ::mxCreateNumericArray(2, dims, ::mxINT32_CLASS, ::mxREAL);
  if (! mx_array)
  {
    MEX_UTILS->set_error("out of memory",
                         "mxCreateNumericArray failed",
                         "TangoBinding::group_write_attribute_asynch");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  reinterpret_cast<Tango::DevLong*>(::mxGetPr(mx_array))[0] = grp_id;
  ::mxSetFieldByNumber(argout, 0, 0, mx_array);
  //- set field argout.dev_id
  mx_array = ::mxCreateString(dp->dev_name().c_str());
  if (! mx_array)
  {
    MEX_UTILS->set_error("out of memory",
                         "mxCreateString failed",
                         "TangoBinding::group_write_attribute_asynch");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  ::mxSetFieldByNumber(argout, 0, 1, mx_array);
  //- set field argout.cmd_id
  mx_array = ::mxCreateNumericArray(2, dims, ::mxINT32_CLASS, ::mxREAL);
  if (! mx_array)
  {
    MEX_UTILS->set_error("out of memory",
                         "mxCreateNumericArray failed",
                         "TangoBinding::group_write_attribute_asynch");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  reinterpret_cast<Tango::DevLong*>(::mxGetPr(mx_array))[0] = attr_id;
  ::mxSetFieldByNumber(argout, 0, 2, mx_array);
  //- set field argout.req_id
  mx_array = ::mxCreateNumericArray(2, dims, ::mxINT32_CLASS, ::mxREAL);
  if (! mx_array)
  {
    MEX_UTILS->set_error("out of memory",
                         "mxCreateNumericArray failed",
                         "TangoBinding::group_write_attribute_asynch");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  reinterpret_cast<Tango::DevLong*>(::mxGetPr(mx_array))[0] = req_id;
  ::mxSetFieldByNumber(argout, 0, 3, mx_array);
  //- Set output arg
  if (MEX_ARGS->set_output_array(k1ST_ARG, argout) == kError)
  {
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  return kNoError;
}

//=============================================================================
//- TangoBinding::group_write_attr_asynch_specific_scalar
//=============================================================================
Tango::DevLong TangoBinding::group_write_attr_asynch_specific_scalar (Tango::Group* g, 
                                                            const std::string& attr,
                                                            bool forward,
                                                            const mxArray* mx_array)
{
  Tango::DevLong req_id = kError;
  ::mxClassID class_id = ::mxUNKNOWN_CLASS;
  if (MEX_UTILS->is_vector(mx_array, class_id)) 
  {
    switch(class_id) 
    {
      case mxDOUBLE_CLASS:
        {
          double dummy;
          std::vector<double>* v 
              = DATA_ADAPTER->mxarray_to_vector(mx_array, dummy);
          _TRY_DELETE (req_id = g->write_attribute_asynch(attr, *v, forward), 
                       g->get_name(), 
                       "write_attribute_asynch",
                       v);
          delete v;
        }
        break;
      case mxINT16_CLASS:
        {
          short dummy;
          std::vector<short>* v 
              = DATA_ADAPTER->mxarray_to_vector(mx_array, dummy);
          _TRY_DELETE (req_id = g->write_attribute_asynch(attr, *v, forward), 
                       g->get_name(), 
                       "write_attribute_asynch",
                       v);
          delete v;
        }
        break;
      case mxINT32_CLASS:
        {
          Tango::DevLong dummy;
          std::vector<Tango::DevLong>* v 
              = DATA_ADAPTER->mxarray_to_vector(mx_array, dummy);
          _TRY_DELETE (req_id = g->write_attribute_asynch(attr, *v, forward), 
                       g->get_name(), 
                       "write_attribute_asynch",
                       v);
          delete v;
        }
        break;
      default:
        MEX_UTILS->set_error("invalid argument specified",
                             "unexpected data type",
                             "TangoBinding::write_attribute_asynch");
        SET_DEFAULT_PRHS_THEN_RETURN(kError); 
        break;
    }
  }
  else if (MEX_UTILS->is_array_of_string(mx_array)) 
  {
    std::vector<std::string>* v 
          = DATA_ADAPTER->mxarray_to_vector_of_string(mx_array);
    _TRY_DELETE (req_id = g->write_attribute_asynch(attr, *v, forward), 
                 g->get_name(), 
                 "write_attribute_asynch",
                 v);
    delete v;
  }
  else {
    MEX_UTILS->set_error("invalid argument specified",
                         "expected cell array or simple array",
                         "TangoBinding::write_attribute_asynch");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }

  return req_id;
}

//=============================================================================
//- TangoBinding::group_write_attribute_reply
//=============================================================================
int TangoBinding::group_write_attribute_reply (void)
{
  //- get request descriptor from 2nd input arg
  const mxArray* mx_rdesc = MEX_ARGS->get_input_array(k2ND_ARG, ::mxSTRUCT_CLASS, 1, 1);
  if (mx_rdesc == 0)
  {
    MEX_UTILS->set_error("invalid request descriptor specified",
                          "request descriptor is either missing or has an invalid format",
                          "TangoBinding::group_write_attribute_reply");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  //- mx_rdesc struct should contain a <grp_id> field
  mxArray * mx_array = ::mxGetField(mx_rdesc, 0, "grp_id");
  if (! mx_array)
  {
    MEX_UTILS->set_error("invalid argin specified",
                         "struct should contain a field named 'grp_id'",
                         "TangoBinding::group_write_attribute_reply");
    SET_DEFAULT_PRHS_THEN_RETURN(kError);                 
  }
  //- grp_id must be 1-by-n int32 array
  if (::mxIsInt32(mx_array) == false || ::mxGetM(mx_array) != 1 || ::mxGetN(mx_array) != 1)
  {
    MEX_UTILS->set_error("invalid request descriptor specified",
                         "1-by-1 int32 array expected for field 'grp_id'",
                         "TangoBinding::group_write_attribute_reply");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  Tango::DevLong grp_id = reinterpret_cast<Tango::DevLong*>(::mxGetPr(mx_array))[0];
  //- mx_rdesc struct should contain a <dev_id> field
  mx_array = ::mxGetField(mx_rdesc, 0, "dev_id");
  if (! mx_array)
  {
    MEX_UTILS->set_error("invalid argin specified",
                         "struct should contain a field named 'dev_id'",
                         "TangoBinding::group_write_attribute_reply");
    SET_DEFAULT_PRHS_THEN_RETURN(kError);                 
  }
  //- dev_id must be 1-by-n char array
  if (::mxIsChar(mx_array) == false || ::mxGetM(mx_array) != 1)
  {
    MEX_UTILS->set_error("invalid request descriptor specified",
                         "1-by-n char array expected for field 'dev_id'",
                         "TangoBinding::group_write_attribute_reply");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  //- extract dev_id string from mxArray
  char* dev_id_cstr = ::mxArrayToString(mx_array);
  if (dev_id_cstr == 0)
  {
    MEX_UTILS->set_error("invalid request descriptor specified",
                         "could not extract string from field",
                         "TangoBinding::group_write_attribute_reply");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  std::string dev_name(dev_id_cstr);
  ::mxFree(dev_id_cstr);
  //- mx_rdesc struct should contain a <attr_id> field
  mx_array = ::mxGetField(mx_rdesc, 0, "attr_id");
  if (! mx_array)
  {
    MEX_UTILS->set_error("invalid request descriptor specified",
                         "struct should contain a field named 'attr_id'",
                         "TangoBinding::group_write_attribute_reply");
    SET_DEFAULT_PRHS_THEN_RETURN(kError);                 
  }
  //- attr_id must be 1-by-1 int32 char array
  if (::mxIsInt32(mx_array) == false || ::mxGetM(mx_array) != 1 || ::mxGetN(mx_array) != 1)
  {
    MEX_UTILS->set_error("invalid request descriptor specified",
                         "1-by-1 int32 array expected for field 'attr_id'",
                         "TangoBinding::group_write_attribute_reply");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  //- extract cmd_id string from mxArray
  Tango::DevLong attr_id = reinterpret_cast<Tango::DevLong*>(::mxGetPr(mx_array))[0];
  //- mx_rdesc struct should contain a <req_id> field
  mx_array = ::mxGetField(mx_rdesc, 0, "req_id");
  if (! mx_array)
  {
    MEX_UTILS->set_error("invalid request descriptor specified",
                         "struct should contain a field named 'req_id'",
                         "TangoBinding::group_write_attribute_reply");
    SET_DEFAULT_PRHS_THEN_RETURN(kError);                 
  }
  //- req_id must be 1-by-1 int32 char array
  if (::mxIsInt32(mx_array) == false || ::mxGetM(mx_array) != 1 || ::mxGetN(mx_array) != 1)
  {
    MEX_UTILS->set_error("invalid argin specified",
                         "1-by-1 int32 array expected for field 'req_id'",
                         "TangoBinding::group_write_attribute_reply");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  //- extract dev_name string from mxArray
  Tango::DevLong req_id = reinterpret_cast<Tango::DevLong*>(::mxGetPr(mx_array))[0];
  //- get <timeout> option from the 3rd input arg
  const mxArray* mx_tmo = MEX_ARGS->get_input_array(k3RD_ARG, kMX_ANY, 1, 1);
  if (mx_tmo == 0)
  {
    MEX_UTILS->push_error("invalid argin specified",
                          "timeout parameter is either missing or has an invalid format",
                          "TangoBinding::group_write_attribute_reply");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  Tango::DevLong tmo = (Tango::DevLong)::mxGetPr(mx_tmo)[0];
  //- get group reference
  Tango::Group*  g = GRP_REP->get(grp_id);
  if (g == 0)
  {
    MEX_UTILS->push_error("failed to execute group_write_attribute_reply",
                          "invalid group id - no such group",
                          "TangoBinding::group_write_attribute_reply");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  //- Asynch. request responses
  Tango::GroupReplyList rl;
  //- Try to get request responses
  _TRY(rl = g->write_attribute_reply(req_id, tmo), g->get_name(), "group_write_attribute_reply");
  //- get device descriptor from repository
  DeviceDesc * ddesc = DEV_REP->device_desc(dev_name);
  if (! ddesc)
  {
    MEX_UTILS->push_error("failed to execute group_write_attribute_reply",
                          "invalid device id - no such device",
                          "TangoBinding::group_write_attribute_reply");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  //- Create a 1-by-1 struct array for results
  const mwSize dims[2] = {1, 1};
  const char *field_names[] = {"has_failed", "replies"};
  mxArray * main_argout = ::mxCreateStructArray(2, dims, 2, field_names);
  if (main_argout == 0)
  {
    MEX_UTILS->set_error("out of memory",
                         "mxCreateStructArray failed",
                         "TangoBinding::group_write_attribute_reply");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  //- Set main_argout.has_failed() field
  mx_array = ::mxCreateNumericArray(2, dims, ::mxDOUBLE_CLASS, ::mxREAL);
  if (! mx_array)
  {
    MEX_UTILS->set_error("out of memory",
                         "mxCreateNumericArray failed",
                         "TangoBinding::group_write_attribute_reply");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  ::mxGetPr(mx_array)[0] = rl.has_failed() ? 1 : 0;
  ::mxSetFieldByNumber(main_argout, 0, 0, mx_array);
  //- Set main_argout.replies field
  const mwSize replies_dims[2] = {1, static_cast<mwSize>(rl.size())};
  const char *replies_field_names[] = {"is_enabled", "has_failed", "dev_name", "obj_name", "error"};
  mxArray * mx_replies = ::mxCreateStructArray(2, replies_dims, 5, replies_field_names);
  if (mx_replies == 0)
  {
    MEX_UTILS->set_error("out of memory",
                         "mxCreateStructArray failed",
                         "TangoBinding::group_write_attribute_reply");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  //- For each reply in le list: set error if needed
  for (size_t r = 0; r < rl.size(); r++) 
  {
    //- set is_enabled field
    mx_array = ::mxCreateNumericArray(2, dims, ::mxDOUBLE_CLASS, ::mxREAL);
    if (! mx_array)
    {
      MEX_UTILS->set_error("out of memory",
                           "mxCreateNumericArray failed",
                           "TangoBinding::group_write_attribute_reply");
     SET_DEFAULT_PRHS_THEN_RETURN(kError); 
    }
    ::mxGetPr(mx_array)[0] = rl[r].group_element_enabled() ? 1 : 0;
    ::mxSetFieldByNumber(mx_replies, r, 0, mx_array);
    //- set has_failed field
    mx_array = ::mxCreateNumericArray(2, dims, ::mxDOUBLE_CLASS, ::mxREAL);
    if (! mx_array)
    {
      MEX_UTILS->set_error("out of memory",
                           "mxCreateNumericArray failed",
                           "TangoBinding::group_write_attribute_reply");
     SET_DEFAULT_PRHS_THEN_RETURN(kError); 
    }
    ::mxGetPr(mx_array)[0] = rl[r].has_failed() ? 1 : 0;
    ::mxSetFieldByNumber(mx_replies, r, 1, mx_array);
    //- set dev_name field
    mx_array = ::mxCreateString(rl[r].dev_name().c_str());
    if (! mx_array)
    {
      MEX_UTILS->set_error("out of memory",
                           "mxCreateString failed",
                           "TangoBinding::group_write_attribute_reply");
     SET_DEFAULT_PRHS_THEN_RETURN(kError); 
    }
    ::mxSetFieldByNumber(mx_replies, r, 2, mx_array);
    //- set obj_name field
    mx_array = ::mxCreateString(rl[r].obj_name().c_str());
    if (! mx_array)
    {
      MEX_UTILS->set_error("out of memory",
                           "mxCreateString failed",
                           "TangoBinding::group_write_attribute_reply");
     SET_DEFAULT_PRHS_THEN_RETURN(kError); 
    }
    ::mxSetFieldByNumber(mx_replies, r, 3, mx_array);
    //- if r-th device gave error then set error field
    if (rl[r].group_element_enabled() && rl[r].has_failed())
    {
      //- set error field
      mxArray * error = 0;
      MEX_UTILS->error_stack_to_mxarray(rl[r].get_err_stack(), error);
      ::mxSetFieldByNumber(mx_replies, r, 4, error);
    } 
    //- otherwise, set data field
    else
    {
      //- set error field to 0
      ::mxSetFieldByNumber(mx_replies, r, 4, 0);
    }
  }
  //- Attach replies to main argout 
  ::mxSetFieldByNumber(main_argout, 0, 1, mx_replies);
  //- Set output arg
  if (MEX_ARGS->set_output_array(k1ST_ARG, main_argout) == kError)
  {
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  return kNoError;
}

//=============================================================================
//- TangoBinding::group_read_attribute_asynch 
//=============================================================================
int TangoBinding::group_read_attribute_asynch (void)
{
  //- get group id from the 2nd input arg
  const mxArray* mx_grp_id = MEX_ARGS->get_input_array(k2ND_ARG, kMX_ANY, 1, 1);
  if (mx_grp_id == 0 || ::mxIsNumeric(mx_grp_id) == false)
  {
    MEX_UTILS->set_error("invalid argin specified",
                         "group id is either missing or has an invalid format",
                         "TangoBinding::group_read_attribute_asynch");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  Tango::DevLong grp_id = (Tango::DevLong)::mxGetPr(mx_grp_id)[0];
  //- get group ref from repository
  Tango::Group* g = GRP_REP->get(grp_id);
  if (g == 0)
  {
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  //- get attr name from the 3rd input arg
  std::string attr;
  if (MEX_ARGS->get_input_string(k3RD_ARG, attr) == kError)
  {
    std::string r = "failed to read attribute on group " + g->get_name();
    MEX_UTILS->push_error(r.c_str(),
                          "invalid argument specified for attribute name",
                          "TangoBinding::group_read_attribute_asynch");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  //- get <forward> option the 4th input arg
  const mxArray* mx_forward = MEX_ARGS->get_input_array(k4TH_ARG, kMX_ANY, 1, 1);
  if (mx_forward == 0 || ::mxIsNumeric(mx_forward) == false)
  {
    std::string r = "failed to read attribute " + attr + " on group " + g->get_name();
    MEX_UTILS->push_error(r.c_str(),
                          "forward option is either missing or has an invalid format",
                          "TangoBinding::group_read_attribute_asynch");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  bool forward = ::mxGetPr(mx_forward)[0] ? true : false;
  //- com tmo expired for the specified group?
  if (GRP_REP->com_timeout_expired(grp_id))
  {
    try
    {
      //- MEX_UTILS->notify("com. tmo expired for specified group. forcing ping...");
      g->ping();
    }
    catch (...)
    {
      //- ignore any error
    }
  }
  int idx = 1;
  int attr_id = kError;
  DeviceDesc * ddesc = 0;
  Tango::DeviceProxy* dp = 0;
  do
  {
    try
    {
      //- get device descriptor of the i-th device in the group
      dp = g->get_device(idx);
      if (dp)
      {
        //- get device descriptor of the i-th device in the group
        ddesc = DEV_REP->device_desc(dp, false, false);
        if (ddesc)
        {
          //- Is <attr> a valid attribute for <dev>
          attr_id = ddesc->attr_exists(attr);
          if (attr_id != kError)
            break;
        }
      }
    }
    catch (...)
    {
      //- i-th device in the group is not reachable
      //- ignore exception and retry with next device
    }
    //- Next device in the group
    idx++;
  } while (idx <= g->get_size());
  //- Attribute not found
  if (attr_id == kError)
  {
    std::string r = "failed to read attribute <" + attr + "> on group " + g->get_name();
    std::string d = "can't find a running device with attribute <" + attr + "> in group " + g->get_name();
    MEX_UTILS->set_error(r.c_str(),
                         d.c_str(),
                         "TangoBinding::group_read_attribute_asynch");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  //- Read attribute 
  Tango::DevLong req_id;
  _TRY(req_id = g->read_attribute_asynch(attr, forward), 
       g->get_name() , 
       "TangoBinding::group_read_attribute_asynch");
  //- Create a 1-by-1 struct array for result
  const mwSize dims[2] = {1, 1};
  const char *field_names[] = {"grp_id", "dev_id", "attr_id", "req_id"};
  mxArray * argout = ::mxCreateStructArray(2, dims, 4, field_names);
  if (! argout)
  {
    MEX_UTILS->set_error("out of memory",
                         "mxCreateStructArray failed",
                         "TangoBinding::group_read_attribute_asynch");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  //- set field argout.group_id
  mxArray * mx_array = ::mxCreateNumericArray(2, dims, ::mxINT32_CLASS, ::mxREAL);
  if (! mx_array)
  {
    MEX_UTILS->set_error("out of memory",
                         "mxCreateNumericArray failed",
                         "TangoBinding::group_read_attribute_asynch");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  reinterpret_cast<Tango::DevLong*>(::mxGetPr(mx_array))[0] = grp_id;
  ::mxSetFieldByNumber(argout, 0, 0, mx_array);
  //- set field argout.dev_id
  mx_array = ::mxCreateString(dp->dev_name().c_str());
  if (! mx_array)
  {
    MEX_UTILS->set_error("out of memory",
                         "mxCreateString failed",
                         "TangoBinding::group_read_attribute_asynch");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  ::mxSetFieldByNumber(argout, 0, 1, mx_array);
  //- set field argout.cmd_id
  mx_array = ::mxCreateNumericArray(2, dims, ::mxINT32_CLASS, ::mxREAL);
  if (! mx_array)
  {
    MEX_UTILS->set_error("out of memory",
                         "mxCreateNumericArray failed",
                         "TangoBinding::group_read_attribute_asynch");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  reinterpret_cast<Tango::DevLong*>(::mxGetPr(mx_array))[0] = attr_id;
  ::mxSetFieldByNumber(argout, 0, 2, mx_array);
  //- set field argout.req_id
  mx_array = ::mxCreateNumericArray(2, dims, ::mxINT32_CLASS, ::mxREAL);
  if (! mx_array)
  {
    MEX_UTILS->set_error("out of memory",
                         "mxCreateNumericArray failed",
                         "TangoBinding::group_read_attribute_asynch");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  reinterpret_cast<Tango::DevLong*>(::mxGetPr(mx_array))[0] = req_id;
  ::mxSetFieldByNumber(argout, 0, 3, mx_array);
  //- Set output arg
  if (MEX_ARGS->set_output_array(k1ST_ARG, argout) == kError)
  {
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  return kNoError;
}

//=============================================================================
//- TangoBinding::group_read_attribute_reply
//=============================================================================
int TangoBinding::group_read_attribute_reply (void)
{
  //- get request descriptor from 2nd input arg
  const mxArray* mx_rdesc = MEX_ARGS->get_input_array(k2ND_ARG, ::mxSTRUCT_CLASS, 1, 1);
  if (mx_rdesc == 0)
  {
    MEX_UTILS->push_error("invalid request descriptor specified",
                          "request descriptor is either missing or has an invalid format",
                          "TangoBinding::group_read_attribute_reply");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  //- mx_rdesc struct should contain a <grp_id> field
  mxArray * mx_array = ::mxGetField(mx_rdesc, 0, "grp_id");
  if (! mx_array)
  {
    MEX_UTILS->set_error("invalid argin specified",
                         "struct should contain a field named 'grp_id'",
                         "TangoBinding::group_read_attribute_reply");
    SET_DEFAULT_PRHS_THEN_RETURN(kError);                 
  }
  //- grp_id must be 1-by-n int32 array
  if (::mxIsInt32(mx_array) == false || ::mxGetM(mx_array) != 1 || ::mxGetN(mx_array) != 1)
  {
    MEX_UTILS->set_error("invalid request descriptor specified",
                         "1-by-1 int32 array expected for field 'grp_id'",
                         "TangoBinding::group_read_attribute_reply");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  Tango::DevLong grp_id = reinterpret_cast<Tango::DevLong*>(::mxGetPr(mx_array))[0];
  //- mx_rdesc struct should contain a <dev_id> field
  mx_array = ::mxGetField(mx_rdesc, 0, "dev_id");
  if (! mx_array)
  {
    MEX_UTILS->set_error("invalid argin specified",
                         "struct should contain a field named 'dev_id'",
                         "TangoBinding::group_read_attribute_reply");
    SET_DEFAULT_PRHS_THEN_RETURN(kError);                 
  }
  //- dev_id must be 1-by-n char array
  if (::mxIsChar(mx_array) == false || ::mxGetM(mx_array) != 1)
  {
    MEX_UTILS->set_error("invalid request descriptor specified",
                         "1-by-n char array expected for field 'dev_id'",
                         "TangoBinding::group_read_attribute_reply");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  //- extract dev_id string from mxArray
  char* dev_id_cstr = ::mxArrayToString(mx_array);
  if (dev_id_cstr == 0)
  {
    MEX_UTILS->set_error("invalid request descriptor specified",
                         "could not extract string from field",
                         "TangoBinding::group_read_attribute_reply");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  std::string dev_name(dev_id_cstr);
  ::mxFree(dev_id_cstr);
  //- mx_rdesc struct should contain a <attr_id> field
  mx_array = ::mxGetField(mx_rdesc, 0, "attr_id");
  if (! mx_array)
  {
    MEX_UTILS->set_error("invalid request descriptor specified",
                         "struct should contain a field named 'attr_id'",
                         "TangoBinding::group_read_attribute_reply");
    SET_DEFAULT_PRHS_THEN_RETURN(kError);                 
  }
  //- attr_id must be 1-by-1 int32 char array
  if (::mxIsInt32(mx_array) == false || ::mxGetM(mx_array) != 1 || ::mxGetN(mx_array) != 1)
  {
    MEX_UTILS->set_error("invalid request descriptor specified",
                         "1-by-1 int32 array expected for field 'attr_id'",
                         "TangoBinding::group_read_attribute_reply");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  //- extract cmd_id string from mxArray
  Tango::DevLong attr_id = reinterpret_cast<Tango::DevLong*>(::mxGetPr(mx_array))[0];
  //- mx_rdesc struct should contain a <req_id> field
  mx_array = ::mxGetField(mx_rdesc, 0, "req_id");
  if (! mx_array)
  {
    MEX_UTILS->set_error("invalid request descriptor specified",
                         "struct should contain a field named 'req_id'",
                         "TangoBinding::group_read_attribute_reply");
    SET_DEFAULT_PRHS_THEN_RETURN(kError);                 
  }
  //- req_id must be 1-by-1 int32 char array
  if (::mxIsInt32(mx_array) == false || ::mxGetM(mx_array) != 1 || ::mxGetN(mx_array) != 1)
  {
    MEX_UTILS->set_error("invalid argin specified",
                         "1-by-1 int32 array expected for field 'req_id'",
                         "TangoBinding::group_read_attribute_reply");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  //- extract request ID from mxArray
  Tango::DevLong req_id = reinterpret_cast<Tango::DevLong*>(::mxGetPr(mx_array))[0];
  //- get <timeout> option from the 3rd input arg
  const mxArray* mx_tmo = MEX_ARGS->get_input_array(k3RD_ARG, kMX_ANY, 1, 1);
  if (mx_tmo == 0)
  {
    MEX_UTILS->push_error("invalid argin specified",
                          "timeout parameter is either missing or has an invalid format",
                          "TangoBinding::group_read_attribute_reply");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  Tango::DevLong tmo = (Tango::DevLong)::mxGetPr(mx_tmo)[0];
  //- get group reference
  Tango::Group*  g = GRP_REP->get(grp_id);
  if (g == 0)
  {
    MEX_UTILS->push_error("failed to execute group_read_attribute_reply",
                          "invalid group id - no such group",
                          "TangoBinding::group_read_attribute_reply");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  //- Asynch. request responses
  Tango::GroupAttrReplyList arl;
  //- Try to get request responses
  _TRY(arl = g->read_attribute_reply(req_id, tmo), 
       g->get_name(), 
       "group_read_attribute_reply");
  //- get device descriptor from repository
  DeviceDesc * ddesc = DEV_REP->device_desc(dev_name);
  if (! ddesc)
  {
    MEX_UTILS->push_error("failed to execute group_read_attribute_reply",
                          "invalid device id - no such device",
                          "TangoBinding::group_read_attribute_reply");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  //- Create a 1-by-1 struct array for results
  const mwSize dims[2] = {1, 1};
  const char *field_names[] = {"has_failed", "replies"};
  mxArray * main_argout = ::mxCreateStructArray(2, dims, 2, field_names);
  if (main_argout == 0)
  {
    MEX_UTILS->set_error("out of memory",
                         "mxCreateStructArray failed",
                         "TangoBinding::group_read_attribute_reply");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  //- Set main_argout.has_failed() field
  mx_array = ::mxCreateDoubleScalar(arl.has_failed() ? 1 : 0);
  ::mxSetFieldByNumber(main_argout, 0, 0, mx_array);
  //- Set main_argout.replies field
  const mwSize replies_dims[2] = {1, arl.size()};
  const char * replies_field_names[] = {
			"name",		//- 0
			"has_failed",	//- 1
			"error",	//- 2
			"m",		//- 3
			"n",		//- 4
			"quality",	//- 5
			"quality_str",	//- 6
			"time",		//- 7
			"value",	//- 8
			"setpoint",	//- 9
			"dev_name",	//- 10
			"is_enabled"	//- 11
                                       };
  mxArray * mx_replies = ::mxCreateStructArray(2, replies_dims, 12, replies_field_names);
  if (mx_replies == 0) 
  {
    MEX_UTILS->set_error("out of memory",
                         "mxCreateStructArray failed",
                         "TangoBinding::group_read_attribute_reply");
    ::mxDestroyArray(main_argout);
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  //- Attach replies to main argout 
  ::mxSetFieldByNumber(main_argout, 0, 1, mx_replies);
  //- Decode argout for each reply in le list
  for (size_t r = 0; r < arl.size(); r++) 
  {
    mx_array = ::mxCreateString(arl[r].dev_name().c_str());
    ::mxSetFieldByNumber(mx_replies, r, 10, mx_array);		// dev_name
    mx_array = ::mxCreateDoubleScalar((arl[r].group_element_enabled()) ? 1 : 0);
    ::mxSetFieldByNumber(mx_replies, r, 11, mx_array);		// enabled field

    if (arl[r].group_element_enabled()) {
       if (this->mxattribute(ddesc,arl[r].get_data(),mx_replies,r,true) == kError) {
         ::mxDestroyArray(main_argout);
          SET_DEFAULT_PRHS_THEN_RETURN(kError);
       }
    }
  }
  //- Set output arg
  if (MEX_ARGS->set_output_array(k1ST_ARG, main_argout) == kError)
  {
    ::mxDestroyArray(main_argout);
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  return kNoError;
}

//=============================================================================
//- TangoBinding::group_read_attributes_asynch 
//=============================================================================
int TangoBinding::group_read_attributes_asynch (void)
{
  //- get group id from the 2nd input arg
  const mxArray* mx_grp_id = MEX_ARGS->get_input_array(k2ND_ARG, kMX_ANY, 1, 1);
  if (mx_grp_id == 0 || ::mxIsNumeric(mx_grp_id) == false)
  {
    MEX_UTILS->set_error("invalid argin specified",
                         "group id is either missing or has an invalid format",
                         "TangoBinding::group_read_attribute_asynch");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }

  //- extract group ref from repository
  Tango::DevLong grp_id = (Tango::DevLong)::mxGetPr(mx_grp_id)[0];

  //- get group ref from repository
  Tango::Group* g = GRP_REP->get(grp_id);
  if (! g) 
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 

  //- get attributes list name from the 3rd MEX-file input arg
  const mxArray* attr_list = MEX_ARGS->get_input_array(k3RD_ARG, ::mxCELL_CLASS, 1);
  if (attr_list == 0) 
  {
    MEX_UTILS->push_error("invalid argin specified",
       "invalid attribute list specified (wrong type or dims)",
       "TangoBinding::read_attributes");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }

  //- get <forward> option the 4th input arg
  const mxArray* mx_forward = MEX_ARGS->get_input_array(k4TH_ARG, kMX_ANY, 1, 1);
  if (mx_forward == 0 || ::mxIsNumeric(mx_forward) == false) 
  {
    std::string r = "failed to read attributes on group " + g->get_name();
    MEX_UTILS->push_error(r.c_str(),
                          "forward option is either missing or has an invalid format",
                          "TangoBinding::group_read_attribute_asynch");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }

  //- extract <forward> option from mxArray
  bool forward = ::mxGetPr(mx_forward)[0] ? true : false;

  //- get num of attr to read
  int n_attrs = ::mxGetN(attr_list);

  //- com tmo expired for the specified group?
  if (GRP_REP->com_timeout_expired(grp_id))
  {
    try
    {
      //- MEX_UTILS->notify("com. tmo expired for specified group. forcing ping...");
      g->ping();
    }
    catch (...)
    {
      //- ignore any error
    }
  }

  //- dev-names
  std::vector<std::string> dev_names;
  dev_names.resize(n_attrs);

  //- attr-names
  std::vector<std::string> attr_names;
  attr_names.resize(n_attrs);

  //- attr-ids
  std::vector<Tango::DevLong> attr_ids;
  attr_ids.resize(n_attrs);

  //- fill attr-names
  for (int i = 0; i < n_attrs; i++) 
  {
    //- get <i>th cell of the array
    mxArray * cell = ::mxGetCell(attr_list, i);
    if (! cell) 
    {
      MEX_UTILS->set_error("internal error",
                           "unexpected NULL cell",
                           "TangoBinding::group_read_attributes_asynch");
      SET_DEFAULT_PRHS_THEN_RETURN(kError); 
    }
    //- cell should contain a string
    if (::mxIsChar(cell) == false) 
    {
      MEX_UTILS->set_error("internal error",
                           "unexpected cell content (string expected)",
                           "TangoBinding::group_read_attributes_asynch");
      SET_DEFAULT_PRHS_THEN_RETURN(kError); 
    }
    //- get c-string from matlab char array.
    char* cstr = ::mxArrayToString(cell);
    if (! cstr) 
    {
      MEX_UTILS->set_error("internal error", 
                           "could not extract string from cell",
                           "TangoBinding::group_read_attributes_asynch");
      SET_DEFAULT_PRHS_THEN_RETURN(kError); 
    }
    //- store attr name
    attr_names[i] = cstr;
    //- release cstr
    ::mxFree(cstr);
    //- set attr_ids[i] to kError
    attr_ids[i] = kError;
    //- set dev_names[i] to "unknown"
    dev_names[i] = "unknown";
    //- find a device having an attribute named attr_names[i] in the group
    int idx = 1;
    DeviceDesc * ddesc = 0;
    do 
    {
      try 
      {
        //- get device descriptor of the i-th device in the group
        Tango::DeviceProxy* dp = g->get_device(idx);
        if (dp) 
        {
          //- get device descriptor of the i-th device in the group
          ddesc = DEV_REP->device_desc(dp, false, false);
          if (ddesc) 
          {
            //- Is <attr> a valid attribute for <dev>
            attr_ids[i] = ddesc->attr_exists(attr_names[i]);
            if (attr_ids[i] != kError) 
            {
              dev_names[i] = dp->dev_name();
              break;
            }
          }
        }
      }
      catch (...) 
      {
        //- i-th device in the group is not reachable
        //- ignore exception and retry with next device
      }
      //- Next device in the group
      idx++;
    } 
    while (idx <= g->get_size());

    //- attribute found?
    if (attr_ids[i] == kError) 
    {
      std::string r = "failed to read attribute <" + attr_names[i] + "> on group " + g->get_name();
      std::string d = "can't find a running device with attribute <" + attr_names[i] + "> in group " + g->get_name();
      MEX_UTILS->set_error(r.c_str(),
                           d.c_str(),
                           "TangoBinding::group_read_attributes_asynch");
      SET_DEFAULT_PRHS_THEN_RETURN(kError); 
    }
  }

  //- read attributes 
  Tango::DevLong req_id;
  _TRY(req_id = g->read_attributes_asynch(attr_names, forward), 
       g->get_name(), 
       "TangoBinding::group_read_attributes_asynch");

  //- create a 1-by-1 struct array to return asynch. request struct
  mwSize dims[2] = {1, 1};
  const char * field_names[] = {"grp_id", "dev_ids", "attr_ids", "req_id"};
  mxArray * argout = ::mxCreateStructArray(2, dims, 4, field_names);
  if (! argout) 
  {
    MEX_UTILS->set_error("out of memory",
                         "mxCreateStructArray failed",
                         "TangoBinding::group_read_attributes_asynch");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }

  //- set field argout.group_id
  mxArray * mx_array = ::mxCreateNumericArray(2, dims, ::mxINT32_CLASS, ::mxREAL);
  if (! mx_array) 
  {
    MEX_UTILS->set_error("out of memory",
                         "mxCreateNumericArray failed",
                         "TangoBinding::group_read_attributes_asynch");
    ::mxDestroyArray(argout);
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  reinterpret_cast<Tango::DevLong*>(::mxGetPr(mx_array))[0] = grp_id;
  ::mxSetFieldByNumber(argout, 0, 0, mx_array);

  //- set field argout.dev_id
  dims[1] = dev_names.size();
  mx_array = ::mxCreateCellArray(2, dims);
  if (! mx_array) 
  {
    MEX_UTILS->set_error("out of memory",
                         "mxCreateCellArray failed",
                         "TangoBinding::group_read_attributes_asynch");
    ::mxDestroyArray(argout);
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  for (size_t i = 0; i < dev_names.size(); i++)
  {
    ::mxSetCell(mx_array, i, ::mxCreateString(dev_names[i].c_str())); 
  } 
  ::mxSetFieldByNumber(argout, 0, 1, mx_array);
  
  //- set field argout.attr_ids
  dims[1] = attr_ids.size();
  mx_array = ::mxCreateNumericArray(2, dims, ::mxINT32_CLASS, ::mxREAL);
  if (! mx_array) 
  {
    MEX_UTILS->set_error("out of memory",
                         "mxCreateNumericArray failed",
                         "TangoBinding::group_read_attributes_asynch");
    ::mxDestroyArray(argout);
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  for (size_t i = 0; i < attr_ids.size(); i++)
  {
    reinterpret_cast<Tango::DevLong*>(::mxGetPr(mx_array))[i] = attr_ids[i];
  }
  ::mxSetFieldByNumber(argout, 0, 2, mx_array);

  //- set field argout.req_id
  dims[1] = 1;
  mx_array = ::mxCreateNumericArray(2, dims, ::mxINT32_CLASS, ::mxREAL);
  if (! mx_array) 
  {
    MEX_UTILS->set_error("out of memory",
                         "mxCreateNumericArray failed",
                         "TangoBinding::group_read_attributes_asynch");
    ::mxDestroyArray(argout);
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  reinterpret_cast<Tango::DevLong*>(::mxGetPr(mx_array))[0] = req_id;
  ::mxSetFieldByNumber(argout, 0, 3, mx_array);

  //- set output arg
  if (MEX_ARGS->set_output_array(k1ST_ARG, argout) == kError) 
  {
    ::mxDestroyArray(argout);
    SET_DEFAULT_PRHS_THEN_RETURN(kError);
  }

  return kNoError;
}

//=============================================================================
//- TangoBinding::group_read_attributes_reply
//=============================================================================
int TangoBinding::group_read_attributes_reply (void)
{
  //- get request descriptor from 2nd input arg
  const mxArray* mx_rdesc = MEX_ARGS->get_input_array(k2ND_ARG, ::mxSTRUCT_CLASS, 1, 1);
  if (! mx_rdesc) 
  {
    MEX_UTILS->push_error("invalid request descriptor specified",
                          "request descriptor is either missing or has an invalid format",
                          "TangoBinding::group_read_attributes_reply");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }

  //- mx_rdesc struct should contain a <grp_id> field
  mxArray * mx_array = ::mxGetField(mx_rdesc, 0, "grp_id");
  if (! mx_array) 
  {
    MEX_UTILS->set_error("invalid argin specified",
                         "struct should contain a field named 'grp_id'",
                         "TangoBinding::group_read_attributes_reply");
    SET_DEFAULT_PRHS_THEN_RETURN(kError);                 
  }

  //- grp_id must be 1-by-n int32 array
  if (::mxIsInt32(mx_array) == false || ::mxGetM(mx_array) != 1 || ::mxGetN(mx_array) != 1) 
  {
    MEX_UTILS->set_error("invalid request descriptor specified",
                         "1-by-1 int32 array expected for field 'grp_id'",
                         "TangoBinding::group_read_attributes_reply");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }

  //- extract group identidier from mxArray
  Tango::DevLong grp_id = reinterpret_cast<Tango::DevLong*>(::mxGetPr(mx_array))[0];

  //- mx_rdesc struct should contain a <dev_ids> field
  mx_array = ::mxGetField(mx_rdesc, 0, "dev_ids");
  if (! mx_array) 
  {
    MEX_UTILS->set_error("invalid argin specified",
                         "struct should contain a field named 'dev_ids'",
                         "TangoBinding::group_read_attributes_reply");
    SET_DEFAULT_PRHS_THEN_RETURN(kError);                 
  }

  //- dev_id must be 1-by-n char array
  if (::mxIsCell(mx_array) == false || ::mxGetM(mx_array) != 1) 
  {
    MEX_UTILS->set_error("invalid request descriptor specified",
                         "1-by-n cell array expected for field 'dev_ids'",
                         "TangoBinding::group_read_attributes_reply");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }

  size_t len = ::mxGetN(mx_array);
  std::vector<std::string> dev_ids(len);
  for (size_t i = 0; i < len; i++) 
  {
    //- get <i>th cell of the array
    mxArray * cell = ::mxGetCell(mx_array, i);
    if (! cell) 
    {
      MEX_UTILS->set_error("internal error",
                           "unexpected NULL cell",
                           "TangoBinding::group_read_attributes_reply");
      SET_DEFAULT_PRHS_THEN_RETURN(kError); 
    }

    //- cell should contain a string
    if (::mxIsChar(cell) == false) 
    {
      MEX_UTILS->set_error("internal error",
                           "unexpected cell content (string expected)",
                           "TangoBinding::group_read_attributes_reply");
      SET_DEFAULT_PRHS_THEN_RETURN(kError); 
    }

    //- get c-string from matlab char array.
    char * cstr = ::mxArrayToString(cell);
    if (! cstr)
    {
      MEX_UTILS->set_error("internal error", 
                           "could not extract string from cell",
                           "TangoBinding::group_read_attributes_reply");
      SET_DEFAULT_PRHS_THEN_RETURN(kError);
    }

    //- assign i-th string of dev_ids
    dev_ids[i] = cstr;
    ::mxFree(cstr);

  } //- for i ...

  //- mx_rdesc struct should contain a <attr_ids> field
  mx_array = ::mxGetField(mx_rdesc, 0, "attr_ids");
  if (! mx_array) 
  {
    MEX_UTILS->set_error("invalid request descriptor specified",
                         "struct should contain a field named 'attr_ids'",
                         "TangoBinding::group_read_attributes_reply");
    SET_DEFAULT_PRHS_THEN_RETURN(kError);                 
  }

  //- attr_id must be 1-by-n int32 array
  if (::mxIsInt32(mx_array) == false || ::mxGetM(mx_array) != 1) 
  {
    MEX_UTILS->set_error("invalid request descriptor specified",
                         "1-by-1 int32 array expected for field 'attr_ids'",
                         "TangoBinding::group_read_attributes_reply");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }

  len = ::mxGetN(mx_array);
  std::vector<int> attr_ids(len);
  for (size_t i = 0; i < len; i++)
  {
    attr_ids[i] = reinterpret_cast<Tango::DevLong*>(::mxGetPr(mx_array))[i];
  }

  //- mx_rdesc struct should contain a <req_id> field
  mx_array = ::mxGetField(mx_rdesc, 0, "req_id");
  if (! mx_array) 
  {
    MEX_UTILS->set_error("invalid request descriptor specified",
                         "struct should contain a field named 'req_id'",
                         "TangoBinding::group_read_attributes_reply");
    SET_DEFAULT_PRHS_THEN_RETURN(kError);                 
  }

  //- req_id must be 1-by-1 int32 array
  if (::mxIsInt32(mx_array) == false || ::mxGetM(mx_array) != 1 || ::mxGetN(mx_array) != 1) 
  {
    MEX_UTILS->set_error("invalid argin specified",
                         "1-by-1 int32 array expected for field 'req_id'",
                         "TangoBinding::group_read_attributes_reply");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }

  //- extract req_id from mxArray
  Tango::DevLong req_id = reinterpret_cast<Tango::DevLong*>(::mxGetPr(mx_array))[0];

  //- get <timeout> option from the 3rd input arg
  const mxArray* mx_tmo = MEX_ARGS->get_input_array(k3RD_ARG, kMX_ANY, 1, 1);
  if (! mx_tmo) 
  {
    MEX_UTILS->push_error("invalid argin specified",
                          "timeout parameter is either missing or has an invalid format",
                          "TangoBinding::group_read_attributes_reply");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }

  //- extract tmo from mxArray
  Tango::DevLong tmo = (Tango::DevLong)::mxGetPr(mx_tmo)[0];

  //- get group reference
  Tango::Group*  g = GRP_REP->get(grp_id);
  if (g == 0) 
  {
    MEX_UTILS->push_error("failed to execute group_read_attributes_reply",
                          "invalid group id - no such group",
                          "TangoBinding::group_read_attributes_reply");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }

  //- asynch. request responses
  Tango::GroupAttrReplyList arl;
  _TRY(arl = g->read_attributes_reply(req_id, tmo), g->get_name(), "group_read_attributes_reply");

  //- create a 1-by-1 struct array for results
  double *has_failed2; 
  mwSize dims[2] = {1, 1};
  const char *field_names[] = {"has_failed", "dev_replies"};
  mxArray * main_argout = ::mxCreateStructArray(2, dims, 2, field_names);
  if (main_argout == 0) 
  {
    MEX_UTILS->set_error("out of memory",
                         "mxCreateStructArray failed",
                         "TangoBinding::group_read_attributes_reply");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }

  //- Set main_argout.has_failed() field
  mx_array = ::mxCreateDoubleScalar(arl.has_failed() ? 1 : 0);
  ::mxSetFieldByNumber(main_argout, 0, 0, mx_array);

  //- device replies
  const mwSize dev_replies_dims[2] = {1, g->get_size()};
  const char *dev_replies_field_names[] = {"dev_name", "is_enabled", "has_failed", "attr_values"};
  mxArray * dev_replies = ::mxCreateStructArray(2, dev_replies_dims, 4, dev_replies_field_names);
  if (dev_replies == 0) 
  {
    MEX_UTILS->set_error("out of memory",
                         "mxCreateStructArray failed",
                         "TangoBinding::group_read_attributes_reply");
    ::mxDestroyArray(main_argout);
    SET_DEFAULT_PRHS_THEN_RETURN(kError);
  }
  ::mxSetFieldByNumber(main_argout, 0, 1, dev_replies);

  //- now build returned struct 
  size_t num_of_attr_read_per_device =   arl.size() / g->get_size();
  if (num_of_attr_read_per_device == 0)
  {
    MEX_UTILS->set_error("internal error",
                         "unexpected array length",
                         "TangoBinding::group_read_attributes_reply");
    ::mxDestroyArray(main_argout);
    SET_DEFAULT_PRHS_THEN_RETURN(kError);
  }

  const mwSize attr_values_dims[2] = {1, static_cast<mwSize>(num_of_attr_read_per_device)};
  const char *attr_values_field_names[] = { 
			"name",		//- 0
			"has_failed",	//- 1
			"error",	//- 2
			"m",		//- 3
			"n",		//- 4
			"quality",	//- 5
			"quality_str",	//- 6
			"time",		//- 7
			"value",	//- 8
			"setpoint",	//- 9
			"dev_name",	//- 10
			"is_enabled"	//- 11
                                          };
  mxArray * attr_values_array = 0;
  std::string dev_name("none");
  int cdn = -1;
  int attr_values_idx = -1;
  double *is_enabled2;
  for (size_t r = 0; r < arl.size(); r++) 
  {
    //- reach next device?
    if (dev_name != arl[r].dev_name())
    {
      //- update device index
      cdn++;

      //- set dev_replies.dev_name field
      dev_name = arl[r].dev_name();

      mx_array = ::mxCreateString(dev_name.c_str());
      if (! mx_array) 
      {
        MEX_UTILS->set_error("out of memory",
                             "mxCreateString failed",
                             "TangoBinding::group_read_attributes_reply");
        ::mxDestroyArray(main_argout);
        SET_DEFAULT_PRHS_THEN_RETURN(kError);
      }
      ::mxSetFieldByNumber(dev_replies, cdn, 0, mx_array);

      //- set dev_replies.is_enabled field to its default value
      mx_array = ::mxCreateDoubleScalar(1);
      if (! mx_array) 
      {
        MEX_UTILS->set_error("out of memory",
                             "mxCreateString failed",
                             "TangoBinding::group_read_attributes_reply");
        ::mxDestroyArray(main_argout);
        SET_DEFAULT_PRHS_THEN_RETURN(kError);
      }
      ::mxSetFieldByNumber(dev_replies, cdn, 1, mx_array);
      is_enabled2=::mxGetPr(mx_array);

      //- set dev_replies.has_failed field to its default value
      mx_array = ::mxCreateDoubleScalar(0);
      if (! mx_array) 
      {
        MEX_UTILS->set_error("out of memory",
                             "mxCreateString failed",
                             "TangoBinding::group_read_attributes_reply");
        ::mxDestroyArray(main_argout);
        SET_DEFAULT_PRHS_THEN_RETURN(kError);
      }
      ::mxSetFieldByNumber(dev_replies, cdn, 2, mx_array);
      has_failed2=::mxGetPr(mx_array);

      //- create dev_replies.attr_values field
      attr_values_array = ::mxCreateStructArray(2, attr_values_dims, 12, attr_values_field_names);
      if (attr_values_array == 0) 
      {
        MEX_UTILS->set_error("out of memory",
                             "mxCreateStructArray failed",
                             "TangoBinding::group_read_attributes_reply");
        ::mxDestroyArray(main_argout);
        SET_DEFAULT_PRHS_THEN_RETURN(kError);
      }
      ::mxSetFieldByNumber(dev_replies, cdn, 3, attr_values_array);

      //- reset attr_values index
      attr_values_idx = -1;
    }

    //- next attribute for current device
    attr_values_idx++;

    mx_array = ::mxCreateString(arl[r].dev_name().c_str());
    ::mxSetFieldByNumber(attr_values_array, attr_values_idx, 10, mx_array);	// dev_name
    mx_array = ::mxCreateDoubleScalar((arl[r].group_element_enabled()) ? 1 : 0);
    ::mxSetFieldByNumber(attr_values_array, attr_values_idx, 11, mx_array);	// is_enabled
    
    if (arl[r].get_data().has_failed()) {
       *has_failed2 = 1;
    }
    if (arl[r].group_element_enabled()) {
       DeviceDesc * ddesc = DEV_REP->device_desc(dev_ids[r % dev_ids.size()]);
       if (this->mxattribute(ddesc,arl[r].get_data(),attr_values_array,attr_values_idx,this->v1_compatibility_enabled()) == kError) {
         ::mxDestroyArray(main_argout);
          SET_DEFAULT_PRHS_THEN_RETURN(kError);
       }
    }
    else {
      *is_enabled2 = 0;
    }
  }

  //- Set output arg
  if (MEX_ARGS->set_output_array(k1ST_ARG, main_argout) == kError) 
  {
    ::mxDestroyArray(main_argout);
    SET_DEFAULT_PRHS_THEN_RETURN(kError);
  }
  return kNoError;
}

//=============================================================================
//- TangoBinding::group_enable_device
//=============================================================================
int TangoBinding::group_enable_device (void) 
{
  //- get group id from the 2nd input arg
  const mxArray* mx_grp_id = MEX_ARGS->get_input_array(k2ND_ARG, kMX_ANY, 1, 1);
  if (mx_grp_id == 0 || ::mxIsNumeric(mx_grp_id) == false) 
  {
    MEX_UTILS->set_error("invalid argin specified",
                         "group id is either missing or has an invalid format",
                         "TangoBinding::group_enable_device");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }

  Tango::DevLong grp_id = (Tango::DevLong)::mxGetPr(mx_grp_id)[0];

  //- get parent group from repository
  Tango::Group* g = GRP_REP->get(grp_id);
  if (g == 0)
  {
     //- Invalid group id specified (error handled by repository)
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }

  //- get <device name> from 3rd input arg
  std::string dev_name;
  if (MEX_ARGS->get_input_string(k3RD_ARG, dev_name) == kError) 
  {
    SET_DEFAULT_PRHS_THEN_RETURN(kError);
  }

  //- get <forward> option from 4th input arg
  const mxArray* mx_forward = MEX_ARGS->get_input_array(k4TH_ARG, kMX_ANY, 1, 1);
  if (mx_forward == 0 || ::mxIsNumeric(mx_forward) == false) 
  {
    std::string r = "could not ping group " + g->get_name();
    MEX_UTILS->push_error(r.c_str(),
                          "forward option is either missing or has an invalid format",
                          "TangoBinding::group_enable_device");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }

  bool forward = ::mxGetPr(mx_forward)[0] ? true : false;

  //- enable the device
  g->enable(dev_name, forward);

  //- create a 1-by-1 double array
  mxArray * argout = ::mxCreateDoubleMatrix(1,1,mxREAL);
  if (! argout) 
  {
    MEX_UTILS->set_error("out of memory",
                         "mxCreateDoubleMatrix failed",
                         "TangoBinding::group_enable_device");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  ::mxGetPr(argout)[0] = 0;

  //- set output arg
  if (MEX_ARGS->set_output_array(k1ST_ARG, argout) == kError) 
  {
    ::mxDestroyArray(argout);
    SET_DEFAULT_PRHS_THEN_RETURN(kError);
  }

  return kNoError;  
}

//=============================================================================
//- TangoBinding::group_disable_device
//=============================================================================
int TangoBinding::group_disable_device (void) 
{
  //- get group id from the 2nd input arg
  const mxArray* mx_grp_id = MEX_ARGS->get_input_array(k2ND_ARG, kMX_ANY, 1, 1);
  if (mx_grp_id == 0 || ::mxIsNumeric(mx_grp_id) == false) 
  {
    MEX_UTILS->set_error("invalid argin specified",
                         "group id is either missing or has an invalid format",
                         "TangoBinding::group_disable_device");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }

  Tango::DevLong grp_id = (Tango::DevLong)::mxGetPr(mx_grp_id)[0];

  //- get parent group from repository
  Tango::Group* g = GRP_REP->get(grp_id);
  if (g == 0) 
  {
     //- Invalid group id specified (error handled by repository)
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }

  //- get <device name> from 3rd input arg
  std::string dev_name;
  if (MEX_ARGS->get_input_string(k3RD_ARG, dev_name) == kError)
    SET_DEFAULT_PRHS_THEN_RETURN(kError);

  //- get <forward> option from 4th input arg
  const mxArray* mx_forward = MEX_ARGS->get_input_array(k4TH_ARG, kMX_ANY, 1, 1);
  if (mx_forward == 0 || ::mxIsNumeric(mx_forward) == false) 
  {
    std::string r = "could not ping group " + g->get_name();
    MEX_UTILS->push_error(r.c_str(),
                          "forward option is either missing or has an invalid format",
                          "TangoBinding::group_disable_device");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  bool forward = ::mxGetPr(mx_forward)[0] ? true : false;

  //- disable the device
  g->disable(dev_name, forward);

  //- create a 1-by-1 double array
  mxArray * argout = ::mxCreateDoubleMatrix(1,1,mxREAL);
  if (! argout) 
  {
    MEX_UTILS->set_error("out of memory",
                         "mxCreateDoubleMatrix failed",
                         "TangoBinding::group_disable_device");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }

  ::mxGetPr(argout)[0] = 0;

  //- set output arg
  if (MEX_ARGS->set_output_array(k1ST_ARG, argout) == kError) 
  {
    ::mxDestroyArray(argout);
    SET_DEFAULT_PRHS_THEN_RETURN(kError);
  }

  return kNoError;  
}

#endif //- #if !defined (DISABLE_TANGO_GROUP_SUPPORT)

#if defined(_WINDOWS)
# pragma warning(pop)
#endif


