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
## @deftypefn {Function} bml_write (@var{fid}, @var{node})
##
## The @code{bml_write} function writes a bml node in a bml file (opened with an appropriate mode) or in a socket
##
## The input arguments are:
##
## @table @var
##
##  @item fid
##   File descriptor
##
##  @item node
##   Node to write
##
## @end table
##
## For example,
## @smallexample
##  node = struct('id', 0x1234, 'data', uint64([1:8]));
##  fid = bml_open ("my_file.bml", 'w');
##  [res, cnt] = bml_write(fid, node);
##  bml_close(fid);
## @end smallexample
##
## @seealso{bml_open, bml_read, bml_close}
## @end deftypefn

function varargout = bml_write (varargin)
  error ("bar_calc: Not available (dummy function)");
end 

