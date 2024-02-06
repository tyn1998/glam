#include <cmath>

#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics/mean.hpp>
#include <boost/accumulators/statistics/stats.hpp>
#include <boost/accumulators/statistics/variance.hpp>

#include <boost/foreach.hpp>
#include <boost/geometry.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/math/special_functions/pow.hpp>
#include <boost/range/combine.hpp>

#include <kw/algorithm/normalize.hpp>
#include <kw/iterator_range.hpp>

#include <vector> // 加入vector头文件

namespace kw {

// The coefficient of variance of the edge length
template <typename Graph, typename PositionMap>
double edge_length_cv(Graph const& g, PositionMap pos) {
    typedef typename boost::graph_traits<Graph>::edge_descriptor edge_desc;
    auto const num_edges = boost::num_edges(g);
    std::vector<double> lengths(num_edges);

    // 替换 parallel_for_each 为简单的 for 循环
    auto edges = kw::make_iterator_range(boost::edges(g));
    size_t i = 0; // 初始化索引计数器
    for (auto itr = edges.begin(); itr != edges.end(); ++itr, ++i) {
        auto const e = *itr;
        auto& l = lengths[i];

        auto const p = boost::get(pos, boost::source(e, g));
        auto const q = boost::get(pos, boost::target(e, g));
        l = boost::geometry::distance(p, q);
    }

    bool const same = !kw::minmax_normalize(lengths);
    if (same) return 0.0;

    // 替换 parallel_reduce 为简单的累加循环
    double sum = 0.0;
    for (size_t i = 0; i < num_edges; ++i) {
        sum += lengths[i];
    }
    auto const mean = sum / num_edges;

    std::vector<double> sq_diffs(num_edges);
    // 替换另一个 parallel_for 为普通循环
    for (std::size_t i = 0; i < num_edges; ++i) {
        sq_diffs[i] = boost::math::pow<2>(lengths[i] - mean);
    }

    // 再次替换 parallel_reduce 为简单的累加循环
    double sq_diff_sum = 0.0;
    for (std::size_t i = 0; i < num_edges; ++i) {
        sq_diff_sum += sq_diffs[i];
    }

    auto const stddev = std::sqrt(sq_diff_sum / num_edges);

    return stddev / mean;
}

// The normalized coefficient of variance of the edge length
// The upper bound of the coefficient of variation of n values is sqrt(n - 1).
template <typename Graph, typename PositionMap>
double edge_length_normalized_cv(Graph const& g, PositionMap pos, double& cv) {
    cv = edge_length_cv(g, pos);
    return cv / std::sqrt(boost::num_edges(g) - 1);
}

template <typename Graph, typename PositionMap>
double edge_length_normalized_cv(Graph const& g, PositionMap pos) {
    double cv;
    return edge_length_normalized_cv(g, pos, cv);
}

}  // namespace kw
