/*
 * object_serializer.hpp
 *
 *  Created on: May 8, 2018
 *      Author: gilang
 */

#ifndef INCLUDE_GSERIALIZER_OBJECT_SERIALIZER_HPP_
#define INCLUDE_GSERIALIZER_OBJECT_SERIALIZER_HPP_

#include "gserializer/serialization.hpp"

namespace gserializer
{
	template<typename SerializerClass, typename Obj>
	struct object_serializer
	{
		static typename SerializerClass::serialized_type serialize(Obj const& obj)
		{
			SerializerClass ser;
			serializer_functor<SerializerClass, Obj>::Func(ser, obj);
			return ser.end_serialize();
		}
	};

	template<typename SerializerClass>
	struct object_serializer<SerializerClass, void>
	{
		static typename SerializerClass::serialized_type serialize()
		{
			SerializerClass ser;
			return ser.end_serialize();
		}
	};

	template<typename DeserializerClass, typename Obj>
	struct object_deserializer
	{
		static Obj deserialize(typename DeserializerClass::serialized_type const& p)
		{
			DeserializerClass deser(p);
			return generic_deserializer<DeserializerClass, Obj>::deserialize(deser); //std::get<0>(ParameterDeserializerFunctor<TDeserializerClass, TObj>::Func(unpacker));
		}
	};

	template<typename DeserializerClass>
	struct object_deserializer<DeserializerClass, void>
	{
		static void deserialize(typename DeserializerClass::serialized_type const& p)
		{
			return;
		}
	};

}




#endif /* INCLUDE_GSERIALIZER_OBJECT_SERIALIZER_HPP_ */
