#
# Copyright (c) 2020 Nordic Semiconductor
#
# SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
#

target_sources_ifdef(CONFIG_MODULE_MICROPHONE_PDM
		     app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/pdm/module_microphone.c)
target_sources_ifdef(CONFIG_MODULE_MICROPHONE_PDM
		     app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/pdm/microphone_test.c)