// ============================================================================
//
// = CONTEXT
//   Tango Generic Client for Matlab
//
// = FILENAME
//   MexUtils.i
//
// = AUTHOR
//   Nicolas Leclercq - SOLEIL
//
// ============================================================================

//=============================================================================
// MexUtils::instance
//=============================================================================
MEX_INLINE MexUtils* MexUtils::instance (void)
{
  return MexUtils::instance_;
}

//=============================================================================
// MexUtils::notify
//=============================================================================
MEX_INLINE void MexUtils::notify (const std::string& txt)
{
  this->notify(txt.c_str());
}

//=============================================================================
// MexUtils::reset_error                                         
//=============================================================================
MEX_INLINE void MexUtils::reset_error (void)
{
  this->set_global_error_code(0);
  
  this->error_.errors.length(0);
}

//=============================================================================
// MexUtils::set_error                                         
//=============================================================================
MEX_INLINE void MexUtils::set_error (const std::string& _r, 
                                     const std::string& _d, 
                                     const std::string& _o,
                                     Tango::ErrSeverity _s)
{
  this->set_error(_r.c_str(), _d.c_str(), _o.c_str(), _s);
}

//=============================================================================
// MexUtils::push_error                                         
//=============================================================================
MEX_INLINE void MexUtils::push_error (const std::string& _r, 
                                      const std::string& _d, 
                                      const std::string& _o,
                                      Tango::ErrSeverity _s)
{
  this->push_error(_r.c_str(), _d.c_str(), _o.c_str(), _s);
}

//=============================================================================
// MexUtils::is_scalar                                       
//=============================================================================
MEX_INLINE bool MexUtils::is_scalar (const mxArray* mx_array) 
{
  if (!mx_array) return false;
  return (::mxIsNumeric(mx_array)) 
      && (::mxGetM(mx_array) == 1) 
      && (::mxGetN(mx_array) == 1);
}

//=============================================================================
// MexUtils::is_scalar                                       
//=============================================================================
MEX_INLINE bool MexUtils::is_scalar (const mxArray* mx_array, mxClassID& class_id) 
{  
  if (!mx_array) return false;
  class_id = ::mxGetClassID(mx_array);
  return is_scalar(mx_array);
}

//=============================================================================
// MexUtils::is_vector                                      
//=============================================================================
MEX_INLINE bool MexUtils::is_vector (const mxArray* mx_array) 
{
  if (!mx_array) return false;
  return (::mxIsNumeric(mx_array)) 
      && (::mxGetM(mx_array) == 1) 
      && (::mxGetN(mx_array) > 1);
}


//=============================================================================
// MexUtils::is_vector                                      
//=============================================================================
MEX_INLINE bool MexUtils::is_vector (const mxArray* mx_array, mxClassID& class_id) 
{
  if (!mx_array) return false;
  class_id = ::mxGetClassID(mx_array);
  return is_vector(mx_array);
}

//=============================================================================
// MexUtils::is_array_of_string                                     
//=============================================================================
MEX_INLINE bool MexUtils::is_array_of_string (const mxArray* mx_array)
{
  if (!mx_array) return false;
  mxClassID class_id;
  bool is_it = is_array_of_array(mx_array, class_id);
  return is_it && (class_id == ::mxCHAR_CLASS);
}

//=============================================================================
// MexUtils::is_array_of_struct                                     
//=============================================================================
MEX_INLINE bool MexUtils::is_array_of_struct (const mxArray* mx_array)
{
  if (!mx_array) return false;
  mxClassID class_id;
  return is_array_of_struct(mx_array, class_id);
}

//=============================================================================
// MexUtils::is_array_of_struct                                     
//=============================================================================
MEX_INLINE bool MexUtils::is_array_of_struct (const mxArray* mx_array, mxClassID& class_id)
{  
  if (!mx_array) return false;
  bool is_it = is_array_of_array(mx_array, class_id);
  if (!is_it || (class_id != ::mxSTRUCT_CLASS)) {
    return false;
  }
  mxArray* cell = ::mxGetCell(mx_array, 0);
  if (::mxIsStruct(cell) == false) {
    return false; 
  }
  mxArray* tmp = ::mxGetField(cell, 0, "svalue");
  if (!tmp || !::mxIsCell(tmp) || !::mxIsChar(::mxGetCell(tmp, 0))) {
    return false;
  }
  tmp = ::mxGetField(cell, 0, "lvalue");
  if (tmp && ::mxIsInt32(tmp)) {
    class_id = ::mxINT32_CLASS;
    return true;
  }
  tmp = ::mxGetField(cell, 0, "dvalue");
  if (tmp && ::mxIsDouble(tmp)) {
    class_id = ::mxDOUBLE_CLASS;
    return true;
  }
  return false;
}

//=============================================================================
// MexUtils::is_array_of_array                                      
//=============================================================================
MEX_INLINE bool MexUtils::is_array_of_array (const mxArray* mx_array)
{
  if (!mx_array) return false;
  return ::mxIsCell(mx_array);
}

//=============================================================================
// MexUtils::is_array_of_array                                      
//=============================================================================
MEX_INLINE bool MexUtils::is_array_of_array (const mxArray* mx_array, mxClassID& class_id)
{
  if (!mx_array) return false;
  class_id = ::mxGetClassID(mx_array);
  bool is_it = ::mxIsCell(mx_array);
  if (is_it) {
    mxArray* tmp = ::mxGetCell(mx_array, 0);
    if (tmp != 0) {
      class_id = ::mxGetClassID(tmp);
    }
  }
  return is_it;
}

//=============================================================================
// MexUtils::is_array_of_array_of_string                                    
//=============================================================================
MEX_INLINE bool MexUtils::is_array_of_array_of_string (const mxArray* mx_array)
{
  if (!mx_array || ::mxIsCell(mx_array) == false) {
    return false;
  }
  mxArray* tmp = ::mxGetCell(mx_array, 0);
  if (tmp == 0 || ::mxIsCell(tmp) == false) {
    return false;
  }
  tmp = ::mxGetCell(tmp, 0);
  if (tmp == 0 || ::mxIsChar(tmp) == false) {
    return false;
  }
  return true;
}

//=============================================================================
// MexUtils::is_scalar                                       
//=============================================================================
MEX_INLINE bool MexUtils::is_scalar (Tango::CmdArgType arg_type) 
{
  switch (arg_type) 
  {
    case Tango::DEV_BOOLEAN:
    case Tango::DEV_SHORT:
    case Tango::DEV_LONG:
    case Tango::DEV_FLOAT:
    case Tango::DEV_DOUBLE:
    case Tango::DEV_USHORT:
    case Tango::DEV_ULONG:
    case Tango::DEV_STRING:
      return true;
      break;
    default:
      break;
  }
  return false;
}

//=============================================================================
// MexUtils::is_vector                                       
//=============================================================================
MEX_INLINE bool MexUtils::is_vector (Tango::CmdArgType arg_type) 
{
  switch (arg_type) 
  {
   case Tango::DEVVAR_CHARARRAY:
   case Tango::DEVVAR_SHORTARRAY:
   case Tango::DEVVAR_LONGARRAY:
   case Tango::DEVVAR_FLOATARRAY:
   case Tango::DEVVAR_DOUBLEARRAY:
   case Tango::DEVVAR_USHORTARRAY:
   case Tango::DEVVAR_ULONGARRAY:
   case Tango::DEVVAR_STRINGARRAY:
      return true;
      break;
   default:
      break;
  }
  return false;
}

//=============================================================================
// MexUtils::is_struct                                       
//=============================================================================
MEX_INLINE bool MexUtils::is_struct (Tango::CmdArgType arg_type) 
{
  switch (arg_type) 
  {
   case Tango::DEVVAR_LONGSTRINGARRAY:
   case Tango::DEVVAR_DOUBLESTRINGARRAY:
      return true;
      break;
   default:
      break;
  }
  return false;
}

 



