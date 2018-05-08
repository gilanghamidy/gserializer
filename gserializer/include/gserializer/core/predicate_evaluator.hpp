/*
 * predicate_evaluator.hpp
 *
 *  Created on: May 8, 2018
 *      Author: gilang
 */

#ifndef INCLUDE_GSERIALIZER_SERIALIZATION_HPP_
#error "Cannot include SerializerFunctor.h by itself without including TFC/Serialization.h"
#else

#ifndef INCLUDE_GSERIALIZER_CORE_PREDICATE_EVALUATOR_HPP_
#define INCLUDE_GSERIALIZER_CORE_PREDICATE_EVALUATOR_HPP_

namespace gserializer
{
	template<typename... Predicates>
	struct predicate_evaluator<std::tuple<Predicates...>>
	{

		template<typename Predicate, typename T>
		static bool call_evaluate(T const& obj)
		{
			return Predicate::evaluate(obj);
		}

		template<typename T>
		static bool evaluate(T const& obj)
		{
			bool res = true;
			using expansion  = int[];
			(void)expansion {0, ((res = res && call_evaluate<Predicates>(obj)), 0)... };

			return res;
		}
	};

	template<>
	struct predicate_evaluator<std::tuple<>>
	{
		template<typename T>
		static bool evaluate(T const& obj)
		{
			return true;
		}
	};
}



#endif /* INCLUDE_GSERIALIZER_CORE_PREDICATE_EVALUATOR_HPP_ */
#endif
