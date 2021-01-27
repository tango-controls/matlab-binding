// ============================================================================
//
// = CONTEXT
//   Tango Generic Client for Matlab
//
// = FILENAME
//   DevRepository.h
//
// = AUTHOR
//   Nicolas Leclercq - SOLEIL
//
// ============================================================================

#ifndef _DEVICE_REPOSITORY_H_
#define _DEVICE_REPOSITORY_H_

//=============================================================================
// DEPENDENCIEs
//=============================================================================
#include <map>

//=============================================================================
// FORWARD DECLARATION
//=============================================================================
//-class Tango::DeviceProxy;
class DeviceDesc;

//=============================================================================
// TYPEDEFs
//=============================================================================
typedef std::map<std::string, DeviceDesc*> DeviceDescMap;
// DeviceMap : a STL map of <DeviceProxy*> indexed by device name.
typedef DeviceDescMap::iterator DeviceDescMapIterator;
// DeviceMapIterator : a DeviceMap iterator;

//=============================================================================
// DEFINEs
//=============================================================================
#define DEV_REP (DevRepository::instance())

//=============================================================================
// CLASS : DeviceDesc
//=============================================================================
class DeviceDesc 
{
  friend class DevRepository;

public:

  typedef struct {
	  string name;
	  Tango::DevLong in_type;
	  Tango::DevLong out_type;
  } MinCmdInfo;
  // Used to store locally the mininum required cmd info.

  typedef std::vector<MinCmdInfo> MinCmdInfoList;
  // Define what is a list of <MinCmdInfo>;

  typedef struct {
	  string name;
	  Tango::AttrWriteType writable;
	  Tango::AttrDataFormat data_format;
	  int data_type;
	  std::vector<std::string> enum_labels;
  } MinAttrInfo;
  // Used to store locally the mininum required attr info.

  typedef std::vector<MinAttrInfo> MinAttrInfoList;
  // Define what is a list of <MinAttrInfo>;

  const std::string& name (void) const;
  // Returns the device name.

  Tango::DeviceProxy* proxy (void) const;
  // Returns the device proxy.

  const MinCmdInfoList& cmd_list (void) const;
  // Returns the device commands list.

  const MinAttrInfoList& attr_list (void) const;
  // Returns the device attributes list.

  int cmd_exists (const std::string& cmd_name);
  // If <cmd_name> is supported by the device, returns its 
  // indice in <cmd_list>, -1 otherwise.

  int cmd_exists (const char* cmd_name);
  // If <cmd_name> is supported by the device, returns its 
  // indice in <cmd_list>, -1 otherwise.

  int attr_exists (const std::string& attr_name);
  // If <attr_name> is an attribute of the device, returns its 
  // indice in <attr_list>, -1 otherwise.

  int attr_exists (const char* attr_name);
  // If <attr_name> is an attribute of the device, returns its 
  // indice in <attr_list>, -1 otherwise.

  bool is_attr_writable (const std::string& _attr_name);
  // Returns <true> if the attribute named <_attr_name> is 
  // writable, <false> otherwise.

  bool is_attr_writable (int attr_id);
  // Returns <true> if the attribute named <_attr_name> is 
  // writable, <false> otherwise.

  bool is_attr_readable (const std::string& _attr_name);
  // Returns <true> if the attribute named <_attr_name> is 
  // readable, <false> otherwise.

  bool is_attr_readable (int attr_id);
  // Returns <true> if the attribute named <_attr_name> is 
  // readable, <false> otherwise.

private:
  DeviceDesc (std::string _dev_name);
  // Ctor.

  DeviceDesc(Tango::DeviceProxy* _dp, bool _ownership = true);
  // Ctor.

  void cmd_list (Tango::CommandInfoList* cmd_list);
  // Set the device commands list.

  void attr_list (Tango::AttributeInfoListEx* attr_list);
  // Set the device attributes list.

  std::string dev_name_;
  // The device name.

  Tango::DeviceProxy* dev_proxy_;
  // The device proxy

  MinCmdInfoList dev_cmd_list_;
  // The device command list.

  MinAttrInfoList dev_attr_list_;
  // The device attribute list.

  bool ownership_;
  //- Do this instance has ownership of the underlying <Tango::DeviceProxy>?

  // = Disallow these operations (except for DevRepository).
  //---------------------------------------------------------
  DeviceDesc (const DeviceDesc&);
  virtual ~DeviceDesc();
  void operator= (const DeviceDesc&); 
};

//=============================================================================
// CLASS : DevRepository (SINGLETON)
//=============================================================================
class DevRepository 
{
  friend class TangoBinding;

public:
  static DevRepository* instance (void);
  // Returns the unique instance of <DevRepository>.
    
  DeviceDesc* device_desc (const char* device_name);
  // Providing a device name, returns its device proxy or NULL on error.

  DeviceDesc* device_desc (const std::string& device_name);
  // Providing a device name, returns its device proxy or NULL on error.

  DeviceDesc* device_desc (Tango::DeviceProxy* device_proxy, bool _ownership = true, bool _set_error = true);
  // Providing a device proxy, returns its device descriptor or NULL on error.

private:
  int remove_device (const std::string& device_name);
  //- Providing a device name, returns removes it from the repository.

  static int init (void);
  //- Instanciates the singleton.
  
  static void cleanup (void);
  //- Releases the singleton.

  static DevRepository* instance_;
  //- The unique instance of <DevRepository>.

  DeviceDescMap dev_map_;
  //- The actual repository (see typedef above).

  // = Disallow these operations (except for TangoClientXop).
  //---------------------------------------------------------
  DevRepository();
  DevRepository (const DevRepository&);
  virtual ~DevRepository();
  void operator= (const DevRepository&); 
};

//=============================================================================
// INLINED CODE
//=============================================================================
#if defined (_MEX_INLINE_)
  #include "DevRepository.i"
#endif 

#endif // _DEVICE_REPOSITORY_H_

   
   
