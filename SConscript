#!/usr/bin/python
Import('bs')


sw_dict = {
	'name':'node',
	'type':'library',
	'sources':'bml_node.cc',
	'includes':['./'],
	'libs':{'shared':[], 'static':[]}
}

bs.AddSW(sw_dict)

sw_dict = {
    'name':'bml-sab',
    'type':'library',
    'sources':'sab_parser/*.c',
    'includes':['./sab_parser/'],
    'libs':{'shared':[], 'static':[]}
}

bs.AddSW(sw_dict)
