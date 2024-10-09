#ifndef SINGLEPP_LOADERS_RANKINGS_HPP
#define SINGLEPP_LOADERS_RANKINGS_HPP

#include "byteme/PerByte.hpp"
#include "byteme/RawFileReader.hpp"
#include "byteme/GzipFileReader.hpp"
#include "byteme/ZlibBufferReader.hpp"

#include "tatami/tatami.hpp"
#include "singlepp/singlepp.hpp"

#include <string>
#include <vector>
#include <cctype>
#include <type_traits>
#include <stdexcept>

/**
 * @file rankings.hpp
 * @brief Load ranking matrices for **singlepp** reference datasets.
 */

namespace singlepp_loaders {

/**
 * Matrix of ranks as a dense column-major matrix.
 * Each column corresponds to a sample while each row corresponds to a feature.
 * Each column contains the ranked expression values for all features.
 *
 * @tparam Value_ Numeric type for data in the matrix interface.
 * @tparam Index_ Integer type for indices in the matrix interface.
 * @tparam Rank_ Integer type for the ranks being stored.
 */
template<typename Value_ = double, typename Index_ = int, typename Rank_ = int>
using RankMatrix = tatami::DenseMatrix<Value_, Index_, std::vector<Rank_> >;

/**
 * @brief Options for loading rankings.
 */
struct LoadRankingsOptions {
    /**
     * Whether to perform file reading, decompression and parsing in parallel.
     */
    bool parallel = false;

    /**
     * Size of the buffer for file reading and decompression.
     */
    size_t buffer_size = 65536;
};


/** 
 * @cond
 */
namespace internal {

template<typename Value_, typename Index_, bool parallel_>
RankMatrix<Value_, Index_> load_rankings(byteme::Reader& reader) {
    size_t nfeatures = 0;
    size_t line = 0;
    std::vector<int> values;

    size_t field = 0;
    bool non_empty = false;
    int current = 0;

    bool has_nfeatures = false;
    auto check_nfeatures = [&]() -> void {
        if (!has_nfeatures) {
            has_nfeatures = true;
            nfeatures = field + 1;
        } else if (field + 1 != nfeatures) {
            throw std::runtime_error("number of fields on each line should be equal to the number of features");
        }
    };

    typename std::conditional<parallel_, byteme::PerByte<char>, byteme::PerByteParallel<char> >::type pb(&reader);
    bool okay = pb.valid();
    while (okay) {
        char x = pb.get();
        okay = pb.advance();

        if (x == '\n') {
            check_nfeatures();
            if (!non_empty) {
                throw std::runtime_error("fields should not be empty");
            }
            values.push_back(current);
            current = 0;
            field = 0;
            non_empty = false;
            ++line;

        } else if (x == ',') {
            if (!non_empty) {
                throw std::runtime_error("fields should not be empty");
            }
            values.push_back(current);
            current = 0;
            ++field;
            non_empty = false;

        } else if (std::isdigit(x)) {
            non_empty = true;
            current *= 10;
            current += (x - '0');

        } else {
            throw std::runtime_error("fields should only contain integer ranks");
        }
    }

    if (field || non_empty) { // aka no terminating newline.
        check_nfeatures();
        if (!non_empty) {
            throw std::runtime_error("fields should not be empty");
        }
        values.push_back(current);
        ++line;
    }

    return RankMatrix<Value_, Index_>(nfeatures, line, std::move(values), false);
}

template<typename Value_, typename Index_>
RankMatrix<Value_, Index_> load_rankings(byteme::Reader& reader, bool parallel) {
    if (parallel) {
        return load_rankings<Value_, Index_, true>(reader);
    } else {
        return load_rankings<Value_, Index_, false>(reader);
    }
}

}
/** 
 * @endcond
 */

/**
 * @tparam Value_ Numeric type for data in the matrix interface.
 * @tparam Index_ Integer type for indices in the matrix interface.
 * 
 * @param path Path to a text file containing the ranking matrix.
 * @param options Options for reading the rankings.
 *
 * @return A `RankMatrix` containing the feature rankings for each reference profile.
 * Each column corresponds to a reference profile while each row corresponds to a feature.
 *
 * The file should contain one line per reference profile, with the total number of lines equal to the number of profiles in the dataset.
 * Each line should contain the rank of each feature's expression within that profile, separated by commas.
 * The number of comma-separated fields on each line should be equal to the number of features.
 * Ranks should be strictly integer - tied ranks should default to the minimum rank among the index set of ties.
 */
template<typename Value_ = singlepp::DefaultValue, typename Index_ = singlepp::DefaultIndex>
RankMatrix<Value_, Index_> load_rankings_from_text_file(const char* path, const LoadRankingsOptions& options) {
    byteme::RawFileReader reader(path, options.buffer_size);
    return internal::load_rankings<Value_, Index_>(reader, options.parallel);
}

/**
 * @tparam Value_ Numeric type for data in the matrix interface.
 * @tparam Index_ Integer type for indices in the matrix interface.
 *
 * @param path Path to a Gzip-compressed file containing the ranking matrix.
 * @param options Options for reading the rankings.
 *
 * @return A `RankMatrix` containing the feature rankings for each reference profile.
 * Each column corresponds to a reference profile while each row corresponds to a feature.
 *
 * See `load_rankings_from_text_file()` for details about the format.
 */
template<typename Value_ = singlepp::DefaultValue, typename Index_ = singlepp::DefaultIndex>
RankMatrix<Value_, Index_> load_rankings_from_gzip_file(const char* path, const LoadRankingsOptions& options) {
    byteme::GzipFileReader reader(path, options.buffer_size);
    return internal::load_rankings<Value_, Index_>(reader, options.parallel);
}

/**
 * @tparam Value_ Numeric type for data in the matrix interface.
 * @tparam Index_ Integer type for indices in the matrix interface.
 *
 * @param[in] buffer Pointer to an array containing a Zlib/Gzip-compressed string containing the ranking matrix.
 * @param len Length of the array for `buffer`.
 * @param options Options for reading the rankings.
 *
 * @return A `RankMatrix` containing the feature rankings for each reference profile.
 * Each column corresponds to a reference profile while each row corresponds to a feature.
 *
 * See `load_rankings_from_text_file()` for details about the format.
 */
template<typename Value_ = singlepp::DefaultValue, typename Index_ = singlepp::DefaultIndex>
RankMatrix<Value_, Index_> load_rankings_from_zlib_buffer(const unsigned char* buffer, size_t len, const LoadRankingsOptions& options) {
    byteme::ZlibBufferReader reader(buffer, len, 3, options.buffer_size);
    return internal::load_rankings<Value_, Index_>(reader, options.parallel);
}

}

#endif
