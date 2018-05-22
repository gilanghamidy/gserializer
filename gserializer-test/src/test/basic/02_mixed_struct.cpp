/*
 * 02_mixed_struct.cpp
 *
 *  Created on: May 22, 2018
 *      Author: gmh
 */

#include <gtest/gtest.h>

#include <gserializer/serialization.hpp>
#include <gserializer/class_serializer.hpp>
#include <gserializer/adapter/binary.hpp>


using namespace gserializer;

struct test1_inner
{
	float x;
	int y;
	std::string z;
};

struct test1_outer
{
	int a;
	char b;
	test1_inner c;
};

GSERIALIZER_DEFINE_TYPE_INFO(test1_outer,
	GS_FIELD(test1_outer::a),
	GS_FIELD(test1_outer::b),
	GS_FIELD(test1_outer::c)
);

GSERIALIZER_DEFINE_TYPE_INFO(test1_inner,
	GS_FIELD(test1_inner::x),
	GS_FIELD(test1_inner::y),
	GS_FIELD(test1_inner::z)
);

TEST(Basic_MixedStruct, OuterInner)
{
	test1_outer var1 { rand(), (char)rand(), { rand() * 1.0f, rand(), "I am the string" } };

	auto serialized = generic_serializer<adapter::binary_serializer, test1_outer>::serialize(var1);
	test1_outer deserialized = generic_deserializer<adapter::binary_deserializer, test1_outer>::deserialize(serialized);

	EXPECT_EQ(var1.a, deserialized.a);
	EXPECT_EQ(var1.b, deserialized.b);
	EXPECT_EQ(var1.c.x, deserialized.c.x);
	EXPECT_EQ(var1.c.y, deserialized.c.y);
	EXPECT_STREQ(var1.c.z.c_str(), deserialized.c.z.c_str());
}
