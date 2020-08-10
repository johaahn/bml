%octave
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% TEST
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
addpath('./../');
if ~ispc % not windows	
    filename=[getenv("HOME") + '/bml_test.dat'];
else % windows
    filename= 'bml_test.dat';
end

ID=24;
SUB_ID=16;

fprintf('TEST WRITE: %s\n',filename);

% Opening file in order to write data into
fid = bml_open(filename, 'w');

% Inserting node inside file
bml_write(fid, struct('id', 1,  'data', 'data1'));
bml_write(fid, struct('id', 10, 'data', "data10"));
bml_write(fid, struct('id', 1,  'data',  uint64(20000)));
bml_write(fid, struct('id', 10, 'data', "data10.2"));
bml_write(fid, struct('id', 11, 'data', []));
bml_write(fid, struct('id', 12,	'data',  uint32([5,6])));

% Inserting node inside file with extension
node = struct('id', 1, 'data', uint64(30000));
node.ext = {[],[],uint64([3])};
bml_write(fid, node);

% Inserting node inside file with 2 childs
a = struct('id', 13, 'data', "data child 1");
b = struct('id', 14, 'data', "data child 2");
bml_write(fid, struct('id', 10, 'data', "data parent" , "childs", [a, b]));

% Closing file
bml_close(fid);


fprintf('TEST READ\n');

% Opening file in order to read data from
fid = bml_open(filename, 'r');
cnt = 1;
count = 0;
ii = 1;
id = zeros(1500,1);

% Loop while there is node to read
while cnt
  % Read nodes from files. Return multiple nodes
  [res , cnt] = bml_read(fid);
  % Node user read function
  count = count + display_node(res,cnt);
end
bml_close(fid);
