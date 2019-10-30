lib_LTLIBRARIES = libbml.la
libbml_la_SOURCES = bml_node.cc
bmlincludedir = $(includedir)/bml
bmlinclude_HEADERS = bml_node.hh


TESTS = bml_test
check_PROGRAMS = bml_test
bml_test_SOURCES = bml_test.cc
bml_test_CFLAGS = @CHECK_CFLAGS@
bml_test_LDADD = $(top_builddir)/libbml.la @CHECK_LIBS@
