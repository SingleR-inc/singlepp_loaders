#ifndef SINGLEPP_LOADERS_MARKERS_HPP
#define SINGLEPP_LOADERS_MARKERS_HPP

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
 * @file markers.hpp
 * @brief Load pre-compiled marker lists for **singlepp** reference datasets.
 */

namespace singlepp_loaders {

/**
 * @brief Options for loading markers.
 */
struct LoadMarkersOptions {
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

template<typename Index_, bool parallel_>
singlepp::Markers<Index_> load_markers(byteme::Reader& reader) {
    singlepp::Markers<Index_> markers;
    typename std::conditional<parallel_, byteme::PerByte<char>, byteme::PerByteParallel<char> >::type pb(&reader);

    bool okay = pb.valid();
    while (okay) {
        // Processing the label IDs.
        size_t first = 0, second = 0;
        for (int l = 0; l < 2; ++l) {
            auto& current = (l == 0 ? first : second);
            bool non_empty = false;

            do {
                char x = pb.get();
                okay = pb.advance();

                if (x == '\t') {
                    if (!non_empty) {
                        throw std::runtime_error("empty field detected in the label indices");
                    }
                    break;
                } else if (x == '\n') {
                    okay = false; // hit the error below.
                    break;
                } else if (!std::isdigit(x)) {
                    throw std::runtime_error("label indices should be integers");
                }

                non_empty = true;
                current *= 10;
                current += (x - '0');
            } while (okay);

            if (!okay) {
                throw std::runtime_error("expected at least three tab-separated fields on each line");
            }
        }

        if (first >= markers.size()) {
            markers.resize(first + 1);
        }
        auto& fmarkers = markers[first];
        if (second >= fmarkers.size()) {
            fmarkers.resize(second + 1);
        }
        auto& values = fmarkers[second];
        if (!values.empty()) {
            throw std::runtime_error("multiple marker sets listed for a single pairwise comparison");
        }

        // Processing the actual gene indices.
        bool non_empty = false;
        Index_ current = 0;
        while (okay) {
            char x = pb.get();
            okay = pb.advance();

            if (std::isdigit(x)) {
                non_empty = true;
                current *= 10;
                current += (x - '0');

            } else if (x == '\t') {
                if (!non_empty) {
                    throw std::runtime_error("gene index fields should not be empty");
                }
                values.push_back(current);
                current = 0;
                non_empty = false;

            } else if (x == '\n') {
                break;

            } else {
                throw std::runtime_error("gene index fields should be integers");
            }
        }

        // Adding the last element. We don't do this inside the newline check,
        // as we need to account for cases where the file is not newline-terminated.
        if (!non_empty) {
            throw std::runtime_error("gene index fields should not be empty");
        }
        values.push_back(current);
    }

    // Now, making sure that every label has the same number of elements.
    size_t expected_nlabels = markers.size();
    for (const auto& m : markers) {
        expected_nlabels = std::max(expected_nlabels, m.size());
    }
    markers.resize(expected_nlabels);
    for (auto& m : markers) {
        m.resize(expected_nlabels);
    }

    return markers;
}

template<typename Index_>
singlepp::Markers<Index_> load_markers(byteme::Reader& reader, bool parallel) {
    if (parallel) {
        return load_markers<Index_, true>(reader);
    } else {
        return load_markers<Index_, false>(reader);
    }
}

}
/** 
 * @endcond
 */

/**
 * @tparam Index_ Integer type for the marker indices.
 *
 * @param path Path to a text file containing the marker lists.
 * @param options Further options for reading.
 *
 * @return A `Markers` object containing the markers from each pairwise comparison between labels.
 *
 * The file should contain one line per pairwise comparison between labels.
 * Each line should at least 3 tab-delimited fields - the index of the first label, the index of the second label, 
 * and then the indices of the features selected as marker genes for the first label relative to the second.
 * Any (non-zero) number of marker indices may be reported provided they are ordered by marker strength.
 * The total number of lines in this file should be equal to the total number of pairwise comparisons between different labels, including permutations.
 */
template<typename Index_ = singlepp::DefaultIndex>
singlepp::Markers<Index_> load_markers_from_text_file(const char* path, const LoadMarkersOptions& options) {
    byteme::RawFileReader reader(path, options.buffer_size);
    return internal::load_markers<Index_>(reader, options.parallel);
}

/**
 * @tparam Index_ Integer type for the marker indices.
 *
 * @param path Path to a Gzip-compressed file containing the marker lists.
 * @param options Further options for reading.
 *
 * @return A `Markers` object containing the markers from each pairwise comparison between labels.
 *
 * See `load_markers_from_text_file()` for details about the format.
 */
template<typename Index_ = singlepp::DefaultIndex>
singlepp::Markers<Index_> load_markers_from_gzip_file(const char* path, const LoadMarkersOptions& options) {
    byteme::GzipFileReader reader(path, options.buffer_size);
    return internal::load_markers<Index_>(reader, options.parallel);
}

/**
 * @tparam Index_ Integer type for the marker indices.
 *
 * @param[in] buffer Pointer to an array containing a Zlib/Gzip-compressed string containing the marker lists.
 * @param len Length of the array for `buffer`.
 * @param options Further options for reading.
 *
 * @return A `Markers` object containing the markers from each pairwise comparison between labels.
 *
 * See `load_markers_from_text_file()` for details about the format.
 */
template<typename Index_ = singlepp::DefaultIndex>
singlepp::Markers<Index_> load_markers_from_zlib_buffer(const unsigned char* buffer, size_t len, const LoadMarkersOptions& options) {
    byteme::ZlibBufferReader reader(buffer, len, 3, options.buffer_size);
    return internal::load_markers<Index_>(reader, options.parallel);
}

}

#endif
