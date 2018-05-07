/*
 * serializer_functor.hpp
 *
 *  Created on: May 7, 2018
 *      Author: gilang
 */

#ifndef INCLUDE_GSERIALIZER_SERIALIZATION_HPP_
#error "Cannot include SerializerFunctor.h by itself without including TFC/Serialization.h"
#else


#ifndef INCLUDE_GSERIALIZER_CORE_SERIALIZER_FUNCTOR_HPP_
#define INCLUDE_GSERIALIZER_CORE_SERIALIZER_FUNCTOR_HPP_

#include "gserializer/serialization.hpp"

#include <utility>

namespace gserializer
{
	template<typename SerializerClass, typename ValueType>
	struct serializer_exist
	{
		typedef char correct;
		typedef struct { char val[2]; } incorrect;

		template<typename TA, typename TB> static correct test(decltype(std::declval<TA>().serialize(std::declval<TB>()))*);
		template<typename TA, typename TB> static incorrect test(...);

		public:
			static constexpr bool value = sizeof(test<SerializerClass, ValueType>(0)) == sizeof(correct);
	};

	template<typename SerializerClass, typename CurrentType, typename = void, bool = serializer_exist<SerializerClass, CurrentType>::value>
	struct serializer_select
	{
		static void serialize(SerializerClass& s, CurrentType const& t)
		{
			s.serialize(t);
		}
	};

	template<typename SerializerClass, typename CurrentType>
	struct serializer_select<SerializerClass, CurrentType, typename std::enable_if<std::is_enum<CurrentType>::value>::type, false>
	{
		static void serialize(SerializerClass& s, CurrentType const& t)
		{
			typedef typename std::underlying_type<CurrentType>::type casted_type;
			s.serialize(static_cast<casted_type>(t));
		}
	};

	template<typename SerializerClass, typename CurrentType, typename Void>
	struct serializer_select<SerializerClass, CurrentType, Void, false>
	{
		static void Serialize(SerializerClass& ser, CurrentType const& t)
		{
			SerializerClass serIn = ser.begin_scope();
			generic_serializer<SerializerClass, CurrentType>::serialize(serIn, t);
			ser.serialize(serIn);
		}
	};

	template<typename T>
	struct serializer_field
	{
		T const& value;
		bool serialize;
	};

	/**
	 * Recursion case for SerializerFunctor
	 */
	template<typename SerializerClass, typename CurrentType, typename... Args>
	struct serializer_functor<SerializerClass, CurrentType, Args...>
	{

		static void func(SerializerClass& s, CurrentType const& t, Args const&... next)
		{
			serializer_select<SerializerClass, typename std::decay<CurrentType>::type>::serialize(s, t);
			// Call SerializerFunctor recursive by passing the TArgs tails as arguments
			serializer_functor<SerializerClass, Args...>::func(s, next...);
		}


		static void func(SerializerClass& s, serializer_field<CurrentType> const& t, serializer_field<Args> const&... next)
		{
			if(t.serialize)
			{
				serializer_select<SerializerClass, CurrentType>::serialize(s, t.value);
			}

			// Call SerializerFunctor recursive by passing the TArgs tails as arguments
			serializer_functor<SerializerClass, Args...>::func(s, next...);
		}
	};



	/**
	 * Base case for SerializerFunctor where TArgs is nil entirely
	 */
	template<typename SerializerClass>
	struct serializer_functor<SerializerClass>
	{
		// Base function just do nothing
		static void func(SerializerClass& p) { }
	};
}



#endif /* INCLUDE_GSERIALIZER_CORE_SERIALIZER_FUNCTOR_HPP_ */

#endif
