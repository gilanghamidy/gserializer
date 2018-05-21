/*
 * 01_singleton.cpp
 *
 *  Created on: May 21, 2018
 *      Author: gmh
 */

#include <gtest/gtest.h>

#include <gserializer/serialization.hpp>
#include <gserializer/class_serializer.hpp>
#include <gserializer/adapter/binary.hpp>

using namespace gserializer;

struct simple_struct_test
{
	int a;
	int b;
	int c;
};

GSERIALIZER_DEFINE_TYPE_INFO(simple_struct_test,
	GS_FIELD(simple_struct_test::a),
	GS_FIELD(simple_struct_test::b),
	GS_FIELD(simple_struct_test::c)
);

TEST(Basic_Singleton, SimpleStruct)
{
	simple_struct_test var1 { rand(), rand(), rand() };

	auto serialized = generic_serializer<adapter::binary_serializer, simple_struct_test>::serialize(var1);
	simple_struct_test deserialized = generic_deserializer<adapter::binary_deserializer, simple_struct_test>::deserialize(serialized);

	ASSERT_EQ(var1.a, deserialized.a);
	ASSERT_EQ(var1.b, deserialized.b);
	ASSERT_EQ(var1.c, deserialized.c);
}


