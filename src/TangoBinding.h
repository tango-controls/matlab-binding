// ============================================================================
//
// = CONTEXT
//   Tango Generic Client for Matlab
//
// = FILENAME
//   TangoBinding.h
//
// = AUTHOR
//   Nicolas Leclercq - SOLEIL
//
// ============================================================================

#ifndef _TANGO_BINDING_H_
#define _TANGO_BINDING_H_

//=============================================================================
// #DEFINEs
//=============================================================================
#define TANGO_BINDING (TangoBinding::instance())

//=============================================================================
// FORWARD DEC
//=============================================================================
class DeviceDesc;

//=============================================================================
// CLASS : TangoBinding (SINGLETON)
//=============================================================================
class TangoBinding 
{
  friend class MexFile;

public:
  
  static TangoBinding * instance (void);
  //- Returns the unique instance of <TangoBinding>.
    
  //------------------------------------------------------------------
  // NOTE: MEX-FILE IN/OUT ARGUMENTS ARE PASSED USING THE <MEX_ARGS> 
  //                  SINGLETON (SEE MEXFILE.H)
  //------------------------------------------------------------------

  //- Attribute oriented methods
  //---------------------------------------------
    int get_attr_config (void);
    //- DevProxy::get_attribute_config binding.

    int set_attr_config (void);
    //- DevProxy::set_attribute_config binding.

    int read_attribute (void);
    //- DevProxy::read_attribute binding.

    int read_attribute_asynch (void);
    //- DevProxy::read_attribute_asynch binding.

    int read_attribute_reply (void);
    //- DevProxy::read_attribute_reply binding.
    
    int read_attributes (void);
    //- DevProxy::read_attributes binding.

    int read_attributes_asynch (void);
    //- DevProxy::read_attributes_asynch binding.

    int read_attributes_reply (void);
    //- DevProxy::read_attributes_reply binding.
    
    int write_attribute (void);
    //- DevProxy::write_attribute binding.

    int write_attribute_asynch (void);
    //- DevProxy::write_attribute_asynch binding.

    int write_attribute_reply (void);
    //- DevProxy::write_attribute_reply binding.
    
    int write_attributes (void);
    //- DevProxy::write_attributes binding.

    int write_attributes_asynch (void);
    //- DevProxy::write_attributes_asynch binding.

    int write_attributes_reply (void);
    //- DevProxy::write_attributes_reply binding.
    
    int get_attribute_list (void);
    //- DevProxy::get_attribute_list binding.

    int attribute_history (void);
    //- DevProxy::attribute_history binding.

  //- Command oriented methods
  //---------------------------------------------
    int command_inout (void);
    //- DevProxy::command_inout binding.

    int command_inout_asynch (void);
    //- DevProxy::command_inout_asynch binding.

    int command_inout_reply (void);
    //- DevProxy::command_inout_reply binding.
    
    int command_query (void);
    //- DevProxy::command_query binding.

    int command_list_query (void);
    //- DevProxy::command_list_query binding.

    int command_history (void);
    //- DevProxy::command_history binding.

  //- Polling oriented methods
  //---------------------------------------------
    int is_command_polled (void);
    //- DevProxy::is_command_polled binding.

    int poll_command (void);
    //- DevProxy::poll_command binding.

    int stop_poll_command (void);
    //- DevProxy::stop_poll_command binding.

    int get_command_poll_period (void);
    //- DevProxy::get_command_poll_period binding.

    int is_attribute_polled (void);
    //- DevProxy::is_attribute_polled binding.

    int poll_attribute (void);
    //- DevProxy::poll_attribute binding.

    int stop_poll_attribute (void);
    //- DevProxy::stop_poll_attribute binding.

    int get_attribute_poll_period (void);
    //- DevProxy::get_attribute_poll_period binding.

    int polling_status (void);
    //- DevProxy::polling_status binding.

  //- Properties oriented methods
  //---------------------------------------------
    int get_property (void);
    int get_properties (void);
    //- DevProxy::get_property binding.

    int put_property (void);
    int put_properties (void);
    //- DevProxy::put_property binding.

    int del_property (void);
    int del_properties (void);
    //- DevProxy::delete_property binding.

  //- Group oriented methods
  //---------------------------------------------
    int group_create (void);
    //- Create a new group 

    int group_kill (void);
    //- Kill/Delete a group 

    int group_add (void);
    //- Attach an element to a group

    int group_remove (void);
    //- Remove an element from a group

    int group_id (void);
    //- Retrieve a group by name 

    int group_dump (void);
    //- Dump a group content in Matlab command window

    int group_size (void);
    //- Returns the size of a group

    int group_ping (void);
    //- Returns true if all group's devices are alive, false otherwise

    int group_contains (void);
    //- Returns true if the group contains device(s) matching the specifed wild card

    int group_command_inout_asynch (void);
    //- Execute a TANGO command on a group

    int group_command_inout_reply (void);
    //- Return replies associated with a async TANGO command  

    int group_write_attribute_asynch (void);
    //- Write an attribute on a group

    int group_write_attribute_reply (void);
    //- Return replies associated with a async attribute writing

    int group_read_attribute_asynch (void);
    //- Read an attribute on a group

    int group_read_attribute_reply (void);
    //- Return replies associated with a async attribute reading

    int group_read_attributes_asynch (void);
    //- Read several attributes on a group

    int group_read_attributes_reply (void);
    //- Return replies associated with a async attributes reading

    int group_enable_device (void);
    //- Enables a device in a group

    int group_disable_device (void);
    //- Disables a device in a group
    
    int group_set_timeout_msecs (void);
    //- Sets timeout (in msecs)
    
    int group_device_list (void);
    //- Returns the group's device list

  //- Miscellaneaous methods
  //---------------------------------------------
    int info (void);
    //- DevProxy::info binding.

    int get_idl_version (void);
    //- Connection::get_idl_version binding.

    int status (void);
    //- DevProxy::status binding.

    int state (void);
    //- DevProxy::state binding.

    int ping (void);
    //- DevProxy::ping binding.

    int description (void);
    //- DevProxy::description binding.

    int admin_name (void);
    //- DevProxy::adm_name binding.

    int set_timeout (void);
    //- Connection::set_timeout binding.

    int get_timeout (void);
    //- Connection::get_timeout binding.

    int set_source (void);
    //- Connection::set_source binding.

    int get_source (void);
    //- Connection::get_source binding.

    int black_box (void);
    //- DevProxy::black_box binding.

    int tango_version (void);
    //- Returns the version of the TANGO lib used
    //- to compile this binding.

  //- Miscellaneaous specific methods
  //---------------------------------------------
    int open_device (void);
    //- Just for test purpose - should be removed in a near future.

    int close_device (void);
    //- Just for test purpose - should be removed in a near future.

  //- Backward compatibility
  //---------------------------------------------
    void enable_v1_compatibility (void);
    void disable_v1_compatibility (void);
    bool v1_compatibility_enabled (void);

private:
  static int init (void);
  //- Instanciates the singleton.
  
  static void cleanup (void);
  //- Releases the singleton.

  int read_attributes_i (DeviceDesc* ddesc);
  // read_attributes internal implementation

  int write_attributes_i (DeviceDesc* ddesc);
  // write_attributes internal implementation

  template <typename T> 
  int extract_attribute_values (DeviceDesc * ddesc, 
                                std::vector<T>* avs, 
                                mxArray *& argout);
                                            
  int mxattribute (DeviceDesc * ddesc, 
	  Tango::DeviceAttribute& tgattr,
	  mxArray *mx_array,
	  mwSize index,
	  bool exit_on_error);

  Tango::DevLong group_command_inout_asynch_specific_scalar (Tango::Group*  g, 
                                                             const std::string & cmd,
                                                             bool forget,
                                                             bool forward, 
                                                             const mxArray* mx_array);
  // special case of group_command_inout_asynch 

  Tango::DevLong group_command_inout_asynch_specific_vector (Tango::Group*  g, 
                                                             const std::string & cmd,
                                                             bool forget,
                                                             bool forward, 
                                                             const mxArray* mx_array);
  // special case of group_command_inout_asynch 

  Tango::DevLong group_write_attr_asynch_specific_scalar (Tango::Group*  g, 
                                                          const std::string & attr,
                                                					bool forward, 
                                                          const mxArray* mx_array);
  // special case of group_write_attr_asynch 

  static TangoBinding * instance_;
  //- The unique instance of <TangoBinding>.

  // = Disallow these operations (except for friend classes).
  //---------------------------------------------------------
  TangoBinding(void);
  TangoBinding (const TangoBinding&);
  virtual ~TangoBinding(void);
  void operator= (const TangoBinding&); 

  //- v1 backward compatibility
  //----------------------------------------------------------------------------
  //- first version (i.e. v1) of this binding was written using Tango <= 3.x!
  //- at this time, the Tango core lib wasn't able to return both valid
  //- data and exceptions (i.e. error) within the same reply to <read_attributes>.
  //- The following boolean provides some kind of backward compatibility and 
  //- simulates this <old> behaviour. It defaults to true but can be forced to 
  //- false using the <tango_binding_disable_v1_compatibility.m>
  bool v1_compatibility_enabled_;
};

//=============================================================================
// INLINED CODE
//=============================================================================
#if defined (_MEX_INLINE_)
  #include "TangoBinding.i"
#endif 

#endif // _TANGO_BINDING_H_

   
   
