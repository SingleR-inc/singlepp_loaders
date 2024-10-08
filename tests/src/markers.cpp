#include <gtest/gtest.h>

#include "singlepp_loaders/markers.hpp"
#include "byteme/temp_file_path.hpp"
#include "zlib.h"

#include "utils.h"

#include <fstream>
#include <string>
#include <vector>

class LoadMarkersTest : public ::testing::TestWithParam<std::tuple<int, bool> > {
protected:
    void compare_markers(const singlepp::Markers<int>& ref, const singlepp::Markers<int>& obs) { 
        ASSERT_EQ(ref.size(), obs.size());

        for (size_t m = 0; m < obs.size(); ++m) {
            const auto& observed = obs[m];
            const auto& expected = ref[m];

            ASSERT_EQ(observed.size(), expected.size());
            for (size_t m2 = 0; m2 < observed.size(); ++m2) {
                EXPECT_EQ(observed[m2], expected[m2]);
            }
        }
    }        
};

TEST_P(LoadMarkersTest, TextFile) {
    auto path = byteme::temp_file_path("mark_text");
    auto params = GetParam();

    std::mt19937_64 rng(std::get<0>(params) + std::get<1>(params));
    size_t nfeatures = 1000, nlabels = 3;
    singlepp::Markers<int> markers(nlabels);
    {
        std::ofstream out(path, std::ofstream::out);

        for (size_t l = 0; l < nlabels; ++l) {
            markers[l].resize(nlabels);
            for (size_t l2 = 0; l2 < nlabels; ++l2) {
                if (l == l2) {
                    continue;
                }
                out << l << "\t" << l2;

                size_t ngenes = rng() % 20 + 1;
                for (size_t i = 0; i < ngenes; ++i) {
                    auto current = rng() % nfeatures;
                    markers[l][l2].push_back(current);
                    out << "\t" << current;
                }

                out << "\n";
            }
        }
    }

    singlepp_loaders::LoadMarkersOptions options;
    options.buffer_size = std::get<0>(params);
    options.parallel = std::get<1>(params);

    auto reloaded = singlepp_loaders::load_markers_from_text_file(path.c_str(), nfeatures, nlabels, options);
    compare_markers(markers, reloaded);
}

TEST_P(LoadMarkersTest, GzipFile) {
    auto path = byteme::temp_file_path("mark_text");
    auto params = GetParam();

    std::mt19937_64 rng(std::get<0>(params) + std::get<1>(params));
    size_t nfeatures = 1000, nlabels = 3;
    singlepp::Markers<int> markers(nlabels);
    {
        std::ofstream out(path, std::ofstream::out);
        std::string output;

        for (size_t l = 0; l < nlabels; ++l) {
            markers[l].resize(nlabels);
            for (size_t l2 = 0; l2 < nlabels; ++l2) {
                if (l == l2) {
                    continue;
                }
                output += std::to_string(l) + "\t" + std::to_string(l2);

                size_t ngenes = rng() % 20 + 1;
                for (size_t i = 0; i < ngenes; ++i) {
                    auto current = rng() % nfeatures;
                    markers[l][l2].push_back(current);
                    output += "\t" + std::to_string(current); 
                }
                output += "\n";
            }
        }

        gzFile ohandle = gzopen(path.c_str(), "w");
        gzwrite(ohandle, output.c_str(), output.size());
        gzclose(ohandle);
    }

    singlepp_loaders::LoadMarkersOptions options;
    options.buffer_size = std::get<0>(params);
    options.parallel = std::get<1>(params);

    auto reloaded = singlepp_loaders::load_markers_from_gzip_file(path.c_str(), nfeatures, nlabels, options);
    compare_markers(markers, reloaded);

    std::ifstream in(path, std::ios::binary);
    std::vector<unsigned char> buffer(std::istreambuf_iterator<char>(in), {});
    auto reloaded2 = singlepp_loaders::load_markers_from_zlib_buffer(buffer.data(), buffer.size(), nfeatures, nlabels, options);
    compare_markers(markers, reloaded2);
}

INSTANTIATE_TEST_SUITE_P(
    LoadMarkers,
    LoadMarkersTest,
    ::testing::Combine(
        ::testing::Values(10, 100, 1000),
        ::testing::Values(false, true)
    )
);

static void quick_marker_err(std::string path, size_t nf, size_t nl, std::string msg) {
    EXPECT_ANY_THROW({
        try {
            singlepp_loaders::load_markers_from_text_file(path.c_str(), nf, nl, singlepp_loaders::LoadMarkersOptions());
        } catch (std::exception& e) {
            EXPECT_TRUE(std::string(e.what()).find(msg) != std::string::npos);
            throw;
        }
    });
    return;
}

TEST(LoadMarkers, EdgeCases) {
    {
        auto path = byteme::temp_file_path("mark_err");
        quick_dump(path, "1\t2\t1000\n");
        quick_marker_err(path, 1, 3, "gene index out of range");
    }

    {
        auto path = byteme::temp_file_path("mark_err");
        quick_dump(path, "5\t1\t2\n");
        quick_marker_err(path, 5, 3, "label index out of range");
    }

    {
        auto path = byteme::temp_file_path("mark_err");
        quick_dump(path, "1\t5\t2\n");
        quick_marker_err(path, 5, 3, "label index out of range");
    }

    {
        auto path = byteme::temp_file_path("mark_err");
        quick_dump(path, "1\t1\t\n");
        quick_marker_err(path, 5, 3, "not be empty");
    }

    {
        auto path = byteme::temp_file_path("mark_err");
        quick_dump(path, "1\t1\t\t1\n");
        quick_marker_err(path, 5, 3, "not be empty");
    }

    {
        auto path = byteme::temp_file_path("mark_err");
        quick_dump(path, "1\t1\n");
        quick_marker_err(path, 5, 3, "at least three tab-separated fields");
    }

    {
        auto path = byteme::temp_file_path("mark_err");
        quick_dump(path, "1\t1\t1\n1\t1\t1\n");
        quick_marker_err(path, 5, 3, "multiple marker");
    }

    {
        auto path = byteme::temp_file_path("mark_err");
        quick_dump(path, "2\t1\t1\n1\t2\t1a\n");
        quick_marker_err(path, 5, 3, "integer");
    }

    {
        auto path = byteme::temp_file_path("mark_ok");
        quick_dump(path, "2\t1\t1\n1\t2\t0");
        auto output = singlepp_loaders::load_markers_from_text_file(path.c_str(), 5, 3, singlepp_loaders::LoadMarkersOptions());
        EXPECT_EQ(output.size(), 3);
        EXPECT_EQ(output[1][2].size(), 1);
        EXPECT_EQ(output[1][2].front(), 0);
    }
}
