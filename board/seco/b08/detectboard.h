/*
 * Copyright (C) UDOO Team
 *
 * Author: Francesco Montefoschi <francesco.monte@gmail.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#define SECO_B08_TYPE_BASIC         1
#define SECO_B08_TYPE_FULL          2

int detect_board(void);
int detect_video(void);
int detect_revision(void);
