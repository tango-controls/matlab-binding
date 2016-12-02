// ============================================================================
//
// = CONTEXT
//   Tango Generic Client for Matlab
//
// = FILENAME
//   GroupRepository.cpp
//
// = AUTHOR
//   Nicolas Leclercq - SOLEIL
//
// ============================================================================

//=============================================================================
// DEPENDENCIES
//=============================================================================
#include "StandardHeader.h"
#include "GroupRepository.h"

#if !defined (_MEX_INLINE_)
  #include "GroupRepository.i"
#endif 

//=============================================================================
// STATIC MEMBERS
//=============================================================================
GroupRepository* GroupRepository::instance_ = 0;
//-----------------------------------------------------------------------------
Tango::DevLong GroupRepository::group_cnt = 0;
//-----------------------------------------------------------------------------

//=============================================================================
// GroupRepository::init
//=============================================================================
int GroupRepository::init (void)
{
  if (GroupRepository::instance_ != 0)
    return 0;
 
  GroupRepository::instance_ = new GroupRepository;

  return (GroupRepository::instance_) ? 0 : -1;
}

//=============================================================================
// GroupRepository::cleanup
//=============================================================================
void GroupRepository::cleanup (void)
{
  if (GroupRepository::instance_) 
  {
    delete GroupRepository::instance_;
    GroupRepository::instance_ = 0;
  }
}

//=============================================================================
// GroupRepository::GroupRepository 
//=============================================================================
GroupRepository::GroupRepository (void)
{
  //- noop
}

//=============================================================================
// GroupRepository::~GroupRepository 
//=============================================================================
GroupRepository::~GroupRepository (void)
{
 GroupMapIterator _cur = begin();
 GroupMapIterator _end = end();
 
 std::vector<Tango::Group*> gl(0);
 
 for (; _cur != _end; _cur++) 
 {
   //- delete only <root> groups 
   if (_cur->second && _cur->second->is_root_group())
      gl.push_back(_cur->second);
 }
 
 clear();
 
 //- actual delete
 for (size_t i = 0; i < gl.size(); i++) 
   if (gl[i]) delete gl[i];
}

//=============================================================================
// GroupRepository::add
//=============================================================================
Tango::DevLong GroupRepository::add (const std::string& name)
{ 
  Tango::Group* g = new Tango::Group(name);
  if (g == 0) {
    MEX_UTILS->push_error("internal error",
			                    "out of memory error (could not instanciate Tango::Group)",
			                    "GroupRepository::create_group");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  
  Tango::DevLong gid = GroupRepository::group_cnt++;
  
  pair<GroupMapIterator, bool> gresult;
  gresult = insert(GroupMap::value_type(gid, g));
  if (gresult.second == 0) 
  {
    delete g;
    MEX_UTILS->set_error("group already exists",
			                   "GroupRepository::insert failed",
			                   "GroupRepository::create_group");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  
  pair<TimeoutMapIterator, bool> tresult;
  tresult = this->timer_map_.insert(TimeoutMap::value_type(gid, Timeout(30, Timeout::TMO_UNIT_SEC, true)));
  if (tresult.second == 0) 
  {
    delete g;
    MEX_UTILS->set_error("timer instanciation/registration failed",
			                   "failed to create/register Timer for new devices group",
			                   "GroupRepository::create_group");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }

  return gid;
}

//=============================================================================
// GroupRepository::remove
//=============================================================================
int GroupRepository::remove (Tango::Group* g, bool release)
{ 
  if (!g) {
    MEX_UTILS->set_error("invalid group pointer",
			                   "no such group",
			                   "GroupRepository::remove");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }

  GroupMapIterator it = begin();
  GroupMapIterator _end = end();
  
  for (; it != _end; it++) {
    if (g->get_fully_qualified_name() == it->second->get_fully_qualified_name())
      this->remove(it->first, release);
  }

  return kNoError; 
}

//=============================================================================
// GroupRepository::remove
//=============================================================================
int GroupRepository::remove (Tango::DevLong group_id, bool release)
{ 
  //- Is <g> in the repository?
  GroupMapIterator it = find(group_id);
  if (it == end()) {
    MEX_UTILS->set_error("invalid group identifier",
			                   "no such group",
			                   "GroupRepository::remove");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  
  Tango::Group* g = it->second;  
  if (! g) {
    MEX_UTILS->set_error("internal error",
			                   "unable to associate <group identifier> to its <actual group>",
			                   "GroupRepository::remove");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }
  
  //- Remove all groups belonging to <g> from the repository (no delete)
  std::vector<Tango::DevLong> groups_to_remove(0);
  it = begin();
  GroupMapIterator _end = end();
  for (; it != _end; it++) {
    if (it->second != g && g->contains(it->second->get_fully_qualified_name()))
      groups_to_remove.push_back(it->first);
  }
  for (size_t i = 0; i < groups_to_remove.size(); i++) 
  {
    it = find(groups_to_remove[i]);
    if (it != end())
      erase(it);
  }

  //- Remove <g> from the repository
  it = find(group_id);
  if (it != end()) 
  {
    if (release && it->second)
      delete it->second;
    erase(it);
  }
  else 
  {
    MEX_UTILS->set_error("internal error - invalid group identifier",
			                   "could not retrieve group from repository",
			                   "GroupRepository::remove");
    SET_DEFAULT_PRHS_THEN_RETURN(kError); 
  }

  //- Remove <g> from the timer repository
  TimeoutMapIterator tm_it = this->timer_map_.find(group_id);
  if (tm_it != this->timer_map_.end()) 
    this->timer_map_.erase(tm_it);

  return kNoError; 
}

//=============================================================================
// GroupRepository::com_timeout_expired
//=============================================================================
bool GroupRepository::com_timeout_expired (Tango::DevLong group_id)
{  
  bool expired = false;
  TimeoutMapIterator it = this->timer_map_.find(group_id);
  if (it != this->timer_map_.end()) 
  {
    expired = it->second.expired();
    it->second.restart();
  }
  return expired; 
}

//=============================================================================
// GroupRepository::dump
//=============================================================================
void GroupRepository::dump (Tango::DevLong group_id)
{
  if (group_id == -1) 
  {
    MEX_MSG(("Dumping Group Repository Content"));
    if (empty()) 
    {
      MEX_DEBUG(("TANGO group repository is empty\n")); 
      return;
    }
    for (GroupMapIterator it = begin(); it != end(); it++) 
    {
      MEX_DEBUG("Group: %s\n", it->second->get_fully_qualified_name().c_str()); 
      {
        std::vector<std::string> dl = it->second->get_device_list(false);
        for (size_t i = 0; i < dl.size(); i++)
          MEX_DEBUG("\t%s\n", dl[i].c_str());
      }
    }
  } 
  else 
  {
    Tango::Group*  g = get(group_id, false);
    if (g) 
    {
      MEX_DEBUG("* TANGO Group: %s\n", g->get_fully_qualified_name().c_str()); 
      std::vector<std::string> dl = g->get_device_list(false);
      for (size_t i = 0; i < dl.size(); i++)
        MEX_DEBUG("\t- %s\n", dl[i].c_str()); 
    }
  }
}




