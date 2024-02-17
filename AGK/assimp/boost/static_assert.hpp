
#ifndef AI_BOOST_STATIC_ASSERT_INCLUDED
#define AI_BOOST_STATIC_ASSERT_INCLUDED

#ifndef BOOST_STATIC_ASSERT

namespace assimpboost {
	namespace detail {

		template <bool b>  class static_assertion_failure;
		template <>        class static_assertion_failure<true> {};
	}
}


#define BOOST_STATIC_ASSERT(eval) \
{assimpboost::detail::static_assertion_failure<(eval)> assert_dummy;(void)assert_dummy;}

#endif
#endif // !! AI_BOOST_STATIC_ASSERT_INCLUDED
