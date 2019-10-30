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
## @deftypefn {Function} bml_close (@var{fid})
##
## The @code{bml_close} function closes a bml file or a socket
##
## The input argument @var{fid} is the file descriptor
##
## @smallexample
##  fid = bml_open ("my_file.bml", 'r');
##  [res, cnt] = bml_read(fid);
##  bml_close(fid);
## @end smallexample
##
## @seealso{bml_open, bml_read, bml_write}
## @end deftypefn

function varargout = bml_close (varargin)
  error ("bar_calc: Not available (dummy function)");
end 

