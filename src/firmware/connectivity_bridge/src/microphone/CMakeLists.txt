#
# Copyright (c) 2020 Nordic Semiconductor
#
# SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
#

target_sources_ifdef(CONFIG_MICROPHONE_PDM
		     app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/pdm/microphone.c)