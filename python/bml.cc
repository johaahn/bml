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

#include <bml.h>
#include <boost/shared_ptr.hpp>
#include <stdint.h>
#include <list>
#include <map>
#include <string>
#include <assert.h>
#include <string.h>
#include <iostream>

//*********** BOOST PYTHON SPECIFIC *****************
#include <boost/python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#include <boost/python/suite/indexing/map_indexing_suite.hpp>
#include <boost/python/class.hpp>
#include <boost/python/module_init.hpp>
#include <boost/python/def.hpp>
#include <boost/python/call_method.hpp>
#include <boost/ref.hpp>
#include <boost/utility.hpp>

using namespace boost::python;

#define M_ALIGN(x,n) (((x)+(n)-1) & (~((n)-1)))
//#define DEBUG

class exception: public std::exception {
private:
	std::string message;
public:
	exception(std::string message) {
		this->message = message;

	}
	const char *what() const throw () {
		return this->message.c_str();
	}
	~exception() throw () {
	}
	std::string getMessage() {
		return this->message;
	}
};

//typedef std::vector<char> node_data;
class node;
class parser;
class writer;

class parser  {
	node * _pc_node;
public:
	bml_parser s_parser;
	bool b_end_element;
	parser(void);
	virtual ~parser();
	void set_current_node(node * in_pc_node);
	node * get_current_node(void);
	virtual int internal_io_read(char * in_pc_data, size_t in_sz_data_max,
			char in_b_first_header) = 0;
};

class writer {
public:
	bml_writer s_writer;
	writer(void);
	virtual ~writer();
	virtual int internal_io_write(char * in_pc_data, bml_size_t in_sz_data) = 0;
};

typedef std::map<int, std::string> node_ext;

class node {
public:
	typedef std::list<boost::shared_ptr<node> >::iterator childs_iterator;
	typedef std::map<int, std::string>::iterator ext_iterator;

private:
	std::list<boost::shared_ptr<node> > _childs;
	bool _invalid;
	node_ext _exts;

public:
	int32_t id;
	std::vector<unsigned char> data;

private:
	void clear(void);

public:
	node(void);
	node(int32_t in_id);

	/* child handling */
	void append(boost::shared_ptr<node> & in_node);
	boost::shared_ptr<node> get(int32_t in_id, uint32_t in_index = 0);
	bool has(int32_t in_id, uint32_t in_index = 0);

	/* Extensions */
	void set_ext(int32_t in_id, std::string in_value);
	std::string get_ext(int32_t in_id);

	/* Writer/Parser */
	void to_writer(writer & in_c_writer, int in_i_align);
	int from_parser(parser & in_c_parser);

	/* BML callbacks */
	void bml_start_element(void * in_pv_id, bml_size_t in_sz_id,
			bml_off_t in_off_id, void * in_pv_data, bml_size_t in_sz_data,
			bml_off_t in_off_data, struct bml_ext * in_as_ext,
			int32_t in_i_nb_ext, int32_t in_i_level);
	//void bml_end_element(void);
	int bml_new_id(char ** in_ppc_id, bml_size_t in_sz_id);
	int bml_new_data(char ** in_ppc_data, bml_size_t in_sz_data);
	int bml_new_ext(char ** in_ppc_data, bml_size_t in_sz_data,
			uint8_t in_i_ext_id);
	bool is_invalid(void);
};

void cb_start_element(struct bml_parser * in_ps_ctx, void * in_pv_user_arg,
		void * in_pv_id, bml_size_t in_sz_id, bml_off_t in_off_id,
		void * in_pv_data, bml_size_t in_sz_data, bml_off_t in_off_data,
		struct bml_ext * in_as_ext, int32_t in_i_nb_ext, int32_t in_i_level) {
	//int i;
	//class parser* pc_parser = (class parser*) in_pv_user_arg;
	parser * pc_parser = (class parser*) in_pv_user_arg;
	pc_parser->get_current_node()->bml_start_element(in_pv_id, in_sz_id,
			in_off_id, in_pv_data, in_sz_data, in_off_data, in_as_ext,
			in_i_nb_ext, in_i_level);

	pc_parser->b_end_element = false;
	bml_parser_stop(in_ps_ctx);
}

void cb_end_element(struct bml_parser * in_ps_ctx, void *in_pv_user_arg) {
	//class parser* pc_parser = (class parser*) in_pv_user_arg;
	//ps_handle->ps_result->i_level = ps_result->i_level-1;
	//pc_parser->bml_end_element();
	parser * pc_parser = (class parser*) in_pv_user_arg;

	pc_parser->b_end_element = true;
	bml_parser_stop(in_ps_ctx);
}
int cb_new_id(struct bml_parser * in_ps_ctx, void *in_pv_user_arg,
		char ** in_ppc_id, bml_size_t in_sz_id) {
	parser * pc_parser = (class parser*) in_pv_user_arg;
	return pc_parser->get_current_node()->bml_new_id(in_ppc_id, in_sz_id);
}
int cb_new_data(struct bml_parser * in_ps_ctx, void *in_pv_user_arg,
		char ** in_ppc_data, bml_size_t in_sz_data) {
	parser * pc_parser = (class parser*) in_pv_user_arg;
	return pc_parser->get_current_node()->bml_new_data(in_ppc_data, in_sz_data);
}
int cb_new_ext(struct bml_parser * in_ps_ctx, void *in_pv_user_arg,
		char ** in_ppc_data, bml_size_t in_sz_data, uint8_t in_i_ext_id) {
	parser * pc_parser = (class parser*) in_pv_user_arg;
	return pc_parser->get_current_node()->bml_new_ext(in_ppc_data, in_sz_data,
			in_i_ext_id);
}
int cb_io_read(struct bml_parser * in_ps_ctx, void *in_pv_user_arg,
		char * in_pc_data, bml_size_t in_sz_data_max, char in_b_first_header) {
	parser * pc_parser = (class parser*) in_pv_user_arg;
	return pc_parser->internal_io_read(in_pc_data, in_sz_data_max, in_b_first_header);
}
int cb_io_write(struct bml_writer * in_ps_ctx, void *in_pv_user_arg,
		char * in_pc_data, bml_size_t in_sz_data) {
	writer * pc_writer = (class writer*) in_pv_user_arg;
	return pc_writer->internal_io_write(in_pc_data, in_sz_data);
}

parser::parser() {

	int ec;
	bml_parser_callbacks s_cb;
	s_cb.cb_start_element = cb_start_element;
	s_cb.cb_end_element = cb_end_element;
	s_cb.cb_io_read = cb_io_read;
	s_cb.cb_io_seek = NULL;
	s_cb.cb_new_id = cb_new_id;
	s_cb.cb_new_data = cb_new_data;
	s_cb.cb_new_ext = cb_new_ext;

	ec = bml_parser_init(&s_parser, &s_cb, this);
	if (ec != EC_BML_SUCCESS) {
		throw exception("Unable to initialize parser");
	}

	_pc_node = NULL;

}

parser::~parser() {

}
void parser::set_current_node(node * in_pc_node) {
	_pc_node = in_pc_node;
}

node * parser::get_current_node(void) {
	return _pc_node;
}

writer::writer(void) {
	int ec;
	struct bml_writer_callbacks s_cb;
	s_cb.cb_io_write = cb_io_write;

	ec = bml_writer_init(&s_writer, &s_cb, this);
	if (ec != EC_BML_SUCCESS) {
		throw exception("Unable to initialize writer");
	}
}
writer::~writer() {
}

node::node(void) {
	clear();
}

node::node(int32_t in_id) {
	clear();
	id = in_id;
}

void node::clear(void) {
	id = 0;
	_childs.clear();
	_invalid  = true;
}

void node::set_ext(int32_t in_id, std::string in_value) {
	_exts[in_id] = in_value;
}

std::string node::get_ext(int32_t in_id) {
	return _exts[in_id];
}

void node::append(boost::shared_ptr<node> & in_node) {
	_childs.push_back(in_node);
}

boost::shared_ptr<node> node::get(int32_t in_id, uint32_t in_index) {
	childs_iterator pc_it;
	for (pc_it = _childs.begin(); pc_it != _childs.end(); pc_it++) {
		if ((**pc_it).id == in_id) {
			if (in_index == 0) {
				break;
			}
			in_index--;
		}
	}

	if (pc_it != _childs.end()) {
		return *pc_it;
	}
	throw exception("Throwing an exception as requested.");
}

bool node::has(int32_t in_id, uint32_t in_index) {
	childs_iterator pc_it;
	for (pc_it = _childs.begin(); pc_it != _childs.end(); pc_it++) {
		if ((**pc_it).id == in_id) {
			if (in_index == 0) {
				break;
			}
			in_index--;
		}
	}

	if (pc_it != _childs.end()) {
		return true;
	}
	return false;
}

void node::to_writer(class writer & in_c_writer, int in_i_align) {
	int ec;
	unsigned char * pc_buf = &data[0];
	size_t sz_buf = data.size();

	size_t i_nb_ext = _exts.size();
	bml_ext as_ext[i_nb_ext];
	/* Filling extensions */
	{
		ext_iterator pc_it;
		int i=0;
		for (pc_it = _exts.begin(); pc_it != _exts.end(); pc_it++) {
			as_ext[i].i_id = pc_it->first;
			as_ext[i].pv_data = &(pc_it->second)[0];
			as_ext[i].sz_data = pc_it->second.size();
		}
	}

	//std::cout << "DBG " << __LINE__ << " " <<   sizeof(id) << " " << id << std::endl;

	/* Write data */
	if (in_i_align) {
		ec = bml_writer_element_start(&in_c_writer.s_writer, (char*) &id, in_i_align,
				pc_buf, M_ALIGN(sz_buf, in_i_align), as_ext, i_nb_ext,
				in_i_align - 1, in_i_align, in_i_align - 1);
		if (ec != EC_BML_SUCCESS) {
			throw exception("Unable to write start element");
		}
	} else {
		ec = bml_writer_element_start(&in_c_writer.s_writer, &id, sizeof(id), pc_buf,
				sz_buf, as_ext, i_nb_ext, 0, 0, 0);
		if (ec != EC_BML_SUCCESS) {
			throw exception("Unable to write start element");
		}
	}

	/* Align next node */
	if (in_i_align) {
		ec = bml_writer_align(&in_c_writer.s_writer, in_i_align);
		if (ec != EC_BML_SUCCESS) {
			throw exception("Unable to write align element");
		}
	}

	{
		childs_iterator pc_it;
		for (pc_it = _childs.begin(); pc_it != _childs.end(); pc_it++) {
			(**pc_it).to_writer(in_c_writer, in_i_align);
		}
	}

	/* Write end node */
	ec = bml_writer_element_end(&in_c_writer.s_writer);
	if (ec != EC_BML_SUCCESS) {
		throw exception("Unable to write align element");
	}

	/* Align next node */
	if (in_i_align) {
		ec = bml_writer_align(&in_c_writer.s_writer, in_i_align);
		if (ec != EC_BML_SUCCESS) {
			throw exception("Unable to write align element");
		}
	}
}

int node::from_parser(parser & in_c_parser) {
	int ec;

	clear();

	/* set current node in parser */
	in_c_parser.set_current_node(this);


	/* Execute parser */
#ifdef DEBUG
	std::cout << this << " -- RUN START" << std::endl;
#endif
	/* Reset end flag */
	in_c_parser.b_end_element = false;

	ec = bml_parser_run(&in_c_parser.s_parser);
	if (ec != EC_BML_SUCCESS) {
#ifdef DEBUG
		std::cout << this << " -- RUN END ERROR/TIMEOUT" << std::endl;
#endif
		return ec;
	}

	/* On end element stop parsing childs */
	if(is_invalid()) {
#ifdef DEBUG
		std::cout << this << " -- RUN END INVALID" << std::endl;
#endif
		return EC_BML_SUCCESS;
	}
#ifdef DEBUG
	else {

		std::cout << this << " -- RUN END NEW CHILD" << std::endl;

	}
	std::cout << this << " -- RUN END CURRENT" << std::endl;
#endif
	while (1) {
#ifdef DEBUG
		std::cout << this << " -- RUN START CHILD" << std::endl;
#endif
		boost::shared_ptr<node> pc_new(new node());


		ec = pc_new->from_parser(in_c_parser);
		if(ec != EC_BML_SUCCESS) {
#ifdef DEBUG
			std::cout << this << " -- RUN END CHILD ERROR" << std::endl;
#endif
			break;
		}

		if (pc_new->is_invalid()) {
#ifdef DEBUG
			std::cout << this << " -- RUN END CHILD INVALID" << std::endl;
#endif
			ec = EC_BML_SUCCESS;
			break;
		}
#ifdef DEBUG
		std::cout << this << " -- RUN END CHILD" << std::endl;
		std::cout << "Appending NEW NODE:" << pc_new->id<< std::endl;
#endif
		append(pc_new);
	}
#ifdef DEBUG
	std::cout << this << " -- RUN END" << std::endl;
#endif

	return ec;
}

void node::bml_start_element(void * in_pv_id, bml_size_t in_sz_id,
		bml_off_t in_off_id, void * in_pv_data, bml_size_t in_sz_data,
		bml_off_t in_off_data, struct bml_ext * in_as_ext, int32_t in_i_nb_ext,
		int32_t in_i_level) {
	_invalid = false;
#ifdef DEBUG
	std::cout << "ID:"<< id << std::endl;
#endif
}
#if 0
void node::bml_end_element(void) {
	_b_end = true;
}
#endif
#if 0
int node::bml_new_id(char ** in_ppc_id, bml_size_t in_sz_id) {
	assert(in_sz_id == sizeof(id));
	*in_ppc_id = (char *) &id;
	return EC_BML_SUCCESS;
}
#endif

int node::bml_new_id(char ** in_ppc_id, bml_size_t in_sz_id) {
	//INTEGER CASE LITTLE ENDIAN ONLY
	//std::cout << "Size ID:"<< in_sz_id << std::endl;
	assert(in_sz_id <= sizeof(id));
	*in_ppc_id = (char *) &id;
	return EC_BML_SUCCESS;
}

int node::bml_new_data(char ** in_ppc_data, bml_size_t in_sz_data) {
	data.resize(in_sz_data);
	*in_ppc_data = (char *) &data[0];
	return EC_BML_SUCCESS;
}

int node::bml_new_ext(char ** in_ppc_data, bml_size_t in_sz_data,
		uint8_t in_i_ext_id) {
	std::string & str_tmp = _exts[in_i_ext_id];
	str_tmp.resize(in_sz_data);
	*in_ppc_data = (char *) &str_tmp[0];
	return EC_BML_SUCCESS;
}

bool node::is_invalid(void) {
	return _invalid;
}



struct writer_callback: writer {
	writer_callback(PyObject *p) :
			self(p) {
	}
	int internal_io_write(char * in_pc_data, bml_size_t in_sz_data) {

		object in_data = object(handle<>(PyMemoryView_FromMemory(in_pc_data, in_sz_data, PyBUF_READ)));
		if(call_method<int>(self, "io_write", in_data))
		{
			return EC_BML_FAILURE;
		}
		return EC_BML_SUCCESS;
	}

	PyObject *self;
};

struct parser_callback: parser {
	parser_callback(PyObject *p) :
			self(p) {
	}

	int internal_io_read(char * in_pc_data, size_t in_sz_data_max,
		char in_b_first_header) {
		int ec;
#ifdef DEBUG
	std::cout << this << " -- IO_READ START" << std::endl;
#endif

		std::string in_data = call_method < std::string > (self, "io_read", in_sz_data_max, int(in_b_first_header));

			if (!in_data.size()) {
				ec = EC_BML_NODATA;
			} else if ((size_t)in_data.size() != in_sz_data_max) {
				ec = EC_BML_FAILURE;
			} else {
				memcpy(in_pc_data, in_data.c_str(), in_data.size());
				ec = EC_BML_SUCCESS;
			}
#if 0
	std::cout << this << " -- IO_READ 2" << std::endl;
		if (!PyMemoryView_Check(in_data.ptr())) {
			std::cout << "Returned buffer is not a memory view" << std::endl;
			ec = EC_BML_FAILURE;
		} else {

	std::cout << this << " -- IO_READ 3" << std::endl;
			Py_buffer * py_buffer = PyMemoryView_GET_BUFFER(in_data.ptr());

	std::cout << this << " -- IO_READ 4" << std::endl;
			if (!py_buffer->len) {
				ec = EC_BML_NODATA;
			} else if ((size_t)py_buffer->len != in_sz_data_max) {
				ec = EC_BML_FAILURE;
			} else {
				memcpy(in_pc_data, py_buffer->buf, py_buffer->len);
				ec = EC_BML_SUCCESS;
			}
		}
#endif
#ifdef DEBUG
	std::cout << this << " -- IO_READ END" << std::endl;
#endif
		return ec;
	}

	PyObject *self;
};

PyObject *exception_type = NULL;

void translate_exception(exception const &e) {
	assert(exception_type != NULL);
	boost::python::object pythonExceptionInstance(e);
	PyErr_SetObject(exception_type, pythonExceptionInstance.ptr());
}

template <typename T>
struct vector_to_list
{
  static PyObject* convert(const std::vector<T>& container)
  {

    boost::python::object memoryView(boost::python::handle<>(PyMemoryView_FromMemory((char*)container.data(), container.size(), PyBUF_READ)));
    return boost::python::incref(memoryView.ptr());
  }
};

template<typename T>
struct vector_from_list
{

    vector_from_list()
    {
      boost::python::converter::registry::push_back(&vector_from_list<T>::convertible,
			  &vector_from_list<T>::construct,
			  boost::python::type_id<std::vector<T,std::allocator<T> > >());
    }

    // Determine if obj_ptr can be converted in a std::vector<T>
    static void* convertible(PyObject* obj_ptr)
    {

      if (PyList_Check(obj_ptr)){
        return obj_ptr;
      } else if (PyBytes_Check(obj_ptr)) {
        return obj_ptr;
      }
      return 0;
    }

    // Convert obj_ptr into a std::vector<T>
    static void construct(
    PyObject* obj_ptr,
    boost::python::converter::rvalue_from_python_stage1_data* data)
    {

      using namespace boost::python;
      // Grab pointer to memory into which to construct the new std::vector<T>
      void* storage = ((boost::python::converter::rvalue_from_python_storage<std::vector<T> >*)data)->storage.bytes;
      // in-place construct the new std::vector<T> using the character data
      // extraced from the python object
      std::vector<T>& v = *(new (storage) std::vector<T>());

      // Extract the character data from the python string
      //      const char* value = PyString_AsString(obj_ptr);
      if (PyList_Check(obj_ptr)) {
        list l(handle<>(borrowed(obj_ptr)));

        // populate the vector from list contains !!!
        int le = len(l);
        v.resize(le);
        for(int i = 0;i!=le;++i){
          v[i] = extract<T>(l[i]);
        }
      } else if (PyBytes_Check(obj_ptr)) {
        const char* value = PyBytes_AsString(obj_ptr);
        int le = PyBytes_Size(obj_ptr);
        v.resize(le);
        for(int i = 0;i!=le;++i){
          v[i] = value[i];
        }
      } else {
        std::cout << " -- Unknown type !" << std::endl;
      }

      // Stash the memory chunk pointer for later use by boost.python
      data->convertible = storage;
    }
};


using namespace boost::python;
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(node_get_overloads, node::get, 1, 2)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(node_has_overloads, node::has, 1, 2)

void initializeConverters()
{
  /* Enable std::list<int> to Python list conversion. */
  boost::python::to_python_converter<std::vector<unsigned char>, vector_to_list<unsigned char> >();
  vector_from_list<unsigned char>();
}

BOOST_PYTHON_MODULE(bml)
{
/* Exception class */
	boost::python::class_<exception> exception_class("exception",
			boost::python::init<std::string>());
	exception_class.add_property("message", &exception::getMessage);
	exception_type = exception_class.ptr();
	boost::python::register_exception_translator<exception>(
			&translate_exception);


  initializeConverters();

/* BML writer */
	class_<writer, boost::noncopyable, boost::shared_ptr<writer_callback> >(
			"writer");

/* BML parser */
	class_<parser, boost::noncopyable, boost::shared_ptr<parser_callback> >(
			"parser");

/* Node class */
	class_<node, boost::shared_ptr<node> >("node", init<int32_t>())
	    .def(init<>())
			.def_readwrite("id", &node::id)
			.add_property("data",boost::python::make_getter(&node::data,
          boost::python::return_value_policy<boost::python::return_by_value>()),boost::python::make_setter(&node::data,
          boost::python::return_value_policy<boost::python::return_by_value>()))
			.def("append", &node::append)
			.def("set_ext", &node::set_ext)
			.def("get_ext", &node::get_ext)
			.def("get", &node::get,	node_get_overloads(args("in_index"), "node index in list"))
			.def("has", &node::has, node_has_overloads(args("in_index"), "node index in list"))
			.def("to_writer", &node::to_writer)
			.def("from_parser", &node::from_parser)
			.def("is_invalid", &node::is_invalid);

}
