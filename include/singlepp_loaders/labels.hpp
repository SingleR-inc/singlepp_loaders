#ifndef SINGLEPP_LOADERS_LABELS_HPP
#define SINGLEPP_LOADERS_LABELS_HPP

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
 * @file labels.hpp
 * @brief Load label indices for **single** reference datasets.
 */

namespace singlepp_loaders {

/**
 * @brief Options for loading labels.
 */
struct LoadLabelsOptions {
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

template<typename Label_, bool parallel_>
std::vector<Label_> load_labels(byteme::Reader& reader) {
    bool non_empty = false;
    int current = 0;
    std::vector<Label_> labels;

    typename std::conditional<parallel_, byteme::PerByte<char>, byteme::PerByteParallel<char> >::type pb(&reader);
    bool okay = pb.valid();
    while (okay) {
        char x = pb.get();
        okay = pb.advance();

        if (x == '\n') {
            if (!non_empty) {
                throw std::runtime_error("label index must be an integer");
            }
            labels.push_back(current);
            current = 0;
            non_empty = false;
        } else if (std::isdigit(x)) {
            non_empty = true;
            current *= 10;
            current += (x - '0');
        } else {
            throw std::runtime_error("label index must be an integer");
        }
    } 

    if (non_empty) {
        labels.push_back(current);
    }

    return labels;
}

template<typename Label_>
std::vector<Label_> load_labels(byteme::Reader& reader, bool parallel) {
    if (parallel) {
        return load_labels<Label_, true>(reader);
    } else {
        return load_labels<Label_, false>(reader);
    }
}

}
/** 
 * @endcond
 */

/**
 * @tparam Label_ Integer type for the label identity.
 *
 * @param path Path to a text file containing the labels.
 * @param options Options for reading the labels.
 *
 * @return Vector containing the label index for each reference profile.
 *
 * The file should contain one line per profile, containing an integer label index for that profile.
 * Label indices refer to another array containing the actual names of the labels (see `load_label_names_from_text_file()`).
 * The total number of lines should be equal to the number of profiles in the dataset.
 * The file should not contain any header.
 */
template<typename Label_ = singlepp::DefaultLabel>
std::vector<Label_> load_labels_from_text_file(const char* path, const LoadLabelsOptions& options) {
    byteme::RawFileReader reader(path, options.buffer_size);
    return internal::load_labels<Label_>(reader, options.parallel);
}

/**
 * @tparam Label_ Integer type for the label identity.
 *
 * @param path Path to a Gzip-compressed file containing the labels.
 * @param options Options for reading the labels.
 *
 * @return Vector containing the label index for each reference profile.
 *
 * See `load_labels_from_text_file()` for details about the format.
 */
template<typename Label_ = singlepp::DefaultLabel>
std::vector<Label_> load_labels_from_gzip_file(const char* path, const LoadLabelsOptions& options) {
    byteme::GzipFileReader reader(path, options.buffer_size);
    return internal::load_labels<Label_>(reader, options.parallel);
}

/**
 * @tparam Label_ Integer type for the label identity.
 *
 * @param[in] buffer Pointer to an array containing a Zlib/Gzip-compressed string of labels.
 * @param len Length of the array for `buffer`.
 * @param options Options for reading the labels.
 *
 * @return Vector containing the label index for each reference profile.
 *
 * See `load_labels_from_text_file()` for details about the format.
 */
template<typename Label_ = singlepp::DefaultLabel>
std::vector<Label_> load_labels_from_zlib_buffer(const unsigned char* buffer, size_t len, const LoadLabelsOptions& options) {
    byteme::ZlibBufferReader reader(buffer, len, 3, options.buffer_size);
    return internal::load_labels<Label_>(reader, options.parallel);
}

}

#endif
