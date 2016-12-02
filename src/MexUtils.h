// ============================================================================
//
// = CONTEXT
//   Tango Generic Client for Matlab
//
// = FILENAME
//   MexUtils::.h
//
// = AUTHOR
//   Nicolas Leclercq - SOLEIL
//
// ============================================================================

#ifndef _MEX_UTILS_H_
#define _MEX_UTILS_H_

//=============================================================================
// Class MexUtils (SINGLETON)
//=============================================================================
class MexUtils
{
  friend class MexFile;

public:

  static MexUtils * instance (void);

  void notify (const char *format, ...); 

  void notify (const std::string& txt); 

  void reset_error (void);

  void set_error (const char *r, 
                  const char *d, 
                  const char *o,
                  Tango::ErrSeverity s = Tango::ERR);

  void set_error (const std::string& r, 
                  const std::string& d, 
                  const std::string& o,
                  Tango::ErrSeverity s = Tango::ERR); 

  void set_error (const Tango::DevFailed &e);

  void set_error (const Tango::DevErrorList& e);

  void push_error (const char *r, 
                   const char *d, 
                   const char *o,
                   Tango::ErrSeverity s = Tango::ERR); 

  void push_error (const std::string& r, 
                   const std::string& d, 
                   const std::string& o,
                   Tango::ErrSeverity s = Tango::ERR); 

  void push_error (const Tango::DevFailed &e);

  void push_error (const Tango::DevErrorList& e);

  int error_code (void);

  int error_stack (void);

  int error_stack_to_mxarray (const Tango::DevErrorList& es, mxArray*& mx_array);

  int set_global_error_code (int _err_code = -1);

  bool is_array_of_string (const mxArray* mx_array);

  bool is_array_of_struct (const mxArray* mx_array);

  bool is_array_of_struct (const mxArray* mx_array, mxClassID& class_id);

  bool is_array_of_array (const mxArray* mx_array);

  bool is_array_of_array (const mxArray* mx_array, mxClassID& class_id);

  bool is_array_of_array_of_string (const mxArray* mx_array);

  bool is_scalar (const mxArray* mx_array);

  bool is_scalar (const mxArray* mx_array, mxClassID& class_id);

  bool is_scalar (Tango::CmdArgType arg_type);

  bool is_vector (const mxArray* mx_array);

  bool is_vector (const mxArray* mx_array, mxClassID& class_id);

  bool is_vector (Tango::CmdArgType arg_type);

  bool is_struct (Tango::CmdArgType arg_type);

private:
  static int init (void);
  //- Instanciates the singleton.
  
  static void cleanup (void);
  //- Releases the singleton.

  static MexUtils* instance_;
  //- The unique instance of <MexUtils::>.

  MexUtils ();
  //- Ctor

  Tango::DevFailed error_;
  //- Last error

  int global_err_code_;
  //- Last error code  (stored in the global workspace).
  //- Set to -1 on error, 0 otherwise.

  // = Disallow these operations (except for friends).
  //---------------------------------------------------------
  MexUtils (const MexUtils&);
  virtual ~MexUtils ();
  void operator= (const MexUtils&); 
};


//=============================================================================
// MACROS 
//=============================================================================
#define MEX_UTILS MexUtils::instance()

#if defined (_MEX_DEBUG_)

# define MEX_ERROR(X) \
    do { \
      MEX_UTILS->notify("*** TANGO BINDING::ERROR\n"); \
      MEX_UTILS->notify("\t|- near line...%d\n",__LINE__ - 2); \
      MEX_UTILS->notify("\t|- in file.....%s\n",__FILE__); \
      MEX_UTILS->notify("\t|- msg........."); \
      MEX_UTILS->notify X; \
      MEX_UTILS->notify("\n"); \
    } while (0)

# define MEX_WARNING(X) \
    do { \
      MEX_UTILS->notify("@@@ TANGO BINDING::WARNING\n"); \
      MEX_UTILS->notify("\t|- near line...%d\n",__LINE__ - 2); \
      MEX_UTILS->notify("\t|- in file.....%s\n",__FILE__); \
      MEX_UTILS->notify("\t|- msg........."); \
      MEX_UTILS->notify X; \
      MEX_UTILS->notify("\n"); \
    } while (0)

# define MEX_MSG(X) \
    do { \
      MEX_UTILS->notify("--- TANGO BINDING MSG\n"); \
      MEX_UTILS->notify("\t|- near line...%d\n",__LINE__ - 2); \
      MEX_UTILS->notify("\t|- in file.....%s\n",__FILE__); \
      MEX_UTILS->notify("\t|- "); \
      MEX_UTILS->notify X; \
      MEX_UTILS->notify("\n"); \
    } while (0) 

# define MEX_DEBUG ::mexPrintf 

#else

# define MEX_ERROR(X)
# define MEX_WARNING(X)
# define MEX_MSG(X)
# define MEX_DEBUG

#endif // _MEX_DEBUG_


//=============================================================================
// INLINED CODE
//=============================================================================
#if defined (_MEX_INLINE_)
  #include "MexUtils.i"
#endif 

#endif // _MEX_UTILS_H_
