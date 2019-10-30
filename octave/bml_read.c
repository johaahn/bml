#define _FILE_OFFSET_BITS 64
#include <mex.h>
#include <stdint.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#include <bml.h>
#include "bml_handle.h"
//#define DEBUG

const char *astr_keys[] = { "id", "data", "ext", "offset", "size", "childs" };

void f_bml_node_init(struct ST_BML_NODE * in_ps) {
	int i;
	in_ps->ps_id = NULL;
	in_ps->ps_data = NULL;
	in_ps->sz_id = 0;
	in_ps->sz_data = 0;
	for (i = 0; i < C_BML_MAX_EXT; i++) {
		in_ps->as_ext[i].sz_data = 0;
	}
	in_ps->i_nb_ext = 0;
}

void f_bml_node_update_array(struct ST_BML_NODE * in_ps_node, mxArray * in_ps_res, int in_i_cnt_res) {
	int i;
	/* Allocate data buffer */
	if (!in_ps_node->ps_id) {
		in_ps_node->ps_id = mxCreateNumericMatrix(0, 0,
				mxUINT8_CLASS, mxREAL);
	}

	if (!in_ps_node->ps_data) {
		//TODO Create allocation CB
		in_ps_node->ps_data = mxCreateNumericMatrix(0, 0,
				mxUINT8_CLASS, mxREAL);
	}


	mxSetFieldByNumber(in_ps_res, in_i_cnt_res, 0, in_ps_node->ps_id);
	mxSetFieldByNumber(in_ps_res, in_i_cnt_res, 1, in_ps_node->ps_data);

	/* Exts */
	{

    uint32_t i_last_id=0;
		for (i = 0; i < C_BML_MAX_EXT; i++) {
	    if (in_ps_node->as_ext[i].ps_data){
	      i_last_id = i;
      }
		}

		mxArray * ps_ext = mxCreateCellMatrix(1,
				i_last_id+1);
		
		for (i = 0; i <= i_last_id; i++) {
	      if (!in_ps_node->as_ext[i].ps_data) {
				  //TODO Create allocation CB
				  in_ps_node->as_ext[i].ps_data =
						  mxCreateNumericMatrix(0, 0, mxUINT8_CLASS,
								  mxREAL);
			  }
			  mxSetCell(ps_ext, i, in_ps_node->as_ext[i].ps_data);
		  
  	}
		mxSetFieldByNumber(in_ps_res, in_i_cnt_res, 2, ps_ext);
	}

	/* Offset */
	{
		mxArray * ps_tmp = mxCreateNumericMatrix(1, 1, mxUINT64_CLASS,
				mxREAL);
		uint64_t * pi_tmp = (uint64_t *) mxGetPr(ps_tmp);
		*pi_tmp = in_ps_node->off_data;
		mxSetFieldByNumber(in_ps_res, in_i_cnt_res, 3, ps_tmp);
	}

	/* Offset Size */
	{
		mxArray * ps_tmp = mxCreateNumericMatrix(1, 1, mxUINT32_CLASS,
				mxREAL);
		uint32_t * pi_tmp = (uint32_t *) mxGetPr(ps_tmp);
		*pi_tmp = in_ps_node->sz_data;
		mxSetFieldByNumber(in_ps_res, in_i_cnt_res, 4, ps_tmp);
	}
	//mxSetFieldByNumber(ps_res,i_cnt_res, i_cnt_res, ps_res_int);

	{
		size_t sz_child = 0;
		struct ST_BML_NODE * ps_child;
		 for (ps_child = in_ps_node->s_head.tqh_first; ps_child != NULL; ps_child = ps_child->s_item.tqe_next) {
			sz_child++;
		}

		{
			mxArray * ps_res = mxCreateStructMatrix(1, sz_child, 6, astr_keys);
			int i_cnt_res = 0;
			 for (ps_child = in_ps_node->s_head.tqh_first; ps_child != NULL; ps_child = ps_child->s_item.tqe_next) {				
				f_bml_node_update_array(ps_child, ps_res,i_cnt_res);
				i_cnt_res++;
			}
			mxSetFieldByNumber(in_ps_res, in_i_cnt_res, 5, ps_res);
		}
	}
}

void mexFunction(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) {
	int i;

	int ec;
	int i_nb_elem = 8192;

#ifdef DEBUG
	mexPrintf("Reading BML %d\n", nrhs);
#endif

	if ((nrhs < 1) || (nrhs > 2)) {
		mexErrMsgTxt(
				"usage: [struct, cnt] = bml_read (fid, nb_elem)");
		return;
	}


	if (!mxIsNumeric(prhs[0])) {
		mexErrMsgTxt("expecting handle as fid");
		return;
	}

	if(nrhs > 1) {
		if (!mxIsDouble(prhs[1])) {
			mexErrMsgTxt("expecting nb_elem as double");
			return;
		}
		double * pi_nb_elem = mxGetPr(prhs[1]);
		i_nb_elem = *pi_nb_elem;
		
	}

	/* Get file handle */
	uintptr_t * pi_file = (uintptr_t*) mxGetPr(prhs[0]);
	struct ST_BML_HANDLE * ps_handle = (struct ST_BML_HANDLE *) *pi_file;

	/* Check read mode */
	if (!ps_handle->b_reader_initialized) {
		mexErrMsgTxt("expecting read mode");
		return;
	}
#ifdef DEBUG
	mexPrintf("i_nb_elem:%d\n", i_nb_elem);

	mexPrintf("Handle:%p\n", ps_handle);
#endif

	/* Create cell output */
	int i_cnt_res = 0;
#define LC_NB_VALUE 6
	//mxArray * ps_res = mxCreateCellMatrix(1, 1024);
	mxArray * ps_res = mxCreateStructMatrix(1, i_nb_elem, 6, astr_keys);

	while (i_cnt_res < i_nb_elem) {
		/* Reset values */
		f_bml_node_init(&(ps_handle->s_result));

		/* Execute parser */
		ec = bml_parser_run(&ps_handle->s_parser);

#ifdef DEBUG
		mexPrintf("Sz_id:%d Sz_data:%d ec:%d\n", ps_handle->s_result.sz_id, ps_handle->s_result.sz_data, ec);
#endif

		if (ec != EC_BML_SUCCESS) {
			break;
		}

		f_bml_node_update_array(&ps_handle->s_result, ps_res, i_cnt_res);

		/* Increment */
		i_cnt_res++;
	}

	/* Set output */
	if (nlhs) {
		plhs[0] = ps_res;
	}

	if (nlhs > 1) {
		mxArray * ps_cnt = mxCreateNumericMatrix(1, 1, mxUINT32_CLASS, mxREAL);
		uint32_t * pi_cnt = (uint32_t *) mxGetPr(ps_cnt);
		*pi_cnt = i_cnt_res;
		plhs[1] = ps_cnt;
	}
}

