## Copyright (C) 2015 SEAGNAL
##
## This program is free software; you can redistribute it and/or modify it under
## the terms of the GNU General Public License as published by the Free Software
## Foundation; either version 3 of the License, or (at your option) any later
## version.
##
## This program is distributed in the hope that it will be useful, but WITHOUT
## ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
## FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
## details.
##
## You should have received a copy of the GNU General Public License along with
## this program; if not, see <http://www.gnu.org/licenses/>.

## -*- texinfo -*-
## @deftypefn  {Function} { @var{node} =} bml_read (@var{fid})
## @deftypefnx {Function} { @var{node} =} bml_read (@var{fid}, @var{nb_elem})
## @deftypefnx {Function} {[@var{node}, @var{cnt}] =} bml_read (@var{fid})
## @deftypefnx {Function} {[@var{node}, @var{cnt}] =} bml_read (@var{fid}, @var{nb_elem})
##
## The @code{bml_read} function reads bml nodes from a bml file or from a socket.
##
## The input arguments are:
##
## @table @var
##
##  @item fid
##   File descriptor
##
##  @item nb_elem
##   Number of elements to read (optionnal, default value is 8192)
## @end table
##
## The output arguments are:
##
## @table @var
##
##  @item node
##   List of nodes read
##
##  @item cnt
##   Number of nodes read (optionnal)
## @end table
##
## For example,
## @smallexample
##  fid = bml_open ("my_file.bml", 'r');
##  [res, cnt] = bml_read(fid);
##  bml_close(fid);
## @end smallexample
##
## @seealso{bml_open, bml_write, bml_close}
## @end deftypefn

function varargout = bml_read (varargin)
  error ("bar_calc: Not available (dummy function)");
end 

