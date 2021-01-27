## Copyright (C) 2019 Emmanuel Farhi
##
## This file is part of Octave.
##
## Octave is free software; you can redistribute it and/or modify it
## under the terms of the GNU General Public License as published by
## the Free Software Foundation; either version 3 of the License, or (at
## your option) any later version.
##
## Octave is distributed in the hope that it will be useful, but
## WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
## General Public License for more details.
##
## You should have received a copy of the GNU General Public License
## along with Octave; see the file COPYING.  If not, see
## <http://www.gnu.org/licenses/>.

## -*- texinfo -*-
## @deftypefn  {Function File} dynamicprops
## A superclass to dynamically add properties to an object.
##
## This superclass should be used to derive an other class from it, e.g.
##
## @code{classdef} @var{myClass} < @code{dynamicprops}
##
## then use the @code{addprop} method to add a new property, e.g.
##
## @var{obj} = myClass;
## @code{addprop} (@var{obj}, "field")
## @var{obj}.field = 42;
##
## The @code{addprop} method also triggers a @var{PropertyAdded} event.
##
## @multitable @columnfractions 0.15 0.8
## @headitem Method @tab Description
## @item addprop @tab Add a new property to an existing object
## @item subsref @tab Perform the subscripted element selection in object
## @item subsasgn @tab Perform the subscripted assignment operation
## @end multitable
##
## NOTE
## ====
## If you come to redefine 'subsref' and 'subsasgn' methods, you will need to add a line such as:
## 
##   S = subs_added (a,S);
##  
## to allow the substructure S to search for added properties.
##
## @seealso{handle,hgsetget}
## @end deftypefn

## Author: Farhi

classdef (Abstract) dynamicprops < handle

  properties
    dynamicprops_added = []; # Properties dynamically added
  endproperties
  
  events
    PropertyAdded
  endevents
  
  methods
    function prop = addprop (h, prop)  
      ## -*- texinfo -*-
      ## @deftypefn {Function File} {@var{prop} =} addprop (@var{obj},@var{prop})
      ## Add a new property @var{prop} to the object @var{obj}.
      ##
      ## @var{obj} is an object derived from @code{dynamicprops}, and @var{prop} is a property name (string).
      ## @end deftypefn
      if (nargin < 2 || ~ischar(prop) || isempty(prop) || isempty(h))
        error ([ mfilename ": addprop: Parameter must be a string." ]); 
      endif
      prop = prop(:)';
      if (!isvarname (prop))
        error ([ mfilename "': addprop: Parameter must be a valid property name." ]); 
      endif
      for index=1:numel(h)
        this = h (index);
        this.dynamicprops_added.(prop) = [];
        h (index) = this;
        try; notify (this, "PropertyAdded"); end
      endfor
    endfunction # addprop
    
    function v = subsref (a,S)
      ## -*- texinfo -*-
      ## @deftypefn {Function File} @var{VAL} = subsref (@var{OBJ}, @var{IDX})
      ## Perform the subscripted element selection operation according to
      ## the subscript specified by IDX.
      ##
      ## The subscript @var{IDX} is expected to be a structure array with fields
      ## 'type' and 'subs'.  Valid values for 'type' are '"()"', '"{}"', and
      ## '"."'.  The 'subs' field may be either '":"' or a cell array of
      ## index values.
      ##
      ## This method is used when getting the object content with e.g. 
      ##    @var{VAL} = @var{OBJ}.@var{FIELD} 
      ## or @var{VAL} = @var{OBJ}(@var{IDX})
      ## @end deftypefn
      S = subs_added (a,S);
      v = builtin ("subsref", a, S);
    endfunction # subsref
    
    function a = subsasgn (a, S, v)
      ## -*- texinfo -*-
      ## @deftypefn {Function File} subsasgn (@var{OBJ}, @var{IDX}, @var{VAL})
      ## Perform the subscripted assignment operation according to the
      ## subscript specified by IDX.
      ##
      ## The subscript @var{IDX} is expected to be a structure array with fields
      ## 'type' and 'subs'.  Valid values for 'type' are '"()"', '"{}"', and
      ## '"."'.  The 'subs' field may be either '":"' or a cell array of
      ## index values.
      ##
      ## This method is used when setting the object content with e.g. 
      ##    @var{OBJ}.@var{FIELD}  = @var{VAL}
      ## or @var{OBJ}(@var{IDX})    = @var{VAL}
      ## @end deftypefn
      S = subs_added (a, S);
      a = builtin ("subsasgn", a, S, v);
    endfunction # subsasgn
    
  end # methods
  
  methods (Access=protected)
  
    function S = subs_added (a,S)
      ## -*- texinfo -*-
      ## @deftypefn {Function File} @var{IDX} = subs_added (@var{OBJ}, @var{IDX})
      ## Take into account the dynamic properties when getting/setting added content.
      ##
      ## The subscript @var{IDX} is expected to be a structure array with fields
      ## 'type' and 'subs'.  Valid values for 'type' are '"()"', '"{}"', and
      ## '"."'.  The 'subs' field may be either '":"' or a cell array of
      ## index values.
      ##
      ## This method should be inserted at the beginning of any overridden 'subsref'
      ## or 'subsasgn' method.
      ## @end deftypefn
      if (isempty (S)), return; endif
      if (isempty (a.dynamicprops_added)), return; endif
      if (ischar (S)), S = struct ("type",".","subs", S); endif
      ## we search for fields that match any of the dynamicprops_added elements
      ## and add a new level above with it
      
      f     = fieldnames (a.dynamicprops_added);
      index = find (strcmp (S(1).subs, f));
      if (strcmp (S(1).type, ".") && ~isempty (index))
        S0 = struct ("type","." ,"subs", "dynamicprops_added");
        S = [ S0 S ];
      endif
      
    endfunction # subs_added
    
  endmethods # methods protected
endclassdef # classdef dynamicprops

## Tests for dynamicprops
%!  classdef dynamicprops_class < dynamicprops
%!    properties
%!    endproperties
%!  endclassdef

## check normal use: add prop, set and get it.
%!test
%!  obj = dynamicprops_class;
%!  addprop (obj, 'field1');
%!  assert (obj.field1, [])
%!  obj.field1 = 42;
%!  assert (obj.field1, 42)

