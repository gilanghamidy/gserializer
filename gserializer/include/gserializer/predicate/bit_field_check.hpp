/*
 * bit_field_check.hpp
 *
 *  Created on: May 8, 2018
 *      Author: gilang
 */

#ifndef INCLUDE_GSERIALIZER_PREDICATE_BIT_FIELD_CHECK_HPP_
#define INCLUDE_GSERIALIZER_PREDICATE_BIT_FIELD_CHECK_HPP_

#include "gserializer/utility/type_introspection.hpp"

namespace gserializer::predicate
{

	template<auto memPtr, uint32_t v>
	struct bit_field_check_predicate
	{
		static_assert(utility::is_member_field_pointer_v<memPtr>, "memPtr is not a member function pointer");

		using member_info = utility::member_field_pointer<decltype(memPtr)>;
		using declaring_type = typename member_info::declaring_type;

		static bool evaluate(declaring_type const& obj)
		{
			uint32_t val = (uint32_t)(obj.*memPtr);
			return (val & v) != 0;
		}
	};

}

#define GSP_BITFIELD(MEMPTR, VAL) gserializer::predicate::bit_field_check_predicate< & MEMPTR, VAL >



#endif /* INCLUDE_GSERIALIZER_PREDICATE_BIT_FIELD_CHECK_HPP_ */
