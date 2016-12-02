// ============================================================================
//
// = CONTEXT
//   Tango Generic Client for Matlab
//
// = FILENAME
//   TangoBinding.i
//
// = AUTHOR
//   Nicolas Leclercq - SOLEIL
//
// ============================================================================

//=============================================================================
// TangoBinding::instance
//=============================================================================
MEX_INLINE TangoBinding * TangoBinding::instance (void)
{
  return TangoBinding::instance_;
}

//=============================================================================
// TangoBinding::enable_v1_compatibility
//=============================================================================
MEX_INLINE void TangoBinding::enable_v1_compatibility (void)
{
  this->v1_compatibility_enabled_ = true;
}

//=============================================================================
// TangoBinding::disable_v1_compatibility
//=============================================================================
MEX_INLINE void TangoBinding::disable_v1_compatibility (void)
{
  this->v1_compatibility_enabled_ = false;
}

//=============================================================================
// TangoBinding::v1_compatibility_enabled
//=============================================================================
MEX_INLINE bool TangoBinding::v1_compatibility_enabled (void)
{
  return this->v1_compatibility_enabled_;
}
