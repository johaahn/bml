#define _FILE_OFFSET_BITS 64
#include <mex.h>
#include <stdint.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#include <bml.h>
#include "bml_handle.h"
//#define DEBUG


int f_bml_node_write(mxArray const * in_ps_struct, int in_i_cnt, struct ST_BML_HANDLE * in_ps_handle) {
	int i;
	int ec;

	struct bml_ext as_ext[C_BML_MAX_EXT];
	uint8_t i_nb_ext = 0;


	mxArray * ps_id = mxGetField( in_ps_struct, in_i_cnt, "id");
	mxArray * ps_data = mxGetField( in_ps_struct, in_i_cnt, "data");
	mxArray * ps_ext = mxGetField( in_ps_struct, in_i_cnt, "ext");
	mxArray * ps_childs = mxGetField( in_ps_struct, in_i_cnt, "childs");


#ifdef DEBUG
	mexPrintf("Writing BML NODE (%p)\n", ps_ext);
#endif

	if (!ps_id) {
		mexErrMsgTxt("expecting a id field in struct");
		return EC_FAILURE;
	}

	if (!mxIsNumeric(ps_id)) {
		mexErrMsgTxt("expecting a id as numeric");
		return EC_FAILURE;
	}


	/* Check ext argument */
	if (ps_ext) {

		i_nb_ext = mxGetNumberOfElements (ps_ext);
		if (mxIsCell(ps_ext)) {
			for(i= 0; i< i_nb_ext; i++) {
				mxArray *ps_tmp = mxGetCell (ps_ext, i);
				as_ext[i].sz_data = (mxGetM(ps_tmp) * mxGetN(ps_tmp) * mxGetElementSize(ps_tmp));
				as_ext[i].pv_data = (char *) mxGetPr(ps_tmp);
				as_ext[i].i_id = i;

				if(in_ps_handle->i_align) {
					as_ext[i].sz_data = M_ALIGN(as_ext[i].sz_data,in_ps_handle->i_align);
				}
			}
		} else if (mxIsNumeric(ps_ext)) {
			char *ps_tmp = (char*) mxGetData (ps_ext);
			for(i= 0; i< i_nb_ext; i++) {
				as_ext[i].sz_data = mxGetElementSize(ps_ext);
				as_ext[i].pv_data = ps_tmp;
				as_ext[i].i_id = i;

				if(in_ps_handle->i_align) {
					as_ext[i].sz_data = M_ALIGN(as_ext[i].sz_data,in_ps_handle->i_align);
				}
				ps_tmp += mxGetElementSize(ps_ext);
			}
		} else {
			mexErrMsgTxt("expecting cell as extension");
			return EC_FAILURE;
		}


	}


	/* Check writer mode */
	if (!in_ps_handle->b_writer_initialized) {
		mexErrMsgTxt("expecting writer mode");
		return EC_FAILURE;
	}

	/* Get Id */
	int64_t i_id;


	if (mxGetClassID(ps_id) == mxUINT8_CLASS) {
		uint32_t *ptr = mxGetData(ps_id);
		i_id = *ptr;
	} else if (mxGetClassID(ps_id) == mxDOUBLE_CLASS) {
		i_id = (int64_t) *mxGetPr(ps_id);
	}

	size_t sz_buf = 0;
	char * pc_buf = NULL;
	/* Get Data */
	if(ps_data) {
		sz_buf = (mxGetM(ps_data) * mxGetN(ps_data) * mxGetElementSize(ps_data));
		pc_buf = (char *) mxGetPr(ps_data);
	}

	/* Write data */
	if (in_ps_handle->i_align) {
		ec = bml_writer_element_start(&in_ps_handle->s_writer, &i_id, in_ps_handle->i_align,
				pc_buf, M_ALIGN(sz_buf, in_ps_handle->i_align), as_ext, i_nb_ext, in_ps_handle->i_align-1, in_ps_handle->i_align, in_ps_handle->i_align-1);
		if (ec != EC_BML_SUCCESS) {
			mexErrMsgTxt("Unable to write start element");
			return EC_FAILURE;
		}
	} else {
		ec = bml_writer_element_start(&in_ps_handle->s_writer, &i_id, sizeof(i_id),
				pc_buf, sz_buf, as_ext, i_nb_ext, 0, 0, 0);
		if (ec != EC_BML_SUCCESS) {
			mexErrMsgTxt("Unable to write start element");
			return EC_FAILURE;		}
	}

	/* Align next node */
	if (in_ps_handle->i_align) {
		ec = bml_writer_align(&in_ps_handle->s_writer, in_ps_handle->i_align);
		if (ec != EC_BML_SUCCESS) {
			mexErrMsgTxt("Unable to write end element");
			return EC_FAILURE;
		}
	}


	if(ps_childs) {
		int i_nb_childs = mxGetNumberOfElements (ps_childs);
		for(i= 0; i< i_nb_childs; i++) {
			 ec = f_bml_node_write(ps_childs, i, in_ps_handle);
			if(ec != EC_SUCCESS) {
				return ec;
			}
		}
	}

	/* Align next node */
	if (in_ps_handle->i_align) {
		/* Prealign element end - size = 1 byte*/
		ec = bml_writer_prealign(&in_ps_handle->s_writer, in_ps_handle->i_align,1);
		if (ec != EC_BML_SUCCESS) {
			mexErrMsgTxt("Unable to write end element");
			return EC_FAILURE;
		}
	}

	/* Write end node */
	ec = bml_writer_element_end(&in_ps_handle->s_writer);
	if (ec != EC_BML_SUCCESS) {
		mexErrMsgTxt("Unable to write end element");
    return EC_FAILURE;
	}


	return EC_SUCCESS;

}

void mexFunction(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) {
	int i;
	int ec;

#ifdef DEBUG
	mexPrintf("Writing BML\n");
#endif

	if (nrhs < 2) {
		mexErrMsgTxt("usage: bml_write (fid, struct)");
		return;
	}

	if (!mxIsNumeric(prhs[0])) {
		mexErrMsgTxt("expecting handle as fid");
		return;
	}

	if (!mxIsStruct(prhs[1])) {
		mexErrMsgTxt("expecting a struct");
		return;
	}



	/* Get file handle */
	uintptr_t * pi_file = (uintptr_t*) mxGetPr(prhs[0]);
	struct ST_BML_HANDLE * ps_handle = (struct ST_BML_HANDLE *) *pi_file;

	{
		int i_nb_childs = mxGetNumberOfElements (prhs[1]);
		for(i= 0; i< i_nb_childs; i++) {
			 ec = f_bml_node_write(prhs[1], i, ps_handle);
			if(ec != EC_SUCCESS) {
				mexErrMsgTxt("Error during BML node write");
				return ;
			}
		}
	}
#ifdef DEBUG
	mexPrintf("Handle:%p\n", ps_handle);
#endif
}
