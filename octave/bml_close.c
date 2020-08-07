#define _FILE_OFFSET_BITS 64
#include <mex.h>
#include <stdint.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

//#ifndef WIN32
//#include <unistd.h>
//#else
//#include <winsock2.h>
//#endif

#include <bml.h>
#include "bml_handle.h"
//#define DEBUG

void mexFunction(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) {

	int ec;

#ifdef DEBUG
	mexPrintf("Closing BML\n");
#endif

	if ((nrhs < 1) || (nrhs > 1)) {
		mexErrMsgTxt("usage: bml_close (fid)");
		return;
	}

	if (!mxIsNumeric(prhs[0])) {
		mexErrMsgTxt("expecting handle as fid");
		return;
	}

	/* Get file handle */
	uintptr_t * ps_mxhandle = (uintptr_t*) mxGetPr(prhs[0]);
	struct ST_BML_HANDLE * ps_handle = (struct ST_BML_HANDLE *) *ps_mxhandle;
	if (!ps_handle) {
		mexErrMsgTxt("expecting open handle");
		return;
	}

	/* Check write mode */
	if (ps_handle->b_writer_initialized) {
		while (ps_handle->i_level >= 0) {

			ec = bml_writer_element_end(&ps_handle->s_writer);
			if (ec != EC_BML_SUCCESS) {
				mexErrMsgTxt("Unable to write end element");
				ec = EC_FAILURE;
				goto out_err;
			}

			if (ps_handle->i_align) {
				ec = bml_writer_align(&ps_handle->s_writer, ps_handle->i_align);
				if (ec != EC_BML_SUCCESS) {
					mexErrMsgTxt("Unable to write end element");
					ec = EC_FAILURE;
					return;
				}
			}
			ps_handle->i_level--;
		}
	}

	out_err:
	/* Write data */
	if (ps_handle->fd) {
		fclose(ps_handle->fd);
	}
	if (ps_handle->i_socket != -1) {
		close(ps_handle->i_socket);
	}
	/* Free handle */
	free(ps_handle);
	*(void **) ps_mxhandle = NULL;

}
