#include <gtest/gtest.h>

#include "singlepp_loaders/rankings.hpp"
#include "byteme/temp_file_path.hpp"
#include "zlib.h"

#include "utils.h"

#include <fstream>
#include <string>
#include <vector>
#include <random>

class LoadRankingsTest : public ::testing::TestWithParam<std::tuple<int, bool> > {};

static std::vector<int> extract_ranks(const singlepp_loaders::RankMatrix<int, int>& mat) {
    size_t NR = mat.nrow(), NC = mat.ncol();
    std::vector<int> copy(NR * NC);
    auto wrk = mat.dense_column();
    for (size_t i = 0; i < NC; ++i) {
        auto dest = copy.data() + i * NR;
        auto ptr = wrk->fetch(i, dest);
        tatami::copy_n(ptr, mat.nrow(), dest);
    }
    return copy;
}

TEST_P(LoadRankingsTest, TextFile) {
    auto path = byteme::temp_file_path("rank_text");
    size_t nfeat = 49, nprof = 13;

    auto params = GetParam();
    std::vector<int> ranks;
    {
        std::ofstream out(path, std::ofstream::out);
        std::vector<int> payload(nfeat);
        std::iota(payload.begin(), payload.end(), 0);
        std::mt19937_64 rng(std::get<0>(params) + std::get<1>(params));

        for (size_t p = 0; p < nprof; ++p) {
            std::shuffle(payload.begin(), payload.end(), rng);
            ranks.insert(ranks.end(), payload.begin(), payload.end());
            for (size_t f = 0; f < nfeat; ++f) {
                if (f != 0) {
                    out << ",";
                }
                out << payload[f];
            }
            out << "\n";
        }
    }

    singlepp_loaders::LoadRankingsOptions options;
    options.buffer_size = std::get<0>(params);
    options.parallel = std::get<1>(params);

    auto reloaded = singlepp_loaders::load_rankings_from_text_file<int, int>(path.c_str(), options);
    EXPECT_EQ(reloaded.nrow(), nfeat);
    EXPECT_EQ(reloaded.ncol(), nprof);

    auto copy = extract_ranks(reloaded);
    EXPECT_EQ(copy, ranks);
}

TEST_P(LoadRankingsTest, GzipFile) {
    auto path = byteme::temp_file_path("rank_gzip");
    size_t nfeat = 51, nprof = 17;

    auto params = GetParam();
    std::vector<int> ranks;
    {
        std::string output;
        std::vector<int> payload(nfeat);
        std::iota(payload.begin(), payload.end(), 0);
        std::mt19937_64 rng(std::get<0>(params) + std::get<1>(params));

        for (size_t p = 0; p < nprof; ++p) {
            std::shuffle(payload.begin(), payload.end(), rng);
            ranks.insert(ranks.end(), payload.begin(), payload.end());
            for (size_t f = 0; f < nfeat; ++f) {
                if (f != 0) {
                    output += ",";
                }
                output += std::to_string(payload[f]);
            }
            output += "\n";
        }

        gzFile ohandle = gzopen(path.c_str(), "w");
        gzwrite(ohandle, output.c_str(), output.size());
        gzclose(ohandle);
    }

    singlepp_loaders::LoadRankingsOptions options;
    options.buffer_size = std::get<0>(params);
    options.parallel = std::get<1>(params);

    auto reloaded = singlepp_loaders::load_rankings_from_gzip_file<int, int>(path.c_str(), options);
    auto copy = extract_ranks(reloaded);
    EXPECT_EQ(copy, ranks);

    std::ifstream in(path, std::ios::binary);
    std::vector<unsigned char> buffer(std::istreambuf_iterator<char>(in), {});
    auto reloaded2 = singlepp_loaders::load_rankings_from_zlib_buffer<int, int>(buffer.data(), buffer.size(), options);
    auto copy2 = extract_ranks(reloaded2);
    EXPECT_EQ(copy2, ranks);
}

INSTANTIATE_TEST_SUITE_P(
    LoadRankings,
    LoadRankingsTest,
    ::testing::Combine(
        ::testing::Values(10, 100, 1000),
        ::testing::Values(false, true)
    )
);

static void quick_ranking_err(std::string path, std::string msg) {
    EXPECT_ANY_THROW({
        try {
            singlepp_loaders::load_rankings_from_text_file(path.c_str(), singlepp_loaders::LoadRankingsOptions());
        } catch (std::exception& e) {
            EXPECT_TRUE(std::string(e.what()).find(msg) != std::string::npos);
            throw;
        }
    });
    return;
}

TEST(LoadRankings, EdgeCases) {
    {
        auto path = byteme::temp_file_path("rank_err");
        quick_dump(path, "a,v,b,d\n");
        quick_ranking_err(path, "integer ranks");
    }

    {
        auto path = byteme::temp_file_path("rank_err");
        quick_dump(path, "1,2,3,4\n1,2,3\n1,2,3,4\n");
        quick_ranking_err(path, "number of fields");
    }

    {
        auto path = byteme::temp_file_path("rank_err");
        quick_dump(path, "1,2,3,4\n1,2,3,\n1,2,3,4\n");
        quick_ranking_err(path, "not be empty");
    }

    {
        auto path = byteme::temp_file_path("rank_err");
        quick_dump(path, "1,2,3,4\n1,2,,4\n1,2,3,4\n");
        quick_ranking_err(path, "not be empty");
    }

    {
        auto path = byteme::temp_file_path("rank_err");
        quick_dump(path, "1,2,3,4\n1,2,3,4\n1,2,3\n");
        quick_ranking_err(path, "number of fields");
    }

    {
        auto path = byteme::temp_file_path("rank_err");
        quick_dump(path, "1,2,3,4\n1,2,3,4\n1,2,3,\n");
        quick_ranking_err(path, "not be empty");
    }

    // Non-newline termination is ok.
    {
        auto path = byteme::temp_file_path("feat_ok");
        quick_dump(path, "1,2,3,4\n5,6,7,8");
        auto output = singlepp_loaders::load_rankings_from_text_file<int, int>(path.c_str(), singlepp_loaders::LoadRankingsOptions());
        std::vector<int> expected { 1,2,3,4,5,6,7,8 };
        EXPECT_EQ(extract_ranks(output), expected);
    }
}
