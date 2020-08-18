% Matlab
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% COMPILE BML MEX FILES
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 clear
 
if ~ispc
	%not windows
    if exist('bml_open.mexa64')
        delete('bml_open.mexa64')
    end
    if exist('bml_read.mexa64')
        delete('bml_read.mexa64')
    end
    if exist('bml_write.mexa64')
        delete('bml_write.mexa64')
    end
    if exist('bml_close.mexa64')
        delete('bml_close.mexa64')
    end
    
	mex  -v ./bml_open.c ../sab_parser/bml.c -I../sab_parser -lrt COMPFLAGS='$COMPFLAGS WIN32 -Werror' 
	mex  -v ./bml_read.c ../sab_parser/bml.c -I../sab_parser -lrt COMPFLAGS='$COMPFLAGS -Werror' 
	mex  -v ./bml_close.c ../sab_parser/bml.c -I../sab_parser -lrt COMPFLAGS='$COMPFLAGS -Werror'
	mex  -v ./bml_write.c ../sab_parser/bml.c -I../sab_parser -lrt COMPFLAGS='$COMPFLAGS -Werror'
else
	%windows
    if exist('bml_open.mexw64')
        delete('bml_open.mexw64')
    end
    if exist('bml_read.mexw64')
        delete('bml_read.mexw64')
    end
    if exist('bml_write.mexw64')
        delete('bml_write.mexw64')
    end
    if exist('bml_close.mexw64')
        delete('bml_close.mexw64')
    end
    
     % to display the current C/C++ compiler and instruction to change it
     % (Visual Studio/ MinGW)
    mex -setup
 
	mex  -v ./bml_open.c ../sab_parser/bml.c -I../sab_parser  -lws2_32 COMPFLAGS="$COMPFLAGS -DWIN32 /W4" 
	mex  -v ./bml_read.c ../sab_parser/bml.c -I../sab_parser  -lws2_32 COMPFLAGS="$COMPFLAGS -DWIN32 /W4" 
	mex  -v ./bml_close.c ../sab_parser/bml.c -I../sab_parser  -lws2_32 COMPFLAGS="$COMPFLAGS -DWIN32 /W4" 
	mex  -v ./bml_write.c ../sab_parser/bml.c -I../sab_parser  -lws2_32 COMPFLAGS="$COMPFLAGS -DWIN32 /W4" 
end

 disp("Compilation terminated.\n");

