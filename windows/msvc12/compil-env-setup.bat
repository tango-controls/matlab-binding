@ECHO off

::------------------------------------------------------------------------
:: ======== TANGO BINDING PATHS ======== 
::------------------------------------------------------------------------
::- git clone directory
set BINDING_PATH=C:\mystuffs\dev\bindings\matlab-binding@github
::- runtime directory
set RUNTIME_PATH_X64=%BINDING_PATH%\runtime\windows
::- .h
set RUNTIME_INC_X64=%RUNTIME_PATH_X64%\include
::- .lib
set RUNTIME_LIB_X64=%RUNTIME_PATH_X64%\lib\x64\tango-9.2.2-vc12
::- .dll
set RUNTIME_BIN_X64=%RUNTIME_PATH_X64%\lib\x64\tango-9.2.2-vc12
::- path
set PATH=%RUNTIME_BIN_X64%;%PATH%
set PATH=%BINDING_PATH%\mex-file;%PATH%

::------------------------------------------------------------------------
:: ======== MATLAB OR OCTAVE X64 PATHS 
::------------------------------------------------------------------------
set ML_ROOT64=C:\Program Files\MATLAB\R2016b
set ML_INC64=%ML_ROOT64%\extern\include
set ML_LIB64=%ML_ROOT64%\extern\lib\win64\microsoft
set ML_RC64=%ML_ROOT64%\extern\include
SET ML_LIB64_LIST=libmex.lib;libmx.lib

::------------------------------------------------------------------------
:: ======== YAT64 ======== 
::------------------------------------------------------------------------
set YAT_ROOT64=%RUNTIME_PATH_X64%
set YAT_INC64=%RUNTIME_INC_X64%
set YAT_LIB64=%RUNTIME_LIB_X64%
set YAT_BIN64=%RUNTIME_BIN_X64%
set YAT_LIB64_LIST=yat.lib
set YAT_DBG_LIB64_LIST=yatd.lib
SET PATH=%YAT_BIN64%;%PATH%

::------------------------------------------------------------------------
:: ======== YAT4TANGO64 ======== 
::------------------------------------------------------------------------
set YAT4TANGO_ROOT64=%RUNTIME_PATH_X64%
set YAT4TANGO_INC64=%RUNTIME_INC_X64%
set YAT4TANGO_LIB64=%RUNTIME_LIB_X64%
set YAT4TANGO_BIN64=%RUNTIME_BIN_X64%
set YAT4TANGO_LIB64_LIST=yat4tango.lib
set YAT4TANGO_DBG_LIB64_LIST=yat4tangod.lib
SET PATH=%YAT4TANGO_BIN64%;%PATH%

::------------------------------------------------------------------------
:: ======== LOG4TANGO ======== 
::------------------------------------------------------------------------
SET LOG4TANGO_ROOT64=%RUNTIME_PATH_X64%
SET LOG4TANGO_INC64=%RUNTIME_INC_X64%
SET LOG4TANGO_LIB64=%RUNTIME_LIB_X64%
SET LOG4TANGO_BIN64=%RUNTIME_BIN_X64%
SET LOG4TANGO_LIB64_LIST=log4tango.lib
SET PATH=%LOG4TANGO_BIN64%;%PATH%

::------------------------------------------------------------------------
:: ======== ZMQ ======== 
::------------------------------------------------------------------------
SET ZMQ_ROOT64=%RUNTIME_PATH_X64%
SET ZMQ_INC64=%RUNTIME_INC_X64%
SET ZMQ_LIB64=%RUNTIME_LIB_X64%
SET ZMQ_BIN64=%RUNTIME_BIN_X64%
SET ZMQ_LIB64_LIST=zmq.lib
SET PATH=%ZMQ_BIN64%;%PATH%

::------------------------------------------------------------------------
:: ======== OMNIORB ======== 
::------------------------------------------------------------------------
SET OMNIORB_ROOT64=%RUNTIME_PATH_X64%
SET OMNIORB_INC64=%RUNTIME_INC_X64%
SET OMNIORB_LIB64=%RUNTIME_LIB_X64%
SET OMNIORB_BIN64=%RUNTIME_BIN_X64%
SET OMNIORB_LIB64_LIST=omnithread40_rt.lib;omniORB421_rt.lib;COS421_rt.lib;omniDynamic421_rt.lib
SET PATH=%OMNIORB_BIN64%;%PATH%

::------------------------------------------------------------------------
:: ======== TANGO ======== 
::------------------------------------------------------------------------
SET TANGO_ROOT64=%RUNTIME_PATH_X64%
SET TANGO_INC64=%RUNTIME_INC_X64%\tango
SET TANGO_LIB64=%RUNTIME_LIB_X64%
SET TANGO_BIN64=%RUNTIME_BIN_X64%
SET TANGO_LIB64_LIST=tango.lib
SET PATH=%TANGO_BIN64%;%PATH%

