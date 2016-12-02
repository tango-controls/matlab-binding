[![logo](http://www.tango-controls.org/static/tango/img/logo_tangocontrols.png)](http://www.tango-controls.org)

# About the [Matlab](http://www.mathworks.com) and [Octave](https://www.gnu.org/software/octave/) binding for Tango

This is the Matlab (or Octave) client API for [Tango](http://tango-controls.org). 
It runs on Linux and supports both the x86 and x64 version of Matlab/Octave.
The braves can also make it run on Windows - at least with Matlab - see `/windows/msvcxx`.

# Important Note
In any release <= 3.0.0, the Tango DevBoolean type used to be mapped to Matlab unint8. Since release 3.1.0, it’s now mapped to Matlab logical type. It means that any existing code must be adapted to become compatible with the new DevBoolean mapping:

```
argin = unint8(1) -- becomes --> argin = true
argin = unint8(0) -- becomes --> argin = false
```

Another solution is to recompile the binding with `MAP_DEV_BOOLEAN_TO_MATLAB_LOGICAL` undefined in order to retrieve the initial behavior. 

# Documentation

Each TANGO m-function is self-documented (online documentation). 
Use the Matlab <help> function to get information on a particular TANGO m-function (e.g. help tango_read_attribute). 

# How to build on Windows using Microsoft Visual C++ 
- software requirements 
  * Microsoft Visual C++ >= 2013 (a.k.a msvc12) 
  * Matlab x86 and/or x64 >= 2009
  * No octave support on Windows

- clone the github repository in a directory of your choice (here we use c:\github)
```
cd c:\github 
git clone git@github.com:tango-controls/matlab-binding.git
```

- edit the `compil-env-setup.bat` file
```
{edit} c:\github\matlab-binding\windows\msvcxx\compil-env-setup.bat
```

- set BINDING_PATH to the matlab-binding clone location  
```
set BINDING_PATH=c:\github\matlab-binding 
```

- set ML_ROOT64 and/or ML_ROOT32 according to your local Matlab installation
```
set LV_ROOT32=C:\Program Files (x86)\Mathworks\Matlab-20XX
set LV_ROOT64=C:\Program Files\Mathworks\Matlab-20XX
```

- save the `compil-env-setup.bat` file then open the Visual C++ project
```
{double-click-on} c:\github\matlab-binding\windows\msvcxx\run-me-to-compile.bat
```

- select the configuration you want to compile then build the solution
```
{select one of the following combination} [release or debug] / [win32 or x64]
{build}
```

- the compilation will hopefully succeed
```
{safely ignore} warning LNK4088: image being generated due to /FORCE option; image may not run
{check for presence of} c:\github\matlab-binding\mex-file\tango_binding.mexw64
```
- in case you want to use your own Tango runtime, just edit the `compil-env-setup.bat`, modify the paths then rebuild the solution

# How to build on Linux using gcc 
The main constraint we have is to compile the binding with a gcc version producing a shared library which is binary compatible with the Matlab/Octave version we plan to use. It means we have to identify the proper gcc version to use before trying to compile the code. Fortunately, the Matworks website contains [a page providing this critical information](https://fr.mathworks.com/support/sysreq/previous_releases.html). 

Here is the compilation process...
 
- clone the github repository in a directory of your choice (here we use $HOME/github)
```
cd $HOME/github
git clone git@github.com:tango-controls/matlab-binding.git
```

- optionally, recompile the whole Tango software chain with the appropriate gcc version
  * see [cppTango](https://github.com/tango-controls/cppTango) for details
  * using your own Tango runtime requires `$HOME/github/matlab-binding/src/Makefile.[octave].linux` to be adapted 
  * `$HOME/github/matlab-binding/src/Makefile.[octave].linux` is also the place where the gcc version to be used is specified
  
- compile the binding 
```
cd $HOME/github/matlab-binding/src
make -f Makefile.linux or make -f Makefile.octave.linux
```

- the compilation will hopefully succeed
```
{check for presence of} $HOME/github/matlab-binding/mex-file/tango_binding.mexglx
```

# How to build on Linux using cmake

This is the top priority task of the todo list. Stay tuned.

# Links

[Tango Controls web page](http://tango-controls.org)
