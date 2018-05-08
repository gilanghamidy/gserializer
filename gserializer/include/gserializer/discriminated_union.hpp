/*
 * discriminated_union.hpp
 *
 *  Created on: May 8, 2018
 *      Author: gmh
 */

#ifndef INCLUDE_GSERIALIZER_DISCRIMINATED_UNION_HPP_
#define INCLUDE_GSERIALIZER_DISCRIMINATED_UNION_HPP_

#include "gserializer/serialization.hpp"
#include "gserializer/utility/type_introspection.hpp"

namespace gserializer
{
	template<typename T>
	struct discriminated_union_type_info_selector
	{
		static constexpr bool is_discriminated_union = false;
	};

	template<typename T, typename... Tails>
	struct discriminated_union_selector;

	template<typename T, typename... Cases>
	struct discriminated_union_field;

	template<typename T, auto discriminator, typename... Cases>
	struct discriminated_union_type_info
	{
		static_assert(utility::is_member_field_pointer_v<decltype(discriminator)>, "Discriminator parameter is not pointer to member field");

		using value_type = discriminated_union_field<T, Cases...>;
		using discriminator_field = utility::member_field_pointer<decltype(discriminator)>;

		static_assert(std::is_same_v<typename discriminator_field::declaring_type, T>, "Discriminator is not a member of T");

		using discriminator_type = typename discriminator_field::field_type;

		template<typename DeclaringClass>
		static value_type const get(T DeclaringClass::* memPtr, DeclaringClass const& ptr)
		{
			auto& val =  ptr.*memPtr;
			uint32_t disc = (uint32_t)(val.*discriminator);
			return { val, disc };
		}

		static value_type const get(T const& obj)
		{
			uint32_t disc = uint32_t(obj.*discriminator);
			return { obj, disc };
		}

		template<typename DeclaringClass, typename DeserializerClass>
		static void deserialize_and_set(T DeclaringClass::* memPtr, DeclaringClass& ptr, uint32_t discriminatorVal, DeserializerClass& deser, int curIdx)
		{
			auto& val = ptr.*memPtr;
			val.*discriminator = (discriminator_type)discriminatorVal;
			discriminated_union_selector<T, Cases...>::deserialize_and_set(deser, val, discriminatorVal, curIdx);
		}

		template<typename DeserializerClass>
		static void deserialize_and_set(T& ref, DeserializerClass& deser)
		{
			uint32_t discriminatorVal = 0;
			deser.deserialize(discriminatorVal);

			ref.*discriminator = (discriminator_type)discriminatorVal;
			discriminated_union_selector<T, Cases...>::deserialize_and_set(deser, ref, discriminatorVal, 0);
		}

		template<typename DeserializerClass>
		static T deserialize(DeserializerClass& deser)
		{
			T ret;
			uint32_t discriminatorVal = 0;
			deser.deserialize(discriminatorVal);

			ret.*discriminator = (discriminator_type)discriminatorVal;
			discriminated_union_selector<T, Cases...>::deserialize_and_set(deser, ret, discriminatorVal, 0);
			return ret;
		}

		template<typename DeserializerClass>
		static T deserialize(DeserializerClass& deser, uint32_t discriminatorVal)
		{
			T ret;
			ret.*discriminator = (discriminator_type)discriminatorVal;
			discriminated_union_selector<T, Cases...>::deserialize_and_set(deser, ret, discriminatorVal, 0);
			return ret;
		}
	};

	template<auto memPtr, typename Predicates>
	struct field_info<memPtr, Predicates, utility::void_t<typename discriminated_union_type_info_selector<typename utility::member_field_pointer<decltype(memPtr)>::field_type>::type>>
	{
		using member_field_info = typename utility::member_field_pointer<decltype(memPtr)>;
		using type_info = typename discriminated_union_type_info_selector<typename member_field_info::field_type>::type ;
		using value_type = typename type_info::value_type;

		static value_type get(typename member_field_info::declaring_type const& ref)
		{
			return type_info::get(memPtr, ref);
		}

		static bool evaluate(typename member_field_info::declaring_type const& ref)
		{
			return predicate_evaluator<Predicates>::evaluate(ref);
		}
	};

	template<uint32_t discriminatorV, auto memPtr>
	struct discriminated_union_case
	{
		static_assert(utility::is_member_field_pointer_v<decltype(memPtr)>, "Discriminator parameter is not pointer to member field");
		using member_field_info = typename utility::member_field_pointer<decltype(memPtr)>;
		using value_type = typename member_field_info::field_type;
		using declaring_type = typename member_field_info::declaring_type;

		static uint32_t const discriminator = discriminatorV;
		static constexpr auto target_pointer = memPtr;

		static constexpr bool match(uint32_t v) { return discriminator == v; }
		static value_type const& get(declaring_type const& o) { return o.*memPtr; }
	};
}


#define GSERIALIZER_DEFINE_DISCRIMINATED_UNION_TYPE_INFO(TYPENAME, DISCR, ... ) \
	template<> \
	struct gserializer::discriminated_union_type_info_selector < TYPENAME > \
	{ \
		static constexpr bool is_discriminated_union = true; \
		typedef gserializer::discriminated_union_type_info< TYPENAME , & TYPENAME :: DISCR , __VA_ARGS__ > type; \
	}

#define GS_UNION_CASE(DISCR, MEMPTR) gserializer::discriminated_union_case< (uint32_t) DISCR, & MEMPTR>



#endif /* INCLUDE_GSERIALIZER_DISCRIMINATED_UNION_HPP_ */
