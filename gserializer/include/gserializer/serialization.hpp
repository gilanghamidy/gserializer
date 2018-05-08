/*
 * gserializer
 * Copyright (c) 2018 Gilang Mentari Hamidy All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 *	  serializer.h
 *
 * Author: Gilang Mentari Hamidy (gilang.hamidy@gmail.com)
 */


#ifndef INCLUDE_GSERIALIZER_SERIALIZATION_HPP_
#define INCLUDE_GSERIALIZER_SERIALIZATION_HPP_

#include <tuple>
#include <memory>

#include "gserializer/utility/type_introspection.hpp"

namespace gserializer
{
	template<typename SerializerClass, typename... Args>
	struct serializer_functor;

	template<typename SerializerClass, typename DeclaringClass, typename = void>
	struct generic_serializer;

	template<typename SerializerClass, typename DeclaringClass, typename = void>
	struct generic_deserializer;

	template<typename Predicates>
	struct predicate_evaluator;

	template<typename DeclaringClass, typename... Fields>
	struct type_serialization_info
	{
		typedef DeclaringClass declaring_type;
		typedef std::tuple<Fields...> field_list;
	};

	template<typename DeclaringClass>
	struct type_serialization_info_selector
	{
		static constexpr bool available = false;
	};

	template<auto memPtr, typename PredicateList = std::tuple<>, typename = void>
	struct field_info
	{
		using pointer_to_member_info = utility::member_function<decltype(memPtr)>;
		using value_type = typename pointer_to_member_info::return_type;
		using declaring_class = typename pointer_to_member_info::declaring_type;

		static value_type const& get(declaring_class const& ref)
		{
			return ref.*memPtr;
		}

		static void set(declaring_class& ref, value_type&& val)
		{
			ref.*memPtr = std::move(val);
		}

		static bool evaluate(declaring_class const& ref)
		{
			return predicate_evaluator<PredicateList>::evaluate(ref);
		}

		template<typename DeserializerClass>
		static void deserialize_and_set(declaring_class& ref, DeserializerClass& deser)
		{
			generic_deserializer<DeserializerClass, value_type>::deserialize(deser, ref.*memPtr);
		}
	};

	template<auto memPtr, typename PredicateList>
	struct field_info<memPtr, PredicateList, typename std::enable_if<std::is_enum_v<typename utility::member_function<decltype(memPtr)>::return_type>>::type>
	{
		using pointer_to_member_info = utility::member_function<decltype(memPtr)>;
		using value_type = typename pointer_to_member_info::return_type;
		using declaring_class = typename pointer_to_member_info::declaring_type;

		static value_type const& get(declaring_class const& ref)
		{
			return ref.*memPtr;
		}

		static void set(declaring_class& ref, value_type&& val)
		{
			ref.*memPtr = std::move(val);
		}

		static bool evaluate(declaring_class const& ref)
		{
			return predicate_evaluator<PredicateList>::evaluate(ref);
		}

		template<typename DeserializerClass>
		static void deserialize_and_set(declaring_class& ref, DeserializerClass& deser)
		{
			deser.deserialize(*reinterpret_cast<typename std::underlying_type_t<value_type>*>(&(ref.*memPtr)));
		}
	};

	//TODO use feature check and apply C++17 auto variable feature instead
	template<typename ValueType, ValueType theValue, typename PredicateList>
	struct constant_value
	{
		typedef ValueType value_type;
		static value_type theValueStorage;

		template<typename T>
		static value_type const& get(T& unused)
		{
			return theValueStorage;
		}

		template<typename T>
		static void set(T& unused1, value_type&& unused2)
		{
		}

		template<typename DeclaringClass>
		static bool evaluate(DeclaringClass const& ref)
		{
			return predicate_evaluator<PredicateList>::evaluate(ref);
		}

		template<typename DeclaringClass, typename DeserializerClass>
		static void deserialize_and_set(DeclaringClass& ref, DeserializerClass& deser)
		{
			value_type val;
			deser.deserialize(val);
			//TODO Throw exception when unexpected constant value retrieved
			//TFCAssert<SerializationException>(val == theValue, "Unexpected constant value retrieved");
		}
	};

	template<typename Serializer, typename ValueType>
	class serialization_available
	{
		typedef char correct;
		typedef long long incorrect;

		template<typename T1, typename T2> static correct test(decltype(std::declval<T1>().serialize(std::declval<T2>()))*);
		template<typename T1, typename T2> static incorrect test(...);

	public:
		static constexpr bool value = sizeof(test<Serializer, ValueType>(0)) == sizeof(correct);
	};

	template<typename Deserializer, typename ValueType>
	class deserialization_available
	{
		typedef char correct;
		typedef long long incorrect;


		template<typename T>
		static typename std::add_lvalue_reference<T>::type declref();


		template<typename T1, typename T2> static correct test(decltype(std::declval<T1>().Deserialize(declref<T2>()))*);
		template<typename T1, typename T2> static incorrect test(...);

	public:
		static constexpr bool value = sizeof(test<Deserializer, ValueType>(0)) == sizeof(correct);
	};

	template<typename SerializerClass, typename ValueType>
	struct generic_serializer<SerializerClass, ValueType, typename std::enable_if<serialization_available<SerializerClass, ValueType>::value>::type>
	{
		static auto serialize(ValueType const& ref)
		{
			SerializerClass serializer;
			serializer.serialize(ref);
			return serializer.end_serialize();
		}

		static void serialize(SerializerClass& serializer, ValueType const& ref)
		{
			serializer.serialize(ref);
		}
	};

	template<typename SerializerClass, typename ValueType>
	struct generic_serializer<SerializerClass, ValueType, typename std::enable_if<std::is_enum<ValueType>::value>::type>
	{
		static void serialize(SerializerClass& serializer, ValueType const& ref)
		{
			serializer.serialize((typename std::underlying_type<ValueType>::type)ref);
		}
	};

	template<typename SerializerClass, typename ValueType>
	struct generic_serializer<SerializerClass, std::unique_ptr<ValueType>, void>
	{
		static void serialize(SerializerClass& serializer, std::unique_ptr<ValueType> const& ref)
		{
			//TODO check for null value, pitfall
			generic_serializer<SerializerClass, ValueType>::serialize(serializer, *ref.get());
		}
	};

	template<typename DeserializerClass, typename ValueType>
	struct generic_deserializer<DeserializerClass, ValueType, typename std::enable_if<deserialization_available<DeserializerClass, ValueType>::value>::type>
	{
		static ValueType deserialize(DeserializerClass& d)
		{
			ValueType ret;
			d.deserialize(ret);
			return ret;
		}

		static void deserialize(DeserializerClass& d, ValueType& ret)
		{
			d.deserialize(ret);
		}
	};

	template<typename DeserializerClass, typename ValueType>
	struct generic_deserializer<DeserializerClass, ValueType, typename std::enable_if<std::is_enum<ValueType>::value>::type>
	{
		static ValueType deserialize(DeserializerClass& d)
		{
			typename std::underlying_type<ValueType>::type ret;
			d.deserialize(ret);
			return (ValueType)ret;
		}

		static void deserialize(DeserializerClass& d, ValueType& ret)
		{
			typename std::underlying_type<ValueType>::type tmp;
			d.deserialize(tmp);
			ret = (ValueType)tmp;
		}
	};


	template<typename DeserializerClass, typename ValueType>
	struct generic_deserializer<DeserializerClass, std::unique_ptr<ValueType>, void>
	{
		static std::unique_ptr<ValueType> deserialize(DeserializerClass& d)
		{
			std::unique_ptr<ValueType> ret;
			deserialize(d, ret);
			return ret;
		}

		static void deserialize(DeserializerClass& d, std::unique_ptr<ValueType>& ret)
		{
			ret.reset(new ValueType);
			generic_deserializer<DeserializerClass, ValueType>::deserialize(d, *ret.get());
		}
	};

	template<typename DeserializerClass, typename... Args>
	struct parameter_deserializer_functor
	{
		//TODO migrate sequence generator
		//typedef typename Core::Metaprogramming::SequenceGenerator<sizeof...(Args)>::Type args_sequence;

		typedef std::integer_sequence<std::size_t, sizeof...(Args)> args_sequence;

		template<typename T, typename TVoid = void>
		struct deserializer_selector
		{
			static void deserialize(DeserializerClass& dOut, T& ref)
			{
				auto d = dOut.deserialize_scope();
				generic_deserializer<DeserializerClass, T>::deserialize(d, ref);
				d.finalize();
			}
		};

		template<typename T>
		struct deserializer_selector<T, typename std::enable_if<deserialization_available<DeserializerClass, T>::value>::type>
		{
			static void deserialize(DeserializerClass& d, T& ref)
			{
				d.deserialize(ref);
			}
		};

		template<typename T>
		struct deserializer_selector<T, typename std::enable_if<std::is_enum<T>::value>::type>
		{
			static void deserialize(DeserializerClass& d, T& ref)
			{
				typename std::underlying_type<T>::type val;
				d.deserialize(val);
				ref = (T)val;
			}
		};

		template<int... S>
		static std::tuple<typename std::decay<Args>::type...> func(DeserializerClass& d, std::integer_sequence<std::size_t, S...>)
		{
			std::tuple<typename std::decay<Args>::type...> ret;

			int list[] = {(deserializer_selector<Args>::deserialize(d, std::get<S>(ret)), 0)...};
			(void)list;

			return ret;
		}

		static std::tuple<typename std::decay<Args>::type...> func(DeserializerClass& d)
		{
			return Func(d, args_sequence());
		}
	};
}

template<typename ValueType, ValueType theValue, typename Predicates>
ValueType gserializer::constant_value<ValueType, theValue, Predicates>::theValueStorage = theValue;

//TODO migrate headers
#include "core/serializer_functor.hpp"
//#include "TFC/Serialization/PredicateEvaluator.h"

#define GSERIALIZER_DEFINE_TYPE_INFO( CLASS, ... ) \
	template<> \
	struct gserializer::type_serialization_info_selector< CLASS > \
	{ \
		static constexpr bool available = true; \
		typedef gserializer::type_serialization_info< CLASS , ##__VA_ARGS__ > type; \
	}

//TODO migrate introspect library and correct macro below
//TODO feature check and use C++ auto template variable type instead
#define GS_FIELD(MEMPTR, ...) gserializer::field_info<& MEMPTR, std::tuple < __VA_ARGS__ > >
#define GS_CONSTANT(CONSTANT, ...) gserializer::constant_value< std::remove_const<decltype( CONSTANT )>::type, CONSTANT, std::tuple < __VA_ARGS__ > >




#endif /* INCLUDE_GSERIALIZER_SERIALIZATION_HPP_ */
