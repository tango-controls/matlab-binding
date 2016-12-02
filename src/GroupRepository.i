// ============================================================================
//
// = CONTEXT
//   Tango Generic Client for Matlab
//
// = FILENAME
//   GroupRepository.i
//
// = AUTHOR
//   Nicolas Leclercq - SOLEIL
//
// ============================================================================

//=============================================================================
// GroupRepository::instance
//=============================================================================
MEX_INLINE GroupRepository* GroupRepository::instance (void)
{
  return GroupRepository::instance_;
}

//=============================================================================
// GroupRepository::get
//=============================================================================
MEX_INLINE Tango::DevLong GroupRepository::gid (const std::string& name, bool set_error)
{
  GroupMapIterator it = begin();
  GroupMapIterator _end = end();
  
  Tango::Group * g = 0;
  
  for (; it != _end && g == 0; it++)
    g = it->second->get_group(name);
  
  if (! g) 
  {
    if (set_error) 
      MEX_UTILS->set_error("invalid group name",
			                     "no group by that name",
			                     "GroupRepository::get");
    return -1;
  }

  return it->first;
}

//=============================================================================
// GroupRepository::get
//=============================================================================
MEX_INLINE Tango::Group* GroupRepository::get (const std::string& name, bool set_error)
{
  GroupMapIterator it = begin();
  GroupMapIterator _end = end();
  
  Tango::Group * g = 0;
  
  for (; it != _end && g == 0; it++) 
    g = it->second->get_group(name);

  if (! g) 
  {
    if (set_error) 
      MEX_UTILS->set_error("invalid group name",
			                     "no group by that name",
			                     "GroupRepository::get");
    return 0;
  }
  
  return g;
}

//=============================================================================
// GroupRepository::get
//=============================================================================
MEX_INLINE Tango::Group* GroupRepository::get (Tango::DevLong group_id, bool set_error)
{
  GroupMapIterator it = find(group_id);

  if (it == end() || it->second == 0) 
  {
    if (set_error) 
      MEX_UTILS->set_error("invalid group id",
			                     "no such group",
			                     "GroupRepository::get");
    return 0;
  }
  
  return it->second;
}

