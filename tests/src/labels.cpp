#include <gtest/gtest.h>

#include "singlepp_loaders/labels.hpp"
#include "byteme/temp_file_path.hpp"
#include "zlib.h"

#include "utils.h"

#include <fstream>
#include <string>
#include <vector>

class LoadLabelsTest : public ::testing::TestWithParam<std::tuple<int, bool> > {};

TEST_P(LoadLabelsTest, TextFile) {
    auto path = byteme::temp_file_path("lab_text");
    std::vector<int> labels;
    {
        std::ofstream out(path, std::ofstream::out);
        for (size_t i = 0; i < 1000; ++i) {
            labels.push_back(i);
            out << i << "\n";
        }
    }

    auto params = GetParam();
    singlepp_loaders::LoadLabelsOptions options;
    options.buffer_size = std::get<0>(params);
    options.parallel = std::get<1>(params);

    auto reloaded = singlepp_loaders::load_labels_from_text_file(path.c_str(), options);
    EXPECT_EQ(reloaded, labels);
}

TEST_P(LoadLabelsTest, GzipFile) {
    auto path = byteme::temp_file_path("lab_gzip");
    std::vector<int> labels;
    {
        std::string output;
        for (size_t i = 0; i < 1000; ++i) {
            labels.push_back(i);
            output += std::to_string(i) + "\n";
        }

        gzFile ohandle = gzopen(path.c_str(), "w");
        gzwrite(ohandle, output.c_str(), output.size());
        gzclose(ohandle);
    }

    auto params = GetParam();
    singlepp_loaders::LoadLabelsOptions options;
    options.buffer_size = std::get<0>(params);
    options.parallel = std::get<1>(params);

    auto reloaded = singlepp_loaders::load_labels_from_gzip_file(path.c_str(), options);
    EXPECT_EQ(reloaded, labels);

    std::ifstream in(path, std::ios::binary);
    std::vector<unsigned char> buffer(std::istreambuf_iterator<char>(in), {});
    auto reloaded2 = singlepp_loaders::load_labels_from_zlib_buffer(buffer.data(), buffer.size(), options);
    EXPECT_EQ(reloaded2, labels);
}

INSTANTIATE_TEST_SUITE_P(
    LoadLabels,
    LoadLabelsTest,
    ::testing::Combine(
        ::testing::Values(10, 100, 1000),
        ::testing::Values(false, true)
    )
);

void quick_label_err(std::string path, std::string msg) {
    EXPECT_ANY_THROW({
        try {
            singlepp_loaders::load_labels_from_text_file(path.c_str(), singlepp_loaders::LoadLabelsOptions());
        } catch (std::exception& e) {
            EXPECT_TRUE(std::string(e.what()).find(msg) != std::string::npos);
            throw;
        }
    });
    return;
}

TEST(LoadLabels, EdgeCases) {
    {
        auto path = byteme::temp_file_path("label_err");
        quick_dump(path, "1\n2\n3a\n4\n");
        quick_label_err(path, "must be an integer");
    }

    {
        auto path = byteme::temp_file_path("label_err");
        quick_dump(path, "1\n2\n\n4\n");
        quick_label_err(path, "must be an integer");
    }

    // Non-newline termination is ok, as are empty fields.
    {
        auto path = byteme::temp_file_path("feat_ok");
        quick_dump(path, "1\n2");
        auto output = singlepp_loaders::load_labels_from_text_file(path.c_str(), singlepp_loaders::LoadLabelsOptions());
        EXPECT_EQ(output.size(), 2);
        EXPECT_EQ(output[0], 1);
        EXPECT_EQ(output[1], 2);
    }
}
