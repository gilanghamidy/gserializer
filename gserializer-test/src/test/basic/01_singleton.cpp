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

	EXPECT_EQ(var1.a, deserialized.a);
	EXPECT_EQ(var1.b, deserialized.b);
	EXPECT_EQ(var1.c, deserialized.c);
}

struct simple_string_test
{
	std::string str;
};

GSERIALIZER_DEFINE_TYPE_INFO(simple_string_test,
	GS_FIELD(simple_string_test::str)
);

TEST(Basic_Singleton, SimpleString)
{
	simple_string_test raw { "I am a string test" };

	auto serialized = generic_serializer<adapter::binary_serializer, simple_string_test>::serialize(raw);
	simple_string_test deserialized = generic_deserializer<adapter::binary_deserializer, simple_string_test>::deserialize(serialized);

	EXPECT_STREQ(raw.str.c_str(), deserialized.str.c_str());
}

struct simple_combination_test
{
	int intVal;
	uint64_t longVal;
	std::string strVal;
	bool boolVal;
};

GSERIALIZER_DEFINE_TYPE_INFO(simple_combination_test,
	GS_FIELD(simple_combination_test::intVal),
	GS_FIELD(simple_combination_test::longVal),
	GS_FIELD(simple_combination_test::strVal),
	GS_FIELD(simple_combination_test::boolVal)
);

TEST(Basic_Singleton, SimpleCombination)
{
	simple_combination_test raw { rand(), 1ul | (uint64_t)rand() << 32 | rand(), "I am a string test", rand() % 2 == 0 };

	auto serialized = generic_serializer<adapter::binary_serializer, simple_combination_test>::serialize(raw);
	simple_combination_test deserialized = generic_deserializer<adapter::binary_deserializer, simple_combination_test>::deserialize(serialized);

	EXPECT_EQ(raw.intVal, deserialized.intVal);
	EXPECT_STREQ(raw.strVal.c_str(), deserialized.strVal.c_str());
	EXPECT_EQ(raw.longVal, deserialized.longVal);
	EXPECT_EQ(raw.boolVal, deserialized.boolVal);
}


