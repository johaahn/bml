#define _FILE_OFFSET_BITS 64
#include <mex.h>
#include <stdint.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
//#include <netdb.h>
#include <unistd.h>

#ifdef WIN32
#include <winsock2.h>
#endif

//#define DEBUG
//#define DEBUG_IO

#include <bml.h>
#include "bml_handle.h"
void cb_start_element(struct bml_parser * in_ps_ctx, void * in_pv_user_arg,
		void * in_pv_id, bml_size_t in_sz_id, bml_off_t in_off_id,
		void * in_pv_data, bml_size_t in_sz_data, bml_off_t in_off_data,
		struct bml_ext * in_as_ext, int32_t in_i_nb_ext, int32_t in_i_level) {
	int i;
	struct ST_BML_HANDLE * ps_handle = (struct ST_BML_HANDLE *) in_pv_user_arg;
	struct ST_BML_NODE * ps_result = NULL;
	ps_handle->i_level++;

	if(ps_handle->i_level) {
		ps_result = (struct ST_BML_NODE*) malloc(sizeof(struct ST_BML_NODE));
		ps_result->ps_parent = ps_handle->ps_current;

		TAILQ_INSERT_TAIL(&(ps_result->ps_parent->s_head), ps_result, s_item);
	} else {
		ps_result = &ps_handle->s_result;
		ps_result->ps_parent = NULL;

	}

	TAILQ_INIT(&(ps_result->s_head));
	ps_handle->ps_current = ps_result;

	ps_result->pc_data = in_pv_data;
	ps_result->sz_data = in_sz_data;
	ps_result->off_data = in_off_data;
	ps_result->pc_id = in_pv_id;
	ps_result->sz_id = in_sz_id;

	/* Copy data from tmp */
	ps_result->ps_id = ps_handle->ps_id;
	ps_result->ps_data = ps_handle->ps_data;
	for (i = 0; i < C_BML_MAX_EXT; i++) {
		ps_result->as_ext[i].ps_data = ps_handle->as_ext[i].ps_data;
	}
	ps_result->i_nb_ext = in_i_nb_ext;

	/* Reset temo */
	ps_handle->ps_id = NULL;
	ps_handle->ps_data = NULL;
	for (i = 0; i < C_BML_MAX_EXT; i++) {
		ps_handle->as_ext[i].ps_data = NULL;
	}

#ifdef DEBUG
	mexPrintf("cb_start_element lvl:%d sz_data:%d nb_ext:%d\n", ps_handle->i_level, in_sz_data, in_i_nb_ext);
#endif

}

void cb_end_element(struct bml_parser * in_ps_ctx, void *in_pv_user_arg) {
	struct ST_BML_HANDLE * ps_handle = (struct ST_BML_HANDLE *) in_pv_user_arg;
	//ps_handle->ps_result->i_level = ps_result->i_level-1;

#ifdef DEBUG
	mexPrintf("cb_end_element %d\n", ps_handle->i_level);
#endif
	ps_handle->ps_current = ps_handle->ps_current->ps_parent;
	ps_handle->i_level--;

	/* Stop on end of root */
	if(ps_handle->i_level == -1) {
		bml_parser_stop(in_ps_ctx);
	}
}
#define LF_SIZE_BUFFER (32*1024)
static char ac_tmp[LF_SIZE_BUFFER];
static int ac_tmp_acc = 0;
static int ac_tmp_index = 0;

int cb_io_seek(struct bml_parser * in_ps_ctx, void *in_pv_user_arg,
		bml_size_t in_sz_offset) {
	struct ST_BML_HANDLE * ps_handle = (struct ST_BML_HANDLE *) in_pv_user_arg;
#ifdef DEBUG
	mexPrintf("Allocating SEEK %d\n", in_sz_offset);
#endif
	bml_size_t sz_data_remaining = in_sz_offset;
	size_t sz_elem;
	while (sz_data_remaining) {
#ifdef DEBUG_IO
		printf("s- %d %d %d\n", ac_tmp_index, ac_tmp_acc,
				(int) sz_data_remaining);
#endif
		if (ac_tmp_index == ac_tmp_acc) {
			fseek(ps_handle->fd, sz_data_remaining, SEEK_CUR);
#ifdef DEBUG_IO
			printf("seek %d\n", (int) in_sz_offset);
#endif
			sz_elem = sz_data_remaining;
		} else {

			if (sz_data_remaining <= (ac_tmp_acc - ac_tmp_index)) {
				sz_elem = sz_data_remaining;
			} else {
				sz_elem = ac_tmp_acc - ac_tmp_index;
			}
			ac_tmp_index += sz_elem;
		}

		sz_data_remaining -= sz_elem;
	}
	return EC_BML_SUCCESS;
}
;

int cb_io_read(struct bml_parser * in_ps_ctx, void *in_pv_user_arg,
		char * in_pc_data, bml_size_t in_sz_data_max, char in_b_first_header) {
	struct ST_BML_HANDLE * ps_handle = (struct ST_BML_HANDLE *) in_pv_user_arg;
	bml_size_t sz_data_remaining = in_sz_data_max;
	bml_size_t sz_data_done = 0;
	size_t sz_elem;

	while (sz_data_remaining) {
#ifdef DEBUG_IO
		printf("r- %d %d %d\n", ac_tmp_index, ac_tmp_acc,
				(int) sz_data_remaining);
#endif
		if (ac_tmp_index > ac_tmp_acc) {
			printf("read error\n");
			return EC_BML_FAILURE;
		}

		if (ac_tmp_index == ac_tmp_acc) {
			ac_tmp_acc = fread(&ac_tmp[0], 1, LF_SIZE_BUFFER, ps_handle->fd);
			ac_tmp_index = 0;
			if (ac_tmp_acc <= 0) {
				ac_tmp_acc = 0;
				if(feof(ps_handle->fd)) {
					return EC_BML_NODATA;
				} else {
					printf("read error %d\n", feof(ps_handle->fd));
					return EC_BML_FAILURE;
				}
			}
#ifdef DEBUG_IO
			printf("read %d\n", ac_tmp_acc);
#endif
		}

		//assert(sz_data_remaining > 0);
		if (sz_data_remaining <= (ac_tmp_acc - ac_tmp_index)) {
			sz_elem = sz_data_remaining;
		} else {
			sz_elem = ac_tmp_acc - ac_tmp_index;
		}

		memcpy(&in_pc_data[sz_data_done], &ac_tmp[ac_tmp_index], sz_elem);
		ac_tmp_index += sz_elem;
		sz_data_remaining -= sz_elem;
		sz_data_done += sz_elem;
	}
	//printf("= %d %d %d\n", ac_tmp_index, ac_tmp_acc, (int) sz_data_remaining);

	return EC_BML_SUCCESS;
}

int cb_io_write(struct bml_writer * in_ps_writer, void *in_pv_user_arg,
		char * in_pc_data, bml_size_t in_sz_data) {
	struct ST_BML_HANDLE * ps_handle = (struct ST_BML_HANDLE *) in_pv_user_arg;
	bml_size_t sz_data_remaining = in_sz_data;
	bml_size_t sz_data_done = 0;
	size_t sz_elem;
#if 0
	if (in_sz_data > 8) {
		printf("Writing %d bytes\n",(int)in_sz_data);
	}
#endif
	while (sz_data_remaining) {
		int i_nb_elem;

		//assert(sz_data_remaining > 0);
		if (sz_data_remaining <= 512) {
			sz_elem = sz_data_remaining;
			i_nb_elem = fwrite(&in_pc_data[sz_data_done], sz_elem, 1,
					ps_handle->fd);
#if 0
			{
				int i;
				printf("Writing %d element of %i\n",i_nb_elem,(int)sz_elem);
				for(i=0; i<sz_elem; i++) {
					printf("%02x",in_pc_data[sz_data_done+i]);
				}
				printf("\n");
			}
#endif
		} else {
			sz_elem = 512;
			i_nb_elem = fwrite(&in_pc_data[sz_data_done], sz_elem,
					sz_data_remaining / 512, ps_handle->fd);
		}
		if (i_nb_elem <= 0) {
			printf("write error\n");
			return EC_BML_FAILURE;
		}
		sz_data_remaining -= sz_elem * i_nb_elem;
		sz_data_done += sz_elem * i_nb_elem;
		//D("%d",sz_data_remaining);
	}

	return EC_BML_SUCCESS;
}

int cb_new_id(struct bml_parser * in_ps_ctx, void *in_pv_user_arg,
		char ** in_ppc_id, bml_size_t in_sz_id) {
	struct ST_BML_HANDLE * ps_handle = (struct ST_BML_HANDLE *) in_pv_user_arg;
#ifdef DEBUG
	mexPrintf("Allocating ID %d\n", in_sz_id);
#endif

	ps_handle->ps_id = mxCreateNumericMatrix(in_sz_id, 1,
			mxUINT8_CLASS, mxREAL);

	*in_ppc_id = (char*) mxGetPr(ps_handle->ps_id);
	return EC_BML_SUCCESS;
}
int cb_new_data(struct bml_parser * in_ps_ctx, void *in_pv_user_arg,
		char ** in_ppc_data, bml_size_t in_sz_data) {
	struct ST_BML_HANDLE * ps_handle = (struct ST_BML_HANDLE *) in_pv_user_arg;
#ifdef DEBUG
	mexPrintf("Allocating DATA %d\n", in_sz_data);
#endif
	ps_handle->ps_data = mxCreateNumericMatrix(in_sz_data, 1,
			mxUINT8_CLASS, mxREAL);

	*in_ppc_data = (char *) mxGetPr(ps_handle->ps_data);
	return EC_BML_SUCCESS;
}

int cb_new_ext(struct bml_parser * in_ps_ctx, void *in_pv_user_arg,
		char ** in_ppc_data, bml_size_t in_sz_data, uint8_t in_i_ext_id) {
	struct ST_BML_HANDLE * ps_handle = (struct ST_BML_HANDLE *) in_pv_user_arg;
	int i_ext_id = in_i_ext_id;
#ifdef DEBUG
	mexPrintf("Allocating EXT %d : %d\n", in_i_ext_id, in_sz_data);
#endif
  if (i_ext_id < C_BML_MAX_EXT) {
  	ps_handle->as_ext[i_ext_id].ps_data = mxCreateNumericMatrix(
			in_sz_data, 1, mxUINT8_CLASS, mxREAL);
  	*in_ppc_data = (char *) mxGetPr(ps_handle->as_ext[i_ext_id].ps_data);
	} else {
  	mexPrintf("i_ext_id too big\n");
  }	
	//ps_handle->s_result.s_ext[i_ext_id].ps_id = mxCreateNumericMatrix(1, 1,mxUINT8_CLASS, mxREAL);
	//ps_handle->s_tmp.as_ext[i_ext_id].sz_data = in_sz_data;
	//ps_handle->s_tmp.as_ext[i_ext_id].pc_data = ;
	//ps_handle->s_result.s_ext[i_ext_id].pc_id = (char *) mxGetPr(ps_handle->s_result.ps_id);

	//ps_handle->s_tmp.i_nb_ext 	M_MAX(i_ext_id+1, ps_handle->s_result.i_nb_ext);

	return EC_BML_SUCCESS;
}


int cb_io_socket_read(struct bml_parser * in_ps_ctx, void *in_pv_user_arg,
		char * in_pc_data, bml_size_t in_sz_data_max, char in_b_first_header) {
	struct ST_BML_HANDLE * ps_handle = (struct ST_BML_HANDLE *) in_pv_user_arg;
	bml_size_t sz_data_remaining = in_sz_data_max;
	bml_size_t sz_data_done = 0;
	size_t sz_elem;

	while (sz_data_remaining) {
#ifdef DEBUG_IO
		printf("r- %d %d %d\n", ac_tmp_index, ac_tmp_acc,
				(int) sz_data_remaining);
#endif
		if (ac_tmp_index > ac_tmp_acc) {
			printf("read error --%d %d %d\n", ac_tmp_index, ac_tmp_acc,
				(int) sz_data_remaining);
			return EC_BML_FAILURE;
		}

		if (ac_tmp_index == ac_tmp_acc) {

#ifndef WIN32
			ac_tmp_acc = recv(ps_handle->i_socket, &ac_tmp[0], LF_SIZE_BUFFER, MSG_DONTWAIT);
#else
            ac_tmp_acc = recv(ps_handle->i_socket, &ac_tmp[0], LF_SIZE_BUFFER, 0);
#endif
			if((ac_tmp_acc < 0) && ((errno == EAGAIN) || (errno == EWOULDBLOCK)) ) {
#ifdef DEBUG_IO
				printf("read WAIT %d\n", ac_tmp_acc);
#endif
				do {
					ac_tmp_acc = recv(ps_handle->i_socket, &ac_tmp[0], sz_data_remaining > LF_SIZE_BUFFER ? LF_SIZE_BUFFER : sz_data_remaining, 0);
#ifdef DEBUG_IO
					printf("errno %d\n", (errno == EAGAIN) || (errno == EWOULDBLOCK));
#endif
					/* Allow timeout on first header */
					if(ac_tmp_acc < 0) {
						if ((errno == EAGAIN) || (errno == EWOULDBLOCK)) {
							printf("Socket timeout\n");
							ac_tmp_acc = 0;
							ac_tmp_index = 0;
							if( (in_b_first_header) ) {
								return EC_BML_NODATA;
							}
						} else {
							break;
						}
					}
				} while(ac_tmp_acc < 0);
			}

			ac_tmp_index = 0;
			if (ac_tmp_acc <= 0) {
				mexPrintf("Error in receive\n");
				perror("recv");
				//printf("read error\n");
				return EC_BML_FAILURE;
			}

#ifdef DEBUG_IO
			printf("read %d\n", ac_tmp_acc);
#endif
		}

		//assert(sz_data_remaining > 0);
		if (sz_data_remaining <= (ac_tmp_acc - ac_tmp_index)) {
			sz_elem = sz_data_remaining;
		} else {
			sz_elem = ac_tmp_acc - ac_tmp_index;
		}

		memcpy(&in_pc_data[sz_data_done], &ac_tmp[ac_tmp_index], sz_elem);
		ac_tmp_index += sz_elem;
		sz_data_remaining -= sz_elem;
		sz_data_done += sz_elem;
	}
#ifdef DEBUG_IO
	printf("= %d %d %d\n", ac_tmp_index, ac_tmp_acc, (int) sz_data_remaining);
#endif
	return EC_BML_SUCCESS;
}

int cb_io_socket_write(struct bml_writer * in_ps_writer, void *in_pv_user_arg,
		char * in_pc_data, bml_size_t in_sz_data) {
	struct ST_BML_HANDLE * ps_handle = (struct ST_BML_HANDLE *) in_pv_user_arg;

	int i_nb_elem = send(ps_handle->i_socket, in_pc_data, in_sz_data, 0);
	if (i_nb_elem == -1){
		mexPrintf("Error in send\n");
		perror("send");
		return EC_BML_FAILURE;
	}

	return EC_BML_SUCCESS;
}


void mexFunction(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) {
	int i;
	mxArray *v;
	int ec;
	int sz_buf, sz_buf2;
	char * pc_mode = NULL;
	char * pc_file = NULL;
	int i_align = 0;
	char ac_header[4] = { 'B', 'M', 'L', 0 };

	if (nrhs < 2) {
		mexErrMsgTxt("usage: [fid] = bml_open (name, mode, align)");
		return;
	}

	if (!mxIsChar(prhs[0])) {
		mexErrMsgTxt("expecting char as input file argument");
		return;
	}

	if (!mxIsChar(prhs[1])) {
		mexErrMsgTxt("expecting char as mode argument : r|w");
		return;
	}

	if (nrhs > 2 && !mxIsNumeric(prhs[2])) {
		mexErrMsgTxt("expecting align as numeric");
		return;
	}

	/* Get the length of the input string. */
	sz_buf = (mxGetM(prhs[0]) * mxGetN(prhs[0]) * sizeof(mxChar)) + 1;
	/* Allocate memory for input and output strings. */
	pc_file = mxCalloc(sz_buf, sizeof(char));
	/* Copy the string data from prhs[0] into a C string
	 * input_buf. If the string array contains several rows,
	 * they are copied, one column at a time, into one long
	 * string array. */
	ec = mxGetString(prhs[0], pc_file, sz_buf);
	if (ec != 0) {
		mexWarnMsgTxt("Not enough space. String is truncated.");
		ec = EC_FAILURE;
		goto out_err;
	}


	/* Get the length of the input string. */
	sz_buf2 = (mxGetM(prhs[1]) * mxGetN(prhs[1]) * sizeof(mxChar)) + 1;
	/* Allocate memory for input and output strings. */
	pc_mode = mxCalloc(sz_buf2, sizeof(char));
	/* Copy the string data from prhs[1] into a C string
	 * input_buf. If the string array contains several rows,
	 * they are copied, one column at a time, into one long
	 * string array. */
	ec = mxGetString(prhs[1], pc_mode, sz_buf2);
	if (ec != 0) {
		mexWarnMsgTxt("Not enough space. String is truncated.");
		ec = EC_FAILURE;
		goto out_err;
	}

	if (nrhs > 2) {
		i_align = (int) *mxGetPr(prhs[2]);
		switch (i_align) {
		case 0:
		case 2:
		case 4:
		case 8:
			break;
		default:
			mexErrMsgTxt("Invalid alignment");
			return;
			break;
		}
	}

#ifdef DEBUG
	mexPrintf("Opening:%s\n", pc_file);
#endif

	/* Allocate BML handle */
	struct ST_BML_HANDLE * ps_handle = (struct ST_BML_HANDLE*) malloc(
			sizeof(struct ST_BML_HANDLE));
	if (!ps_handle) {
		mexPrintf("unable to allocate more memory (%s)\n", pc_file);
		mexErrMsgTxt("unable to allocate more memory");
		ec = EC_FAILURE;
		goto out_err;
	}

	ps_handle->b_writer_initialized = 0;
	ps_handle->b_reader_initialized = 0;

	ps_handle->i_socket = -1;
	ps_handle->i_level = -1;

	ps_handle->fd = NULL;

  ac_tmp_acc = 0;
  ac_tmp_index = 0;

	/* Reset read temp pointers */
	ps_handle->ps_id = NULL;
	ps_handle->ps_data = NULL;

	for (i = 0; i < C_BML_MAX_EXT; i++) {
		ps_handle->as_ext[i].ps_data = NULL;
	}

	if ((pc_mode[0] == 'r') || (pc_mode[0] == 'p')) {

		/* Opening FILE */
		ps_handle->fd = fopen(pc_file, "rb");



		if (ps_handle->fd == NULL) {
			mexPrintf("unable to open file %s: %s\n", pc_file, strerror(errno));
			mexErrMsgTxt("unable to open file");
			ec = EC_FAILURE;
			goto out_err;
		}
#ifdef DEBUG
		mexPrintf("Handle:%p\n", ps_handle);
#endif
		/* Check for header */
		{
			char ac_tmp[4];
			/* write header */
			ec = fread(ac_tmp, 4, 1, ps_handle->fd);
			if (ec <= 0) {
				mexPrintf("unable to read file %d: %s\n", ec, strerror(errno));
				mexErrMsgTxt("unable to read file");
				ec = EC_FAILURE;
				goto out_err;
			}

			if (memcmp(ac_header, ac_tmp, 4) != 0) {
				mexErrMsgTxt("No BML Header found");
			}
		}

		{
			struct bml_parser_callbacks s_cb = { .cb_start_element =
					cb_start_element, .cb_end_element = cb_end_element,
					.cb_io_read = cb_io_read, .cb_io_seek = cb_io_seek,
					.cb_new_id = cb_new_id, .cb_new_data =
							(pc_mode[0] == 'r') ? cb_new_data : NULL,
					.cb_new_ext = cb_new_ext };

			ec = bml_parser_init(&ps_handle->s_parser, &s_cb, ps_handle);
			if (ec != EC_BML_SUCCESS) {
				mexErrMsgTxt("unable to initialize BML parser");
				ec = EC_FAILURE;
				goto out_err;
			}
		}

		ps_handle->b_reader_initialized = 1;

	} else if (pc_mode[0] == 'a') {
		ps_handle->i_align = i_align;

		/* Opening FILE */
		ps_handle->fd = fopen(pc_file, "ab");
		if (ps_handle->fd == NULL) {
			mexPrintf("unable to open file %s: %s\n", pc_file, strerror(errno));
			mexErrMsgTxt("unable to open file");
			ec = EC_FAILURE;
			goto out_err;
		}

		{
			struct bml_writer_callbacks s_cb = { .cb_io_write = cb_io_write, };

			ec = bml_writer_init(&ps_handle->s_writer, &s_cb, ps_handle);
			if (ec != EC_BML_SUCCESS) {
				mexErrMsgTxt("unable to initialize BML writer");
				ec = EC_FAILURE;
				goto out_err;
			}
		}

		ps_handle->b_writer_initialized = 1;


	} else if (pc_mode[0] == 'w') {
		ps_handle->i_align = i_align;

		/* Opening FILE */
		ps_handle->fd = fopen(pc_file, "wb");
		if (ps_handle->fd == NULL) {
			mexPrintf("unable to open file %s: %s\n", pc_file, strerror(errno));
			mexErrMsgTxt("unable to open file");
			ec = EC_FAILURE;
			goto out_err;
		}

		/* write header */

		{
			ec = fwrite(ac_header, 4, 1, ps_handle->fd);
			if (ec <= 0) {
				mexErrMsgTxt("unable to read file");
				ec = EC_FAILURE;
				goto out_err;
			}
		}
#ifdef DEBUG
		mexPrintf("Handle:%p\n", ps_handle);
#endif

		{
			struct bml_writer_callbacks s_cb = { .cb_io_write = cb_io_write, };

			ec = bml_writer_init(&ps_handle->s_writer, &s_cb, ps_handle);
			if (ec != EC_BML_SUCCESS) {
				mexErrMsgTxt("unable to initialize BML writer");
				ec = EC_FAILURE;
				goto out_err;
			}
		}

		ps_handle->b_writer_initialized = 1;

	} else if (pc_mode[0] == 's'){
		ps_handle->i_align = i_align;

		int is_udp = 0;
		int is_tcp = 0;
		int socket_fd;
		int is_server = 0;
		int i_port;

		if (sz_buf >= strlen("udp://x.x.x.x:P")){
			is_udp = (strncmp(pc_file, "udp://", strlen("udp://")) == 0);
			is_tcp = (strncmp(pc_file, "tcp://", strlen("tcp://")) == 0);
		} else {
			mexErrMsgTxt("Invalid socket syntax (\"tcp://xxx.xxx.xxx.xxx:port\" expected)\n");
			ec = EC_FAILURE;
			goto out_err;
		}

		struct hostent *he;
		struct sockaddr_in server_info;

		char *ac_port;

		is_server = (strncmp(&pc_file[6], "server", strlen("server")) == 0);

		if (is_server) {
			ac_port = &pc_file[strlen("XXX://server:")];
		} else {

			char *ac_ip = &pc_file[6];
			ac_port = strchr(ac_ip, ':');

			if (ac_port == NULL){
				mexPrintf("Unable to find ':' in %s\n", ac_ip);
				ec = EC_FAILURE;
				goto out_err;
			}

			*(ac_port++) = '\0';

#ifdef DEBUG
			mexPrintf("IP : %s\n", ac_ip);
#endif

			he = (struct hostent *) gethostbyname(ac_ip);
			if (he == NULL) {  // get the host info
				//herror("gethostbyname");
                fprintf(stderr, "gethostbyname");
				mexErrMsgTxt("Error in gethostbyname");
				ec = EC_FAILURE;
				goto out_err;
			}
		}

		if (sscanf(ac_port, "%i", &i_port)!=1) {
			mexPrintf("Unable to extract port name from string %s\n", ac_port);
			ec = EC_FAILURE;
			goto out_err;
		}

#ifdef DEBUG
		mexPrintf("Port : %i\n", i_port);
#endif


		if (is_udp) {
			if ((socket_fd = socket(AF_INET, SOCK_DGRAM, 0))== -1) {
				mexErrMsgTxt("Socket Failure\n");
				ec = EC_FAILURE;
				goto out_err;
			}
		} else {
			if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0))== -1) {
				mexErrMsgTxt("Socket Failure\n");
				ec = EC_FAILURE;
				goto out_err;
			}
		}



		if (is_server) {

			struct sockaddr_in server;
			struct sockaddr_in dest;
			int yes =1;



			if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, (const void *)&yes, sizeof(int)) == -1) {
				mexErrMsgTxt("setsockopt\n");
				ec = EC_FAILURE;
				goto out_err;
			}

			memset(&server, 0, sizeof(server));
			memset(&dest,0,sizeof(dest));
			server.sin_family = AF_INET;
			server.sin_port = htons(i_port);
			server.sin_addr.s_addr = INADDR_ANY;
			if ((bind(socket_fd, (struct sockaddr *)&server, sizeof(struct sockaddr )))== -1)	{ //sizeof(struct sockaddr)
				mexErrMsgTxt("Binding Failure\n");
				ec = EC_FAILURE;
				goto out_err;
			}

			if (is_tcp){
				if ((listen(socket_fd, 1))== -1){
					mexErrMsgTxt("Listening Failure\n");
					ec = EC_FAILURE;
					goto out_err;
				}

				{
#ifndef WIN32
					socklen_t size = sizeof(struct sockaddr_in);
#else
                    int size = sizeof(struct sockaddr_in);
#endif
					int client_fd;
					if ((client_fd = accept(socket_fd, (struct sockaddr *)&dest, &size))==-1) {
						//fprintf(stderr,"Accept Failure\n");
						perror("accept");
						exit(1);
					}
					/* Close parent socket */
					close(socket_fd);
					/* Keep client socket */
					socket_fd = client_fd;
				}
			}

		} else {

			memset(&server_info, 0, sizeof(server_info));
			server_info.sin_family = AF_INET;
			server_info.sin_port = htons(i_port);

			if (is_server) {
				server_info.sin_addr.s_addr = INADDR_ANY;
			} else {
				server_info.sin_addr = *((struct in_addr *)he->h_addr);
			}

			if (connect(socket_fd, (struct sockaddr *)&server_info, sizeof(struct sockaddr))<0) {
				mexErrMsgTxt("Connection Failure. Is client running ?\n");
				ec = EC_FAILURE;
				goto out_err;
			}

		}

		{
	      		struct timeval tv; /* timeval and timeout stuff added by davekw7x */
			int timeouts = 0;
			tv.tv_sec = 3;
			tv.tv_usec = 0;
			if (setsockopt(socket_fd, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv,  sizeof tv))
			{
				perror("setsockopt");
				mexErrMsgTxt("setsockopt Failure\n");
				ec = EC_FAILURE;
				goto out_err;
			}
		}

		ps_handle->i_socket = socket_fd;

		{
			struct bml_writer_callbacks s_cb = { .cb_io_write = cb_io_socket_write, };

			ec = bml_writer_init(&ps_handle->s_writer, &s_cb, ps_handle);
			if (ec != EC_BML_SUCCESS) {
				mexErrMsgTxt("unable to initialize BML writer");
				ec = EC_FAILURE;
				goto out_err;
			}
		}

		{
			struct bml_parser_callbacks s_cb = { .cb_start_element =
					cb_start_element, .cb_end_element = cb_end_element,
					.cb_io_read = cb_io_socket_read, .cb_io_seek = NULL,
					.cb_new_id = cb_new_id, .cb_new_data =
							 cb_new_data,
					.cb_new_ext = cb_new_ext };

			ec = bml_parser_init(&ps_handle->s_parser, &s_cb, ps_handle);
			if (ec != EC_BML_SUCCESS) {
				mexErrMsgTxt("unable to initialize BML parser");
				ec = EC_FAILURE;
				goto out_err;
			}
		}

		ps_handle->b_writer_initialized = ps_handle->b_reader_initialized = 1;


	} else {
		mexErrMsgTxt("Unknown mode");
	}

	/* Create FILE pointer */
#if defined(__x86_64__) || defined(__aarch64__) 
	v = mxCreateNumericMatrix(1, 1, mxUINT64_CLASS, mxREAL);
#else
	v = mxCreateNumericMatrix(1, 1, mxUINT32_CLASS, mxREAL);
#endif
	uintptr_t * pi_value = (uintptr_t *) mxGetPr(v);
	*pi_value = (uintptr_t) ps_handle;

	ec = EC_SUCCESS;
	out_close: if (ec != EC_SUCCESS) {
		if (ps_handle->fd) {
			fclose(ps_handle->fd);
		}
		if (ps_handle->i_socket != -1) {
			close(ps_handle->i_socket);
		}
	}

	out_err: if (pc_file) {
		mxFree(pc_file);
	}
	if (pc_mode) {
		mxFree(pc_mode);
	}

	if (nlhs)
		plhs[0] = v;

}
