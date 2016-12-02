// ============================================================================
//
// = CONTEXT
//   Tango Generic Client for Matlab
//
// = FILENAME
//   MexFile.h
//
// = AUTHOR
//   Nicolas Leclercq - SOLEIL
//
// ============================================================================

#ifndef _MEX_FILE_H_
#define _MEX_FILE_H_

//=============================================================================
// CLASS MexFile
//=============================================================================
class MexFile
{
 public:

  static const char * kMEX_VERSION;
  
  enum {
    MEX_VERSION = -2,
    TANGO_VERSION = -1,
    ERROR_CODE = 0,
    ERROR_STACK,
    OPEN_DEVICE,
    CLOSE_DEVICE,
    COMMAND_INOUT,
    READ_ATTRIBUTE, // 5 
    READ_ATTRIBUTES,
    WRITE_ATTRIBUTE,
    WRITE_ATTRIBUTES,
    STATUS,
    PING, // 10
    INFO,
    STATE,
    DESCRIPTION,
    ADMIN_NAME,
    GET_ATTR_CONFIG, // 15
    SET_ATTR_CONFIG,
    BLACK_BOX,
    CMD_LIST_QUERY,
    CMD_QUERY,
    SET_TIMEOUT, // 20
    GET_TIMEOUT,
    SET_SOURCE,
    GET_SOURCE,
    COMMAND_HISTORY,
    ATTRIBUTE_LIST, // 25
    ATTRIBUTE_HISTORY,
    POLLING_STATUS,
    IS_CMD_POLLED,
    IS_ATTR_POLLED,
    POLL_CMD, // 30
    POLL_ATTR,
    STOP_POLL_CMD,
    STOP_POLL_ATTR,
    GET_CMD_POLL_PERIOD,
    GET_ATTR_POLL_PERIOD, // 35
    IDL_VERSION,
    GET_PROPERTIES,
    PUT_PROPERTIES,
    DEL_PROPERTIES,
    GET_PROPERTY, // 40
    PUT_PROPERTY,
    DEL_PROPERTY,
    COMMAND_INOUT_ASYNCH, 
    COMMAND_INOUT_REPLY, 
    READ_ATTRIBUTE_ASYNCH, // 45
    READ_ATTRIBUTE_REPLY,
    READ_ATTRIBUTES_ASYNCH,
    READ_ATTRIBUTES_REPLY,
    WRITE_ATTRIBUTE_ASYNCH,
    WRITE_ATTRIBUTE_REPLY, // 50
    WRITE_ATTRIBUTES_ASYNCH,
    WRITE_ATTRIBUTES_REPLY,
    GROUP_CREATE,
    GROUP_KILL,
    GROUP_ADD, // 55
    GROUP_REMOVE,
    GROUP_GETID,
    GROUP_CMD_INOUT_ASYNCH,
    GROUP_CMD_INOUT_REPLY,
    GROUP_DUMP, // 60
    GROUP_WRITE_ATTRIBUTE_ASYNCH,
    GROUP_WRITE_ATTRIBUTE_REPLY,
    GROUP_READ_ATTRIBUTE_ASYNCH,
    GROUP_READ_ATTRIBUTE_REPLY, 
    SUBSCRIBE_EVT, // 65
    NEXT_EVT, 
    UNSUBSCRIBE_EVT, 
    GROUP_SIZE,
    GROUP_PING,
    GROUP_CONTAINS, // 70
    GROUP_READ_ATTRIBUTES_ASYNCH,
    GROUP_READ_ATTRIBUTES_REPLY,
    GROUP_ENABLE_DEVICE,
    GROUP_DISABLE_DEVICE,
    ENABLE_V1_COMPATIBILITY, // 75 
    DISABLE_V1_COMPATIBILITY,
    GROUP_SET_TIMEOUT_MSECS,
    GROUP_DEVICE_LIST, // 78
  };
  //- Mexfile cmd IDs

  static int init (void);
  //- Initialization.

  static void cleanup (void);
  //- Terminaison.

  static MexArgs args;
  //- MEX-File arguments

  static void exec (int nlhs, mxArray ** plhs, int nrhs, const mxArray ** prhs);
  //- Executes a MEX-File command providing the command ID and the i/o arguments.

  static void version (void);
  //- Returns this MEX-file version

private:
  static int initialized;
  //- Initialization flag  
};

//=============================================================================
// #DEFINEs
//=============================================================================
#define MEX_ARGS (&MexFile::args)

#endif // _MEX_FILE_H_


