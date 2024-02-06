// Copyright 2017 Oh-Hyun Kwon (kwonoh.net)
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef __KW_ALGORITHM_NORMALIZE_HPP__
#define __KW_ALGORITHM_NORMALIZE_HPP__

#include <algorithm>

#include <kw/math/comparison.hpp>

namespace kw {

template <typename Container>
bool
minmax_normalize(Container& c)
{
    auto const minmax = std::minmax_element(c.begin(), c.end());
    auto const min = *minmax.first;
    auto const max = *minmax.second;
    if (kw::epsilon_equal(min, max)) return false;

    auto const range = max - min;

    std::transform(std::begin(c), std::end(c), std::begin(c),
                   [&](auto const x) { return (x - min) / range; });
    return true;
}

}  // namespace kw

#endif  // __KW_ALGORITHM_NORMALIZE_HPP__
