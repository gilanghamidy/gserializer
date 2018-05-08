/*
 * parameter_serializer.hpp
 *
 *  Created on: May 8, 2018
 *      Author: gilang
 */

#ifndef INCLUDE_GSERIALIZER_PARAMETER_SERIALIZER_HPP_
#define INCLUDE_GSERIALIZER_PARAMETER_SERIALIZER_HPP_

#include "gserializer/serialization.hpp"
#include "gserializer/utility/type_introspection.hpp"

namespace gserializer
{

	template<typename SerializerClass, typename FunctionType,
			 typename ParameterPack = typename utility::member_function<FunctionType>::args_tuple>
	struct parameter_serializer;

	template<typename SerializerClass, typename FunctionType, typename... FuncArgs>
	struct parameter_serializer<SerializerClass, FunctionType, std::tuple<FuncArgs...>>
	{
		static typename SerializerClass::serialized_type serialize(FuncArgs... param)
		{
			SerializerClass ser;
			serializer_functor<SerializerClass, FuncArgs...>::func(ser, param...);
			return ser.end_serialize();
		}

		static void serialize(SerializerClass& ser, FuncArgs... param)
		{
			serializer_functor<SerializerClass, FuncArgs...>::func(ser, param...);
		}
	};

	template<typename DeserializerClass, typename FunctionType,
			 typename ParameterPack = typename utility::member_function<FunctionType>::args_tuple>
	struct parameter_deserializer;



	template<typename DeserializerClass, typename FunctionType, typename... FuncArgs>
	struct parameter_deserializer<DeserializerClass, FunctionType, std::tuple<FuncArgs...>>
	{
		static std::tuple<FuncArgs...> deserialize(typename DeserializerClass::serialized_type const& p, bool finalizePackedObject = true)
		{
			DeserializerClass deser(p);

			if(finalizePackedObject)
				deser.end_deserialize();

			return parameter_deserializer_functor<DeserializerClass, FuncArgs...>::func(deser);
		}
	};
}



#endif /* INCLUDE_GSERIALIZER_PARAMETER_SERIALIZER_HPP_ */
