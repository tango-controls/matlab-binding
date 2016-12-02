// ============================================================================
//
// = CONTEXT
//   Tango Generic Client for Matlab
//
// = FILENAME
//   GroupRepository.h
//
// = AUTHOR
//   Nicolas Leclercq - SOLEIL
//
// ============================================================================

#ifndef _GROUP_REPOSITORY_H_
#define _GROUP_REPOSITORY_H_

//=============================================================================
// DEPENDENCIEs
//=============================================================================
#include <set>
#include <map>
#include "Timer.h"

//=============================================================================
// FORWARD DECLARATION
//=============================================================================
//- class Tango::Group;

//=============================================================================
// TYPEDEFs
//=============================================================================
typedef std::map<Tango::DevLong, Tango::Group*> GroupMap;
typedef GroupMap::iterator GroupMapIterator;

typedef std::map<Tango::DevLong, Timeout> TimeoutMap;
typedef TimeoutMap::iterator TimeoutMapIterator;


//=============================================================================
// DEFINEs
//=============================================================================
#define GRP_REP (GroupRepository::instance())

//=============================================================================
// CLASS : GroupRepository (SINGLETON)
//=============================================================================
class GroupRepository : public GroupMap
{
  friend class TangoBinding;

public:
  static GroupRepository* instance (void);
  // Returns the unique instance of <GroupRepository>.

  Tango::DevLong add (const std::string& name);
  // Given a group name, create the group then insert it into the repository.

  int remove (Tango::DevLong group_id, bool release = true);
  // Given a group name, remove the group from the repository.

  int remove (Tango::Group* group, bool release = true);
  // Given a group, remove it from the repository.
  
  Tango::Group * get (Tango::DevLong group_id, bool set_error = true);
  // Given a group id, returns its address or NULL if no such group.

  Tango::Group * get (const std::string& name, bool set_error = true);
  // Given a group name, returns its address or NULL if no such group.

  Tango::DevLong gid (const std::string& name, bool set_error = true);
  // Given a group name, returns its id or -1 if no such group.
  
  bool com_timeout_expired (Tango::DevLong group_id);
  // Given a group id, returns true if no com with the specified group for more than the "com timeout", returns false otherwise

private:
  static Tango::DevLong group_cnt;
  //- group identifier 
  
  static int init (void);
  //- Instanciates the singleton.

  static void cleanup (void);
  //- Releases the singleton.

  static GroupRepository* instance_;
  //- The unique instance of <GroupRepository>.

  void dump (Tango::DevLong group_id = -1);
  //- Dump repository content

  TimeoutMap timer_map_;
  //- The timers map (one per group)

  // = Disallow these operations (except for TangoClientXop).
  //---------------------------------------------------------
  GroupRepository();
  GroupRepository (const GroupRepository&);
  virtual ~GroupRepository();
  void operator= (const GroupRepository&); 
};

//=============================================================================
// INLINED CODE
//=============================================================================
#if defined (_MEX_INLINE_)
  #include "GroupRepository.i"
#endif 

#endif // _GROUP_REPOSITORY_H_

   
   
