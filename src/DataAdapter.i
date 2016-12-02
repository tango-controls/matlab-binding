// ============================================================================
//
// = CONTEXT
//   Tango Generic Client for Matlab
//
// = FILENAME
//   DataAdapter.i
//
// = AUTHOR
//   Nicolas Leclercq - SOLEIL
//
// ============================================================================

//=============================================================================
// DataAdapter::instance
//=============================================================================
MEX_INLINE DataAdapter* DataAdapter::instance (void)
{
  return DataAdapter::instance_;
}
