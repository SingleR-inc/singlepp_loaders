# Load reference datasets for SingleR 

![Unit tests](https://github.com/singler-inc/singlepp_loaders/actions/workflows/run-tests.yaml/badge.svg)
![Documentation](https://github.com/singler-inc/singlepp_loaders/actions/workflows/doxygenate.yaml/badge.svg)
[![codecov](https://codecov.io/gh/singler-inc/singlepp_loaders/branch/master/graph/badge.svg?token=OYTGM9IRSE)](https://codecov.io/gh/singler-inc/singlepp_loaders)

## Overview

This repository implements functions to load [**singlepp**](https://github.com/SingleR-inc/singlepp) reference datasets for use in cell type annotation.
Each reference dataset is pre-processed into a custom format that eliminates the need for ranking and marker detection.
The aim is to avoid unnecessary work on underpowered client devices, e.g., for more responsive web applications.
Briefly, a reference dataset is represented by three files corresponding to the following components:

- A matrix of ranks, where rows are genes and columns are reference profiles.
  Each value contains the rank of each gene within a particular profile.
- A vector of length equal to the number of columns of the matrix, containing the label for each reference profile.
  These labels should be integers from `[0, N)` where `N` is the number of unique labels.
- A vector of vector of integer vectors, containing the chosen marker genes from pairwise comparisons between labels.
  Say that `y` is this object, then `y[i][j][k]` should contain the `k`-th best marker gene that is upregulated in label `i` compared to label `j`. 
  Marker genes should be reported as row indices of the expression matrix.

In practical usage, a reference dataset will also contain:

- Feature names for each row of the expression matrix.
  This can be used by **singlepp** to match to the features of the test matrix, if the feature sets are not the same.
- Label names, to map the integer labels to something that is meaningful to the user.

Check out [some existing datasets](https://github.com/kanaverse/singlepp-references) for concrete examples.

## Quick start

We can parse each component of our reference dataset from a text file, Gzip-compressed file or Zlib-compressed buffer.

```cpp
#include "singlepp_loader/singlepp_loader.hpp"

singlepp_loader::LoadRankingsOptions mopt;
auto ranks = singlepp_loader::load_rankings_from_text_file(
    "some_rankings.csv",
    ropt
);

singlepp_loader::LoadMarkersOptions mopt;
auto markers = singlepp_loader::load_markers_from_gzip_file(
    "some_rankings.csv.gz",
    mopt
);

singlepp_loader::LoadLabelsOptions lopt;
auto labels = singlepp_loader::load_labels_from_zlib_buffer(
    buffer_ptr,
    buffer_length,
    mopt
);
```

These can be used in `singlepp::train_single()` to build a classifier that can be applied to a test dataset.
More details can be found in the [**singlepp** documentation](https://SingleR-inc.github.io/singlepp).

## Building projects 

### CMake with `FetchContent`

If you're using CMake, you just need to add something like this to your `CMakeLists.txt`:

```
include(FetchContent)

FetchContent_Declare(
  singlepp_loaders
  GIT_REPOSITORY https://github.com/SingleR-inc/singlepp_loaders
  GIT_TAG master # or any version of interest
)

FetchContent_MakeAvailable(singlepp_loaders)
```

Then you can link to **singlepp_loaders** to make the headers available during compilation:

```
# For executables:
target_link_libraries(myexe singlepp_loaders)

# For libaries
target_link_libraries(mylib INTERFACE singlepp_loaders)
```

### CMake with `find_package()`

```cmake
find_package(singler_singlepp_loaders CONFIG REQUIRED)
target_link_libraries(mylib INTERFACE singler::singlepp_loaders)
```

To install the library, use:

```sh
mkdir build && cd build
cmake .. -DSINGLEPP_LOADERS_TESTS=OFF
cmake --build . --target install
```

By default, this will use `FetchContent` to fetch all external dependencies.
If you want to install them manually, use `-DSINGLEPP_LOADERS_FETCH_EXTERN=OFF`.
See the tags in [`extern/CMakeLists.txt`](extern/CMakeLists.txt) to find compatible versions of each dependency.

### Manual

If you're not using CMake, the simple approach is to just copy the files in `include/` - either directly or with Git submodules - and include their path during compilation with, e.g., GCC's `-I`.
This requires the external dependencies listed in [`extern/CMakeLists.txt`](extern/CMakeLists.txt). 
