#ifndef SINGLEPP_LOADERS_VERIFY_HPP
#define SINGLEPP_LOADERS_VERIFY_HPP

#include "rankings.hpp"
#include "singlepp/singlepp.hpp"

#include <stdexcept>
#include <vector>

/**
 * @file verify.hpp
 * @brief Verify the integrity of the loaded components.
 */

namespace singlepp_loaders {

/**
 * This checks the consistency of all components of the reference dataset after they have been loaded:
 *
 * - The number of columns of `rankings` should be equal to the length of `labels`.
 * - The number of labels is defined as the largest value in `labels` plus 1.
 * - The length of `markers` and the number of inner vectors in each element of `markers` should be equal to the number of labels.
 * - Each gene index in `markers` should be non-negative and less than the number of rows in `rankings`.
 *
 * @tparam Value_ Numeric type for data in the matrix interface.
 * @tparam Index_ Integer type for indices in the matrix interface.
 * @tparam Rank_ Integer type for the ranks being stored.
 * @tparam Label_ Integer type for the labels.
 *
 * @param rankings The matrix produced by `load_rankings_from_text_file()` or related functions.
 * @param labels Vector of labels produced by `load_labels_from_text_file()` or related functions.
 * @param markers List of markers produced by `load_markers_from_text_file()` or related functions.
 */
template<typename Value_, typename Index_, typename Rank_, typename Label_>
void verify(const RankMatrix<Value_, Index_, Rank_>& rankings, const std::vector<Label_>& labels, const singlepp::Markers<Index_>& markers) {
    size_t NC = rankings.ncol();
    if (labels.size() != NC) {
        throw std::runtime_error("number of columns of 'rankings' does not equal length of 'labels'");
    }

    size_t nlabels = 0;
    if (NC) {
        nlabels = static_cast<size_t>(*std::max_element(labels.begin(), labels.end())) + 1;
    }
    if (markers.size() != nlabels) {
        throw std::runtime_error("length of 'markers' does not equal the number of labels in 'labels'");
    }

    Index_ NR = rankings.nrow();
    for (const auto& mm : markers) {
        if (mm.size() != nlabels) {
            throw std::runtime_error("number of inner vectors in 'markers' does not equal the number of labels in 'labels'");
        }
        for (const auto& m : mm) {
            for (auto x : m) {
                if (x < 0 || x >= NR) {
                    throw std::runtime_error("gene indices in 'markers' should be non-negative and less than the number of rows in 'rankings'");
                }
            }
        }
    }
}

}

#endif
