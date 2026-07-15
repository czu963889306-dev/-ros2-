// Copyright 2019 - present Proyectos y Sistemas de Mantenimiento SL (eProsima).
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.


#include <gtest/gtest.h>

#include "rosidl_runtime_cpp/message_type_support_decl.hpp"
#include "rosidl_typesupport_cpp/message_type_support.hpp"

// Generic MicroXRCE-DDS typesupport includes
#include <rosidl_typesupport_microxrcedds_cpp/identifier.hpp>
#include <rosidl_typesupport_microxrcedds_c/message_type_support.h>

// Specific defined types used during testing
#include "rosidl_typesupport_microxrcedds_test_msg/msg/primitive.hpp"
#include "rosidl_typesupport_microxrcedds_test_msg/msg/sequence.hpp"
#include "rosidl_typesupport_microxrcedds_test_msg/msg/compound.hpp"

/*
 * @brief TestTypeSupport class, used to automate typesupport testing for a specific type.
 */
template <typename T>
class TestTypeSupport : public ::testing::Test
{
public:
  /*
   * @brief Default constructor.
   */
  TestTypeSupport() = default;

  /*
   * @brief Lvalue copy constructor (deleted).
   * @param[in] other Test to be copied.
   */
  TestTypeSupport(
      const TestTypeSupport & other) = delete;

  /*
   * @brief Rvalue copy constructor (deleted).
   * @param[in] other Test to be copied.
   */
  TestTypeSupport(
      TestTypeSupport && other) = delete;

  /*
   * @brief Setup function.
   * @param[in] init_test_type      Initialized rvalue instance of the ROS 2 type to be tested.
   * @param[in] compare_func_handle Function used for comparing two instances of T type.
   */
  void setup(
      T && init_test_type,
      std::function <void (const T &, const T &)> & compare_func_handle)
  {
    rosidl_message_type_support_ = rosidl_typesupport_cpp::get_message_type_support_handle<T>();
    rosidl_message_type_support_ = get_message_typesupport_handle(
      rosidl_message_type_support_, ROSIDL_TYPESUPPORT_MICROXRCEDDS_CPP__IDENTIFIER_VALUE);

    message_type_support_callbacks_ = static_cast<const message_type_support_callbacks_t *>(
      rosidl_message_type_support_->data);

    tested_type_ = std::move(init_test_type);
    compare_func_handle_ = &compare_func_handle;
  }

  /*
   * @brief Checks for correctness of the retrieved typesupport's identifier.
   */
  void check_identifier()
  {
    ASSERT_EQ(strcmp(rosidl_message_type_support_->typesupport_identifier,
      ROSIDL_TYPESUPPORT_MICROXRCEDDS_CPP__IDENTIFIER_VALUE), 0);
  }

  /*
   * @brief Tests serialization and deserialization of a T type message,
   *        and compares the original instance and the deserialized one
   *        in terms of equalness.
   */
  void test_serialize_deserialize()
  {
    ucdrBuffer mb_writer;
    ucdrBuffer mb_reader;

    /*
     * TODO(jamoralp): improve this buffer's creation in terms of size.
     * Maybe we can use CDR max serialized size here, also to check that this method works properly.
     */
    uint8_t mb_buffer[5000];
    ucdr_init_buffer(&mb_writer, mb_buffer, sizeof(mb_buffer));
    ucdr_init_buffer(&mb_reader, mb_buffer, sizeof(mb_buffer));

    ASSERT_TRUE(message_type_support_callbacks_->cdr_serialize(&tested_type_, &mb_writer));

    T test_out;
    ASSERT_TRUE(message_type_support_callbacks_->cdr_deserialize(&mb_reader, &test_out));

    compare(tested_type_, test_out);
  }

protected:
  /*
   * @brief Compare two given instances of the same type (T), in terms of equalness.
   * @param[in] A First instance to be compared.
   * @param[in] B Second instance to be compared.
   */
  void compare(
      const T & A,
      const T & B)
  {
    (*compare_func_handle_)(A, B);
  }

  T tested_type_;
  const rosidl_message_type_support_t * rosidl_message_type_support_;
  const message_type_support_callbacks_t * message_type_support_callbacks_;
  std::function<void (const T &, const T &)> * compare_func_handle_;
};

/******************************************************************************
                      Serialize/deserialize test suites.
 *****************************************************************************/

/*
 * @brief Primitive ROS 2 types serialization and deserialization tests.
 */
template <typename T>
class PrimitivesTestTypeSupport : public TestTypeSupport<T> {};

TYPED_TEST_SUITE(PrimitivesTestTypeSupport,
  testing::Types<rosidl_typesupport_microxrcedds_test_msg::msg::Primitive>);
TYPED_TEST(PrimitivesTestTypeSupport, serialize_primitive_types)
{
  std::function<void (
      const rosidl_typesupport_microxrcedds_test_msg::msg::Primitive &,
      const rosidl_typesupport_microxrcedds_test_msg::msg::Primitive &)> compare_primitives ([](
        const rosidl_typesupport_microxrcedds_test_msg::msg::Primitive & A,
        const rosidl_typesupport_microxrcedds_test_msg::msg::Primitive & B) -> void
  {
    EXPECT_EQ(A.bool_test, B.bool_test);
    EXPECT_EQ(A.byte_test, B.byte_test);
    EXPECT_EQ(A.char_test, B.char_test);
    EXPECT_EQ(A.float32_test, B.float32_test);
    EXPECT_EQ(A.double_test, B.double_test);
    EXPECT_EQ(A.int8_test, B.int8_test);
    EXPECT_EQ(A.uint8_test, B.uint8_test);
    EXPECT_EQ(A.int16_test, B.int16_test);
    EXPECT_EQ(A.uint16_test, B.uint16_test);
    EXPECT_EQ(A.int32_test, B.int32_test);
    EXPECT_EQ(A.uint32_test, B.uint32_test);
    EXPECT_EQ(A.int64_test, B.int64_test);
    EXPECT_EQ(A.uint64_test, B.uint64_test);

    EXPECT_EQ(A.nested_test.unbounded_string1.compare(B.nested_test.unbounded_string1), 0);
    EXPECT_EQ(A.nested_test.unbounded_string2.compare(B.nested_test.unbounded_string2), 0);
    EXPECT_EQ(A.nested_test.unbounded_string3.compare(B.nested_test.unbounded_string3), 0);
    EXPECT_EQ(A.nested_test.unbounded_string4.compare(B.nested_test.unbounded_string4), 0);
  });

  // Initialize data to be serialized and deserialized
  rosidl_typesupport_microxrcedds_test_msg::msg::Primitive init_primitive;
  init_primitive.bool_test = 0x01;
  init_primitive.byte_test = 0x01;
  init_primitive.char_test = 0x01;
  init_primitive.float32_test = 100.001;
  init_primitive.double_test = 100.001;
  init_primitive.int8_test = 0x01;
  init_primitive.uint8_test = 0x01;
  init_primitive.int16_test = 0x0101;
  init_primitive.uint16_test = 0x0101;
  init_primitive.int32_test = 0x01010101;
  init_primitive.uint32_test = 0x01010101;
  init_primitive.int64_test = 0x0101010101010101;
  init_primitive.uint64_test = 0x0101010101010101;
  init_primitive.nested_test.unbounded_string1 = "ABCDEF";
  init_primitive.nested_test.unbounded_string2 = "TGHIJKLMNO";
  init_primitive.nested_test.unbounded_string3 = "PQRSTVWX";
  init_primitive.nested_test.unbounded_string4 = "TYZ0123456789";

  this->setup(std::move(init_primitive), compare_primitives);
  this->check_identifier();
  this->test_serialize_deserialize();
}

/*
 * @brief Sequence ROS 2 types serialization and deserialization tests.
 */
template <typename T>
class SequencesTestTypeSupport : public TestTypeSupport<T> {};

TYPED_TEST_SUITE(SequencesTestTypeSupport,
  testing::Types<rosidl_typesupport_microxrcedds_test_msg::msg::Sequence>);
TYPED_TEST(SequencesTestTypeSupport, serialize_sequence_types)
{
  std::function<void (
      const rosidl_typesupport_microxrcedds_test_msg::msg::Sequence &,
      const rosidl_typesupport_microxrcedds_test_msg::msg::Sequence &)> compare_sequences ([](
          const rosidl_typesupport_microxrcedds_test_msg::msg::Sequence & A,
          const rosidl_typesupport_microxrcedds_test_msg::msg::Sequence & B) -> void
  {
    EXPECT_EQ(A.sequence_string_test.size(), B.sequence_string_test.size());

    for (size_t i = 0; i < A.sequence_string_test.size(); ++i)
    {
      EXPECT_EQ(A.sequence_string_test[i].compare(B.sequence_string_test[i]), 0);
    }
  });

  // Initialize data to be serialized and deserialized
  rosidl_typesupport_microxrcedds_test_msg::msg::Sequence init_sequence;
  init_sequence.sequence_string_test.emplace_back("This");
  init_sequence.sequence_string_test.emplace_back("is");
  init_sequence.sequence_string_test.emplace_back("a");
  init_sequence.sequence_string_test.emplace_back("test");

  this->setup(std::move(init_sequence), compare_sequences);
  this->check_identifier();
  this->test_serialize_deserialize();
}

/*
 * @brief Compound ROS 2 types serialization and deseralization tests,
 *        when padding is required for sequences.
 */
template <typename T>
class CompoundSequencesTestTypeSupport : public TestTypeSupport<T> {};

TYPED_TEST_SUITE(CompoundSequencesTestTypeSupport,
    testing::Types<rosidl_typesupport_microxrcedds_test_msg::msg::Compound>);
TYPED_TEST(CompoundSequencesTestTypeSupport, serialize_compound_types)
{
  std::function<void (
    const rosidl_typesupport_microxrcedds_test_msg::msg::Compound &,
    const rosidl_typesupport_microxrcedds_test_msg::msg::Compound &)> compare_compound ([](
      const rosidl_typesupport_microxrcedds_test_msg::msg::Compound & A,
      const rosidl_typesupport_microxrcedds_test_msg::msg::Compound & B) -> void
  {
    EXPECT_EQ(A.string_data.compare(B.string_data), 0);

    EXPECT_EQ(A.sequence_data.size(), B.sequence_data.size());
    for (size_t i = 0; i < A.sequence_data.size(); ++i)
    {
      EXPECT_EQ(A.sequence_data[i].bool_test, B.sequence_data[i].bool_test);
      EXPECT_EQ(A.sequence_data[i].byte_test, B.sequence_data[i].byte_test);
      EXPECT_EQ(A.sequence_data[i].char_test, B.sequence_data[i].char_test);
      EXPECT_EQ(A.sequence_data[i].float32_test, B.sequence_data[i].float32_test);
      EXPECT_EQ(A.sequence_data[i].double_test, B.sequence_data[i].double_test);
      EXPECT_EQ(A.sequence_data[i].int8_test, B.sequence_data[i].int8_test);
      EXPECT_EQ(A.sequence_data[i].uint8_test, B.sequence_data[i].uint8_test);
      EXPECT_EQ(A.sequence_data[i].int16_test, B.sequence_data[i].int16_test);
      EXPECT_EQ(A.sequence_data[i].uint16_test, B.sequence_data[i].uint16_test);
      EXPECT_EQ(A.sequence_data[i].int32_test, B.sequence_data[i].int32_test);
      EXPECT_EQ(A.sequence_data[i].uint32_test, B.sequence_data[i].uint32_test);
      EXPECT_EQ(A.sequence_data[i].int64_test, B.sequence_data[i].int64_test);
      EXPECT_EQ(A.sequence_data[i].uint64_test, B.sequence_data[i].uint64_test);

      EXPECT_EQ(A.sequence_data[i].nested_test.unbounded_string1.compare(
        B.sequence_data[i].nested_test.unbounded_string1), 0);
      EXPECT_EQ(A.sequence_data[i].nested_test.unbounded_string2.compare(
        B.sequence_data[i].nested_test.unbounded_string2), 0);
      EXPECT_EQ(A.sequence_data[i].nested_test.unbounded_string3.compare(
        B.sequence_data[i].nested_test.unbounded_string3), 0);
      EXPECT_EQ(A.sequence_data[i].nested_test.unbounded_string4.compare(
        B.sequence_data[i].nested_test.unbounded_string4), 0);
    }
  });

  // Initialize data to be serialized and deserialized
  rosidl_typesupport_microxrcedds_test_msg::msg::Compound init_compound;
  init_compound.string_data = "AAAAAAAAAAA";

  size_t compound_seq_size = 4;
  for (size_t i = 1; i <= compound_seq_size; ++i)
  {
    rosidl_typesupport_microxrcedds_test_msg::msg::Primitive primitive_elem;
    primitive_elem.bool_test = 0x01;
    primitive_elem.byte_test = 0x01 * i;
    primitive_elem.char_test = 0x01 * i;
    primitive_elem.float32_test = 100.001 * i;
    primitive_elem.double_test = 100.001 * i;
    primitive_elem.int8_test = 0x01 * i;
    primitive_elem.uint8_test = 0x01 * i;
    primitive_elem.int16_test = 0x0101 * i;
    primitive_elem.uint16_test = 0x0101 * i;
    primitive_elem.int32_test = 0x01010101 * i;
    primitive_elem.uint32_test = 0x01010101 * i;
    primitive_elem.int64_test = 0x0101010101010101 * i;
    primitive_elem.uint64_test = 0x0101010101010101 * i;
    primitive_elem.nested_test.unbounded_string1 = "ABCDEFGH";
    primitive_elem.nested_test.unbounded_string2 = "IJKLMNOPQ";
    primitive_elem.nested_test.unbounded_string3 = "RSTUVWXYZ";
    primitive_elem.nested_test.unbounded_string4 = std::to_string(1111111 * i);

    init_compound.sequence_data.emplace_back(std::move(primitive_elem));
  }

  this->setup(std::move(init_compound), compare_compound);
  this->check_identifier();
  this->test_serialize_deserialize();
}
