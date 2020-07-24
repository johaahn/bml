#ifndef WIN32
#include <sys/queue.h>
#else
#include "queue.h"
#endif

#ifndef BML_HANDLE_H_
#define BML_HANDLE_H_

#define EC_SUCCESS 0
#define EC_FAILURE 1

#define M_ALIGN(x,n) (((x)+(n)-1) & (~((n)-1)))
#define M_MAX(a,b) ((a) > (b) ? (a) : (b))

struct ST_BML_NODE_HEAD;

struct ST_BML_NODE {
	/* Id info */
	mxArray * ps_id;
	char * pc_id;
	bml_size_t sz_id; 

	/* Data info */
	mxArray * ps_data;
	char * pc_data;
	bml_size_t sz_data;
	bml_off_t off_data;

	/* Level */
	//int32_t i_level;

	/* Extension */
	struct {
		mxArray * ps_data;
		char * pc_data;
		bml_size_t sz_data;
	} as_ext[C_BML_MAX_EXT];
	uint8_t i_nb_ext;

	/* parent */
	struct ST_BML_NODE * ps_parent;
#if 0
	/* childs */
	struct ST_BML_NODE * ps_childs_first;
	struct ST_BML_NODE * ps_childs_last;

	/* list item */
	struct ST_BML_NODE * ps_next;
	struct ST_BML_NODE * ps_previous;
#endif
	TAILQ_HEAD(ST_BML_NODE_HEAD, ST_BML_NODE) s_head;
	TAILQ_ENTRY(ST_BML_NODE) s_item;
};



struct ST_BML_HANDLE {
	FILE * fd;
	int i_socket;

	/** WRITER **/
	struct bml_writer s_writer;
	char i_align;
	char b_writer_initialized;

	/** PARSER */
	struct bml_parser s_parser;
	char b_reader_initialized;


	/* current result */
	struct ST_BML_NODE * ps_current;

	/* Current top node */
	struct ST_BML_NODE s_result;

	/* Level */
	int32_t i_level;

	/* Temporary mxarray pointers */
	mxArray * ps_data;
	mxArray * ps_id;
	struct {
		mxArray * ps_data;
	} as_ext[C_BML_MAX_EXT];
};


#endif /* BML_HANDLE_H_ */
