#include <iostream>
#include <memory>
#include <string>
#include <bml_node.hh>


#include <check.h>


START_TEST(test_write_read_from_buffer) {

} END_TEST

START_TEST(test_write_read_from_file) {

} END_TEST

START_TEST(test_copy) {

} END_TEST



START_TEST(test_segment_offset) {

} END_TEST

START_TEST(test_segment_memcpy) {

} END_TEST


START_TEST(test_external_segment) {

} END_TEST


START_TEST(test_find_childs) {

} END_TEST


START_TEST(test_add_get_childs) {

} END_TEST

START_TEST(test_mmap) {

} END_TEST

START_TEST(test_doc) {

  uint64_t i_udata64 = 0xDEADBEEFFEEDDADA;
  uint8_t i_data8 = 10;

  /*** BML CHEAT SHEET ***/

  /* Create an empty node with no id */
  node<uint32_t, std::shared_ptr> c_example_node;

  /* Create an empty node with ID = 1234 */
  {
    node<uint32_t, std::shared_ptr> c_node(1234);
  }

  /* Create a node with data */
  {
    uint32_t i_data = 10;
    node<uint32_t, std::shared_ptr> c_node(20, i_data);
  }

  /* Set id of current node */
  c_example_node.set_id(1234);


  /************** Updating data ****************/

  /* Set data of current node (uint64_t)*/
  c_example_node.set_data(i_udata64);

  /* Get data of current node (uint64_t)*/
  uint64_t i_udata64_2 = c_example_node.get_data<uint64_t>();
  //std::cout <<i_udata64 << " "<<i_udata64_2<< std::endl;
  ck_assert(i_udata64_2 == i_udata64);

  /* Set data on it with cast (int32_t)*/
  c_example_node.set_data<uint32_t>(i_data8);

  /* Get data of current node (int32_t)*/
  uint32_t i_tmp = c_example_node.get_data<uint32_t>();

  /* resize data of node */
  c_example_node.resize(4);

  /* Maping data for update */
  {
    uint32_t * pi_data = c_example_node.mmap<uint32_t>();
    *pi_data = 10;
  }

  /* Accessing data through a type reference */
  uint32_t & i_data = c_example_node.map<uint32_t>();
  i_data++;
  ck_assert(i_data == 11);


  /* Using memcpy property */
  {
    char ai_data[] = "datadata";
    /* Copy data from buffer to node */
    c_example_node.memcpy_from_buffer(ai_data, sizeof(ai_data));

    /* Copy data from node to buffer */
    c_example_node.memcpy_to_buffer(ai_data, sizeof(ai_data));
  }
  ck_assert(1);

  /************** Creating and accessing childs ****************/


  typedef std::shared_ptr< node<uint32_t, std::shared_ptr> > smart_pointer_to_node_t;
  typedef node<uint32_t, std::shared_ptr> node_t;

  /* Accessing child with ID 6 and index 0. Node is automatically created if it does not exist */
  node_t & ref_to_child  = c_example_node(6);
  node_t & ref_to_child2 = c_example_node(6,0);
  smart_pointer_to_node_t smart_pointer_to_child = c_example_node.get(6,0,true);

  /* Accessing child with ID 6 and index 10. DO NOT WORK if index 9,8,..,1,0 is not available. */
  try {
      node_t & ref_to_child3 = c_example_node(6,10);
      smart_pointer_to_node_t smart_pointer_to_child2 = c_example_node.get(6,10,true);
  } catch(...) {
  }
  /* Accessing child with ID 6 and index 0. Node is not automatically created if it does not exist */
  smart_pointer_to_node_t smart_pointer_to_child3 = c_example_node.get(6,0, false);

  /* Update data of child */
  smart_pointer_to_child->set_data(i_data8);

  ck_assert(1);
  /************** Segments & Resource ****************/


  /* Create node from segment */
  {
    std::shared_ptr<node_resource<std::shared_ptr> > c_resource(new node_alloc_resource<std::shared_ptr>(1024));
    node_resource_segment<std::shared_ptr> c_segment(c_resource);
    node<uint32_t, std::shared_ptr> c_example_node2(1234, c_segment, 10, 20);
  }

  /* Accessing data throuh mmap of segment */
  {
    node_resource_segment<std::shared_ptr> segment = c_example_node.get_segment();
    char * pi_data = segment.mmap();
    *pi_data = 10;
  }

  /* Accessing data throuh memcpy of buffer */
  {
    char ai_data[] = "datadata";
    node_resource_segment<std::shared_ptr> c_segment_node = c_example_node.get_segment();
    c_segment_node.memcpy_from_buffer(ai_data, sizeof(ai_data));
    c_segment_node.memcpy_to_buffer(ai_data, sizeof(ai_data));
  }

  /* Accessing data throuh memcpy of segment */
  {
    std::shared_ptr<node_resource<std::shared_ptr> > c_resource(new node_alloc_resource<std::shared_ptr>(1024));
    node_resource_segment<std::shared_ptr> c_segment(c_resource);

    node_resource_segment<std::shared_ptr> c_segment_node = c_example_node.get_segment();
    c_segment_node.resize(c_segment.size());
    c_segment_node.memcpy_from_segment(c_segment);
    c_segment_node.memcpy_to_segment(c_segment);
  }
  ck_assert(1);

  /************* Clearing node content *************/
  c_example_node.clear();

  /*************  Writing node to  file *************/
  {
    node<uint32_t, std::shared_ptr> c_node(1234);
    c_node.set_data<uint32_t>(10);
    c_node(1).set_data<uint32_t>(1);
    c_node(2).set_data<uint32_t>(2);
    c_node(3).set_data<uint32_t>(3);

    node_file_writer<std::shared_ptr> c_writer("/tmp/data.bml");
    c_node.to_writer(c_writer);
  }
  ck_assert(1);
  /*************  Loading node from buffer *************/
  {
    node<uint32_t, std::shared_ptr> c_node;
    node_file_parser<std::shared_ptr> c_parser("/tmp/data.bml");
    c_node.from_parser(c_parser);
    ck_assert(c_node.get_data<uint32_t>() == 10);
  }
  {
    /*************  Writing node to buffer *************/
    node<uint32_t, std::shared_ptr> c_node(1234);
    c_node.set_data<uint32_t>(10);
    c_node(1).set_data<uint32_t>(1);
    c_node(2).set_data<uint32_t>(2);
    c_node(3).set_data<uint32_t>(3);

    char ac_buffer[1024];
    node_buffer_writer<std::shared_ptr> c_writer(ac_buffer, sizeof(ac_buffer));
    c_node.to_writer(c_writer);

    /*************  Loading node from file *************/
    node<uint32_t, std::shared_ptr> c_node_read;
    node_buffer_parser<std::shared_ptr> c_parser(ac_buffer, sizeof(ac_buffer));
    c_node_read.from_parser(c_parser);
    c_node_read.dump();
    ck_assert(c_node.get_data<uint32_t>() == 10);
  }

  /*************  Set and Get extension *************/
  {
    node<uint32_t, std::shared_ptr> c_node(1234);
    uint64_t i_data_ext = 123456;
    c_node.set_ext<uint64_t>(1, i_data_ext);

    uint64_t i_data_ext_get;
    i_data_ext_get = c_node.get_ext<uint64_t>(1);

    ck_assert(i_data_ext_get == i_data_ext);
  }

  /************* Find a child *************/
  {
    node<uint32_t, std::shared_ptr> c_node(1234);
    c_node.set_data<uint32_t>(10);
    c_node(1).set_data<uint32_t>(1);
    c_node(2,0).set_data<uint32_t>(4321);
    c_node(2,1).set_data<uint32_t>(4321);

    node<uint32_t, std::shared_ptr>::find_result c_result;
    node<uint32_t, std::shared_ptr>::find_iterator c_it;
    c_result = c_node.find(2);

    for(c_it = c_result.begin(); c_it != c_result.end(); c_it++) {
      ck_assert( (*c_it)->second->get_data<uint32_t>() == 4321);
    }
    ck_assert(c_result.size() == 2);
  }


  } END_TEST

START_TEST(test_memcpy) {
  node<uint32_t, std::shared_ptr> c_test;
  char ac_data0[] = "I'm a data";
  char ac_data1[] = "xxxxxxxxxx";

  c_test.memcpy_from_buffer(ac_data0, sizeof(ac_data0));

  std::cout << c_test << std::endl;

  c_test.memcpy_to_buffer(ac_data1, sizeof(ac_data0));

  ck_assert_str_eq(ac_data0, ac_data1);
} END_TEST

START_TEST(test_set_get_data) {
  node<uint32_t, std::shared_ptr> c_test;

  uint64_t i_udata64 = 0xDEADBEEFFEEDDADA;
  int64_t i_data64 = -12345678910111213;
  uint32_t i_udata32 = 0xDEADBEEF;
  int32_t i_data32 = -12345678;
  uint16_t i_udata16 = 0xDEAD;

  int16_t i_data16 = -1234;
  uint8_t i_udata8 = 10;
  int8_t i_data8 = -10;

  int32_t i_udata64_res;
  int32_t i_data64_res;
  int32_t i_udata32_res;
  int32_t i_data32_res;
  uint16_t i_udata16_res;
  int16_t i_data16_res;
  uint8_t i_udata8_res;
  int8_t i_data8_res;

  /*! Create a node with id 0 and store 10 */

  c_test(0).set_data(i_data8);
  c_test(1).set_data(i_udata8);
  c_test(2).set_data(i_data16);
  c_test(3).set_data(i_udata16);
  c_test(4).set_data(i_data32);
  c_test(5).set_data(i_udata32);
  c_test(6).set_data(i_data64);
  c_test(7).set_data(i_udata64);

  std::cout << c_test << std::endl;

  ck_assert(c_test(0).get_data<int8_t>() == i_data8);
  ck_assert(c_test(1).get_data<uint8_t>() == i_udata8);
  ck_assert(c_test(2).get_data<int16_t>() == i_data16);
  ck_assert(c_test(3).get_data<uint16_t>() == i_udata16);
  ck_assert(c_test(4).get_data<int32_t>() == i_data32);
  ck_assert(c_test(5).get_data<uint32_t>() == i_udata32);
  ck_assert(c_test(6).get_data<int64_t>() == i_data64);
  ck_assert(c_test(7).get_data<uint64_t>() == i_udata64);

}
END_TEST


Suite * money_suite(void)
{
  Suite *s;
  TCase *tc_core;

  s = suite_create("Money");

  /* Core test case */
  tc_core = tcase_create("Core");

  tcase_add_test(tc_core, test_set_get_data);
  tcase_add_test(tc_core, test_memcpy);
  tcase_add_test(tc_core, test_doc);
  suite_add_tcase(s, tc_core);

  return s;
}

int main(void)
{
  int number_failed;
  Suite *s;
  SRunner *sr;

  s = money_suite();
  sr = srunner_create(s);

  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
