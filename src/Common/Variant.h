// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef COMMON_VARIANT_H_
#define COMMON_VARIANT_H_

#if !__has_include(<optional>)
#    include <experimental/optional>

namespace std
{
    using experimental::make_optional;
    using experimental::nullopt;
}  // namespace std

#else
#    include <optional>
#endif

// clang-format off
#include "ThirdParty/DisableWarnings.h"
#include <mapbox/variant.hpp>  // NOLINT(llvm-include-order)
#include "ThirdParty/ReenableWarnings.h"
// clang-format on

namespace rainbow
{
    template <typename... Types>
    using variant = mapbox::util::variant<Types...>;

    template <typename T, typename... Types>
    auto get(variant<Types...>& v)
    {
        return !v.template is<T>()
                   ? std::nullopt
                   : std::make_optional(v.template get_unchecked<T>());
    }

    template <typename T, typename... Types>
    auto get(const variant<Types...>& v)
    {
        return !v.template is<T>()
                   ? std::nullopt
                   : std::make_optional(v.template get_unchecked<T>());
    }

    template <typename T, typename... Types>
    constexpr auto holds_alternative(const variant<Types...>& v)
    {
        return v.template is<T>();
    }

    template <typename... Args>
    auto visit(Args&&... args)
    {
        return mapbox::util::apply_visitor(std::forward<Args>(args)...);
    }
}  // namespace rainbow

#endif
