// ============================================================================
//
// = CONTEXT
//   TANGO binbing for Matlab
//
// = FILENAME
//   MexFile.cpp
//
// = AUTHOR
//   Nicolas Leclercq - SOLEIL
//
// ============================================================================

//=============================================================================
// DEPENDENCIEs
//=============================================================================
#include "StandardHeader.h"
#include "TangoBinding.h"

//-----------------------------------------------------------------------------
// GLOBAL CONSTS
//-----------------------------------------------------------------------------
const int kNoError =  0;
const int kError   = -1;

//=============================================================================
// STATIC MEMBERS
//=============================================================================
MexArgs MexFile::args;

int MexFile::initialized = 0;

const char *MexFile::kMEX_VERSION = "3.0.0";


//=============================================================================
// WINDOWS DLL HELL: DllMain
//=============================================================================
#if defined(_WINDOWS)
BOOL APIENTRY DllMain (HANDLE h, DWORD  reason_for_call, LPVOID r)
{
  switch (reason_for_call)
  {
    case DLL_PROCESS_ATTACH:
      try {
        MexFile::init();
      }
      catch (...) {
        return FALSE;
      }
      break;
    case DLL_THREAD_ATTACH:
      break;
    case DLL_THREAD_DETACH:
      break;
    case DLL_PROCESS_DETACH:
      try {
        MexFile::cleanup();
      }
      catch (...) {
        return FALSE;
      }
      break;
  }
  return TRUE;
}
#endif

//=============================================================================
// C like exit/cleanup function (just to make some compilers happy)
//=============================================================================
#if !defined(_WINDOWS)

extern "C" 
{
 void c_cleanup ();
}

void c_cleanup () 
{
  // delegate cleanup job to MexFile::cleanup
  MexFile::cleanup();
}

#endif

//=============================================================================
// MexFile::init
//=============================================================================
int MexFile::init (void) 
{
  // return if already initialized
  if (MexFile::initialized) {
    return kNoError;
  }

#if ! defined(SCILAB)
  mexLock();
#endif
  
  try
  {
    Tango::ApiUtil * tapiu = Tango::ApiUtil::instance();
    tapiu->create_orb();
	}
	catch (...)
	{
	  //-::mexPrintf("mexFunction::ORB initialization failed\n");
	  return kError;
  }
  
  // initialize utilities
  if (MexUtils::init() == kError) 
  {
    //-- can't use error stack since it may not be initialized
    return kError;
  }

  // initialize the TANGO binding class
  if (TangoBinding::init() == kError) 
  {
    //-::mexPrintf("mexFunction::TangoBinding initialization failed\n");
    return kError;
  }

#if ! defined(_WINDOWS)
  //- set cleanup function (called when mex-file is discarded from memory)
# if ! defined(SCILAB)
    ::mexAtExit(c_cleanup);
# else
    ::mexAtExit((mxArray*)(&c_cleanup));
# endif
#endif

  // initialization done 
  MexFile::initialized = 1;

  MEX_MSG(("MEX-File initialized"));

  return kNoError;
}

//=============================================================================
// MexFile::cleanup
//=============================================================================
void MexFile::cleanup (void) 
{
  //- mexUnlock();

  if (MexFile::initialized) 
  {
    MEX_MSG(("MexFile is about to be cleared...\n"));
    TangoBinding::cleanup();
    MEX_MSG(("TangoBinding::cleanup done\n"));
    MexUtils::cleanup();
    MEX_MSG(("MexUtils::cleanup done\n"));
    Tango::ApiUtil::cleanup();
    MEX_MSG(("Tango::ApiUtil::cleanup done\n"));
    MexFile::initialized = 0;
    MEX_MSG(("MexFile::cleanup done\n"));
  }
}
 
//=============================================================================
// MexFile::version
//=============================================================================
void MexFile::version (void) 
{
  //- Create a 1-by-n char array
  mxArray* mex_version = ::mxCreateString(kMEX_VERSION);
  if (mex_version == 0) {
    MEX_UTILS->set_error("out of memory",
                         "mxCreateString failed",
                         "MexFile::version");
    SET_DEFAULT_PRHS(-1); 
  }
  //- Set output arg
  if (MEX_ARGS->set_output_array(k1ST_ARG, mex_version) == kError) {
    SET_DEFAULT_PRHS(-1); 
  }
}

//=============================================================================
// MexFile::exec
//=============================================================================
void MexFile::exec (int nlhs, mxArray ** plhs, int nrhs, const mxArray ** prhs) 
{
#if !defined(_WINDOWS)
  // check initialization
  if (MexFile::initialized == 0 && MexFile::init() == kError) {
    ::mexErrMsgTxt("initialization failed");
    //-- no <return> needed here (done by mexErrMsgTxt)
  }
#endif

  //- check num of input arg - at least 1 arg expected (cmd-id).
  if (nrhs == 0) {
    ::mexErrMsgTxt("no input argument specified");
  }

  //- check first arg - must be a 16 bits integer (func_id).
  if (mxIsInt16(prhs[0]) == false) {
    ::mexErrMsgTxt("first mex function argument must be a 16 bits integer");
  }

  //- get cmd_id
  short cmd_id = *(static_cast<short*>(::mxGetData(prhs[0])));

  //- store mex function in/out arguments for later retrieval
  MEX_ARGS->set(nlhs, plhs, nrhs, prhs);

  //- reset global error code and stack
  if (cmd_id != MexFile::ERROR_CODE && cmd_id != MexFile::ERROR_STACK) {
    MEX_UTILS->reset_error();
  }

  try 
  {
    //- exec cmd
    switch (cmd_id) 
    {
      //-- MEX_VERSION
      case MexFile::MEX_VERSION:
        MexFile::version();
        break;
      //-- TANGO_VERSION
      case MexFile::TANGO_VERSION:
        TANGO_BINDING->tango_version();
        break;
      //-- EXPORT_ERROR_CODE
      case MexFile::ERROR_CODE:
        MEX_UTILS->error_code();
        break;
      //-- EXPORT_ERROR_STACK
      case MexFile::ERROR_STACK:
        MEX_UTILS->error_stack();
        break;
      //-- OPEN_DEVICE
      case MexFile::OPEN_DEVICE:
        TANGO_BINDING->open_device();
        break;
      //-- CLOSE_DEVICE
      case MexFile::CLOSE_DEVICE:
        TANGO_BINDING->close_device();
        break;
      //-- COMMAND_INOUT
      case MexFile::COMMAND_INOUT:
        TANGO_BINDING->command_inout();
        break;
      //-- COMMAND_INOUT_ASYNCH
      case MexFile::COMMAND_INOUT_ASYNCH:
        TANGO_BINDING->command_inout_asynch();
        break;
      //-- COMMAND_INOUT_REPLY
      case MexFile::COMMAND_INOUT_REPLY:
        TANGO_BINDING->command_inout_reply();
        break;
      //-- READ_ATTRIBUTE
      case MexFile::READ_ATTRIBUTE:
        TANGO_BINDING->read_attribute();
        break;
      //-- READ_ATTRIBUTE_ASYNCH
      case MexFile::READ_ATTRIBUTE_ASYNCH:
        TANGO_BINDING->read_attribute_asynch();
        break;
      //-- READ_ATTRIBUTE_REPLY
      case MexFile::READ_ATTRIBUTE_REPLY:
        TANGO_BINDING->read_attribute_reply();
        break;
      //-- READ_ATTRIBUTES
      case MexFile::READ_ATTRIBUTES:
        TANGO_BINDING->read_attributes();
        break;
      //-- READ_ATTRIBUTES_ASYNCH
      case MexFile::READ_ATTRIBUTES_ASYNCH:
        TANGO_BINDING->read_attributes_asynch();
        break;
      //-- READ_ATTRIBUTES_REPLY
      case MexFile::READ_ATTRIBUTES_REPLY:
        TANGO_BINDING->read_attributes_reply();
        break;
      //-- WRITE_ATTRIBUTE
      case MexFile::WRITE_ATTRIBUTE:
        TANGO_BINDING->write_attribute();
        break;
      //-- WRITE_ATTRIBUTE_ASYNCH
      case MexFile::WRITE_ATTRIBUTE_ASYNCH:
        TANGO_BINDING->write_attribute_asynch();
        break;
      //-- WRITE_ATTRIBUTE_REPLY
      case MexFile::WRITE_ATTRIBUTE_REPLY:
        TANGO_BINDING->write_attribute_reply();
        break;
      //-- WRITE_ATTRIBUTES
      case MexFile::WRITE_ATTRIBUTES:
        TANGO_BINDING->write_attributes();
        break;
      //-- WRITE_ATTRIBUTES_ASYNCH
      case MexFile::WRITE_ATTRIBUTES_ASYNCH:
        TANGO_BINDING->write_attributes_asynch();
        break;
      //-- WRITE_ATTRIBUTES_REPLY
      case MexFile::WRITE_ATTRIBUTES_REPLY:
        TANGO_BINDING->write_attributes_reply();
        break;
      //-- STATUS
      case MexFile::STATUS:
        TANGO_BINDING->status();
        break;
      //-- PING
      case MexFile::PING:
        TANGO_BINDING->ping();
        break;
      //-- INFO
      case MexFile::INFO:
        TANGO_BINDING->info();
        break;
      //-- STATE
      case MexFile::STATE:
        TANGO_BINDING->state();
        break;
      //-- DESCRIPTION
      case MexFile::DESCRIPTION:
        TANGO_BINDING->description();
        break;
      //-- ADMIN_NAME
      case MexFile::ADMIN_NAME:
        TANGO_BINDING->admin_name();
        break;
      //-- GET_ATTR_CONFIG
      case MexFile::GET_ATTR_CONFIG:
        TANGO_BINDING->get_attr_config();
        break;
      //-- SET_ATTR_CONFIG
      case MexFile::SET_ATTR_CONFIG:
        TANGO_BINDING->set_attr_config();
        break;
      //-- BLACK_BOX
      case MexFile::BLACK_BOX:
        TANGO_BINDING->black_box();
        break;
      //-- CMD_LIST_QUERY
      case MexFile::CMD_LIST_QUERY:
        TANGO_BINDING->command_list_query();
        break;
      //-- CMD_QUERY
      case MexFile::CMD_QUERY:
        TANGO_BINDING->command_query();
        break; 
      //-- SET_TIMEOUT
      case MexFile::SET_TIMEOUT:
        TANGO_BINDING->set_timeout();
        break; 
      //-- GET_TIMEOUT
      case MexFile::GET_TIMEOUT:
        TANGO_BINDING->get_timeout();
        break;
      //-- SET_SOURCE
      case MexFile::SET_SOURCE:
        TANGO_BINDING->set_source();
        break; 
      //-- GET_SOURCE
      case MexFile::GET_SOURCE:
        TANGO_BINDING->get_source();
        break; 
      //-- COMMAND_HISTORY
      case MexFile::COMMAND_HISTORY:
        TANGO_BINDING->command_history();
        break; 
      //-- ATTRIBUTE_LIST
      case ATTRIBUTE_LIST:
        TANGO_BINDING->get_attribute_list();
        break; 
      //-- ATTRIBUTE_HISTORY
      case MexFile::ATTRIBUTE_HISTORY:
        TANGO_BINDING->attribute_history();
        break; 
      //-- POLLING_STATUS
      case MexFile::POLLING_STATUS:
        TANGO_BINDING->polling_status();
        break;
      //-- IS_CMD_POLLED
      case MexFile::IS_CMD_POLLED:
        TANGO_BINDING->is_command_polled();
        break;
      //-- IS_ATTR_POLLED
      case MexFile::IS_ATTR_POLLED:
        TANGO_BINDING->is_attribute_polled();
        break; 
      //-- POLL_CMD
      case MexFile::POLL_CMD:
        TANGO_BINDING->poll_command();
        break;
      //-- POLL_ATTR
      case MexFile::POLL_ATTR:
        TANGO_BINDING->poll_attribute();
        break;
      //-- STOP_POLL_CMD
      case MexFile::STOP_POLL_CMD:
        TANGO_BINDING->stop_poll_command();
        break;
      //-- STOP_POLL_ATTR
      case MexFile::STOP_POLL_ATTR:
        TANGO_BINDING->stop_poll_attribute();
        break;
      //-- GET_CMD_POLL_PERIOD
      case MexFile::GET_CMD_POLL_PERIOD:
        TANGO_BINDING->get_command_poll_period();
        break;
      //-- GET_ATTR_POLL_PERIOD
      case MexFile::GET_ATTR_POLL_PERIOD:
        TANGO_BINDING->get_attribute_poll_period();
        break;
      //-- IDL_VERSION
      case IDL_VERSION:
        TANGO_BINDING->get_idl_version();
        break;
      //-- GET_PROPERTIES
      case GET_PROPERTIES:
        TANGO_BINDING->get_properties();
        break;
      //-- PUT_PROPERTIES
      case PUT_PROPERTIES:
        TANGO_BINDING->put_properties();
        break;
      //-- DEL_PROPERTIES
      case DEL_PROPERTIES:
        TANGO_BINDING->del_properties();
        break;
      //-- GET_PROPERTY
      case GET_PROPERTY:
        TANGO_BINDING->get_property();
        break;
      //-- PUT_PROPERTY
      case PUT_PROPERTY:
        TANGO_BINDING->put_property();
        break;
      //-- DEL_PROPERTY
      case DEL_PROPERTY:
        TANGO_BINDING->del_property();
        break;
      //-- GROUP_CREATE
      case GROUP_CREATE:
        TANGO_BINDING->group_create();
        break;
      //-- GROUP_KILL:
      case GROUP_KILL:
        TANGO_BINDING->group_kill();
        break;
      //-- GROUP_ADD:
      case GROUP_ADD:
        TANGO_BINDING->group_add();
        break;
      //-- GROUP_REMOVE:
      case GROUP_REMOVE:
        TANGO_BINDING->group_remove();
        break;
      //-- GROUP_GETID:
      case GROUP_GETID:
        TANGO_BINDING->group_id();
        break;
      //-- GROUP_CMD_INOUT_ASYNCH:
      case GROUP_CMD_INOUT_ASYNCH:
        TANGO_BINDING->group_command_inout_asynch();
        break;
      //-- GROUP_CMD_INOUT_REPLY:
      case GROUP_CMD_INOUT_REPLY:
        TANGO_BINDING->group_command_inout_reply();
        break;
      //-- GROUP_DUMP:
      case GROUP_DUMP:
        TANGO_BINDING->group_dump();
        break;
      //-- GROUP_PING:
      case GROUP_PING:
        TANGO_BINDING->group_ping();
        break;
      //-- GROUP_SIZE:
      case GROUP_SIZE:
        TANGO_BINDING->group_size();
        break;
      //-- GROUP_CONTAINS:
      case GROUP_CONTAINS:
        TANGO_BINDING->group_contains();
        break;
      //-- GROUP_WRITE_ATTRIBUTE_ASYNCH:
      case GROUP_WRITE_ATTRIBUTE_ASYNCH:
        TANGO_BINDING->group_write_attribute_asynch();
        break;
      //-- GROUP_WRITE_ATTRIBUTE_REPLY:
      case GROUP_WRITE_ATTRIBUTE_REPLY:
        TANGO_BINDING->group_write_attribute_reply();
        break;
      //-- GROUP_READ_ATTRIBUTE_ASYNCH:
      case GROUP_READ_ATTRIBUTE_ASYNCH:
        TANGO_BINDING->group_read_attribute_asynch();
        break;
      //-- GROUP_READ_ATTRIBUTE_REPLY:
      case GROUP_READ_ATTRIBUTE_REPLY:
        TANGO_BINDING->group_read_attribute_reply();
        break;
      //-- GROUP_READ_ATTRIBUTES_ASYNCH:
      case GROUP_READ_ATTRIBUTES_ASYNCH:
        TANGO_BINDING->group_read_attributes_asynch();
        break;
      //-- GROUP_READ_ATTRIBUTES_REPLY:
      case GROUP_READ_ATTRIBUTES_REPLY:
        TANGO_BINDING->group_read_attributes_reply();
        break;
      //-- GROUP_ENABLE_DEVICE:
      case GROUP_ENABLE_DEVICE:
        TANGO_BINDING->group_enable_device();
        break;
      //-- GROUP_DISABLE_DEVICE:
      case GROUP_DISABLE_DEVICE:
        TANGO_BINDING->group_disable_device();
        break;
      //-- ENABLE_V1_COMPATIBILITY:
      case ENABLE_V1_COMPATIBILITY:
        TANGO_BINDING->enable_v1_compatibility();
        break;
      //-- DISABLE_V1_COMPATIBILITY:
      case DISABLE_V1_COMPATIBILITY:
        TANGO_BINDING->disable_v1_compatibility();
        break;
      //-- GROUP_SET_TIMEOUT_MSECS:
      case GROUP_SET_TIMEOUT_MSECS:
        TANGO_BINDING->group_set_timeout_msecs();
        break;
      //-- GROUP_DEVICE_LIST:
      case GROUP_DEVICE_LIST:
        TANGO_BINDING->group_device_list();
        break;                
      //- DEFAULT
      default:
        MEX_UTILS->set_error("invalid command tag specified", 
                             "unknown command tag",
                             "mexFunction");
        SET_DEFAULT_PRHS(-1); 
        break;
    }
  }
  catch (const Tango::DevFailed &dv) {
    MEX_UTILS->set_error(dv);
    MEX_UTILS->push_error((const char*)"exception caught",
                          (const char*)"Tango::DevFailed exception caught", 
                          (const char*)"MexFile::exec");
	  SET_DEFAULT_PRHS(-1); 
  }
  catch (...) {
    MEX_UTILS->set_error((const char*)"unknown exception caught",
                         (const char*)"unknown exception caught", 
                         (const char*)"MexFile::exec");
		SET_DEFAULT_PRHS(-1); 
  } 
}


