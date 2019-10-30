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

/** Main include */
#include <bml.h>

/** Local include */
#include <stdio.h>
#include <stdlib.h>

//#define DEBUG

uint8_t bml_get_length(bml_size_t in_i_num) {
	uint8_t i_length;
	bml_size_t i_mask;
	for (i_mask = ~0x00, i_length = 0; i_length < 8; ++i_length, i_mask <<= 8) {
		if ((in_i_num & i_mask) == 0)
			break;
	}
	return i_length;
}

int bml_io_write(struct bml_writer * in_ps_writer, char * in_pc_data,
		bml_size_t in_sz_data) {

	int ec = in_ps_writer->s_cb.cb_io_write(in_ps_writer,
			in_ps_writer->pv_user_arg, in_pc_data, in_sz_data);

	if (ec != EC_BML_SUCCESS) {
		return ec;
	}
	in_ps_writer->i_offset += in_sz_data;
	//printf("%d\n", ec);
	return ec;
}

int bml_io_read(struct bml_parser * in_ps_parser, char * in_pc_data,
		bml_size_t in_sz_data, char in_b_first_header) {

	int ec = in_ps_parser->s_cb.cb_io_read(in_ps_parser,
			in_ps_parser->pv_user_arg, in_pc_data, in_sz_data, in_b_first_header);

	if (ec != EC_BML_SUCCESS) {
		return ec;
	}
	in_ps_parser->i_offset += in_sz_data;

	return ec;
}

int bml_io_seek(struct bml_parser * in_ps_parser, bml_size_t in_sz_data) {

	int ec = in_ps_parser->s_cb.cb_io_seek(in_ps_parser,
			in_ps_parser->pv_user_arg, in_sz_data);
	if (ec != EC_BML_SUCCESS) {
		return ec;
	}

	in_ps_parser->i_offset += in_sz_data;
	return ec;
}

int bml_parser_init(struct bml_parser * in_ps_parser,
		struct bml_parser_callbacks * in_ps_cb, void * in_pv_user_arg) {
	/* Store callbacks */
	in_ps_parser->s_cb = *in_ps_cb;
	in_ps_parser->pv_user_arg = in_pv_user_arg;

	/* Set default state */
	in_ps_parser->b_stop = 0;
	in_ps_parser->i_level = 0;
	in_ps_parser->i_offset = 0;

	/* Check callbacks */
	if (!in_ps_parser->s_cb.cb_io_read) {
		return EC_BML_FAILURE;
	}

	return EC_BML_SUCCESS;
}

int bml_parser_run(struct bml_parser * in_ps_parser) {
	int ec;
	in_ps_parser->b_stop = 0;
	do {

		uint8_t i_length_id_size;
		uint8_t i_length_size_size;
		uint8_t b_ext;
		uint8_t i_flags;
		uint8_t i_ext_count;

		bml_size_t sz_id = 0;
		bml_size_t sz_data = 0;
		char * pc_id = 0;
		char * pc_data = 0;
		bml_off_t off_id = 0;
		bml_off_t off_data = 0;

		struct bml_ext as_ext[C_BML_MAX_EXT];
		uint8_t i_ext_cnt = 0;

		/* Read flags */
		ec = bml_io_read(in_ps_parser, (char*) &i_flags, 1, (in_ps_parser->i_level == 0));
		if (ec != EC_BML_SUCCESS) {
			return ec;
		}
		//printf("H %02x\n",i_flags);
		/* Decode flags */
		i_length_size_size = i_flags & 0xF;
		i_length_id_size = ((i_flags >> 4) & 0x7) + 1;
		b_ext = i_flags & 0x80 ? 1 : 0;
#ifdef DEBUG
		printf("New node (Length Id:%d, Length Size:%d, Ext:%d) [%02x, %d]\n",i_length_id_size, i_length_size_size,b_ext, 0xff&(unsigned int)i_flags, in_ps_parser->i_level);
#endif
		/* Check invalid node */
		if (i_length_size_size == 0xF) {
			int i_node_type = ((i_flags >> 4) & 0x7);
			/* Check alignment node or invalid */
			switch(i_node_type) {
				case 0x7:
					if (in_ps_parser->s_cb.cb_end_element) {
						in_ps_parser->s_cb.cb_end_element(in_ps_parser,
								in_ps_parser->pv_user_arg);
					}
					in_ps_parser->i_level--;
					//printf("++ %d\n",in_ps_parser->i_level);
					break;
				case 0x0:
					//printf("++++ %d\n",in_ps_parser->i_level);
					break;
			}
			continue;
		}

		/* Read id size */
		sz_id = 0;
		ec = bml_io_read(in_ps_parser, (char*) &sz_id, i_length_id_size, 0);
		if (ec != EC_BML_SUCCESS) {
			goto out_err;
		}
		off_id = in_ps_parser->i_offset;
#ifdef DEBUG
		printf("- ID size: %d (%d bytes)\n",(int)sz_id, i_length_id_size);
#endif
		/* Allocate id  */
		if (in_ps_parser->s_cb.cb_new_id) {
			in_ps_parser->s_cb.cb_new_id(in_ps_parser,
					in_ps_parser->pv_user_arg, &pc_id, sz_id);

			if (!pc_id) {
				ec = EC_BML_FAILURE;
				fprintf(stderr, "Unable to allocate memory of id\n");
				goto out_err;
			}

			/* Read id */
			ec = bml_io_read(in_ps_parser, (char*) pc_id, sz_id, 0);
			if (ec != EC_BML_SUCCESS) {
				return ec;
			}
#ifdef DEBUG
			printf("- ID on %d bytes\n",(int)sz_id);
#endif
		} else if (in_ps_parser->s_cb.cb_io_seek) {
			ec = bml_io_seek(in_ps_parser, sz_data);
			if (ec != EC_BML_SUCCESS) {
				return ec;
			}
		} else {
			ec = EC_BML_FAILURE;
			fprintf(stderr,
					"Either new id or seek callback must be provided\n");
			goto out_err;
		}

		/* Read Ext */
		i_ext_count = 0;
		while (b_ext) {
			uint8_t i_flags_ext;
			uint8_t sz_ext_size;
			uint8_t i_ext_id;
			char * pc_ext = 0;
			bml_size_t sz_ext = 0;
			bml_off_t off_ext = 0;

			/* Read flags */
			ec = bml_io_read(in_ps_parser, (char*) &i_flags_ext, 1, 0);
			if (ec != EC_BML_SUCCESS) {
				fprintf(stderr, "Unable to read ext header\n");
				return ec;
			}

			/* Decode flags */
			i_ext_id = i_flags_ext & 0xF;
			sz_ext_size = ((i_flags_ext >> 4) & 0x7) + 1;
			b_ext = i_flags_ext & 0x80 ? 1 : 0;
#ifdef DEBUG
			printf("- EXT Header (Id:%d, Size:%d, Ext:%d) [%02x]\n", i_ext_id, sz_ext_size,b_ext, 0xff&(unsigned int)i_flags_ext);
#endif
			/* Read ext size */
			sz_ext = 0;
			ec = bml_io_read(in_ps_parser, (char*) &sz_ext, sz_ext_size, 0);
			if (ec != EC_BML_SUCCESS) {
				goto out_err;
			}
			off_ext = in_ps_parser->i_offset;
#ifdef DEBUG
			printf("- EXT size: %d (%d bytes)\n",(int) sz_ext, sz_ext_size);
#endif

			/* Allocate id  */
			if (in_ps_parser->s_cb.cb_new_ext) {
				in_ps_parser->s_cb.cb_new_ext(in_ps_parser,
						in_ps_parser->pv_user_arg, &pc_ext, sz_ext, i_ext_count > C_BML_MAX_EXT_MIXED ? i_ext_count : i_ext_id );

				if (!pc_ext) {
					ec = EC_BML_FAILURE;
					fprintf(stderr, "Unable to allocate memory of ext\n");
					goto out_err;
				}

				/* Read ext data */
				ec = bml_io_read(in_ps_parser, (char*) pc_ext, sz_ext, 0);
				if (ec != EC_BML_SUCCESS) {
					fprintf(stderr, "Unable to read ext data\n");
					return ec;
				}
			} else if (in_ps_parser->s_cb.cb_io_seek) {

				ec = bml_io_seek(in_ps_parser, sz_ext);
				if (ec != EC_BML_SUCCESS) {
					fprintf(stderr, "Unable to seek ext data\n");
					return ec;
				}
			} else {
				ec = EC_BML_FAILURE;
				fprintf(stderr,
						"Either data new or seek callback must be provided\n");
				goto out_err;
			}

#ifdef DEBUG
			printf("- EXT (%d bytes)\n",(int) sz_ext);
#endif
			/* Store ext into array */
			if (i_ext_cnt < C_BML_MAX_EXT) {
				as_ext[i_ext_cnt].i_id = i_ext_id;
				as_ext[i_ext_cnt].off_data = off_ext;
				as_ext[i_ext_cnt].pv_data = pc_ext;
				as_ext[i_ext_cnt].sz_data = sz_ext;
				i_ext_cnt++;
			} else {
				fprintf(stderr, "Ext counter reach max !\n");
			}

			i_ext_count++;
		}

		if (i_length_size_size) {
			/* Read data size */
			ec = bml_io_read(in_ps_parser, (char*) &sz_data,
					i_length_size_size, 0);
			if (ec != EC_BML_SUCCESS) {
				fprintf(stderr, "Unable to read data size: %d\n", i_length_size_size);
				goto out_err;
			}
			off_data = in_ps_parser->i_offset;

#ifdef DEBUG
			printf("- Data on %d bytes\n",(int)sz_data);
#endif
			/* Allocate id  */
			if (in_ps_parser->s_cb.cb_new_data) {
				in_ps_parser->s_cb.cb_new_data(in_ps_parser,
						in_ps_parser->pv_user_arg, &pc_data, sz_data);

				if (!pc_data) {
					ec = EC_BML_FAILURE;
					fprintf(stderr, "Unable to allocate memory of data\n");
					goto out_err;
				}

				/* Read data */
				ec = bml_io_read(in_ps_parser, (char*) pc_data, sz_data, 0);
				if (ec != EC_BML_SUCCESS) {
					fprintf(stderr, "Unable to read data: %d bytes\n", (int)sz_data);
					return ec;
				}
			} else if (in_ps_parser->s_cb.cb_io_seek) {

				ec = bml_io_seek(in_ps_parser, sz_data);
				if (ec != EC_BML_SUCCESS) {
					fprintf(stderr, "Unable to seek data\n");
					return ec;
				}
			} else {
				ec = EC_BML_FAILURE;
				fprintf(stderr,
						"Either data new or seek callback must be provided\n");
				goto out_err;
			}
		}

		if (in_ps_parser->s_cb.cb_start_element) {
			in_ps_parser->s_cb.cb_start_element(in_ps_parser,
					in_ps_parser->pv_user_arg, pc_id, sz_id, off_id, pc_data,
					sz_data, off_data, as_ext, i_ext_cnt,
					in_ps_parser->i_level);
		}
		//printf("++ %d\n",in_ps_parser->i_level);
		in_ps_parser->i_level++;


	} while (!in_ps_parser->b_stop);

	ec = EC_BML_SUCCESS;
	out_err: return ec;
}
int bml_parser_stop(struct bml_parser * in_ps_parser) {
	in_ps_parser->b_stop = 1;
	return EC_BML_SUCCESS;
}

int bml_writer_init(struct bml_writer * in_ps_writer,
		struct bml_writer_callbacks * in_ps_cb, void * in_pv_user_arg) {
	/* Store callbacks */
	in_ps_writer->s_cb = *in_ps_cb;
	in_ps_writer->pv_user_arg = in_pv_user_arg;
	in_ps_writer->i_offset = 0;

	if (!in_ps_writer->s_cb.cb_io_write) {
		return EC_BML_FAILURE;
	}

	return EC_BML_SUCCESS;
}

int bml_writer_element_start(struct bml_writer * in_ps_writer, void * in_pv_id,
		bml_size_t in_sz_id, void * in_pv_data, bml_size_t in_sz_data,
		struct bml_ext * in_as_ext, uint8_t in_i_nb_ext, bml_size_t in_i_length_id_size, bml_size_t in_i_length_size_size, bml_size_t in_i_length_ext_size_size) {
	int ec;

	uint8_t i_length_id_size = bml_get_length(in_sz_id);
	uint8_t i_length_size_size = bml_get_length(in_sz_data);
	int i;
#ifdef DEBUG
	printf("Arg: %d %d\n",(int)in_i_length_id_size, (int)in_i_length_size_size);
#endif

	/* If provided use user size */
	if(in_i_length_id_size) {
		if(i_length_id_size > in_i_length_id_size) {
			fprintf(stderr,"Size of id too small\n");
			return EC_BML_FAILURE;
		}
		i_length_id_size = in_i_length_id_size;
	}

	/* If provided use user size */
	if(in_i_length_size_size) {
		if(i_length_size_size > in_i_length_size_size) {
			fprintf(stderr,"Size of id too small\n");
			return EC_BML_FAILURE;
		}
		i_length_size_size = in_i_length_size_size;
	}


#ifdef DEBUG
	printf("Add: %p %d %p %d\n", in_pv_id, (int)in_sz_id, in_pv_data, (int)in_sz_data);
#endif
	/* Generate flags */
	uint8_t i_flags = (i_length_id_size - 1) & 0x7;
	if (in_i_nb_ext) {
		i_flags |= 0x8;
	}
	i_flags <<= 4;
	i_flags |= i_length_size_size & 0xF;

	//printf("%d %s\n", __LINE__,__FUNCTION__);
	/* Write flags */
	ec = bml_io_write(in_ps_writer, (char*) &i_flags, 1);
	if (ec != EC_BML_SUCCESS) {
		return ec;
	}
	//printf("%d %s\n", __LINE__,__FUNCTION__);
	/* Write ID size */
	ec = bml_io_write(in_ps_writer, (char*) &in_sz_id, i_length_id_size);
	if (ec != EC_BML_SUCCESS) {
		return ec;
	}
	//printf("%d %s\n", __LINE__,__FUNCTION__);
	/* Write ID */
	ec = bml_io_write(in_ps_writer, (char*) in_pv_id, in_sz_id);
	if (ec != EC_BML_SUCCESS) {
		return ec;
	}
	//printf("%d %s\n", __LINE__,__FUNCTION__);
	/* Write exts */
	for (i = 0; i < in_i_nb_ext; i++) {
		bml_size_t sz_ext = in_as_ext[i].sz_data;
		uint8_t i_ext_size_size = bml_get_length(sz_ext);


		/* Bypass if no size */
		if(!sz_ext) continue;

		/* Align ext */
		if(in_i_length_ext_size_size) {
			if(i_ext_size_size > in_i_length_ext_size_size) {
				fprintf(stderr,"Size of id too small\n");
				return EC_BML_FAILURE;
			}
			i_ext_size_size = in_i_length_ext_size_size;
		}

		uint8_t i_ext_flags = (i_ext_size_size - 1) & 0x7;

		/* Check last ext */
		if (i != (in_i_nb_ext - 1)) {
			i_ext_flags |= 0x8;
		}

		i_ext_flags <<= 4;
		i_ext_flags |= (in_as_ext[i].i_id) & 0xF;

#ifdef DEBUG
	printf("Add Ext: Flags: %x SZSZ:%d P:%p SZ:%d\n", i_ext_flags, (int)i_ext_size_size, &sz_ext, (int)sz_ext);
#endif
		/* Write flags */
		ec = bml_io_write(in_ps_writer, (char*) &i_ext_flags, 1);
		if (ec != EC_BML_SUCCESS) {
			return ec;
		}

		/* Write ext size */
		ec = bml_io_write(in_ps_writer, (char*) &sz_ext, i_ext_size_size);
		if (ec != EC_BML_SUCCESS) {
			return ec;
		}

		/* Write ext data */
		ec = bml_io_write(in_ps_writer, (char*) in_as_ext[i].pv_data,
				sz_ext);
		if (ec != EC_BML_SUCCESS) {
			return ec;
		}
	}

	if (i_length_size_size) {
		/* Write Data size */
		ec = bml_io_write(in_ps_writer, (char*) &in_sz_data,
				i_length_size_size);
		if (ec != EC_BML_SUCCESS) {
			return ec;
		}
    if(in_sz_data) {
		  /* Write Data */
		  ec = bml_io_write(in_ps_writer, (char*) in_pv_data, in_sz_data);
		  if (ec != EC_BML_SUCCESS) {
			  return ec;
		  }
		}
	}

// TODO EXT

	return EC_BML_SUCCESS;
}
int bml_writer_align(struct bml_writer * in_ps_writer, bml_size_t in_i_alignment) {
	int i;
	int ec;
	/* Write alignement */
	for(i = in_ps_writer->i_offset&(in_i_alignment-1); i != 0; i = (i+1)&(in_i_alignment-1)) {
		char i_invalid_word = 0x0F;
		ec = bml_io_write(in_ps_writer, &i_invalid_word, 1);
			if (ec != EC_BML_SUCCESS) {
				return ec;
			}
	}
	return EC_BML_SUCCESS;
}

int bml_writer_prealign(struct bml_writer * in_ps_writer, bml_size_t in_i_alignment, bml_size_t in_i_offset) {
	int i;
	int ec;
	/* Write alignement */
	for(i = (in_ps_writer->i_offset+in_i_offset)&(in_i_alignment-1); i != 0; i = (i+1)&(in_i_alignment-1)) {
		char i_invalid_word = 0x0F;
		ec = bml_io_write(in_ps_writer, &i_invalid_word, 1);
			if (ec != EC_BML_SUCCESS) {
				return ec;
			}
	}
	return EC_BML_SUCCESS;
}

int bml_writer_element_end(struct bml_writer * in_ps_writer) {
	int ec;
	char i_invalid_word = 0xFF;
#ifdef DEBUG
	printf("Add invalid node\n");
#endif

	/* Write Data */
	ec = bml_io_write(in_ps_writer, &i_invalid_word, 1);
	if (ec != EC_BML_SUCCESS) {
		return ec;
	}

	return EC_BML_SUCCESS;
}
