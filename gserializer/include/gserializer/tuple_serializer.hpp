/*
 * tuple_serializer.hpp
 *
 *  Created on: May 8, 2018
 *      Author: gilang
 */

#ifndef INCLUDE_GSERIALIZER_TUPLE_SERIALIZER_HPP_
#define INCLUDE_GSERIALIZER_TUPLE_SERIALIZER_HPP_

#include "gserializer/serialization.hpp"

namespace gserializer
{
	template<typename SerializerClass, typename FieldList>
	struct tuple_serializer;

	template<typename SerializerClass, typename... Fields>
	struct tuple_serializer<SerializerClass, std::tuple<Fields...>>
	{
		using arg_sequence = std::make_index_sequence<sizeof...(Fields)>;

		template<int... S>
		static typename SerializerClass::serialized_type serialize(std::tuple<Fields...> const& ref, std::integer_sequence<S...>)
		{
			SerializerClass ser;
			serializer_functor<SerializerClass, Fields...>::func(ser, std::get<S>(ref)...);
			return ser.end_serialize();
		}

		static typename SerializerClass::serialized_type serialize(std::tuple<Fields...> const& ref)
		{
			return serialize(ref, arg_sequence());
		}

		template<int... S>
		static void serialize(SerializerClass& ser, std::tuple<Fields...> const& ref, Core::Metaprogramming::Sequence<S...>)
		{
			serializer_functor<SerializerClass, Fields...>::func(ser, std::get<S>(ref)...);
		}

		static void serialize(SerializerClass& ser, std::tuple<Fields...> const& ref)
		{
			serialize(ser, ref, arg_sequence());
		}
	};
}



#endif /* INCLUDE_GSERIALIZER_TUPLE_SERIALIZER_HPP_ */
