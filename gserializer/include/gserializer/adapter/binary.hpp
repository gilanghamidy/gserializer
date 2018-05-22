/*
 * binary_serializer.hpp
 *
 *  Created on: May 9, 2018
 *      Author: gilang
 */

#ifndef INCLUDE_GSERIALIZER_ADAPTER_BINARY_HPP_
#define INCLUDE_GSERIALIZER_ADAPTER_BINARY_HPP_

#include <string>
#include <cstdint>
#include <vector>
#include "gserializer/serialization.hpp"

namespace gserializer::adapter
{
	class binary_serializer
	{
	public:
		typedef std::vector<uint8_t> serialized_type;

		binary_serializer();
		binary_serializer(serialized_type* bufferRef);
		void serialize(uint32_t args);
		void serialize(int64_t args);
		void serialize(uint64_t args);
		void serialize(unsigned char args);
		void serialize(int args);
		void serialize(bool args);
		void serialize(double args);
		void serialize(std::string const& args);

		binary_serializer begin_scope();
		void serialize(binary_serializer& p);

		serialized_type end_serialize();

		void serialize(std::vector<uint8_t> const& arg);

		template<typename T>
		void serialize(std::vector<T> const& args)
		{
			auto ser = begin_scope();

			if (args.empty())
			{
				ser.serialize((uint32_t)0);
			}
			else
			{
				ser.serialize((uint32_t)args.size());

				for (auto& obj : args)
				{
					generic_serializer<binary_serializer, T>::serialize(ser, obj);
				}
			}
			serialize(ser);
		}

		~binary_serializer();

	private:
		bool doDestruction;
		serialized_type* buffer;

	};

	struct binary_deserializer
	{
		typedef std::vector<uint8_t> serialized_type;

	private:
		serialized_type const& bufferRef;
		size_t currentPos;

		template<typename T>
		T deserialize_impl()
		{
			T tmp = 0;
			uint8_t* ref = (uint8_t*)&tmp;

			for(size_t i = 0; i < sizeof(tmp); i++)
			{
				ref[i] = bufferRef[currentPos + i];
			}

			currentPos += sizeof(tmp);
			return tmp;
		}

		binary_deserializer(serialized_type const& p, size_t currentPos);

	public:
		binary_deserializer(serialized_type const& p);

		void deserialize(int8_t& target);
		void deserialize(int16_t& target);
		void deserialize(int32_t& target);
		void deserialize(int64_t& target);

		void deserialize(uint8_t& target);
		void deserialize(uint16_t& target);
		void deserialize(uint32_t& target);
		void deserialize(uint64_t& target);

		void deserialize(std::string& target);
		void deserialize(bool& target);
		void deserialize(double& target);

		//void Deserialize(SerializedType& composite);

		binary_deserializer& begin_scope() { return *this; }

		void deserialize(std::vector<uint8_t>& target);

		template<typename T>
		void deserialize(std::vector<T>& target)
		{
			target.clear();

			uint32_t size = 0;
			deserialize(size);

			auto& scope = begin_scope();

			for(int i = 0; i < size; i++)
			{
				using deserializer = gserializer::generic_deserializer<binary_deserializer, T>;
				target.push_back(deserializer::deserialize(scope));
			}
		}

		void end_deserialize();
	};
}



#endif /* INCLUDE_GSERIALIZER_ADAPTER_BINARY_HPP_ */
