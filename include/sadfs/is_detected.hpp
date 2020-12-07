#ifndef SADFS_IS_DETECTED_HPP
#define SADFS_IS_DETECTED_HPP

/*          
 *    =====  template meta-programming black magic  =====
 *
 * a (possibly botched) implementation of the detection idiom
 * as defined here:
 * https://en.cppreference.com/w/cpp/experimental/is_detected
 *
 * references:
 * Marshall Clow's presentation: https://www.youtube.com/watch?v=U3jGdnRL3KI
 * Sy Brand's blogpost: https://blog.tartanllama.xyz/detection-idiom/
 */

#include <type_traits>

namespace sadfs { namespace detail {

template <typename AlwaysVoid,
          template <typename...> typename Op,
          typename... Args>
struct DETECTOR : std::false_type
{ /* handles cases when Op<Args...> is ill-formed */ };

template <template <typename...> typename Op,
          typename... Args>
struct DETECTOR<std::void_t<Op<Args...>>, Op, Args...> : std::true_type
{ /* handles cases when Op<Args...> is well-formed */ };

} // detail namespace

template <template <typename...> typename Op,
          typename... Args>
using is_detected = detail::DETECTOR<void, Op, Args...>;

} // sadfs namespace

#endif // SADFS_IS_DETECTED_HPP
