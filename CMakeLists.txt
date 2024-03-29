# Copyright (c) 2021 Nordic Semiconductor ASA SPDX-License-Identifier:
# Apache-2.0
#
# This CMake file is picked by the Zephyr build system because it is defined as
# the module CMake entry point (see zephyr/module.yml).

# The Zephyr includes moved from /include to /include/zephyr in Zephyr 3.2:
# https://github.com/zephyrproject-rtos/zephyr/commit/53ef68d4598b2f9005c5da3fc0b860ca1999d350
# Add the old path for backwards compatibility. Note that Zephyr itself supports
# the Kconfig option 'CONFIG_LEGACY_INCLUDE_PATH' to enable the same
# compatibility behavior, but it's marked as deprecated and to be removed in the
# future, so just apply the compatibiltiy fix here.
if(${KERNEL_VERSION_MAJOR}.${KERNEL_VERSION_MINOR}.${KERNEL_PATCHLEVEL}
   VERSION_GREATER_EQUAL
   "3.1.99"
)
  zephyr_include_directories(${ZEPHYR_BASE}/include/zephyr)
  # Set this to suppress a nuisance error warning about deprecated include paths
  zephyr_compile_options(-DCONFIG_LEGACY_INCLUDE_PATH=y)
endif()
