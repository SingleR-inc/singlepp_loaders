#include <gtest/gtest.h>

#include "singlepp_loaders/verify.hpp"
#include "utils.h"

#include <fstream>
#include <string>
#include <vector>

static void quick_verify_err(const singlepp_loaders::RankMatrix<>& ranks, const std::vector<int>& labels, const singlepp::Markers<int>& markers, std::string msg) {
    EXPECT_ANY_THROW({
        try {
            singlepp_loaders::verify(ranks, labels, markers);
        } catch (std::exception& e) {
            EXPECT_TRUE(std::string(e.what()).find(msg) != std::string::npos);
            throw;
        }
    });
    return;
}

TEST(Verify, Ranks) {
    singlepp_loaders::RankMatrix<> ranks(10, 5, std::vector<int>(50), true);

    std::vector<int> labels(5);

    singlepp::Markers<int> markers(1);
    markers.front().resize(1);

    singlepp_loaders::verify(ranks, labels, markers);

    labels.push_back(0);
    quick_verify_err(ranks, labels, markers, "number of columns");
    labels.pop_back();

    markers[0][0].push_back(10);
    quick_verify_err(ranks, labels, markers, "gene indices");
}

TEST(Verify, Labels) {
    singlepp_loaders::RankMatrix<> ranks(10, 5, std::vector<int>(50), true);

    std::vector<int> labels(5);
    std::fill(labels.begin(), labels.begin() + 2, 1);

    singlepp::Markers<int> markers(2);
    for (auto& m : markers) {
        m.resize(2);
    }

    singlepp_loaders::verify(ranks, labels, markers);

    markers.resize(markers.size() + 1);
    quick_verify_err(ranks, labels, markers, "number of labels");
    markers.pop_back();

    markers[0].resize(markers[0].size() + 1);
    quick_verify_err(ranks, labels, markers, "inner vectors");
}

TEST(Verify, Empty) {
    singlepp_loaders::RankMatrix<> ranks(0, 0, std::vector<int>(0), true);

    std::vector<int> labels;

    singlepp::Markers<int> markers;

    singlepp_loaders::verify(ranks, labels, markers);
}
