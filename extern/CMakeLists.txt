include(FetchContent)

FetchContent_Declare(
  singlepp
  GIT_REPOSITORY https://github.com/SingleR-inc/singlepp
  GIT_TAG master  # ^2.0.0
)

FetchContent_Declare(
  tatami
  GIT_REPOSITORY https://github.com/tatami-inc/tatami
  GIT_TAG master # ^3.0.0
)

FetchContent_Declare(
  byteme
  GIT_REPOSITORY https://github.com/LTLA/byteme
  GIT_TAG master # ^1.2.0
)

FetchContent_MakeAvailable(singlepp)
FetchContent_MakeAvailable(tatami)
FetchContent_MakeAvailable(byteme)
