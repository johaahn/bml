% octave
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% COMPILE BML MEX FILES
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 clear

if exist('bml_open.mex')
    delete('bml_open.mex')
end
if exist('bml_read.mex')
    delete('bml_read.mex')
end
if exist('bml_write.mex')
    delete('bml_write.mex')
end
if exist('bml_close.mex')
    delete('bml_close.mex')
end


if ~ispc
	% not windows
	mkoctfile --mex -v ./bml_open.c ../sab_parser/bml.c -I../sab_parser -lrt -Werror
	mkoctfile --mex -v ./bml_read.c ../sab_parser/bml.c -I../sab_parser -lrt -Werror
	mkoctfile --mex -v ./bml_close.c ../sab_parser/bml.c -I../sab_parser -lrt -Werror
	mkoctfile --mex -v ./bml_write.c ../sab_parser/bml.c -I../sab_parser -lrt -Werror
else
	% windows
	mkoctfile --mex -v ./bml_open.c ../sab_parser/bml.c -I../sab_parser  -lws2_32 -Werror
	mkoctfile --mex -v ./bml_read.c ../sab_parser/bml.c -I../sab_parser  -lws2_32 -Werror
	mkoctfile --mex -v ./bml_close.c ../sab_parser/bml.c -I../sab_parser  -lws2_32 -Werror
	mkoctfile --mex -v ./bml_write.c ../sab_parser/bml.c -I../sab_parser  -lws2_32 -Werror
end
 
 printf("Compilation terminated.\n");

