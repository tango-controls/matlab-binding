// ============================================================================
//
// = CONTEXT
//   Tango Generic Client for Matlab
//
// = FILENAME
//   DevRepository.i
//
// = AUTHOR
//   Nicolas Leclercq - SOLEIL
//
// ============================================================================

//=============================================================================
// DeviceDesc::name
//=============================================================================
MEX_INLINE const std::string& DeviceDesc::name (void) const
{
  return this->dev_name_;
}

//=============================================================================
// DeviceDesc::proxy
//=============================================================================
MEX_INLINE Tango::DeviceProxy* DeviceDesc::proxy (void) const
{
  return this->dev_proxy_;
}

//=============================================================================
// DeviceDesc::cmd_list
//=============================================================================
MEX_INLINE const DeviceDesc::MinCmdInfoList& DeviceDesc::cmd_list (void) const
{
  return this->dev_cmd_list_;
}

//=============================================================================
// DeviceDesc::attr_list
//=============================================================================
MEX_INLINE const DeviceDesc::MinAttrInfoList& DeviceDesc::attr_list (void) const
{
  return this->dev_attr_list_;
}

//=============================================================================
// DeviceDesc::cmd_exists 
//=============================================================================
MEX_INLINE int DeviceDesc::cmd_exists (const char* _cmd_name)
{
  return this->cmd_exists(std::string(_cmd_name));
}

//=============================================================================
// DeviceDesc::attr_exists 
//=============================================================================
MEX_INLINE int DeviceDesc::attr_exists (const char* _attr_name)
{
  return this->attr_exists(std::string(_attr_name));
}

//=============================================================================
// DevRepository::instance
//=============================================================================
MEX_INLINE DevRepository* DevRepository::instance (void)
{
  return DevRepository::instance_;
}

//=============================================================================
// DevRepository::device_desc
//=============================================================================
MEX_INLINE DeviceDesc* DevRepository::device_desc (const char* device_name)
{
  return this->device_desc(std::string(device_name));
}

