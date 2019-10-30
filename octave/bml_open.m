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
## @deftypefn {Function} {@var{fid} =} bml_open (@var{name}, @var{mode})
## @deftypefnx {Function} {@var{fid} =} bml_open (@var{name}, @var{mode}, @var{align})
##
## The @code{bml_open} function opens a bml file or a socket with a specified mode
## (read-write, read-only, etc.). A third argument can be provided to
## force the allignement, otherwise 0 is used.
##
## The possible values @var{mode} may be:
##
## @table @verb
##
##  @item @qcode{'r'}
##   Open a file for reading.
##
##  @item @qcode{'w'}
##   Open a file for writing. The previous contents are discarded.
##
##  @item @qcode{'a'}
##   Open or create a file for writing at the end of the file.
##
##  @item @qcode{'p'}
##   Open a file a file for reading only node IDs.
##
##  @item @qcode{'s'}
##   Open a socket for reading or writing. The Argument @var{name} can be:
##   @table @sockettype
##   @item @qcode{"tcp://server:8200"}
##    where 8200 is the port
##   @item @qcode{"tcp://127.0.0.1:8200"}
##    where 127.0.0.1 is the IP address and 8200 is the port
##   @item @qcode{"udp://server:8200"}
##    where 8200 is the port
##   @item @qcode{"udp://127.0.0.1:8200"}
##    where 127.0.0.1 is the IP address and 8200 is the port
##  @end table
## @end table
##
## For example,
## @smallexample
##  fid = bml_open ("my_file.bml", 'r');
## @end smallexample
## opens the file 'my_file.bml' for reading.
##
## For example,
## @smallexample
##  fid = bml_open ("tcp://server:8200", 's');
## @end smallexample
## opens a server with the port 8200
##
## For example,
## @smallexample
##  fid = bml_open("tcp://127.0.0.1:5461", 's', 4);
## @end smallexample
## opens a socket to connect to the IP address 127.0.0.1, port 5461
##
## @seealso{bml_read, bml_write, bml_close}
## @end deftypefn

function varargout = bml_open (varargin)
  error ("bml_open: Not available (dummy function)");
end 

