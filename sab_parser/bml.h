/*
 * bml_node.hh
 *
 * Copyright (c) 2013, Johann Baudy. All rights reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301 USA
 */

#ifndef BML_H_
#define BML_H_

#include <stdint.h>

//#define EC_BML_BYPASS -3
#define EC_BML_NODATA -2
#define EC_BML_FAILURE -1
#define EC_BML_SUCCESS 0

#define C_BML_MAX_EXT 64
#define C_BML_MAX_EXT_MIXED 15

/** Types */

typedef uint64_t bml_size_t;
typedef uint64_t bml_off_t;

struct bml_parser;

/*!
 * BML extension structure.
 */
struct bml_ext {
	/*! Id of extension (max 256) */
	uint8_t i_id;
	/*! Pointer of extension data */
	void * pv_data;
	/*! Size of extension data */
	bml_size_t sz_data;
	/*! Offset in parser of extension data */
	bml_off_t off_data;
};

/*!
 * Start BML node callback type.
 *
 * Callback called on new BML node.
 * Cursor of parser must moved to this new node in order to understand further events (start, end, ...)
 *
 * @param in_ps_ctx pointer to BML parser structure
 * @param in_pv_user_arg optional arguments of callbacks
 * @param in_pv_id pointer to BML ID
 * @param in_sz_id size of BML ID
 * @param in_off_id offset in parser of BML ID
 * @param in_pv_data pointer to data buffer (Can be NULL if cb_new_data == NULL)
 * @param in_sz_data size of data buffer
 * @param in_off_data offset in parser of data buffer
 * @param in_as_ext array of extensionsion structure.
 * @param in_i_nb_ext number of extension in array
 * @param in_i_level level of current node
 *
  * @return                EC_BML_SUCCESS on success
  *                        EC_BML_FAILURE on failure
 */
typedef void (*bml_cb_start_element)(struct bml_parser * in_ps_ctx,
		void *in_pv_user_arg, void * in_pv_id, bml_size_t in_sz_id,
		bml_off_t in_off_id, void * in_pv_data, bml_size_t in_sz_data,
		bml_off_t in_off_data, struct bml_ext * in_as_ext, int32_t in_i_nb_ext,
		int32_t in_i_level);

/*!
 * End BML node callback type.
 *
 * Callback called on end of BML node.
 * Cursor of parser must moved to parent node in order to understand further events (start, end, ...)
 *
 * @param in_ps_ctx pointer to BML parser structure
 * @param in_pv_user_arg optional arguments of callbacks
 *
 * @return                EC_BML_SUCCESS on success
 *                        EC_BML_FAILURE on failure
 */
typedef void (*bml_cb_end_element)(struct bml_parser * in_ps_ctx,
		void *in_pv_user_arg);


/*!
 * IO Read Callback type.
 *
 * This callback is used to let user define how the raw BML buffer is read.
 * User must copy data to memory area defined with in_pc_data and in_sz_data_max
 *
 * The flag in_b_first_header let user know if this function can return properly when user have no more data with error code EC_BML_NODATA.
 * This feature allow parser to exit properly at the end of a root node.
 * All other failure must return EC_BML_FAILURE.
 *
 *
 * @param in_ps_ctx pointer to BML parser structure
 * @param in_pv_user_arg optional arguments of callbacks
 * @param in_pc_data pointer of buffer where user must copy data
 * @param in_sz_data_max size of buffer where user must copy data
 * @param in_b_first_header first header flag
 *
 * @return                EC_BML_SUCCESS on success
 *                        EC_BML_FAILURE on failure
 *                        EC_BML_NODATA in case of no data (if in_b_first_header is true only)
 */
typedef int (*bml_cb_io_read)(struct bml_parser * in_ps_ctx,
		void *in_pv_user_arg, char * in_pc_data, bml_size_t in_sz_data_max, char in_b_first_header);

/*!
 * IO Seek Callback type.
 *
 * This callback is used to let user define how the raw BML buffer is seek.
 * User must read or jump over in_sz_offset bytes.
 *
 * This callback is used when cb_new_data is not defined.
 * Parser will then only provide user with offset information of data.
 *
 * @param in_ps_ctx pointer to BML parser structure
 * @param in_pv_user_arg optional arguments of callbacks
 * @param in_sz_offset number of bytes to skip
 *
 * @return                EC_BML_SUCCESS on success
 *                        EC_BML_FAILURE on failure
 */
typedef int (*bml_cb_io_seek)(struct bml_parser * in_ps_ctx,
		void *in_pv_user_arg, bml_size_t in_sz_offset);

/*!
 * Memory allocation of Id callback type.
 *
 * User must fill in_ppc_id with a pointer that parser can use to copy ID into.
 * Parser will use in_sz_offset bytes from the provided pointer (*in_ppc_id).
 *
 * @param in_ps_ctx pointer to BML parser structure
 * @param in_pv_user_arg optional arguments of callbacks
 * @param in_ppc_id pointer of pointer where Id will be copied
 * @param in_sz_offset number of bytes to allocate for Id
 *
 * @return                EC_BML_SUCCESS on success
 *                        EC_BML_FAILURE on failure
 */
typedef int (*bml_cb_new_id)(struct bml_parser * in_ps_ctx,
		void *in_pv_user_arg, char ** in_ppc_id, bml_size_t in_sz_offset);

/*!
 * Memory allocation of Data callback type.
 *
 * User must fill in_ppc_data with a pointer that parser can use to copy data into.
 * Parser will use in_sz_offset bytes from the provided pointer (*in_ppc_id).
 *
 * @param in_ps_ctx pointer to BML parser structure
 * @param in_pv_user_arg optional arguments of callbacks
 * @param in_ppc_data pointer of pointer where data will be copied
 * @param in_sz_offset number of bytes to allocate for data
 *
 * @return                EC_BML_SUCCESS on success
 *                        EC_BML_FAILURE on failure
 */
typedef int (*bml_cb_new_data)(struct bml_parser * in_ps_ctx,
		void *in_pv_user_arg, char ** in_ppc_data, bml_size_t in_sz_offset);

/*!
 * Memory allocation of extension data callback type.
 *
 * User must fill in_ppc_data with a pointer that parser can use to copy extension data into.
 * Parser will use in_sz_offset bytes from the provided pointer (*in_ppc_id).
 *
 * @param in_ps_ctx pointer to BML parser structure
 * @param in_pv_user_arg optional arguments of callbacks
 * @param in_ppc_ext pointer of pointer where extension data will be copied
 * @param in_sz_offset number of bytes to allocate for extension data
 * @param in_i_ext_id id of current extension
 *
 * @return                EC_BML_SUCCESS on success
 *                        EC_BML_FAILURE on failure
 */
typedef int (*bml_cb_new_ext)(struct bml_parser * in_ps_ctx,
		void *in_pv_user_arg, char ** in_ppc_ext, bml_size_t in_sz_offset, uint8_t in_i_ext_id);

struct bml_parser_callbacks {
	/*! Start BML node callback. Means new child of current */
	bml_cb_start_element cb_start_element;
	/*! End BML node callback. Means that we reach end of childs of current node. Moving to parent. */
	bml_cb_end_element cb_end_element;

	/*! Read data callback. */
	bml_cb_io_read cb_io_read;
	/*! Seek inside data callback.
	  Can be NULL if cb_io_read is cb_new_data is not null.
		Parser will jump of bml content, extracting offset for further direct access.  */
	bml_cb_io_seek cb_io_seek;

	/*! Memory allocation of Id callback.
	  Callback callback prior to Id read (through bml_cb_io_read) in order to let user control where data will be copied.
		Reducing number number of memcopy */
	bml_cb_new_id cb_new_id;
	/*! Memory allocation of data callback
	  Callback callback prior to data read (through bml_cb_io_read) in order to let user control where data will be copied.
		Reducing number number of memcopy.
		Can be NULL, if user do not want to copy data during BML parse. */
	bml_cb_new_data cb_new_data;
	/*! Memory allocation of extension data callback
	  Callback callback prior to an extension read (through bml_cb_io_read) in order to let user control where data will be copied.
		Reducing number number of memcopy */
	bml_cb_new_ext cb_new_ext;
};

struct bml_parser {
	/*! User argument */
	void * pv_user_arg;

	/*! IO parser callbacks */
	struct bml_parser_callbacks s_cb;

	/*! Current level */
	int i_level;

	/*! Stop on next node */
	char b_stop;

	/*! Current offset */
	bml_off_t i_offset;
};

/* User argument */
//void bml_set_user_arg(struct bml_parser * in_ps_parser, void * in_pv_arg);
//void * bml_get_user_arg(struct bml_parser * in_ps_parser);

#ifdef __cplusplus
extern "C" {
#endif
/*!
 * Initialize BML reader structure.
 *
 * IO access (read of data) is made through an IO reader callbacks.
 * @param  in_ps_parser   pointer to BML parser structure
 * @param  in_ps_cb       IO reader callbacks structure
 * @param  in_pv_user_arg optional arguments of callbacks

 * @return                EC_BML_SUCCESS on success
 *                        EC_BML_FAILURE on failure
 */
int bml_parser_init(struct bml_parser * in_ps_parser,
		struct bml_parser_callbacks * in_ps_cb, void * in_pv_user_arg);

/*!
 * Run BML parser.
 *
 * The function will run until a BML stop is called.
 * Run means parsing buffer for new BML node and executing callbacks
 * according to data found in the buffer.
 *
 * @param  in_ps_parser pointer to BML parser structure
 *
 * @return                EC_BML_SUCCESS on success
 *                        EC_BML_FAILURE on failure
 */
int bml_parser_run(struct bml_parser * in_ps_parser);

/*!
 * Stop BML Parser.
 *
 * Ex: Stop can be called during cb_end_element callback in order to stop parsing when root node is closed to prevent from further reading.
 *
 * @param  in_ps_parser pointer to BML parser structure
 *
 * @return                EC_BML_SUCCESS on success
 *                        EC_BML_FAILURE on failure
 */
int bml_parser_stop(struct bml_parser * in_ps_parser);
#ifdef __cplusplus
}
#endif
/************ WRITER *************/

struct bml_writer;

/*!
 * Write data callback type.
 *
 * @param  in_ps_writer pointer to BML writer structure
 * @param  in_pv_user_arg optional argument of callbacks provided during bml_writer_init()
 * @param  in_pc_data pointer of char array to write
 * @param  in_sz_data size of char array to write
 *
 * @return                EC_BML_SUCCESS on success
 *                        EC_BML_FAILURE on failure
 */
typedef int (*bml_cb_io_write)(struct bml_writer * in_ps_writer,
		void *in_pv_user_arg, char * in_pc_data, bml_size_t in_sz_data);
/*!
 * Callbacks structure used for BML writer.
 */
struct bml_writer_callbacks {
	/*! Write data callback */
	bml_cb_io_write cb_io_write;
};

struct bml_writer {
	/* User argument */
	void * pv_user_arg;

	/* Current offset */
	bml_off_t i_offset;

	/* Ios */
	struct bml_writer_callbacks s_cb;
};
#ifdef __cplusplus
extern "C" {
#endif
/*!
 * Initialize BML writer Structure.
 *
 * This function must me call after bml_writer structure allocation.
 * Before any bml_writer_element_start, bml_writer_element_end,
 * bml_writer_align and bml_writer_prealign calls.
 *
 * IO access (write of data) is made through an IO writer callbacks.
 *
 * @param  in_ps_writer   pointer to BML writer structure
 * @param  in_ps_cb       IO writer callbacks structure
 * @param  in_pv_user_arg optional argument of callbacks
 * @return                EC_BML_SUCCESS on success
 *                        EC_BML_FAILURE on failure
 */
int bml_writer_init(struct bml_writer * in_ps_writer,
		struct bml_writer_callbacks * in_ps_cb, void * in_pv_user_arg);

/*!
 * Add a BML node.
 *
 * To create a node, user have to provide:
 * 	- an ID (char array + size)
 * 	- a Data (char array + size)
 * 	- a list of extension (optional)
 * 		each extension contains a char array + size + id (8 bit)
 *
 * Calling bml_writer_element_start() just after a bml_writer_element_start() create a child node.
 * Calling bml_writer_element_start() just after a bml_writer_element_end() create a node on same layer (same parent).
 *
 * @param  in_ps_writer              pointer to BML writer structure
 * @param  in_pv_id                  pointer to ID data
 * @param  in_sz_id                  size of ID data (number of bytes)
 * @param  in_pv_data                pointer to DATA
 * @param  in_sz_data                size of data (number of bytes)
 * @param  in_as_ext                 array of extension
 * @param  in_i_nb_ext               number of extension in the array of extension
 * @param  in_i_length_id_size       id field size (optional, default 0)
 * @param  in_i_length_size_size     size field size (optional, default 0)
 * @param  in_i_length_ext_size_size extension size field size (optional, default 0)

 * @return                EC_BML_SUCCESS in case of success
 *                        EC_BML_FAILURE in case of failure
 */
int bml_writer_element_start(struct bml_writer * in_ps_writer, void * in_pv_id,
		bml_size_t in_sz_id, void * in_pv_data, bml_size_t in_sz_data,
		struct bml_ext * in_as_ext, uint8_t in_i_nb_ext,
		bml_size_t in_i_length_id_size, bml_size_t in_i_length_siz_size, bml_size_t in_i_length_ext_size_size);
/*!
 * Close a BML node.
 *
 *
 * Calling bml_writer_element_end() close current node and move cursor to parent node, if it exists.
 *
 * @param  in_ps_writer pointer to BML writer structure
 * @return                EC_BML_SUCCESS in case of success
 *                        EC_BML_FAILURE in case of failure
 */
int bml_writer_element_end(struct bml_writer * in_ps_writer);

/*!
 * Insert invalid/ignored node in order to align content on N bytes.
 *
 * This function can be called at any time. It adds a specific BML header
 * that is ignored during parse.
 *
 * @param  in_ps_writer   pointer to BML writer structure
 * @param  in_i_alignment number of bytes to align
 * @return                EC_BML_SUCCESS in case of success
 *                        EC_BML_FAILURE in case of failure
 */
int bml_writer_align(struct bml_writer * in_ps_writer,
		bml_size_t in_i_alignment);

/*!
 * Insert invalid/ignored node in order to align end of node on N bytes.
 *
 * This function can be called at any time. It adds a specific BML header
 * that is ignored during parse.
 *
 * @param  in_ps_writer   pointer to BML writer structure
 * @param  in_i_alignment number of bytes to align
 * @param  in_i_offset    offset of end of node (default. 1)
 *
 * @return                EC_BML_SUCCESS in case of success
 *                        EC_BML_FAILURE in case of failure
 */
int bml_writer_prealign(struct bml_writer * in_ps_writer, bml_size_t in_i_alignment, bml_size_t in_i_offset);
#ifdef __cplusplus
}
#endif

#endif /* BML_H_ */
