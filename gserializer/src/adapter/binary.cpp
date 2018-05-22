/*
 * binary.cpp
 *
 *  Created on: May 9, 2018
 *      Author: gilang
 */

#include "gserializer/adapter/binary.hpp"

using gserializer::adapter::binary_serializer;
using gserializer::adapter::binary_deserializer;

GSAPI
binary_serializer::binary_serializer()
{
	buffer = new serialized_type;
	doDestruction = true;
}

GSAPI
void binary_serializer::serialize(uint32_t args)
{
	//dlog_print(DLOG_DEBUG, "BinSer", "Serialize uint32_t %d", args);
	uint8_t* ref = (uint8_t*)&args;
	for(size_t i = 0; i < sizeof(args); i++)
		buffer->push_back(ref[i]);
}

GSAPI
void binary_serializer::serialize(int64_t args)
{
	//dlog_print(DLOG_DEBUG, "BinSer", "Serialize int64_t %d", args);
	uint8_t* ref = (uint8_t*)&args;
	for(size_t i = 0; i < sizeof(args); i++)
		buffer->push_back(ref[i]);

}

GSAPI
void binary_serializer::serialize(uint64_t args)
{
	//dlog_print(DLOG_DEBUG, "BinSer", "Serialize int64_t %d", args);
	uint8_t* ref = (uint8_t*)&args;
	for(size_t i = 0; i < sizeof(args); i++)
		buffer->push_back(ref[i]);

}

GSAPI
void binary_serializer::serialize(int args)
{
	//dlog_print(DLOG_DEBUG, "BinSer", "Serialize int32_t %d", args);
	uint8_t* ref = (uint8_t*)&args;
	for(size_t i = 0; i < sizeof(args); i++)
		buffer->push_back(ref[i]);
}

GSAPI
void binary_serializer::serialize(bool args)
{
	//dlog_print(DLOG_DEBUG, "BinSer", "Serialize bool %d", args);
	uint8_t* ref = (uint8_t*)&args;
	for(size_t i = 0; i < sizeof(args); i++)
		buffer->push_back(ref[i]);
}

GSAPI
void binary_serializer::serialize(double args)
{
	//dlog_print(DLOG_DEBUG, "BinSer", "Serialize double %d", args);
	uint8_t* ref = (uint8_t*)&args;
	for(size_t i = 0; i < sizeof(args); i++)
		buffer->push_back(ref[i]);
}

GSAPI
void binary_serializer::serialize(std::string const& args)
{
	//dlog_print(DLOG_DEBUG, "BinSer", "Serialize string %s", args.c_str());
	auto len = args.length();

	uint8_t* ref = (uint8_t*)&len;
	for(size_t i = 0; i < sizeof(len); i++)
		buffer->push_back(ref[i]);

	for(size_t i = 0; i < len; i++)
		buffer->push_back(args[i]);

}

GSAPI
binary_serializer::serialized_type binary_serializer::end_serialize()
{
	return std::move(*buffer);
}



GSAPI
binary_serializer binary_serializer::begin_scope()
{
	return { this->buffer };
}

GSAPI
void binary_serializer::serialize(binary_serializer& p) {
	// Just do nothing as the scope is already pushed on the buffer
}

GSAPI
binary_serializer::~binary_serializer()
{
	if(buffer && doDestruction)
		delete buffer;
}

GSAPI
binary_serializer::binary_serializer(serialized_type* bufferRef) {
	this->buffer = bufferRef;
	this->doDestruction = false;
}

GSAPI
void binary_serializer::serialize(unsigned char args)
{
	this->buffer->push_back(args);
}

GSAPI
void binary_serializer::serialize(const std::vector<uint8_t>& args)
{
	if (args.empty())
	{
		serialize((uint32_t)0);
	}
	else
	{
		serialize((uint32_t)args.size());

		for (auto obj : args)
		{
			serialize(obj);
		}
	}
}


GSAPI
binary_deserializer::binary_deserializer(serialized_type const& p) : bufferRef(p), currentPos(0)
{

}

GSAPI
void binary_deserializer::end_deserialize()
{

}
/*
template<>
GSAPI
uint32_t binary_deserializer::deserialize_impl<uint32_t>()
{

}

template<>
GSAPI
int64_t binary_deserializer::deserialize_impl<int64_t>()
{
	int64_t tmp = 0;
	uint8_t* ref = (uint8_t*)&tmp;

	for(size_t i = 0; i < sizeof(tmp); i++)
	{
		ref[i] = bufferRef[currentPos + i];
	}

	currentPos += sizeof(tmp);
	return tmp;
}

template<>
GSAPI
double binary_deserializer::deserialize_impl<double>()
{
	double tmp = 0;
	uint8_t* ref = (uint8_t*)&tmp;

	for(size_t i = 0; i < sizeof(tmp); i++)
	{
		ref[i] = bufferRef[currentPos + i];
	}

	currentPos += sizeof(tmp);
	return tmp;
}
*/
template<>
GSAPI
std::string binary_deserializer::deserialize_impl<std::string>()
{
	decltype(std::declval<std::string>().length()) len = 0;
	uint8_t* ref = (uint8_t*)&len;

	for(size_t i = 0; i < sizeof(len); i++)
	{
		ref[i] = bufferRef[currentPos + i];
	}

	currentPos += sizeof(len);

	char* buf = new char[len + 1];
	buf[len] = '\0';

	for(size_t i = 0; i < len; i++)
		buf[i] = bufferRef[currentPos + i];



	std::string ret(buf);

	currentPos += len;

	delete[] buf;
	return ret;
}
/*
template<>
GSAPI
int binary_deserializer::deserialize_impl<int>()
{
	int tmp = 0;
	uint8_t* ref = (uint8_t*)&tmp;

	for(size_t i = 0; i < sizeof(tmp); i++)
	{
		ref[i] = bufferRef[currentPos + i];
	}

	currentPos += sizeof(tmp);
	return tmp;
}

template<>
GSAPI
bool binary_deserializer::deserialize_impl<bool>()
{
	bool tmp = 0;
	uint8_t* ref = (uint8_t*)&tmp;

	for(size_t i = 0; i < sizeof(tmp); i++)
	{
		ref[i] = bufferRef[currentPos + i];
	}

	currentPos += sizeof(tmp);
	return tmp;
}
*/

/*
GSAPI
void binary_serializer::serialize(const SerializedType& p)
{
	if(p == this->buffer)
		return;

	//dlog_print(DLOG_DEBUG, "BinSer", "Serialize serialized bytes size: %d", p.size());

	auto len = p.size();

	for(size_t i = 0; i < len; i++)
		buffer->push_back(p[i]);

}
*/

GSAPI
void binary_deserializer::deserialize(int8_t& target)
{
	target = deserialize_impl<int8_t>();
}

GSAPI
void binary_deserializer::deserialize(int16_t& target)
{
	target = deserialize_impl<int16_t>();
}

GSAPI
void binary_deserializer::deserialize(int32_t& target)
{
	target = deserialize_impl<int32_t>();
}

GSAPI
void binary_deserializer::deserialize(int64_t& target)
{
	target = deserialize_impl<int64_t>();
}

GSAPI
void binary_deserializer::deserialize(uint8_t& target)
{
	target = deserialize_impl<uint8_t>();
}

GSAPI
void binary_deserializer::deserialize(uint16_t& target)
{
	target = deserialize_impl<uint16_t>();
}

GSAPI
void binary_deserializer::deserialize(uint32_t& target)
{
	target = deserialize_impl<uint32_t>();
}

GSAPI
void binary_deserializer::deserialize(uint64_t& target)
{
	target = deserialize_impl<uint64_t>();
}

GSAPI
void binary_deserializer::deserialize(std::string& target)
{
	target = deserialize_impl<std::string>();
}

GSAPI
void binary_deserializer::deserialize(bool& target)
{
	target = deserialize_impl<bool>();
}

GSAPI
void binary_deserializer::deserialize(double& target)
{
	target = deserialize_impl<double>();
}

GSAPI
void binary_deserializer::deserialize(std::vector<uint8_t>& target)
{
	target.clear();

	uint32_t size = 0;
	deserialize(size);

	for(int i = 0; i < size; i++)
	{
		uint8_t tmp = 0;
		deserialize(tmp);
		target.push_back(tmp);
	}
}
