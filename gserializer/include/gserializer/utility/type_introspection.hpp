/*
 * type_introspection.hpp
 *
 *  Created on: May 7, 2018
 *      Author: gilang
 */

#ifndef INCLUDE_GSERIALIZER_UTILITY_TYPE_INTROSPECTION_HPP_
#define INCLUDE_GSERIALIZER_UTILITY_TYPE_INTROSPECTION_HPP_

#include <type_traits>
#include <tuple>
#include <functional>

namespace gserializer::utility
{
	template<typename T>
	class has_member_access_operator
	{
		typedef char yes;
		typedef long no;
		template<typename TTest> static yes test(decltype(&TTest::operator->));
		template<typename TTest> static no test(...);

	public:
		static constexpr bool value = sizeof(test<T>(0)) == sizeof(yes);
	};

	template<typename T>
	inline constexpr bool has_member_access_operator_v = has_member_access_operator<T>::value;

	template<typename T>
	class has_call_operator
	{
		typedef char yes;
		typedef long no;
		template<typename TTest> static yes test(decltype(&TTest::operator()));
		template<typename TTest> static no test(...);

	public:
		static constexpr bool value = sizeof(test<T>(0)) == sizeof(yes);
	};

	template<typename T>
	inline constexpr bool has_call_operator_v = has_call_operator<T>::value;


	template<typename MemPtrType>
	struct member_field
	{

	};

	template<typename Class, typename FieldType>
	struct member_field<FieldType (Class::*)>
	{
		typedef Class declaring_type;
		typedef FieldType field_type;
	};

	template<typename FuncType>
	struct member_function
	{

	};

	template<typename Class, typename ReturnType, typename... Args>
	struct member_function<ReturnType (Class::*)(Args...)>
	{
		static constexpr auto arity = sizeof...(Args);
		typedef ReturnType return_type;
		typedef Class	declaring_type;

		template<size_t idx>
		using args = typename std::tuple_element<idx, std::tuple<Args...>>::type;

		typedef std::tuple<Args...> args_tuple;
		typedef std::tuple<typename std::decay<Args>::type...> args_tuple_decay;
	};

	template<typename Class, typename ReturnType, typename... Args>
	struct member_function<ReturnType (Class::*)(Args...) const> : member_function<ReturnType (Class::*)(Args...)>
	{

	};

	template<typename T>
	struct static_function
	{

	};

	template<typename ReturnType, typename... Args>
	struct static_function<ReturnType(Args...)>
	{
		static constexpr auto arity = sizeof...(Args);
		typedef ReturnType return_type;

		template<size_t idx>
		using args = typename std::tuple_element<idx, std::tuple<Args...>>::type;

		typedef std::tuple<Args...> args_tuple;
		typedef std::tuple<typename std::decay<Args>::type...> args_tuple_decay;
	};

	template<typename T>
	struct is_member_function
	{
		typedef char yes;
		typedef long no;
		template<typename TTest> static yes test(typename TTest::declaring_type*);
		template<typename TTest> static no test(...);

	public:
		static constexpr bool value = sizeof(test<T>(0)) == sizeof(yes);
		typedef member_function<T> introspect;
	};

	template<typename T>
	inline constexpr bool is_member_function_v = is_member_function<T>::value;

	template<typename T>
	struct is_function_pointer
	{
		static constexpr bool value = false;
	};

	template<typename ReturnType, typename... Args>
	struct is_function_pointer<ReturnType(*)(Args...)>
	{
		static constexpr bool value = true;
	};

	template<typename T>
	inline constexpr bool is_function_pointer_v = is_function_pointer<T>::value;


	template<typename T, bool = has_call_operator_v<T> || std::is_function_v<typename std::remove_pointer<T>::type>>
	struct callable_object;

	template<typename T>
	struct callable_object<T, false>
	{
		static constexpr bool callable = false; // Fallback
	};

	template<typename T>
	struct callable_object<T, true> : member_function<decltype(&T::operator())>
	{
		static constexpr bool callable = true;
		using return_type = typename member_function<decltype(&T::operator())>::return_type;
	};

	template<typename ReturnType, typename... Args>
	struct callable_object<ReturnType(*)(Args...), true> : static_function<ReturnType(Args...)>
	{
		static constexpr bool callable = true;
		using return_type = typename static_function<ReturnType(Args...)>::return_type;
	};

	template<typename T>
	inline constexpr bool is_callable_object_v = callable_object<T>::callable;

	template<size_t size>
	struct storage_selector
	{
		using type = uint8_t[size];
	};

	template<>
	struct storage_selector<1>
	{
		using type = uint8_t;
	};

	template<>
	struct storage_selector<2>
	{
		using type = uint16_t;
	};

	template<>
	struct storage_selector<4>
	{
		using type = uint32_t;
	};

	template<>
	struct storage_selector<8>
	{
		using type = uint64_t;
	};

	template<>
	struct storage_selector<16>
	{
		typedef struct { uint64_t value[2]; } type;
	};

	inline bool operator==(storage_selector<16>::type const& a, storage_selector<16>::type const& b)
	{
		return a.value[0] == b.value[0] && a.value[1] == b.value[1];
	}

	inline bool operator!=(storage_selector<16>::type const& a, storage_selector<16>::type const& b)
	{
		return a.value[0] != b.value[0] || a.value[1] != b.value[1];
	}

	template<size_t size>
	using storage_selector_t = typename storage_selector<size>::type;

	struct pointer_to_member_function
	{
	private:
		class dummy { };
		static constexpr auto pointerSize = sizeof(void(dummy::*)(void));

		template<typename PtrType>
		union pointer_to_member_function_value;

		template<typename Class, typename ReturnType, typename... Args>
		union pointer_to_member_function_value<ReturnType (Class::*) (Args...)>
		{
			ReturnType (Class::* pointerValue) (Args...);
			storage_selector_t<pointerSize> value;
		};

		storage_selector_t<pointerSize> value;

		friend struct pointer_to_member_function_hash;

	public:
		template<typename PtrType>
		static constexpr pointer_to_member_function get(PtrType ptr)
		{
			pointer_to_member_function_value<PtrType> obj { ptr };
			return { obj.value };
		}

		bool operator==(pointer_to_member_function const& other)
		{
			return value == other.value;
		}
	};



	struct pointer_to_member
	{
	private:
		class dummy { };
		static constexpr auto pointerSize = sizeof(int (dummy::*));

		template<typename PtrType>
		union pointer_to_member_value;

		template<typename Class, typename FieldType>
		union pointer_to_member_value<FieldType (Class::*)>
		{
			FieldType (Class::* pointerValue);
			storage_selector_t<pointerSize> value;
		};

		storage_selector_t<pointerSize> value;

		friend struct pointer_to_member_hash;
	public:
		template<typename PtrType>
		static constexpr pointer_to_member get(PtrType ptr)
		{
			pointer_to_member_value<PtrType> obj { ptr };
			return { obj.value };
		}

		bool operator==(pointer_to_member const& other)
		{
			return value == other.value;
		}
	};
}

template<>
struct std::hash<gserializer::utility::storage_selector_t<16>>
{
	typedef gserializer::utility::storage_selector_t<16> argument_type;
	typedef std::size_t result_type;

	result_type operator()(argument_type const& s) const
	{
		std::hash<long long> hasher;

		return hasher(s.value[0]) ^ hasher(s.value[1]);
	}
};

namespace gserializer::utility
{
	struct pointer_to_member_function_hash
	{
		size_t operator()(pointer_to_member_function const& obj) const
		{
			auto val = obj.value;
			return std::hash<decltype(val)>()(val);
		}
	};

	struct pointer_to_member_hash
	{
		size_t operator()(pointer_to_member const& obj) const
		{
			auto val = obj.value;
			return std::hash<decltype(val)>()(val);
		}
	};
}

#endif /* INCLUDE_GSERIALIZER_UTILITY_TYPE_INTROSPECTION_HPP_ */
