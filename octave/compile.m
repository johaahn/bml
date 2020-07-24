% octave
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% COMPILE BML MEX FILES
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 clear
 delete('bml_open.mex')
 delete('bml_read.mex')
 delete('bml_write.mex')
 delete('bml_close.mex')

if ~ispc
	% not windows
	mkoctfile --mex ./bml_open.c ../sab_parser/bml.c -I../sab_parser -lrt -Werror
	mkoctfile --mex ./bml_read.c ../sab_parser/bml.c -I../sab_parser -lrt -Werror
	mkoctfile --mex ./bml_close.c ../sab_parser/bml.c -I../sab_parser -lrt -Werror
	mkoctfile --mex ./bml_write.c ../sab_parser/bml.c -I../sab_parser -lrt -Werror
else
	% windows
	mkoctfile --mex ./bml_open.c ../sab_parser/bml.c -I../sab_parser  -lws2_32 -Werror
	mkoctfile --mex ./bml_read.c ../sab_parser/bml.c -I../sab_parser  -lws2_32 -Werror
	mkoctfile --mex ./bml_close.c ../sab_parser/bml.c -I../sab_parser  -lws2_32 -Werror
	mkoctfile --mex ./bml_write.c ../sab_parser/bml.c -I../sab_parser  -lws2_32 -Werror
end
 
 printf("Compilation OK\n");

