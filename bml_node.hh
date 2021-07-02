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

#ifndef BML_NODE_HH_
#define BML_NODE_HH_




//#define DEBUG_WRITER
//#define DEBUG_PARSER

#ifdef FF_VALGRIND
#include <valgrind/memcheck.h>
#else
#ifndef VALGRIND_CHECK_VALUE_IS_DEFINED
#define VALGRIND_CHECK_VALUE_IS_DEFINED(a)
#endif
#ifndef VALGRIND_CHECK_MEM_IS_DEFINED
#define VALGRIND_CHECK_MEM_IS_DEFINED(a,b)
#endif
#endif


#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <map>
#include <cstring>
#include <vector>
#include <string>
#include <fstream>
#include <array>
#include <list>
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <algorithm>

namespace bml {

#define EC_BML_SUCCESS 0
#define EC_BML_FAILURE -1
#define EC_BML_NODATA -2
#define EC_BML_OVERFLOW -3


#ifndef M_BUG
#define M_BUG(x) abort()
#endif

#ifndef M_ASSERT
#define M_ASSERT(x) assert(x)
#endif

#ifndef DBG_LINE
#define DBG_LINE() printf("%s -- %d\n", __FUNCTION__, __LINE__); fflush(0);
#endif

#ifndef D
#define D(in_str_format, ...) printf(in_str_format "\n", ##__VA_ARGS__)
#endif



# ifndef __off_t_defined
typedef uint64_t off_t;
#endif

template<typename T, template<class > class G> class node;
template<template<class > class G> class node_resource_segment;

#define LF_NODE_DBG 1
/*! Debug list used for tracking and display */
extern std::list<void *> gl_dbg;

/*!
 * Debug function that add a pointer to a debug list.
 * In order to track any change on a node, a segment or a resource.
 *
 * @param in_ptr pointer of a node, a segment or a resource
 */
void f_node_dbg_push(void * in_ptr);

/*!
 * Check if in_ptr is in debug list.
 *
 * @param  in_ptr pointer of a node, a segment or a resource
 * @return        true if in list
 *                     if not in list
 */
bool f_node_dbg_in(void * in_ptr);


extern "C" {
extern void f_debug_print_backtrace(void);
}




/*!
  Node resource class handles data.
  This class is dedicated to data
*/
template<template<class > class G>
class node_resource {
private:

protected:
	/*! Size of resource */
	size_t _sz_data;

public:

  /*!
  *   node_resource class constructor
  */
	node_resource();

  /*!
  *   node_resource class constructor from another resource
  *  @param other : another node_resource
  */
	node_resource(node_resource<G> & in_c_resource);

  /*!
  *   node_resource class constructor from subdata of another resource
  *
  *  @param in_off_data : offset inside parent resource
  *  @param in_sz_data : size of data inside parent resource
  *  @param in_pc_parent_resource : size of data inside parent resource
  */
	node_resource(off_t in_off_data, size_t in_sz_data,
			node_resource<G> * in_pc_parent_resource);

  /*!
  *   node_resource class destructor
  */
	virtual ~node_resource();

  /*!
  *   Operator that check difference with another node_resource
  *
  *  Does current node_resource differ from another node_resource.
  *  Difference is made with node_resource_segment rules.
  *
  *  @param other : other node_resource
  *  @return true if current and other node_resource are identical,
  *  false if not identical
  */
	bool operator !=(node_resource<G> const & other) const;

	/* segment methods */

	/* mmap methods */
 /*!
  *   Memory mapping of node_resource
  *
  *  Subset of current node can be mapped using params.

  *  @param in_off_data : offset inside current resource
  *  @param in_sz_data : size of data inside parent resource
  *  @return pointer that can be used for direct memory access
  */
	virtual char * mmap(off_t in_off_data = 0, size_t in_sz_data = 0) const;

	/* memcpy function */
  /*!
  *   Memory copy from current node resource (offset, size) to an external buffer (pointer, size)
  *
  *  @param in_pc_buffer : pointer to buffer to copy data to
  *  @param in_sz_buffer : size of data to copy
  *  @param in_off_data : offset inside current node resource
  */
	virtual void memcpy_to_buffer(char const * in_pc_buffer,
			size_t in_sz_buffer, off_t in_off_data = 0) const;

  /*!
  *   Memory copy from an external buffer (pointer, size) to current node resource (offset, size)
  *
  *  @param in_pc_buffer : pointer to buffer to copy data to
  *  @param in_sz_buffer : size of data to copy
  *  @param in_off_data : offset inside current node resource
  */
	virtual void memcpy_from_buffer(char const * in_pc_buffer,
			size_t in_sz_buffer, off_t in_off_data = 0);

  /*!
  *   Memory copy from an external node_resource segment to current node resource (offset, size)
  *
  *  @param in_c_seg : external node resource segment
  *  @param in_sz_buffer : size of data to copy
  *  @param in_off_data : offset inside current node resource
  */
	virtual void memcpy_to_segment(node_resource_segment<G> &in_c_seg,
			size_t in_sz_buffer, off_t in_off_data = 0) const;

  /*!
  *   Memory copy from current node resource (offset, size) to an external node_resource segment
  *
  *  @param in_c_seg : external node resource segment
  *  @param in_sz_buffer : size of data to copy
  *  @param in_off_data : offset inside current node resource
  */
	virtual void memcpy_from_segment(node_resource_segment<G> const &in_c_seg,
			size_t in_sz_buffer, off_t in_off_data = 0);

	 /*!
  *   Memory set of current node resource on specific subset (offset + size)
  *
  *  @param in_i_value : value to set inside
  *  @param in_sz_buffer : size of data to memset
  *  @param in_off_data : offset inside current node resource
  */
	virtual void memset_from_value(char in_i_value, size_t in_sz_buffer,
			off_t in_off_data);

	 /*!
  *   Memory resize of current node resource
  *  @param in_sz_data : size of data to allocate
  */
	virtual void resize(size_t in_sz_data);

	 /*!
  *   Get size of current node resource
  *  @return number of bytes allocated
  */
  	size_t size() const {
		return _sz_data;
	}
};

/*!
  Node resource segment.
  This class decribe a segment of a node resource. Thus a data segment.
*/
template<template<class > class G>
struct node_resource_segment {

	/*! Offset of data inside node resource */
	off_t _off_data;

	/*! Smart pointer to node resource */
	G<node_resource<G> > _pc_resource;

	/*! Size of data inside node resource */
	size_t _sz_data;

  /*!
  *   node_resource_segment class constructor from a subset of another node resource segment
  *
  *  @param in_off_data : offset inside parent resource segment
  *  @param in_sz_data : size of data inside parent resource segment
  *  @param in_pc_seg : smart pointer to other resource segment
  */
	node_resource_segment(node_resource_segment<G> const & in_pc_seg,
			off_t in_off_data, size_t in_sz_data) {
		//D("%p",this);
		_pc_resource = NULL;
		_sz_data = 0;
		_off_data = 0;
		set_from_segment(in_pc_seg, in_off_data, in_sz_data);
	}

 /*!
  *   node_resource_segment class constructor from another node resource segment
  *  @param other : smart pointer to other resource segment
  */
	node_resource_segment(node_resource_segment<G> const &other) {
		//D("%p",this);
		_pc_resource = NULL;
		_sz_data = 0;
		_off_data = 0;
		*this = other;
	}

 /*!
  *   node_resource_segment class constructor from a whole node resource
  *
  *  @param other : smart pointer to other resource segment
  */
	node_resource_segment(G<node_resource<G> > const &other) {
		//D("%p",this);
		_pc_resource = other;
		_sz_data = other->size();
		_off_data = 0;
	}

  /*!
  *   node_resource_segment class constructor of an empty segment
  */
	node_resource_segment() {
		//D("%p",this);
		_pc_resource = NULL;
		_sz_data = 0;
		_off_data = 0;
	}

  /*!
  *   node_resource_segment class destructor
  */
	~node_resource_segment() {
		clear();
	}

  /*!
  *   Create a memory maping of current node resource segment.
  *  Node resource may not support mmap and then return a NULL pointer.
  *  @return pointer to memory maping
  */
	char * mmap(void) const {
		M_ASSERT(_pc_resource);
		return _pc_resource->mmap(_off_data, _sz_data);
	}


  /*!
  *   Clearing any reference to a resource
  */
	void clear(void) {
		if (_pc_resource) {
			_pc_resource = NULL;
		}
	}

  /*!
  *   Operator = NULL means clearing
  */
	node_resource_segment<G> & operator=(std::nullptr_t) {
		clear();
		return *this;
	}


  /*!
  *   Link segment to a node resource
  *  @params in_ps_resource smart pointer of node resource
  */
	void set_from_resource(G<node_resource<G> > in_pc_resource) {
		M_ASSERT(in_pc_resource);

		/* Release old resource */
		clear();

		_off_data = 0;
		_sz_data = in_pc_resource->size();
		_pc_resource = in_pc_resource;
	}


  /*!
  *   Link segment to a subset of a node resource segment

  *  @param in_off_data : offset inside external resource segment
  *  @param in_sz_data : size of data external provided resource segment
  *  @param in_pc_seg : smart pointer to external resource segment
  */
	void set_from_segment(node_resource_segment<G> const & in_pc_seg,
			off_t in_off_data, size_t in_sz_data) {
		M_ASSERT(in_sz_data+in_off_data <= in_pc_seg._sz_data);
		//DBG_LINE();
		/* Release old resource */
		clear();

		if (!in_sz_data) {
			in_sz_data = in_pc_seg._sz_data - in_off_data;
		}
		//DBG_LINE();
		/* Configure segment */
		_pc_resource = in_pc_seg._pc_resource;
		_off_data = in_pc_seg._off_data + in_off_data;
		_sz_data = in_sz_data;
	}

  /*!
  *   Get a node resource segment from offset and size of current node resource segment

  *  @param in_off_data : offset inside current resource segment
  *  @param in_sz_data : size of data inside current resource segment
  */
	node_resource_segment<G> get_segment(off_t in_off_data,
			size_t in_sz_data) const {
		return node_resource_segment<G>(*this, in_off_data, in_sz_data);
	}

  /*!
  *   Memory copy from memory buffer (pointer + size) to current segment
  *
  *  @param in_pc_buffer : pointer to buffer
  *  @param in_sz_buffer : size of buffer
  */
	void memcpy_from_buffer(char const * in_pc_buffer, size_t in_sz_buffer) {
		M_ASSERT(_sz_data == in_sz_buffer);
		M_ASSERT(_pc_resource);
		_pc_resource->memcpy_from_buffer(in_pc_buffer, in_sz_buffer, _off_data);
	}

  /*!
  *   Memory copy from current segment to memory buffer (pointer + size)
  *
  *  @param in_pc_buffer : pointer to buffer
  *  @param in_sz_buffer : size of buffer
  */
	void memcpy_to_buffer(char const * in_pc_buffer,
			size_t in_sz_buffer) const {
		M_ASSERT(_pc_resource);
		M_ASSERT(_sz_data == in_sz_buffer);
		_pc_resource->memcpy_to_buffer(in_pc_buffer, in_sz_buffer, _off_data);
	}

  /*!
  *   Memory copy from an external node resource segment to current segment
  *
  *  @param in_c_segment : external node resource segment
  */
	void memcpy_from_segment(node_resource_segment<G> const & in_c_segment) {
		M_ASSERT(_sz_data == in_c_segment.size());
		M_ASSERT(_pc_resource);
		//in_c_segment.memcpy_to_segment(*this);
		_pc_resource->memcpy_from_segment(in_c_segment, in_c_segment.size(),
				_off_data);
	}

  /*!
  *   Memory copy from current segment to an external node resource segment
  *
  *  @param in_c_segment : external node resource segment
  */
	void memcpy_to_segment(node_resource_segment<G> & in_c_segment) const {
		M_ASSERT(_sz_data == in_c_segment.size());
		M_ASSERT(_pc_resource);
		_pc_resource->memcpy_to_segment(in_c_segment, _sz_data, _off_data);
	}

  /*!
  *   Memory set of segment.
  *
  *  @param in_i_value Value to set inside memory
  */
	void memset_from_value(char in_i_value) const {
		_pc_resource->memset_from_value(in_i_value, _sz_data, _off_data);
	}

  /*!
  *   Return emptiness of segment.
  *
  *  @return true if segment is empty
  *  false if not empty
  */
	bool isempty(void) const {
		return _pc_resource == NULL;
	}

  /*!
  *   Check if segment is a root segment.
  *
  *  @return true if segment is a root segment
  *  false if not a root segment or empty
  */
	bool isglobal(void) const {
		return (_pc_resource != NULL) && (_sz_data == (_pc_resource->size()))
				&& (_off_data == 0);
	}
 /*!
  *   Get size of segment.
  *  @return size of segment (number of bytes)
  */
	size_t size(void) const {
		return _pc_resource ? _sz_data : 0;
	}


 /*!
  *   Get pointer of resource.
  *  @return pointer of resource
  */
	node_resource<G> * resource(void) const {
		return _pc_resource.get();
	}

  /*!
  *   Get offset of segment.
  *  @return offset of current segment inside node resource
  */
	off_t offset(void) const {
		return _off_data;
	}

/*!
  *   Resize segment (Root segment ONLY).
  *  @param in_i_size new size to allocate inside segment
  */
	void resize(size_t in_i_size) {
		M_ASSERT(_pc_resource);
		M_ASSERT(isglobal());
		_pc_resource->resize(in_i_size);
		_sz_data = in_i_size;
	}

  /*!
  *   Map segment to a type T
  *  @return T reference filled with segment data
  */
	template<typename T> T & map(void) const {
		M_ASSERT(_pc_resource);
		M_ASSERT(_sz_data == sizeof(T));
		return *((T*)mmap());
	}

  /*!
  *   Get copy of segment as an object of type T
  *  @return T object filled with segment data
  */
	template<typename T> T get_data(void) const {
		T c_tmp;
		M_ASSERT(_pc_resource);
		M_ASSERT(_sz_data == sizeof(T));
		char const * pc_tmp = (char const *) &c_tmp;
		memcpy_to_buffer(pc_tmp, _sz_data);
		return c_tmp;
	}

  /*!
  *   Set segment from an object of type T
  *  @param in T object to copy data from
  */
	template<typename T>
	void set_data(T const & in) {
		M_ASSERT(_pc_resource);
		M_ASSERT(_sz_data == sizeof(T));
		memcpy_from_buffer((char const *) &in, _sz_data);
	}

  /*!
  *   Operator = to another node resource segment.
  *  Link segment as other node resource segment
  *  @param other other node resource segment
  */
	node_resource_segment<G> & operator =(
			node_resource_segment<G> const & other) {
		if (this == &other)
			return *this;

//		if (other._pc_resource) {
			_pc_resource = other._pc_resource;
			_sz_data = other._sz_data;
			_off_data = other._off_data;
//		}
		return *this;
	}

 /*!
  *   Operator != to another node resource segment.
  *  Execute a memory compare between current and another node resource segment
  *  @param other other node resource segment
  */
	bool operator !=(node_resource_segment<G> const & other) const {
		int ec;
		void * pv_data_1;
		void * pv_data_2;

		if (_sz_data != other._sz_data) {
			return false;
		}

		pv_data_1 = mmap();
		pv_data_2 = other.mmap();

		M_ASSERT(pv_data_1);
		M_ASSERT(pv_data_2);

		ec = memcmp(pv_data_1, pv_data_2, _sz_data);

		//unmmap();
		//other.unmmap();

		return ec ? false : true;
	}

};


 /*!
  *  Malloc node resource.
  * Node resource using Malloc/Free
  */
template<template<class > class G>
class node_alloc_resource: public node_resource<G> {
protected:
 /*!
  *  Data Pointer
  */
	void * _pv_data;

 /*!
  *  Data size
  */
  using node_resource<G>::_sz_data;

public:
 /*!
  *   Constructor of malloc node resource.
  *  @param in_sz_data size of data to be malloced
  */
	node_alloc_resource(size_t in_sz_data);
 /*!
  *   Destructor of malloc node resource.
  */
	virtual ~node_alloc_resource();

 /*!
  *   Resize of malloced data
  *  @param in_sz_data new size of resource data
  */
	void resize(size_t in_sz_data);

	/*!
  *   Memory mapping of node resource
  * Virtual method of memory mapping for malloced buffer
  *  @param in_off_data : offset inside node resource
  *  @param in_sz_data : size of data after offset
  *  @return pointer of buffer mmaped into memory
  */
	char * mmap(off_t in_off_data = 0, size_t in_sz_data = 0) const;
};

 /*!
  *  Node resource using static buffer
  */
template<template<class > class G>
class node_buffer_resource: public node_resource<G> {
protected:
 /*!
  *  Data Pointer
  */
	void * _pv_data;
 /*!
  *  Data size
  */
	using node_resource<G>::_sz_data;
public:
 /*!
  *   Constructor of static node resource.
  *  @param in_pc_buffer pointer of static buffer
  *  @param in_sz_buffer size of buffer provided
  */
	node_buffer_resource(void * in_pc_buffer, size_t in_sz_buffer);
 /*!
  *   Destructor of malloc node resource.
  */
	virtual ~node_buffer_resource();

		/*!
  *   Memory mapping of node resource
  * Virtual method of memory mapping for static buffer
  *  @param in_off_data : offset inside node resource
  *  @param in_sz_data : size of data after offset
  *  @return pointer of buffer mmaped into memory
  */
	char * mmap(off_t in_off_data = 0, size_t in_sz_data = 0) const;
};

 /*!
  * Node Writer base class.
  * Base Class used to write a BML node into a container (file, buffer, ...)
  */
template<template<class > class G>
class node_writer {
public:
 /*!
  *   Constructor of node writer.
  */
	node_writer();
 /*!
  *   Destructor of node writer.
  */
	virtual ~node_writer();
  /*!
  *   Write char buffer array into container.
  *  Virtual method used to write a constant buffer into memory. Mainly used for BML headers.
  *  @param in_pc_buffer : Pointer to external buffer
  *  @param in_sz_data : size of external buffer
  *  @return EC_BML_SUCCESS on success
  *  EC_BML_FAILURE on failure
  */
	virtual int write_data(char* in_pc_buffer, size_t in_sz_buffer)
			__attribute__ ((warn_unused_result)) = 0;

  /*!
  *   Write node resource segment into container.
  *  Virtual method used to write a bml node segment into memory. Mainly used for BML data content.
  *  By default, this methode use write_data(char* in_pc_buffer, size_t in_sz_buffer).
  *  @param in_pc_buffer : Pointer to external buffer
  *  @param in_sz_data : size of external buffer
  *  @return EC_BML_SUCCESS on success
  *  EC_BML_FAILURE on failure
  */
	virtual int write_resource(node_resource_segment<G> & in_c_resource)
			__attribute__ ((warn_unused_result));
};

 /*!
  * File Node Writer class.
  * Class used to write a BML node into a FILE
  */
template<template<class > class G>
class node_file_writer: public node_writer<G> {
 /*!
  *  Output File descriptor
  */
	std::ofstream _c_os;
 /*!
  *  Internal file offset
  */
	uint64_t _i_offset;
public:
 /*!
  *  Constructor of a file node writer.
  * @param in_str_file Path of file that will be created.
  */
	node_file_writer(std::string const & in_str_file);
 /*!
  *   Destructor of file node writer.
  */
	~node_file_writer();
  /*!
  *   Write char buffer array into FILE.
  *  Write external buffer into a FILE
  *  @param in_pc_buffer : Pointer to external buffer
  *  @param in_sz_data : size of external buffer
  *  @return EC_BML_SUCCESS on success
  *  EC_BML_FAILURE on failure
  */
	int write_data(char* in_pc_buffer, size_t in_sz_buffer);

 /*!
  *  Get current file offset.
  * @return Offset inside file (number of bytes)
  */
	size_t get_offset() {
		return _i_offset;
	}

 /*!
  *  Flush buffer into file.
  */
	void flush() {
		_c_os.flush();
	}


};

 /*!
  * Buffer Node Writer class.
  * Class used to write a BML node into a Raw Buffer
  */
template<template<class > class G>
class node_buffer_writer: public node_writer<G> {
protected:
 /*!
  *  External buffer pointer
  */
	char * _pc_buffer;
 /*!
  *  External buffer maximum size
  */
	uint64_t _sz_buffer;
 /*!
  *  Internal file offset
  */
	uint64_t _i_offset;
public:
 /*!
  *  Constructor of a buffer node writer.
  * @param in_pc_buffer : Pointer to external buffer
  * @param in_sz_data : size of external buffer
  */
	node_buffer_writer(char* in_pc_buffer, size_t in_sz_buffer);
 /*!
  *   Destructor of buffer node writer.
  */
	~node_buffer_writer();
  /*!
  *   Write char buffer array into the buffer container.
  *  Write external buffer into the buffer container
  *  @param in_pc_buffer : Pointer to external buffer
  *  @param in_sz_data : size of external buffer
  *  @return EC_BML_SUCCESS on success
  *  EC_BML_FAILURE on failure
  */
	int write_data(char* in_pc_buffer, size_t in_sz_buffer);
 /*!
  *  Get current file offset.
  * @return Offset inside file (number of bytes)
  */
	int get_offset() {
		return _i_offset;
	}
};

 /*!
  * Segment Node Writer class.
  * Class used to write a BML node into a BML node segment. Used to insert BML content as DATA of a BML node.
  */
template<template<class > class G>
class node_segment_writer: public node_buffer_writer<G> {
	node_resource_segment<G> const * _pc_parent;
public:
 /*!
  *  Constructor of a segment node writer.
  * @param in_s_segment External buffer segment.
  */
	node_segment_writer(node_resource_segment<G> const & in_s_segment);
 /*!
  *   Destructor of segment node writer.
  */
	~node_segment_writer();
};

/*!
* Node Parser base class.
* Base Class used to extract a BML node from a container (file, buffer, ...)
*/
template<template<class > class G>
class node_parser {
public:
 /*!
  *   Constructor of base node parser.
  */
	node_parser();
 /*!
  *   Destructor of base node parser.
  */
	virtual ~node_parser();

	//template<typename T> node<T, G> parse();

  /*!
  *   Read char buffer array from a container.
  *  Virtual method used to fill a char buffer array from a container (file, buffer, ...).
  *  @param in_pc_buffer : Pointer to external buffer
  *  @param in_sz_data : size of external buffer
  *  @return EC_BML_SUCCESS on success
  *  EC_BML_FAILURE on failure
  */
	virtual int parse_data(char* in_pc_buffer, size_t in_sz_buffer);
  /*!
  *   Read node resource segment from a container.
  *  Virtual method used to fill a bml node segment from a container (file, buffer, ...). Mainly used for BML data content.
  *  By default, this methode use parse_data(char* in_pc_buffer, size_t in_sz_buffer).
  *  @param out_c_seg output segment
  *  @param in_sz_buffer number of bytes to parse
  *  @return EC_BML_SUCCESS on success
  *  EC_BML_FAILURE on failure
  */
	virtual int parse_resource(node_resource_segment<G> & out_c_seg,
			size_t in_sz_buffer);
};

/*!
* File Node Parser class.
* Class used to fill a BML node from a file
*/
template<template<class > class G>
class node_file_parser: public node_parser<G> {
	/*!
	 *  Input File descriptor
	 */
	std::ifstream _c_is;
public:
	/*!
	 *  Internal offset type
	 */
	typedef std::streampos offset_t;
	/*!
   *  Constructor of file node parser.
	 * @param in_str_path path of file to open in order to extract bml data from
   */
	node_file_parser(std::string const & in_str_path);
	/*!
	 *   Destructor of file node parser.
	 */
	~node_file_parser();

	/*!
  *   Read char buffer array from a file.
  *  @param in_pc_buffer : Pointer to external buffer
  *  @param in_sz_data : size of external buffer
  *  @return EC_BML_SUCCESS on success
  *  EC_BML_FAILURE on failure
  */
	int parse_data(char* in_pc_buffer, size_t in_sz_buffer);
	/*!
   *  Get current cursor position.
   * @return Offset inside file (number of bytes)
   */
	offset_t get_pos(void);
	/*!
	 *  Update cursor position into file position.
	 * @param in_i_offset Offset inside file (number of bytes)
	 * @return EC_BML_SUCCESS on success
	 *  EC_BML_FAILURE on failure
	 */
	int set_pos(offset_t &in_i_offset);
	/*!
	 *  Get current file size.
	 * @return File of opened file (number of bytes)
	 */
	uint64_t get_size(void);
};

/*!
* Buffer Node Parser class.
* Class used to fill a BML node from a char array buffer
*/
template<template<class > class G>
class node_buffer_parser: public node_parser<G> {
protected:
	/*!
	 * Char array buffer pointer
	 */
	char * _pc_buffer;
	/*!
	 * Maximum size of char array buffer pointer
	 */
	size_t _sz_buffer;
	/*!
	 * Internal offset inside buffer
	 */
	uint64_t _i_offset;
public:
	/*!
	 *  Constructor of a buffer node parser.
	 * @param in_pc_buffer : Pointer to external buffer
	 * @param in_sz_data : size of external buffer
	 */
	node_buffer_parser(char * in_pc_buffer, size_t in_sz_buffer);
	/*!
	 *  Destructor of a buffer node writer.
	 */
	~node_buffer_parser();
	/*!
  *   Read char buffer array from a buffer container.
  *  @param in_pc_buffer : Pointer to external buffer
  *  @param in_sz_data : maximum size of external buffer
  *  @return EC_BML_SUCCESS on success
  *  EC_BML_FAILURE on failure
  */
	int parse_data(char* in_pc_buffer, size_t in_sz_buffer);
	/*!
	 *  Get current cursor position.
	 * @return Offset inside file (number of bytes)
	 */
	uint64_t get_offset(void) {
		return _i_offset;
	}
	/*!
	 *  Get current buffer size.
	 * @return Offset inside file (number of bytes)
	 */
	uint64_t get_size(void) {
		return _sz_buffer;
	}
};


/*!
* Segment node parser class.
* Class used to fill a BML node from a bml node segment.
* Used to create BML node from BML data content.
*/
template<template<class > class G>
class node_segment_parser: public node_buffer_parser<G> {
	/*!
	 * Pointer of node resource segment to write data into
	 */
	node_resource_segment<G> const * _pc_parent;

  /* Using private element of base class */
	using node_buffer_parser<G>::_i_offset;
	using node_buffer_parser<G>::_sz_buffer;
	using node_buffer_parser<G>::_pc_buffer;
public:
	/*!
	 *  Constructor of a node segment parser.
	 * @param in_s_segment External BML segment.
	 */
	node_segment_parser(node_resource_segment<G> const & in_s_segment);
	/*!
	 *  Destructor of a node segment parser.
	 */
	~node_segment_parser();

	/*!
  *   Read node resource segment from a node segment.
  *  Parser is not copying data from segment. It only links data of parsed segment into new nodes.
  *  @param out_c_seg output segment
  *  @param in_sz_buffer number of bytes to parse
  *  @return EC_BML_SUCCESS on success
  *  EC_BML_FAILURE on failure
  */
	int parse_resource(node_resource_segment<G> & out_c_seg,
			size_t in_sz_buffer);
};

/*!
* Is container identification class.
*/

template<typename ...>
using to_void = void; // maps everything to void, used in non-evaluated contexts

template<typename T, typename = void>
struct is_container : std::false_type
{};
template<typename T>
struct is_container<T,
                    to_void<decltype(std::declval<T>().begin()),
                            decltype(std::declval<T>().end()),
                            typename T::value_type
                            >> : std::true_type // will  be enabled for iterable objects
{};
template<typename T, typename = void>
struct is_map : std::false_type
{};
template<typename T>
struct is_map<T,
              to_void<typename T::key_type
                      >> : std::true_type // will  be enabled for iterable objects
{};
template<typename T, typename = void>
struct is_contiguous : std::false_type
{};
template<typename T>
struct is_contiguous<T,
                     to_void<decltype(std::declval<T>().data())
                             >> : std::true_type // will  be enabled for iterable objects
{};

/*!
* BML Node class.
* Class used to manipulate BML nodes:
*  - Add/Remove Childs
*  - Set/Get Data
*  - Set/Get Extensions
*  - Read from a container (parser)
*  - Write into a container (writer).
*
* Data of nodes are stored into node_resource described with node_resource_segment
* Childs are identified with a key of type T (template) and a position. (eg. multimap)
*
* @param T type of node id used as key
* @param G type of smart pointer
*/
template<typename T, template<class > class G> class node /* :public node_internal*/{
	/*!
	 * Field size limits
	 */
	enum ET_NODE_SIZE {
		E_NODE_SIZE_EMPTY = 0, E_NODE_SIZE_MAX = 8, E_NODE_SIZE_INVALID = 0xF,
	};
	/*!
	 * Field node id size limits
	 */
	enum ET_NODE_ID_SIZE {
		E_NODE_ID_SIZE_NULL = 0, E_NODE_ID_SIZE_INVALID = 7,
	};

protected:
	/*! Resource segment that store content of node (data) */
	node_resource_segment<G> _s_segment;

	/*! Childs multimap. Store smart pointer to childs node. Key of  */
	std::multimap<T, G<node<T, G>>> _m_childs;

	/*! Id of current node (used as KEY) */
	T _id;

	/*! Invalid flag. Used only during with parser if a node is complete or invalid */
	bool _b_is_invalid;

	/*! Extensions multimap. A node can store multiple node segment that we can access through a uchar key */
	std::map<uint8_t, node_resource_segment<G>> _m_ext_values;

public:
	/*! Childs iterator type. Used inside multimap of childs */
	typedef typename std::multimap<T, G<node<T,G>>>::iterator it;
	/*! Childs iterator type (const). Used inside multimap of childs */
	typedef typename std::multimap<T, G<node<T,G>>>::const_iterator const_it;

	/*! find() result type. */
	typedef typename std::vector<it> find_result;
	/*! Iterator type of find() result type. */
	typedef typename std::vector<it>::iterator find_iterator;

    /* equal_range() result type. */
    typedef typename std::pair<node<T, G>::it,node<T, G>::it> er_it;

    /* equal_range() result type (const). */
    typedef typename std::pair<node<T, G>::const_it,node<T, G>::const_it> const_er_it;

private:
	/*! memory init function */
	void _init(void);

	/*! operator= (Private assignment) */
	void operator=(const node<T,G>&);

	/*! Private erase methods from a child node reference */
	void erase(node & in_c_node);
	/*! Private erase methods from a child node iterator */
	void erase(node::it & in_pc_it);

	/*!
	 * Write BML id through node_writer.
	 *
	 * @param  in_c_writer node_writer to write id to
	 * @param  in_sz_id    size of id
	 * @return             EC_BML_SUCCESS on success
	 *                     EC_BML_FAILURE on failure
	 */
	int to_writer_id(node_writer<G>& in_c_writer, uint64_t in_sz_id);
	/*!
	 * Read BML id from node_parser.
	 *
	 * @param  in_c_parser node_parser to read id from
	 * @param  in_sz_id    size of id
	 * @return             EC_BML_SUCCESS on success
	 *                     EC_BML_FAILURE on failure
	 */
	int from_parser_id(node_parser<G>& in_c_parser, uint64_t in_sz_id);

	/*!
	 *  Get word size from number.
	 *  Used to specify size of size fields from size value.
	 *  Eg 128 can be contained in 1 byte. 3240 in 2 bytes, ...
	 * @param  in_i_num input number
	 * @return number of bytes needed to store input number
	 */
	uint8_t get_length(uint64_t in_i_num) const;

	/*!
	 * Return Id node size.
	 *
	 * @return  number of bytes needed to store Id
	 */
	int get_id_size(void) const;

	/*!
	 * Invalid node check used in parser.
	 *
	 * @return 			true if invalid.
	 *              false if valid node
	 */
	bool is_invalid();
public:
	/*!
	 *  Constructor of an empty node and default Id.
	 */
	node(void);
	/*!
	 *  Constructor of a node with provided id and data (raw content of data type).
	 * @param in_id Id of node to create
	 * @param in_data Data to insert into created node
	 */
	template<typename T2> node(T const & in_id, T2 const & in_data);
	/*!
	 *  Constructor of an empty node but given Id.
	 * @param in_id Id of node to create
	 */
	node(T const & in_id);

	/*!
	 * Constructor of a node with provided id and data (node resource segment with offset and size).
	 * @param in_off_data Id of node to create
	 * @param in_id Id of node to create
	 */
	node(T const & in_id, node_resource_segment<G> const & in_pc_seg,
			off_t in_off_data, size_t in_sz_data);

	/*! Destructor of node class */
	virtual ~node(void);

  /*!
   * Check if node contains a specific child using Id and indice.
   * @param  in_id       key id.
   * @param  in_i_indice key indice. Useful if node has multiple child with the same id.
   * @return             true if at least one child is found
   *                     false if no child is found
   */
	bool has(T const & in_id, uint64_t in_i_indice = 0);

/*!
 * Get child from Id and indice.
 * @param  in_id       key id.
 * @param  in_i_indice key indice. Useful if node has multiple child with the same id.
 * @return             if child is found, Smart pointer of this node
 *                     if not empty smart pointer
 */
	G<node<T,G>> & get(T const & in_id, uint64_t in_i_indice = 0, bool in_b_create =
			false);

	/*!
	 * Add a new child with Id and Content.
	 * /!\ Node is copied.
	 * @param  in_id       key id.
	 * @param  in_c_node   node to be inserted.
	 * @return             Smart pointer of created node
	 */
	G<node<T,G>> & add_from(T const & in_id, G<node<T,G>> & in_c_node);

	/*!
	 * Add a new empty child with Id.
	 * @param  in_id       key id.
	 * @return             Smart pointer of created node
	 */
	G<node<T,G>> & add(T const & in_id);

	/*!
	 * Search the node for childs with an id equivalent to in_i_id.
	 * @param  in_i_id    key id to be searched for
	 * @return            list of childs found.
	 *
	 * Notice that result is an alias of std::vector<it>
     */
    node<T,G>::find_result find(T const & in_id);
    node<T,G>::er_it equal_range(T const & in_id);
    node<T,G>::const_er_it equal_range(T const & in_id) const;

	/*!
	 * Get all childs of node.
	 * @return            list of childs found.
	 *
	 * Notice that result is an alias of std::vector<it>
	 */
	node<T,G>::find_result childs(void);

    /*!
     * Get first child iterator.
     * @return            begin iterator.
     */
    node<T, G>::it begin(void);
    /*!
     * Get last child iterator.
     * @return            end iterator.
     */
    node<T, G>::it end(void);
    /*!
     * Get first child iterator.
     * @return            begin const iterator.
    */
    node<T, G>::const_it cbegin(void) const;
    /*!
     * Get last child iterator.
     * @return            end const iterator.
     */
    node<T, G>::const_it cend(void) const;

	/*!
	 * Internal event of new resource request.
	 * This method is called when node needs to allocate bytes used for content.
	 * Virtual method in order to allow fast allocation. Default allocation is made with new node_resource<G>()
	 *
	 * @param  in_i_size    Size to be allocated
	 * @return              node resource segment newly allocated.
	 */
	virtual node_resource_segment<G> on_new_resource(size_t in_i_size);

	/*!
	 * Internal event of new node request.
	 * This method is called when node needs to allocate a new node (as child).
	 * Virtual method in order to allow fast allocation. Default allocation is made with new
	 * @return              smart pointer on node class.
	 */
	virtual G<node<T,G>> on_new(void);

	/*!
	 * Internal event of node delete.
	 * This method is called when node needs to delete a node (as child).
	 * Virtual method.
	 * @param  in_pc_child    Size to be allocated
	 */
	virtual void on_delete(G<node<T,G>> in_pc_child);

	/*!
	 * Internal event of node read.
	 * This method is called when node as just been read from parser.
	 * Virtual method. (Debug purpose)
	 */
	virtual void on_read(void);

	/*!
	 * Internal event of node write.
	 * This method is called when node will be write to writer.
	 * Virtual method. (Debug purpose)
	 */
	virtual void on_write(void);

	/*!
	 * Clear everthing the node.
	 * Childs, Data, extentions will be cleared.
	 */
	void clear(void);
	/*!
	 * Make the node empty.
	 * Childs will be cleared.
	 */
	void empty(void);

	/*!
	 * Resize space available for data content of the node.
	 *
	 * @param in_i_size new size to be allocated for content
	 */
	void resize(size_t in_i_size);

	/*!
	 * Operator (), equivalent to get() method.
	 * But with default behavior of node cration enabled.
	 *
	 * @param  in_id       key id.
	 * @param  in_i_indice key indice. Useful if node has multiple child with the same id.
	 */
	node<T,G> & operator()(T const & in_id, uint64_t in_i_indice = 0);

	/*!
	 * Operator !=.
	 * Check that node have same:
	 *  - id
	 *  - data
	 *  - childs
	 *
	 * @param  other       other node to compare to.
	 */
	bool operator !=(node<T,G> const & other) const;

	/*!
	 * Operator <.
	 * Sort operator. It use Operator < of id.
	 * @param  other       other node to compare with current
	 */
	bool operator <(node<T,G> const & other) const;

	/*!
	 * Return data content of node using cast.
	 * Size of T2 must match size of data in order to complete properly.
	 * Underlying bit pattern of the destination type is filled with
	 * node resource segment of node.
	 * @return  copied data of node casted in T2 type.
	 */
	template<typename T2> T2 get_data(void) const;

    template<typename T2, typename std::enable_if<is_container<T2>::value && !is_map<T2>::value && !is_contiguous<T2>::value >::type* = nullptr> T2 get_data(void) const;
    template<typename T2, typename std::enable_if<is_container<T2>::value && !is_map<T2>::value && is_contiguous<T2>::value >::type* = nullptr> T2 get_data(void) const;
    template<typename T2, typename std::enable_if<!is_container<T2>::value >::type* = nullptr> T2 get_data(void) const;

	/*!
	 * Set data content.
	 *
	 * Size size of data is reallocated in order to match T2 size.
	 * Underlying bit pattern of the source type T2 is copied into the
	 * node resource segment of node.
	 *
	 * Old data is cleared.
	 * @param in source of data
     */
    template<typename T2, typename std::enable_if<is_container<T2>::value && !is_map<T2>::value && !is_contiguous<T2>::value >::type* = nullptr> void set_data(T2 const & in);
    template<typename T2, typename std::enable_if<is_container<T2>::value && !is_map<T2>::value && is_contiguous<T2>::value >::type* = nullptr> void set_data(T2 const & in);
    template<typename T2, typename std::enable_if<!is_container<T2>::value >::type* = nullptr> void set_data(T2 const & in);


	/*!
	 * Set data from node resource segment.
	 * Old data is cleared.
	 *
	 * @param in_pc_seg   Input segment
	 * @param in_off_data offset inside segment
	 * @param in_sz_data  size inside segment
	 */
	void set_segment(node_resource_segment<G> const & in_pc_seg,
			off_t in_off_data = 0, size_t in_sz_data = 0);

	/*!
	 * Copy node data content from a memory area.
	 * Node is resized according to in_sz_buffer.
	 *
	 * @param in_pc_buffer [description]
	 * @param in_sz_buffer [description]
	 */
	void memcpy_from_buffer(char const * in_pc_buffer, size_t in_sz_buffer);


	/*!
	 * Copy node data content to a memory area.
	 * Node size must be greater than in_sz_buffer
	 *
	 * @param in_pc_buffer [description]
	 * @param in_sz_buffer [description]
	 */
	void memcpy_to_buffer(char const * in_pc_buffer, size_t in_sz_buffer) const;

	/*!
	 * Check that node can be memory mapped.
	 * @return  true if node can me mmaped
	 *          false if node can't be mmaped
	 */
	bool is_mmapable(void) const;

	/*!
	 * Memory map of node data.
	 * This function will use mmap() feature of node_resource_segment.
	 * So far, no need to unmap, all resources have a permanent pointer
	 * that allow a direct access at anytime.
	 *
	 * As returned pointer is typed (T2). Check that node is big enough to store T2.
	 *
	 * @return  pointer of mmaped area.
	 */
	template<typename T2> T2 * mmap(void) const;

	/*!
	 * Memory map of node data throuh a C++ reference.
	 * As mmap but return a reference of type T2 instead of a pointer.
	 * @return  reference to mmaped area.
	 */
	template<typename T2> T2 & map(void) const;

	/*!
	 * Copy a node with zero copy on data.
	 * This function copy everthing from a node (Except extensions).
	 * On contrary to copy(), this function does not perform a memcopy on data.
	 * It copies only node_resource_segment
	 * @param other other node to copy id, data and childs from
	 */
	virtual void copy_link(node<T,G> const & other);

	/*!
	 * Copy a node with memcpy on data.
	 * This function copy everthing from a node (Except extensions).
	 * On contrary to copy_link(), this function performs a memcopy on data.
	 * It allocates new node_resource and copies data inside.
	 * @param other other node to copy id, data and childs from
	 */
	virtual void copy(node<T,G> const & other);

	/*!
	 * Check if node has a specfic extension with extenion key.
	 *
	 * @param  in_i_id extension key
	 * @return         true if extension is available
	 *                 false if extension is not available
	 */
	bool has_ext(uint8_t in_i_id);
	/*!
	 * Set a node extension.
	 * Each node can store multiple extension through a multimap.
	 * Underlying bit pattern of T2 type is used to fill extension data.
	 * @param in_i_ext Id of extension
	 * @param in       extension data
	 */
	template<typename T2> void set_ext(uint8_t in_i_ext, T2 const & in);

	/*!
	 * Get an extension.
	 * Each node can store multiple extension through a multimap.
	 * Extension data is copied to the underlying bit pattern of T2 type.
	 * @param in_i_ext Id of extension
	 * @return extension data on T2 format
	 */
	template<typename T2> T2 get_ext(uint8_t in_i_ext);

	/*!
	 * Write node into a file using BML Format.
	 * node_file_writer is used to perform file write.
	 *
	 * @param  in_str_file path of file to generate
	 * @return             EC_BML_SUCCESS on success
	 *                     EC_BML_FAILURE on failure
	 */
	int to_file(std::string const & in_str_file);

	/*!
	 * Fill node from a file using BML Format.
	 * node_file_parser is used to perform file parse.
	 *
	 * @param  in_str_file path of file to read data from
	 * @return             EC_BML_SUCCESS on success
	 *                     EC_BML_FAILURE on failure
	 */
	int from_file(std::string const & in_str_file);

	/*!
	 * Write node in BML format through a node_writer.
	 * alignment can be provided to align HeaderBML, Data, HeaderExtension.
	 *
	 * @param  in_c_writer node_writer to write data to
	 * @param  in_i_align  data alignment in bytes.
	 * @return             EC_BML_SUCCESS on success
	 *                     EC_BML_FAILURE on failure
	 */
	int to_writer(node_writer<G>& in_c_writer, int in_i_align = 0);

	/*!
	 * Fill node from BML format through a node_parser.
	 *
	 * @param  in_c_parser node_parder to read data from
	 * @return             EC_BML_SUCCESS on success
	 *                     EC_BML_FAILURE on failure
	 */
	int from_parser(node_parser<G>& in_c_parser);



	/*!
	 * Get node_resource_segment of data.
	 * @return reference of internal node_resource_segment
	 */
	node_resource_segment<G> & get_segment(void);
	/*!
	 * Get node_resource_segment of data (const).
	 * @return reference of internal node_resource_segment
	 */
	node_resource_segment<G> const & get_segment(void) const;

	/*!
	 * Get data size.
	 * Return size of data content.
	 *
	 * @return  number of bytes in node_resource_segment of data
	 */
	size_t get_size(void) const;

	/*!
	 * Get size of node once in BML format.
	 *
	 * @param  in_i_align data aligment in bytes.
	 * @return            number of bytes
	 */
	size_t get_whole_size(int in_i_align = 0) const;

	/*!
	 * Get id of node.
	 *
	 * @return  Id of node
	 */
	T get_id(void) const;

	/*!
	 * Set id of node.
	 *
	 * @param in_c_id new Id to store
	 */
	virtual void set_id(T const & in_c_id);

	/*! stream converter function.
		Mainly used for debug.
	 */
	template<typename X, template<class> class Y>
	friend std::ostream& operator<<(std::ostream & os,
			node<X,Y> const & in_c_node);

	/*! Dump node to stdout */
	void dump();
};

template<typename T, template<class > class G>
node<T, G>::node(void) {
	//D("%p",this);
	_init();
}

template<typename T, template<class > class G> template<typename T2>
node<T, G>::node(T const & in_id, T2 const & in_data) {
	//D("%p",this);
	_init();
	this->set_id(in_id);
	this->set_data<T2>(in_data);

	VALGRIND_CHECK_VALUE_IS_DEFINED(in_data);
	VALGRIND_CHECK_VALUE_IS_DEFINED(_id);
}

template<typename T, template<class > class G>
node<T, G>::node(T const & in_id) {
	//D("%p",this);
	_init();
	this->set_id(in_id);

	VALGRIND_CHECK_VALUE_IS_DEFINED(_id);
}

template<typename T, template<class > class G>
node<T, G>::node(T const & in_id, node_resource_segment<G> const & in_pc_seg,
		off_t in_off_data, size_t in_sz_data) {
	//D("%p",this);
	_init();
	this->set_id(in_id);
	set_segment(in_pc_seg, in_off_data, in_sz_data);
}

template<typename T, template<class > class G>
node<T, G>::~node(void) {
	//D("Destructor %p", this);
	clear();
}

template<typename T, template<class > class G>
void node<T, G>::_init(void) {
	_b_is_invalid = false;
//	_b_mmapable = true;
//	_pc_allocator = NULL;
}

template<typename T, template<class > class G>
bool node<T, G>::has(T const & in_id, uint64_t in_i_indice) {
	node::find_result v_tmp;
	v_tmp = find(in_id);
	//D("%d",v_tmp.size());
	if (v_tmp.size() > in_i_indice) {
		return true;
	} else {
		return false;
	}
}
template<typename T, template<class > class G>
G<node<T, G>> & node<T, G>::get(T const & in_id, uint64_t in_i_indice,
		bool in_b_create) {
	node<T, G>::find_result v_tmp = find(in_id);
	if (in_i_indice < v_tmp.size()) {
		node<T, G>::it pc_node = v_tmp.at(in_i_indice);
		return pc_node->second;
	} else {
		if (in_b_create) {
			if (v_tmp.size() == in_i_indice) {
				return add(in_id);
			} else {
				throw std::runtime_error("Indice is incorrect :" + in_id);
			}
		} else {
			throw std::runtime_error("Id/Indice does not exist");
		}
	}
}

template<typename T, template<class > class G>
void node<T, G>::dump() {
	std::cout << (*this);
}

template<typename T, template<class > class G>
typename node<T,G>::er_it node<T,G>::equal_range(T const & in_id) {
    return _m_childs.equal_range(in_id);
}


template<typename T, template<class > class G>
typename node<T,G>::const_er_it node<T,G>::equal_range(T const & in_id) const {
  return _m_childs.equal_range(in_id);
}


template<typename T, template<class > class G>
typename node<T, G>::find_result node<T, G>::find(T const & in_id) {
	node<T, G>::find_result v_tmp;
	std::pair<it, it> c_ret = _m_childs.equal_range(in_id);

	for (it pc_it = c_ret.first; pc_it != c_ret.second; ++pc_it) {
		v_tmp.push_back(pc_it);
	}

	return v_tmp;
}

template<typename T, template<class > class G>
typename node<T, G>::it node<T,G>::begin() { return _m_childs.begin(); }

template<typename T, template<class > class G>
typename node<T, G>::it node<T,G>::end() { return _m_childs.end(); }

template<typename T, template<class > class G>
typename node<T, G>::const_it node<T,G>::cbegin() const { return _m_childs.cbegin(); }

template<typename T, template<class > class G>
typename node<T, G>::const_it node<T,G>::cend() const { return _m_childs.cend(); }

template<typename T, template<class > class G>
typename node<T, G>::find_result node<T, G>::childs(void) {
	node<T, G>::find_result v_tmp;
	for (it pc_it = _m_childs.begin(); pc_it != _m_childs.end(); ++pc_it) {
		v_tmp.push_back(pc_it);
	}
	return v_tmp;
}


template<typename T, template<class > class G>
G<node<T, G>> & node<T, G>::add_from(T const & in_id,
		G<node<T, G>> & in_c_node) {
	M_ASSERT(in_c_node.get() != this);

	std::pair<T, G<node<T, G> > > c_tmp;
	/* Fill pair values */
	c_tmp.first = in_id;
	c_tmp.second = in_c_node;

	/* Force id of node */
	c_tmp.second->set_id(in_id);

	//D("%p", c_tmp.second.get());
	/* Insert pair value */
	/*node::iterator c_it = */

	it pc_it = _m_childs.insert(c_tmp);
	//D("%p", pc_it->second.get());
	return pc_it->second;
}

template<typename T, template<class > class G>
G<node<T, G>> & node<T, G>::add(T const & in_id) {
	/* Create new node */
	G<node<T, G>> pc_node = on_new();

	/* Insert created node */
	return add_from(in_id, pc_node);
}

template<typename T, template<class > class G>
void node<T, G>::erase(node<T, G>::it & in_pc_it) {
	_m_childs.erase(in_pc_it);
}

template<typename T, template<class > class G>
void node<T, G>::erase(node<T, G> & in_c_node) {
	for (node<T, G>::it pc_it = _m_childs.begin(); pc_it != _m_childs.end();
			++pc_it) {
		if (&in_c_node == pc_it->second) {
			erase(pc_it);
		}
	}
}

template<typename T, template<class > class G>
void node<T, G>::resize(size_t in_i_size) {
	//D("%p %d %d %d %p", this, _s_segment.size(), _s_segment.isempty(), in_i_size, &_s_segment);
	if (_s_segment.size() != in_i_size) {
		if (!_s_segment.isempty()) {
			//D("RESIZE %p", this);
			_s_segment.resize(in_i_size);
		} else {
			//D("FIRST %p", this);
			node_resource_segment<G> pc_seg = on_new_resource(in_i_size);

			/* Set resource as segment allocated resource */
			set_segment(pc_seg);
		}
	}
}

template<typename T, template<class > class G>
void node<T, G>::clear() {

	/* Clear extension */
	_m_ext_values.clear();

	/* Clear segment */
	_s_segment.clear();

	/* Clear childs */
	empty();
}

template<typename T, template<class > class G>
void node<T, G>::empty() {

	for (node<T, G>::it pc_it = _m_childs.begin(); pc_it != _m_childs.end();) {
		on_delete(pc_it->second);
		pc_it = _m_childs.erase(pc_it);
	}
}


template<typename T, template<class > class G>
node<T, G> & node<T, G>::operator()(T const & in_id, uint64_t in_i_indice) {
	return *get(in_id, in_i_indice, true);
}

/* common assignment */
template<typename T, template<class > class G>
template<typename T2, typename std::enable_if<!is_container<T2>::value >::type*>
void node<T, G>::set_data(T2 const & in) {
    //static_assert(std::is_integral<T2>::value || std::is_floating_point<T2>::value || std::is_pointer<T2>::value, "Either integral, float or pointer required.");
    resize(sizeof(T2));
    _s_segment.template set_data<T2>(in);
    VALGRIND_CHECK_VALUE_IS_DEFINED(in);
    //std::cout << "SET COPY" << std::endl;
}

template<typename T, template<class > class G>
template<typename T2, typename std::enable_if<is_container<T2>::value && !is_map<T2>::value && !is_contiguous<T2>::value >::type*>
void node<T, G>::set_data(T2 const & in) {
    typedef typename T2::value_type T2S;
    static_assert(!is_container<T2S>::value, "Value type must not be a container.");
    uint32_t i_cnt = 0;
    size_t i_size = in.size();
    resize(sizeof(T2S)*i_size);
    T2S* pc_tmp = mmap<T2S>();
    for (auto pc_it = in.cbegin(); pc_it != in.cend(); pc_it++) {
        *pc_tmp++ = *pc_it;
        i_cnt++;
        VALGRIND_CHECK_VALUE_IS_DEFINED(*pc_it);
    }
    //std::cout << "SET DATA NONCONTIGUOUS" << std::endl;
    assert(i_cnt == i_size);
}


template<typename T, template<class > class G>
template<typename T2, typename std::enable_if<is_container<T2>::value && !is_map<T2>::value && is_contiguous<T2>::value >::type*>
void node<T, G>::set_data(T2 const & in) {
    typedef typename T2::value_type T2S;
    //static_assert(std::is_integral<T2S>::value || std::is_floating_point<T2S>::value || std::is_pointer<T2S>::value, "Either integral, float or pointer required.");
    static_assert(!is_container<T2S>::value, "Value type must not be a container.");
    size_t i_size = in.size();
    resize(sizeof(T2S)*i_size);
    memcpy_from_buffer((const char*)in.data(), in.size()*sizeof(T2S));
    VALGRIND_CHECK_MEM_IS_DEFINED(in.data(), in.size()*sizeof(T2S));
    //std::cout << "SET DATA CONTIGUOUS" << std::endl;
}

/* common assignment */
template<typename T, template<class > class G>
template<typename T2, typename std::enable_if<!is_container<T2>::value >::type*>
T2 node<T, G>::get_data(void) const {
    //static_assert(std::is_integral<T2>::value || std::is_floating_point<T2>::value || std::is_pointer<T2>::value, "Either integral, float or pointer required.");
    return _s_segment.template get_data<T2>();
    VALGRIND_CHECK_VALUE_IS_DEFINED(in);
    //std::cout << "SET COPY" << std::endl;
}


template<typename T, template<class > class G>
template<typename T2, typename std::enable_if<is_container<T2>::value && !is_map<T2>::value && !is_contiguous<T2>::value >::type*>
T2 node<T, G>::get_data(void) const {
    typedef typename T2::value_type T2S;
    T2 c_tmp;
    assert(get_size() >= sizeof(T2S));
    assert(get_size()%sizeof(T2S) == 0);
    auto i_size = (get_size()/sizeof(T2S));
    T2S* pc_tmp = mmap<T2S>();
    for (auto i_cnt = 0U; i_cnt != i_size; i_cnt++)
    {
        c_tmp.push_back(*pc_tmp++);
    }
    return c_tmp;
}

template<typename T, template<class > class G>
template<typename T2, typename std::enable_if<is_container<T2>::value && !is_map<T2>::value && is_contiguous<T2>::value >::type*>
T2 node<T, G>::get_data(void) const {
    typedef typename T2::value_type T2S;
    //static_assert(std::is_integral<T2S>::value || std::is_floating_point<T2S>::value || std::is_pointer<T2S>::value, "Either integral, float or pointer required.");
    T2 c_tmp;
    assert(get_size() >= sizeof(T2S));
    assert(get_size()%sizeof(T2S) == 0);
    c_tmp.resize(get_size()/sizeof(T2S));
    memcpy_to_buffer((const char*)c_tmp.data(), c_tmp.size()*sizeof(T2S));
    return c_tmp;
    //std::cout << "SET DATA CONTIGUOUS" << std::endl;
}

/* mmap assignment */
template<typename T, template<class > class G> template<typename T2>
T2 * node<T, G>::mmap(void) const {
	M_ASSERT(_s_segment.size() >= sizeof(T2));
	T2 * pc_tmp = (T2 *) _s_segment.mmap();
	return pc_tmp;
}

template<typename T, template<class > class G>
bool node<T, G>::is_mmapable(void) const {
	return _s_segment.mmap();
}

template<typename T, template<class > class G> template<typename T2>
T2 & node<T, G>::map(void) const {
    if (_s_segment.size() != sizeof(T2)) {
        std::cout << "_s_segment.size() : " << _s_segment.size() << " ;  sizeof(T2) : " << sizeof(T2) << std::endl;
    }
	M_ASSERT(_s_segment.size() == sizeof(T2));
	T2 * c_tmp = (T2 *) _s_segment.mmap();
	return *c_tmp;
}

/* extension get */

template<typename T, template<class > class G>
bool node<T, G>::has_ext(uint8_t in_i_id) {
	return _m_ext_values.find(in_i_id) != _m_ext_values.end();
}

template<typename T, template<class > class G> template<typename T2>
T2 node<T, G>::get_ext(uint8_t in_i_id) {
	node_resource_segment<G> & c_seg = _m_ext_values.at(in_i_id);
	return c_seg.template get_data<T2>();
}

template<typename T, template<class > class G> template<typename T2>
void node<T, G>::set_ext(uint8_t in_i_id, T2 const & in) {
  typename std::map<uint8_t, node_resource_segment<G>>::iterator it = _m_ext_values.find(in_i_id);
	if(it != _m_ext_values.end()) {
		node_resource_segment<G> & c_seg = it->second;
		c_seg.set_data(in);
	} else {
		G<node_resource<G>> pc_tmp(new node_alloc_resource<G>(sizeof(T2)));
		pc_tmp->memcpy_from_buffer((char*) &in, sizeof(T2));
		_m_ext_values[in_i_id] = node_resource_segment<G>(pc_tmp);
	}

//	*pc_tmp = in;
}

/* events */

template<typename T, template<class > class G>
node_resource_segment<G> node<T, G>::on_new_resource(size_t in_i_size) {

	node_resource<G> * pc_tmp;
	pc_tmp = new node_alloc_resource<G>(in_i_size);
	if (!pc_tmp) {
		throw std::bad_alloc();
	}
	//M_BUG();
	//M_ASSERT(pc_tmp != (node_resource * )0x6192a0);
	//D("ARRGG NEW resource: %p", pc_tmp);

	return node_resource_segment<G>(G<node_resource<G> >(pc_tmp));;
}

template<typename T, template<class > class G>
G<node<T, G>> node<T, G>::on_new(void) {
	node<T, G> * pc_node = new node<T, G>();
	if (!pc_node) {
		std::runtime_error("Unable to allocate memory");
	}
	return G<node<T, G>>(pc_node);
}

template<typename T, template<class > class G>
void node<T, G>::on_delete(G<node<T, G>> in_pc_child) {
	return;
}

template<typename T, template<class > class G>
void node<T, G>::on_read(void) {
	return;
}

template<typename T, template<class > class G>
void node<T, G>::on_write(void) {
	return;
}

template<typename T, template<class > class G>
int node<T, G>::from_parser_id(node_parser<G> & in_c_parser,
		uint64_t in_sz_id) {
	int ec;
	M_ASSERT(sizeof(uint64_t) >= in_sz_id);

	_id = (T) 0;
	uint64_t tmp = 0;

	/* Read ID field - Assuming Little Endianness Id */
	ec = in_c_parser.parse_data((char*) &tmp, in_sz_id);

	set_id((T)tmp);

	return ec;
}

template<typename T, template<class > class G>
int node<T, G>::from_parser(node_parser<G> & in_c_parser) {
	clear();
	char i_flags;
	bool b_loop = true;
	int ec;

	uint64_t i_sz_id;
	int i_length_id;
	int i_length_size;
	bool b_ext;

	while (b_loop) {
		/* Read flags */
		ec = in_c_parser.parse_data(&i_flags, 1);
		if (ec != EC_BML_SUCCESS) {
			return ec;
		}

		/* decode flags */
		i_length_size = i_flags & 0xF;
		i_length_id = ((i_flags >> 4) & 0x7) + 1;
		b_ext = i_flags & 0x80 ? true : false;

#ifdef DEBUG_PARSER
		D("New node (Length Id:%d, Length Size:%d, Ext:%d) [%02x]",
				i_length_id, i_length_size, b_ext, 0xff&(unsigned int)i_flags);
#endif

		if (i_length_size == E_NODE_SIZE_INVALID) {
			switch (i_length_id - 1) {
			case E_NODE_ID_SIZE_NULL:
#ifdef DEBUG_PARSER
			  D("- ALIGN NODE");
#endif
				break;
			case E_NODE_ID_SIZE_INVALID:
				_b_is_invalid = true;
#ifdef DEBUG_PARSER
				D("- INVALID NODE");
#endif
				/* Exit on invalid node, means end of childs */
				return EC_BML_SUCCESS;
				break;
			}
			continue;
		} else {
			b_loop = false;
		}

	}
	/* Read ID size */
	i_sz_id = 0;
	ec = in_c_parser.parse_data((char*) &i_sz_id, i_length_id);
	if (ec != EC_BML_SUCCESS) {
		return ec;
	}
	if (!i_sz_id) {
		throw std::runtime_error("Size ID should be != 0");
	}
#ifdef DEBUG_PARSER
	D("- ID size: %lld (%d bytes)", i_sz_id, i_length_id);
#endif

	/* Read ID field */
	ec = from_parser_id(in_c_parser, i_sz_id);
	if (ec != EC_BML_SUCCESS) {
		return ec;
	}
#ifdef DEBUG_PARSER
	D("- ID on %d bytes", i_sz_id);
#endif

	/* Loop ext field */
	while (b_ext) {
		uint8_t i_id_ext;
		uint8_t sz_sz_ext;
		uint64_t i_sz_ext;
		char i_flags_back;

		/* Read ext flags */
		ec = in_c_parser.parse_data(&i_flags, sizeof(char));
		if (ec != EC_BML_SUCCESS) {
			return ec;
		}
		i_flags_back = i_flags;

		/* Decode ext id */
		i_id_ext = (i_flags_back & 0xF);
		i_flags >>= 4;

		/* Decode ext size */
		sz_sz_ext = (i_flags & 0x7) + 1;
		b_ext = i_flags & 0x8 ? true : false;
#ifdef DEBUG_PARSER
		D("- EXT Header (Id:%d, Size:%d, Ext:%d) [%02x]",
				i_id_ext, sz_sz_ext, b_ext, 0xff&(unsigned int)i_flags_back);
#endif

		/* Read ext size */
		i_sz_ext = 0;
		ec = in_c_parser.parse_data((char*) &i_sz_ext, sz_sz_ext);
		if (ec != EC_BML_SUCCESS) {
			return ec;
		}
#ifdef DEBUG_PARSER
		D("- EXT size: %lld (%d bytes)", i_sz_ext, sz_sz_ext);
#endif

		/* store ext in map */
		ec = in_c_parser.parse_resource(_m_ext_values[i_id_ext], i_sz_ext);
		if (ec != EC_BML_SUCCESS) {
			return ec;
		}

#ifdef DEBUG_PARSER
		D("- EXT (%lld bytes)", i_sz_ext);
#endif

	}

	/* If empty no size field and no data */
	uint64_t i_size;
	if (i_length_size != 0) {
		/* Read Size field */
		i_size = 0;
		ec = in_c_parser.parse_data((char*) &i_size, i_length_size);
		if (ec != EC_BML_SUCCESS) {
			return ec;
		}
#ifdef DEBUG_PARSER
		D("- Data size:%lld (%d bytes)", i_size, i_length_size);
#endif
		//FIXME: ntohll on BE
	} else {
		i_size = 0;
	}

	/* Read data */
	if (i_size) {
		//cout << "Data Size: "<<_sz_data<<endl;
		/* Read data from file to buffer */
#ifdef DEBUG_PARSER
		D("- Data on %lld bytes", i_size);
#endif
		node_resource_segment<G> c_tmp;
		ec = in_c_parser.parse_resource(c_tmp, i_size);
		if (ec != EC_BML_SUCCESS) {
			return ec;
		}
		set_segment(c_tmp);
	}

	/* Create childs, until INVALID NODE reached */
	while (1) {
		std::pair<T, G<node<T, G>>> c_tmp;
		c_tmp.second = on_new();

		/* Read parser */
		ec = c_tmp.second->from_parser(in_c_parser);
		if (ec != EC_BML_SUCCESS) {
			on_delete(c_tmp.second);
			return ec;
		}

		if (c_tmp.second->is_invalid()) {
			on_delete(c_tmp.second);
			break;
		}

		/* Fill id of node */
		c_tmp.first = c_tmp.second->get_id();
		/* Insert into multi map */
		_m_childs.insert(c_tmp);

	}

	on_read();

	return EC_BML_SUCCESS;
}
template<typename T, template<class > class G>
uint8_t node<T, G>::get_length(uint64_t in_i_num) const {
	uint8_t i_length;
	uint64_t i_mask;
	for (i_mask = ~0x00, i_length = 0; i_length < 8; ++i_length, i_mask <<= 8) {
		if ((in_i_num & i_mask) == 0)
			break;
	}
	return i_length;
}

template<typename T, template<class > class G>
int node<T, G>::to_writer_id(node_writer<G> & in_c_writer, uint64_t in_sz_id) {
	return in_c_writer.write_data((char*) &_id, in_sz_id);

}

template<typename T, template<class > class G>
size_t node<T, G>::get_whole_size(int in_i_align) const {
	size_t sz_tmp = 0;
		uint64_t i_sz_id = get_id_size();
		int i_length_id = get_length(i_sz_id);
		size_t sz_buffer = _s_segment._pc_resource ? _s_segment._sz_data : 0;
		int i_length_size = get_length(sz_buffer);

		if (in_i_align) {
			i_length_id = in_i_align - 1;
			i_sz_id = (i_sz_id + (in_i_align - 1)) & -in_i_align;
			i_length_size = (i_length_size + (in_i_align - 1)) & -in_i_align;
		}


		/* Write flags */
		sz_tmp += sizeof(char);

		sz_tmp += i_length_id;

		sz_tmp += i_sz_id;

		/* Write extensions size */

		for (typename std::map<uint8_t, node_resource_segment<G> >::const_iterator pc_it =
				_m_ext_values.begin(); pc_it != _m_ext_values.end(); ++pc_it) {

			/* Generate flags */
			uint64_t i_sz_ext = pc_it->second._sz_data;
			uint64_t i_sz_sz_ext = get_length(i_sz_ext);

			if (in_i_align) {
				i_sz_sz_ext = in_i_align - 1;
			}

			/* Write ext size */
			sz_tmp += sizeof(char);

			/* Write ext size */
			sz_tmp += i_sz_sz_ext;
			/* Write data */
			sz_tmp += i_sz_ext;
		}

		/* If empty no size field and no data */
		if (i_length_size) {
			/* Write Size field */
			sz_tmp+=i_length_size;

			/* Write data */
			sz_tmp+=sz_buffer;
		}

		/* Write childs */
		for (const_it pc_it = _m_childs.begin(); pc_it != _m_childs.end(); ++pc_it) {
			sz_tmp += pc_it->second->get_whole_size(in_i_align);
		}

		/* Write invalid node */
		{
			sz_tmp+=1;
			/* Align invalid node */
			if (in_i_align) {
				sz_tmp+=in_i_align-1;
			}
		}

		return sz_tmp;
}

template<typename T, template<class > class G>
int node<T, G>::to_writer(node_writer<G> & in_c_writer, int in_i_align) {

	char i_flags;
	uint64_t i_sz_id = get_id_size();
	int i_length_id = get_length(i_sz_id);
	size_t sz_buffer = _s_segment._pc_resource ? _s_segment._sz_data : 0;
	int i_length_size = get_length(sz_buffer);
	int ec;

	if (in_i_align) {
		i_length_id = in_i_align - 1;
		i_sz_id = (i_sz_id + (in_i_align - 1)) & -in_i_align;
		i_length_size = (i_length_size + (in_i_align - 1)) & -in_i_align;
	}
//int i_type = (uint8_t) _e_type;

	bool b_ext;

	on_write();

	/* Add extension if dict no empty */
	if (_m_ext_values.size()) {
		b_ext = true;
	} else {
		b_ext = false;
	}

	/* Generate flags */
	i_flags = (i_length_id - 1) & 0x7;
	if (b_ext) {
		i_flags |= 0x8;
	}
	i_flags <<= 4;
	i_flags |= i_length_size & 0xF;

	/* Write flags */
	ec = in_c_writer.write_data(&i_flags, sizeof(char));
	if (ec != EC_BML_SUCCESS) {
		DBG_LINE();
		return ec;
	}

#ifdef DEBUG_WRITER
	D("Create node (Length Id:%d, Length Size:%d, Ext:%d) [%02x] (%p)",i_length_id, i_length_size,b_ext, 0xff&(unsigned int)i_flags, this);
#endif
	/*
	 cout << "Id : " << _i_id << " Size:" << _sz_data << endl;
	 cout << "Id_length : " << i_length_id << " Size_length:" << i_length_size << "ExtSize:"<< _m_ext_values.size()
	 << endl;
	 cout << hex << int(i_flags) << endl;
	 */

	/* Write ID size */
#ifdef DEBUG_WRITER
	D("- ID size: %d (%d bytes)", i_sz_id, i_length_id);
#endif
	M_ASSERT(i_sz_id);
	ec = in_c_writer.write_data((char*) &i_sz_id, i_length_id);
	if (ec != EC_BML_SUCCESS) {
		DBG_LINE();
		return ec;
	}

	/* Write ID field */
#ifdef DEBUG_WRITER
	D("- ID on %d bytes", i_sz_id);
#endif
	ec = to_writer_id(in_c_writer, i_sz_id);
	if (ec != EC_BML_SUCCESS) {
		DBG_LINE();
		return ec;
	}
	/* Write extensions size */

	for (typename std::map<uint8_t, node_resource_segment<G> >::iterator pc_it =
			_m_ext_values.begin(); pc_it != _m_ext_values.end(); ++pc_it) {
		typename std::map<uint8_t, node_resource_segment<G> >::iterator pc_last =
				--_m_ext_values.end();
		if (pc_it != pc_last) {
			b_ext = true;
		} else {
			b_ext = false;
		}
		uint8_t i_id_ext = pc_it->first & 0xF;

		/* Generate flags */
		uint64_t i_sz_ext = pc_it->second._sz_data;
		uint64_t i_sz_sz_ext = get_length(i_sz_ext);

		if (in_i_align) {
			i_sz_sz_ext = in_i_align - 1;
		}

		i_flags = ((i_sz_sz_ext >= 1 ? i_sz_sz_ext : 1) - 1) & 0x7;
		if (b_ext) {
			i_flags |= 0x8;
		}
		i_flags <<= 4;
		i_flags |= i_id_ext;
		/* Write ext size */
#ifdef DEBUG_WRITER
		D("- EXT Header (Id:%d, Size:%d, Ext:%d) [%02x]", i_id_ext, i_sz_sz_ext,b_ext, 0xff&(unsigned int)i_flags);
#endif
		ec = in_c_writer.write_data(&i_flags, sizeof(char));
		if (ec != EC_BML_SUCCESS) {
			DBG_LINE();
			return ec;
		}
		/* Write ext size */
#ifdef DEBUG_WRITER
		D("- EXT size: %d (%d bytes)",i_sz_ext, i_sz_sz_ext);
#endif
		ec = in_c_writer.write_data((char*) &i_sz_ext, i_sz_sz_ext);
		if (ec != EC_BML_SUCCESS) {
			DBG_LINE();
			return ec;
		}

		/* Assert if data ext not aligned */
		if (in_i_align) {
			M_ASSERT(i_sz_ext - (i_sz_ext&(-in_i_align)) == 0);
		}

		/* Write data */
#ifdef DEBUG_WRITER
		D("- EXT (%d bytes)",i_sz_ext);
#endif
		M_ASSERT(pc_it->second.size() == i_sz_ext);

		ec = in_c_writer.write_resource(pc_it->second);
		if (ec != EC_BML_SUCCESS) {
			DBG_LINE();
			return ec;
		}
	}

	/* If empty no size field and no data */
	if (i_length_size) {

		/* Write Size field */
#ifdef DEBUG_WRITER
		D("- Data size %d (%d bytes)", sz_buffer, i_length_size);
#endif
		ec = in_c_writer.write_data((char*) &sz_buffer, i_length_size);
		if (ec != EC_BML_SUCCESS) {
			DBG_LINE();
			return ec;
		}

		/* Write data */
#ifdef DEBUG_WRITER
		D("- Data (%d bytes)",sz_buffer);
#endif
		M_ASSERT(sz_buffer == _s_segment.size());
		ec = in_c_writer.write_resource(_s_segment);
		if (ec != EC_BML_SUCCESS) {
			DBG_LINE();
			return ec;
		}

		/* Assert if data not aligned */
		if (in_i_align) {
			M_ASSERT(sz_buffer - (sz_buffer&(-in_i_align)) == 0);
		}
	}

	/* Write childs */
	for (it pc_it = _m_childs.begin(); pc_it != _m_childs.end(); ++pc_it) {
		ec = pc_it->second->to_writer(in_c_writer);
		if (ec != EC_BML_SUCCESS) {
			DBG_LINE();
			return ec;
		}
	}

	/* Write invalid node */
	{
		//cout << "INVALID NODE"<<endl;
#ifdef DEBUG_WRITER
		D("-INVALID NODE (%p)", this);
#endif
		char i_invalid_node = 0xFF;
		ec = in_c_writer.write_data((char*) &i_invalid_node, 1);
		if (ec != EC_BML_SUCCESS) {
			DBG_LINE();
			return ec;
		}

		/* Align invalid node */
		if (in_i_align) {
			for (int i = 0; i < in_i_align-1; i++) {
				char i_align_node = 0x0F;
				ec = in_c_writer.write_data((char*) &i_align_node, 1);
				if (ec != EC_BML_SUCCESS) {
					DBG_LINE();
					return ec;
				}
			}
		}
	}

	return EC_BML_SUCCESS;
}

template<typename T, template<class > class G> void node<T, G>::set_segment(
		node_resource_segment<G> const & in_pc_seg, off_t in_off_data,
		size_t in_sz_data) {
//D("Bef resource: %p", _s_segment._pc_resource);
//DBG_LINE();
	_s_segment.set_from_segment(in_pc_seg, in_off_data, in_sz_data);
//D("resource: %p", _s_segment._pc_resource);
//DBG_LINE();
}

template<typename T, template<class > class G> int node<T, G>::get_id_size(
		void) const {
	return sizeof(T);
}

template<typename T, template<class > class G>
node_resource_segment<G> & node<T, G>::get_segment(void) {
	return _s_segment;
}

template<typename T, template<class > class G>
node_resource_segment<G> const & node<T, G>::get_segment(void) const {
	return _s_segment;
}

template<typename T, template<class > class G>
size_t node<T, G>::get_size(void) const {
	return _s_segment.size();
}

template<typename T, template<class > class G>
T node<T, G>::get_id(void) const {
	return _id;
}

template<typename T, template<class > class G>
void node<T, G>::set_id(T const & in_c_id) {
	_id = in_c_id;
}

template<typename T, template<class > class G>
bool node<T, G>::is_invalid(void) {
	return _b_is_invalid;
}

template<typename T, template<class > class G>
int node<T, G>::to_file(std::string const & in_str_file) {
	node_file_writer<G> c_writer(in_str_file);
	return to_writer(c_writer);
}

template<typename T, template<class > class G>
int node<T, G>::from_file(std::string const & in_str_file) {
	node_file_parser<G> c_parser(in_str_file);
	return from_parser(c_parser);
}

template<typename T, template<class > class G>
bool node<T, G>::operator !=(node<T, G> const & other) const {
	/* Check id */
	if (_id != other._id) {
		return false;
	}
	/* Check resource */
	if (_s_segment != other._s_segment) {
		return false;
	}

	/* Check childs */
	if (_m_childs.size() != other._m_childs.size()) {
		return false;
	}
	{
		const_it pc_it = _m_childs.begin();
		const_it pc_it2 = other._m_childs.begin();
		while (pc_it != _m_childs.end()) {
			if (*(pc_it->second) != *(pc_it2->second)) {
				return false;
			}
			pc_it++;
			pc_it2++;
		}
	}
	return true;
}

template<typename T, template<class > class G>
void node<T, G>::copy_link(node<T, G> const & other) {
	/* Copy id */
	set_id(other._id);

	/* Copy resource */
	_s_segment = other._s_segment;

	/* Copy childs */
	{
		const_it pc_it = other._m_childs.begin();
		while (pc_it != other._m_childs.end()) {
            add(pc_it->second->get_id())->copy_link(*(pc_it->second));
			pc_it++;
		}
	}
}

template<typename T, template<class > class G>
void node<T, G>::copy(node<T, G> const & other) {


	/* Copy id */
	set_id(other._id);

	/* Copy resource */
	if (other._s_segment.size()) {
		resize(other._s_segment.size());
		_s_segment.memcpy_from_segment(other._s_segment);
	}

	/* Copy childs */
	empty();
	{
		const_it pc_it = other._m_childs.begin();
		while (pc_it != other._m_childs.end()) {
            add(pc_it->second->get_id())->copy(*(pc_it->second));
			pc_it++;
		}
	}
}

template<typename T, template<class > class G>
bool node<T, G>::operator <(node<T, G> const & other) const {
	return _id < other._id;
}

template<typename T, template<class > class G>
void node<T, G>::memcpy_from_buffer(char const * in_pc_buffer,
		size_t in_sz_buffer) {
	resize(in_sz_buffer);
	_s_segment.memcpy_from_buffer(in_pc_buffer, in_sz_buffer);
}
template<typename T, template<class > class G>
void node<T, G>::memcpy_to_buffer(char const * in_pc_buffer,
		size_t in_sz_buffer) const {
	_s_segment.memcpy_to_buffer(in_pc_buffer, in_sz_buffer);
}
#ifdef LF_BML_CRC
uint32_t f_bml_crc(char * in_pc_tmp, size_t in_sz);
#endif
/* display */
template<typename T, template<class > class G>
std::ostream& operator<<(std::ostream & os, node<T, G> const & in_c_node) {

	os << " { ID: " << std::hex << in_c_node.get_id() << ", SZ_DATA: "
			<< std::dec << in_c_node.get_size();

#ifdef LF_BML_CRC
	if (!in_c_node.get_segment().isempty()) {
		char * pc_tmp = in_c_node.get_segment().mmap();
		if (pc_tmp) {
			os << ", CRC: " << std::hex
					<< f_bml_crc(pc_tmp, in_c_node.get_size()) << "("
					<< uintptr_t(pc_tmp) << ")";
		}
	}
#endif

	typename node<T, G>::const_it pc_it;

	if (in_c_node._m_childs.size()) {
		bool b_first = true;
		os << ", CHILDS: ";
		for (pc_it = in_c_node._m_childs.begin();
				pc_it != in_c_node._m_childs.end(); ++pc_it) {
			if (!b_first) {
				os << ", ";
			} else {
				b_first = 0;
			}
			os << *pc_it->second;
		}
	}

	os << " }";

	return os;
}

/******************************
 * resource
 */
template<template<class > class G>
node_resource<G>::node_resource(void) {
	//D("+++ %p", this);
	_sz_data = 0;
}
template<template<class > class G>
node_resource<G>::~node_resource() {
	return;
}
#if 0
template<template <class> class G>
node_resource<G>::operator node_resource_segment<G>() const {
	node_resource_segment<G> c_tmp();
}
#endif
template<template<class > class G>
char * node_resource<G>::mmap(off_t in_off_data, size_t in_sz_data) const {
	DBG_LINE();
	M_BUG();
}
#if 0
void node_resource::unmmap(off_t in_off_data, size_t in_sz_data) const {
	M_BUG();
}
#endif
template<template<class > class G>
void node_resource<G>::resize(size_t in_sz_data) {
	DBG_LINE();
	M_BUG();
}

/* Default Direct buffer  Copy */
template<template<class > class G>
void node_resource<G>::memcpy_from_buffer(char const * in_pc_buffer,
		size_t in_sz_buffer, off_t in_off_data) {
	char * pc_tmp = mmap(in_off_data, in_sz_buffer);
	M_ASSERT(pc_tmp);
	memcpy(pc_tmp, in_pc_buffer, in_sz_buffer);
	//unmmap();
}
template<template<class > class G>
void node_resource<G>::memcpy_to_buffer(char const * in_pc_buffer,
		size_t in_sz_buffer, off_t in_off_data) const {
	M_ASSERT(in_off_data+in_sz_buffer <= _sz_data);
	char * pc_tmp = mmap(in_off_data, in_sz_buffer);
	M_ASSERT(pc_tmp);
	memcpy((char*) in_pc_buffer, pc_tmp, in_sz_buffer);
	//unmmap();
}

template<template<class > class G>
void node_resource<G>::memset_from_value(char in_i_value, size_t in_sz_buffer,
		off_t in_off_data) {
	char * pc_tmp = mmap(in_off_data, in_sz_buffer);
	M_ASSERT(pc_tmp);
	memset(pc_tmp, in_i_value, in_sz_buffer);
	//unmmap();
}

template<template<class > class G>
bool node_resource<G>::operator !=(node_resource const & other) const {
	node_resource_segment<G> c_tmp1 = *this;
	node_resource_segment<G> c_tmp2 = other;
	return c_tmp1 != c_tmp2;
}

/******************************
 * resource MALLOC
 */
template<template<class > class G>
node_alloc_resource<G>::node_alloc_resource(size_t in_sz_data) :
		node_resource<G>() {
	_pv_data = NULL;
	resize(in_sz_data);
}
template<template<class > class G>
node_alloc_resource<G>::~node_alloc_resource() {
	::operator delete(_pv_data);
}

template<template<class > class G>
void node_alloc_resource<G>::resize(size_t in_sz_data) {
	//D("Allocating %d bytes", (int)in_sz_data);
	if (in_sz_data != (size_t) _sz_data) {
		if (_pv_data) {
			if (!in_sz_data) {
				::operator delete(_pv_data);
				_pv_data = NULL;
			} else {
				/* C++ realloc */
				void * pv_tmp = ::operator new(in_sz_data);
				std::copy_n((char*)_pv_data, std::min(_sz_data, in_sz_data), (char*)pv_tmp);
				::operator delete(_pv_data);
				_pv_data = pv_tmp;
			}
		} else {
			_pv_data = ::operator new(in_sz_data);
		}
		if (!_pv_data) {
			throw std::bad_alloc();
		}
		_sz_data = in_sz_data;
	}
}
template<template<class > class G>
char * node_alloc_resource<G>::mmap(off_t in_off_data,
		size_t in_sz_data) const {
//	DBG_LINE();
	return &(((char*) _pv_data)[in_off_data]);
}
#if 0
void node_alloc_resource::unmmap(off_t in_off_data, size_t in_sz_data) const {
	return;
}
#endif
/******************************
 * resource buffer
 */
template<template<class > class G>
node_buffer_resource<G>::node_buffer_resource(void * in_pc_buffer,
		size_t in_sz_buffer) :
		node_resource<G>() {
	_pv_data = in_pc_buffer;
	_sz_data = in_sz_buffer;
}
template<template<class > class G>
node_buffer_resource<G>::~node_buffer_resource() {
}
template<template<class > class G>
char * node_buffer_resource<G>::mmap(off_t in_off_data,
		size_t in_sz_data) const {
	return &(((char*) _pv_data)[in_off_data]);
}

/* Default Segment Copy */
template<template<class > class G>
void node_resource<G>::memcpy_from_segment(
		node_resource_segment<G> const &in_c_seg, size_t in_sz_buffer,
		off_t in_off_data) {
	M_ASSERT(in_off_data+in_sz_buffer <= _sz_data);
	M_ASSERT(in_c_seg.size() == in_sz_buffer);
	char * pc_tmp = in_c_seg.mmap();
	M_ASSERT(pc_tmp);
	memcpy_from_buffer(pc_tmp, in_sz_buffer, in_off_data);
	//unmmap();
}
template<template<class > class G>
void node_resource<G>::memcpy_to_segment(node_resource_segment<G> &in_c_seg,
		size_t in_sz_buffer, off_t in_off_data) const {
	M_ASSERT(in_off_data+in_sz_buffer <= _sz_data);
	M_ASSERT(in_c_seg.size() == in_sz_buffer);
	char * pc_tmp = in_c_seg.mmap();
	M_ASSERT(pc_tmp);
	memcpy_to_buffer(pc_tmp, in_sz_buffer, in_off_data);
	//unmmap();
}

/******************************
 * SEGMENT WRITER
 */

template<template<class > class G>
node_segment_writer<G>::node_segment_writer(
		node_resource_segment<G> const & in_s_segment) :
		node_buffer_writer<G>(in_s_segment.mmap(), in_s_segment.size()) {
	_pc_parent = &in_s_segment;
	//_pc_buffer = in_s_segment.mmap();
}
template<template<class > class G>
node_segment_writer<G>::~node_segment_writer() {
	//_pc_parent->unmmap();
}

/******************************
 * WRITER
 */
template<template<class > class G>
node_writer<G>::node_writer() {
}

template<template<class > class G>
node_writer<G>::~node_writer() {
}

/******************************
 * FILE WRITER
 */

template<template<class > class G>
int node_writer<G>::write_resource(node_resource_segment<G> & in_s_resource) {
	int ec;
	M_ASSERT(in_s_resource._pc_resource);
	char * pv_data = in_s_resource.mmap();
	if (pv_data) {
		ec = write_data(pv_data, in_s_resource._sz_data);
	} else {
		char * pv_data = (char*) malloc(in_s_resource._sz_data);
		in_s_resource.memcpy_to_buffer(pv_data, in_s_resource._sz_data);
		ec = write_data(pv_data, in_s_resource._sz_data);
		free(pv_data);
	}
	//in_s_resource.unmmap();
	return ec;
}

template<template<class > class G>
node_file_writer<G>::node_file_writer(std::string const & in_str_file) {

	_c_os.open(in_str_file.c_str(), std::ios::out | std::ios::binary);

	if (_c_os.fail()) {
		throw std::runtime_error("Failed to open file");
	}

	char ac_tmp[4] = { 'B', 'M', 'L', 0 };
	_c_os.write(ac_tmp, 4);
	_i_offset = 4;

	if (_c_os.fail()) {
		throw std::runtime_error("Failed to write header");
	}
}

template<template<class > class G>
node_file_writer<G>::~node_file_writer() {
	_c_os.close();
}

template<template<class > class G>
int node_file_writer<G>::write_data(char* in_pc_buffer, size_t in_sz_buffer) {
	VALGRIND_CHECK_MEM_IS_DEFINED(in_pc_buffer, in_sz_buffer);

	_c_os.write(in_pc_buffer, in_sz_buffer);

	if (_c_os.fail()) {
		return EC_BML_FAILURE;
	}

	_i_offset += in_sz_buffer;
	return EC_BML_SUCCESS;
}

/******************************
 * BUFFER WRITER
 */
template<template<class > class G>
node_buffer_writer<G>::node_buffer_writer(char* in_pc_buffer,
		size_t in_sz_buffer) {
	_pc_buffer = in_pc_buffer;
	_sz_buffer = in_sz_buffer;
	_i_offset = 0;
}

template<template<class > class G>
node_buffer_writer<G>::~node_buffer_writer() {
}

template<template<class > class G>
int node_buffer_writer<G>::write_data(char* in_pc_buffer, size_t in_sz_buffer) {
	//D("%llu %u %llu",_i_offset,in_sz_buffer,_sz_buffer);
	VALGRIND_CHECK_MEM_IS_DEFINED(in_pc_buffer, in_sz_buffer);

	if ((_i_offset + in_sz_buffer) > _sz_buffer) {
		return EC_BML_OVERFLOW;
	}
	memcpy(&_pc_buffer[_i_offset], in_pc_buffer, in_sz_buffer);
	_i_offset += in_sz_buffer;
	//D("Write %d", in_sz_buffer);

	return EC_BML_SUCCESS;
}

/******************************
 * PARSER
 */
template<template<class > class G>
node_parser<G>::node_parser() {
}

template<template<class > class G>
node_parser<G>::~node_parser() {
}

template<template<class > class G>
int node_parser<G>::parse_data(char* in_pc_buffer, size_t in_sz_buffer) {
	M_BUG();
	return EC_BML_FAILURE;
}

template<template<class > class G>
int node_parser<G>::parse_resource(node_resource_segment<G> & out_c_seg,
		size_t in_sz_buffer) {
	G<node_resource<G>> pc_node(new node_alloc_resource<G>(in_sz_buffer));

	int ec;
	M_ASSERT(pc_node);
	//D("Allocating resource %p", pc_node);

	char * pc_tmp = pc_node->mmap();

	ec = parse_data(pc_tmp, in_sz_buffer);
	if (ec != EC_BML_SUCCESS) {
		return ec;
	}

	out_c_seg = node_resource_segment<G>(pc_node);

	return EC_BML_SUCCESS;
}

/******************************
 * FILE PARSER
 */
template<template<class > class G>
node_file_parser<G>::node_file_parser(std::string const & in_str_path) :
        node_parser<G>() {

    if(!in_str_path.size()){
        return;
    }

	/* Opening file */
	_c_is.open(in_str_path.c_str(), std::ios::binary);

	/* Read first 3 bytes */
	char ac_tmp[4];
	_c_is.read(ac_tmp, 4);

    /* check BML word presence */
    if ((ac_tmp[0] != 'B') || (ac_tmp[1] != 'M') || (ac_tmp[2] != 'L')
        || (ac_tmp[3] != 0)) {
        for (size_t i = 0; i < 4; i++) {
            std::cout << "ac_tmp["<<i<<"] =" << ac_tmp[i] << '\n'; fflush(stdout);
        }
        std::cout << "PATH :" << in_str_path.c_str() <<'\n';fflush(stdout);

        throw std::runtime_error("Invalid BML format");
        //TODO return null pointer
    }

}

template<template<class > class G>
node_file_parser<G>::~node_file_parser() {
	/* Close file */
	_c_is.close();
}

template<template<class > class G>
typename node_file_parser<G>::offset_t node_file_parser<G>::get_pos(void) {
	return _c_is.tellg();
}

template<template<class > class G>
int node_file_parser<G>::set_pos(node_file_parser<G>::offset_t & in_i_offset) {
	try {
		_c_is.seekg(in_i_offset);
	} catch (...) {
		return EC_BML_FAILURE;
	}
	return EC_BML_SUCCESS;
}

template<template<class > class G>
uint64_t node_file_parser<G>::get_size(void) {
	std::streampos c_off;
	std::streampos c_size;
	c_off = _c_is.tellg();
	_c_is.seekg(0, _c_is.end);
	c_size = _c_is.tellg();
	_c_is.seekg(c_off);
	return (uint64_t) c_size;
}

template<template<class > class G>
int node_file_parser<G>::parse_data(char* in_pc_buffer, size_t in_sz_buffer) {
	int i_nb_char_to_store = in_sz_buffer;
	int i_nb_char_stored = 0;
	int i_tmp = 1;

	while (i_nb_char_to_store) {
		if (_c_is.fail()) {
			return EC_BML_FAILURE;
		}
		/* Check end of file */
		if (_c_is.eof() || (i_tmp == 0)) {
			return EC_BML_NODATA;
		}
		/* read data from file */
		i_tmp = _c_is.readsome(&in_pc_buffer[i_nb_char_stored],
				i_nb_char_to_store);
		i_nb_char_stored += i_tmp;
		i_nb_char_to_store -= i_tmp;
		//D("%d %d",i_tmp, _c_is.good());
	}
#if 0
	if (in_sz_buffer < 10) {
		for(int i=0; i<(int)in_sz_buffer; i++) {
			printf("%02x ", (char)in_pc_buffer[i]);
		}
		printf("\n");
	}
#endif
	//printf("%d %d\n", in_sz_buffer,i_nb_char_stored);
	return EC_BML_SUCCESS;
}

/******************************
 * BUFFER PARSER
 */
template<template<class > class G>
node_buffer_parser<G>::node_buffer_parser(char * in_pc_buffer,
		size_t in_sz_buffer) :
		node_parser<G>() {
	_pc_buffer = in_pc_buffer;
	_sz_buffer = in_sz_buffer;
	_i_offset = 0;
}

template<template<class > class G>
node_buffer_parser<G>::~node_buffer_parser() {
}

template<template<class > class G>
int node_buffer_parser<G>::parse_data(char* in_pc_buffer, size_t in_sz_buffer) {
	if ((_i_offset + in_sz_buffer) > _sz_buffer) {
		return EC_BML_NODATA;
	}
	#ifdef DEBUG_PARSER
		D(" - memcpy %p %p %lld %d %d",
				in_pc_buffer, &_pc_buffer[_i_offset],_i_offset, in_sz_buffer, _sz_buffer);
#endif

	/* Copy from buffer */
	memcpy(in_pc_buffer, &_pc_buffer[_i_offset], in_sz_buffer);

	_i_offset += in_sz_buffer;
	return EC_BML_SUCCESS;
}

/******************************
 * SEGMENT PARSER
 */

template<template<class > class G>
node_segment_parser<G>::node_segment_parser(
		node_resource_segment<G> const & in_s_segment) :
		node_buffer_parser<G>(in_s_segment.mmap(), in_s_segment.size()) {
	_pc_parent = &in_s_segment;
}

template<template<class > class G>
node_segment_parser<G>::~node_segment_parser() {
	//_pc_parent->unmmap();
}

template<template<class > class G>
int node_segment_parser<G>::parse_resource(node_resource_segment<G> & out_c_seg,
		size_t in_sz_buffer) {
	if (_i_offset + in_sz_buffer > _sz_buffer) {
		return EC_BML_NODATA;
	}

	out_c_seg.set_from_segment(*_pc_parent, _i_offset, in_sz_buffer);
	_i_offset += in_sz_buffer;
	return EC_BML_SUCCESS;
}

void f_test_node(void);

}

#endif /* NODE_HH_ */
