/*
 * bml_node.cc
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

#include <bml_node.hh>

#include <assert.h>
#include <stdexcept>

#include <string.h>
#include <stdlib.h>
#include <sys/mman.h>

using namespace bml;

std::list<void *> bml::gl_dbg;

void bml::f_node_dbg_push(void * in_ptr) {
	gl_dbg.push_back(in_ptr);
}
bool bml::f_node_dbg_in(void * in_ptr) {
	for (std::list<void *>::iterator pc_it = gl_dbg.begin();
			pc_it != gl_dbg.end(); pc_it++) {
		if (*pc_it == in_ptr) {
			return true;
		}
	}
	return false;
}

uint32_t f_bml_crc(char * in_pc_tmp, size_t in_sz) {
	uint32_t i_check = 0;
	for (size_t i = 0; i < in_sz; i++) {
		i_check = (*in_pc_tmp) ^ i_check;
		i_check <<= 1;
		in_pc_tmp++;
	}
	return i_check;
}


using namespace std;
