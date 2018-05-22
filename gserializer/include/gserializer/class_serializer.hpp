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
 *    core/class_serializer.hpp
 *
 * Classes to declare serialization process for C++ classes
 *
 * Author: 		Gilang Mentari Hamidy (g.hamidy@samsung.com)
 * Contributor: Kevin Winata (k.winata@samsung.com)
 */

#ifndef INCLUDE_GSERIALIZER_CORE_CLASS_SERIALIZER_HPP_
#define INCLUDE_GSERIALIZER_CORE_CLASS_SERIALIZER_HPP_

#include "gserializer/serialization.hpp"

namespace gserializer
{
	template<typename SerializerClass, typename ValueType, typename SerializationInfo = typename type_serialization_info_selector<ValueType>::type>
	struct class_serializer;

	template<typename DeserializerClass, typename ValueType, typename SerializationInfo = typename type_serialization_info_selector<ValueType>::type>
	struct class_deserializer;

	template<typename SerializerClass, typename ValueType>
	struct generic_serializer<SerializerClass, ValueType, typename std::enable_if<type_serialization_info_selector<ValueType>::available>::type>
	{
		typedef class_serializer<SerializerClass, ValueType> serializer;

		static auto serialize(ValueType const& ref)
		{
			return serializer::serialize(ref);
		}

		static void serialize(SerializerClass& ser, ValueType const& ref)
		{
			serializer::serialize(ser, ref);
		}
	};

	template<typename DeserializerClass, typename ValueType>
	struct generic_deserializer<DeserializerClass, ValueType, typename std::enable_if<type_serialization_info_selector<ValueType>::available>::type>
	{
		typedef class_deserializer<DeserializerClass, ValueType> deserializer;

		static auto deserialize(typename DeserializerClass::serialized_type p, bool finalizePackedObject = true)
		{
			return deserializer::deserialize(p, finalizePackedObject);
		}

		static ValueType deserialize(DeserializerClass& deser)
		{
			return deserializer::deserialize(deser);
		}

		static void deserialize(DeserializerClass& deser, ValueType& ret)
		{
			deserializer::deserialize(deser, ret);
		}
	};

	template<typename SerializerClass, typename ValueType, typename... Fields>
	struct class_serializer<SerializerClass, ValueType, type_serialization_info<ValueType, Fields...>>
	{
		static auto serialize(ValueType const& ref)
		{
			SerializerClass ser;
			serializer_functor<SerializerClass, typename Fields::value_type...>::func(ser, serializer_field<typename Fields::value_type> { Fields::get(ref), Fields::evaluate(ref) }...);
			return ser.end_serialize();
		}

		static void serialize(SerializerClass& packer, ValueType const& ref)
		{
			serializer_functor<SerializerClass, typename Fields::value_type...>::func(packer, serializer_field<typename Fields::value_type> { Fields::get(ref), Fields::evaluate(ref) }...);
		}
	};

	template<typename DeserializerClass, typename Declaring, typename Field, typename = void>
	struct class_deserializer_select
	{
		static void deserialize_and_set(DeserializerClass& d, Declaring& obj, int curIdx = 0)
		{
			if(Field::evaluate(obj))
			{
				Field::deserialize_and_set(obj, d);//.template Deserialize<typename TField::ValueType>(curIdx));
			}
		}
	};

	template<typename DeserializerClass, typename ValueType, typename... Fields>
	struct class_deserializer_functor;

	template<typename DeserializerClass, typename ValueType, typename CurrentField, typename... RemainingFields>
	struct class_deserializer_functor<DeserializerClass, ValueType, CurrentField, RemainingFields...>
	{
		static void func(DeserializerClass& d, ValueType& obj, int curIdx = 0)
		{
			class_deserializer_select<DeserializerClass, ValueType, CurrentField>::deserialize_and_set(d, obj, curIdx);
			class_deserializer_functor<DeserializerClass, ValueType, RemainingFields...>::func(d, obj, curIdx + 1);
		}
	};

	template<typename DeserializerClass, typename ValueType>
	struct class_deserializer_functor<DeserializerClass, ValueType>
	{
		static void func(DeserializerClass& p, ValueType& obj, int curIdx = 0) { }
	};

	template<typename DeserializerClass, typename ValueType, typename... Fields>
	struct class_deserializer<DeserializerClass, ValueType, type_serialization_info<ValueType, Fields...>>
	{
		static auto deserialize(typename DeserializerClass::serialized_type p, bool finalizePackedObject = true)
		{
			DeserializerClass deser(p);

			ValueType ret;
			class_deserializer_functor<DeserializerClass, ValueType, Fields...>::func(deser, ret);

			if(finalizePackedObject)
				deser.end_deserialize();

			return ret;
		}

		static auto deserialize(DeserializerClass& deser)
		{
			ValueType ret;
			class_deserializer_functor<DeserializerClass, ValueType, Fields...>::func(deser, ret);
			return ret;
		}

		static void deserialize(DeserializerClass& deser, ValueType& ret)
		{
			class_deserializer_functor<DeserializerClass, ValueType, Fields...>::func(deser, ret);
		}
	};

	template<auto memPtr, typename Predicates>
	struct field_info<memPtr, Predicates, utility::void_t<typename type_serialization_info_selector<typename utility::member_function<decltype(memPtr)>::return_type>::type>>
	{
		using pointer_to_member_info = utility::member_function<decltype(memPtr)>;
		using value_type = typename pointer_to_member_info::return_type;
		using declaring_class = typename pointer_to_member_info::declaring_type;
		using type_info = typename type_serialization_info_selector<value_type>::type;

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
			return predicate_evaluator<Predicates>::evaluate(ref);
		}

		template<typename DeserializerClass>
		static void deserialize_and_set(declaring_class& ref, DeserializerClass& deser)
		{
			decltype(auto) deserInner = deser.begin_scope();
			generic_deserializer<DeserializerClass, value_type>::Deserialize(deserInner, ref.*memPtr);
		}
	};
}

#endif /* INCLUDE_GSERIALIZER_CORE_CLASS_SERIALIZER_HPP_ */
