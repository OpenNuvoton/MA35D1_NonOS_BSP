/*
 * Copyright (c) 2015, Xilinx Inc. and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*
 * @file	config.h
 * @brief	Generated configuration settings for libmetal.
 */

#ifndef __METAL_CONFIG__H__
#define __METAL_CONFIG__H__

#ifdef __cplusplus
extern "C" {
#endif

/** Library major version number. */
#define METAL_VER_MAJOR		1

/** Library minor version number. */
#define METAL_VER_MINOR		5

/** Library patch level. */
#define METAL_VER_PATCH		0

/** Library version string. */
#define METAL_VER		"1.5.0"

/** System type (linux, generic, ...). */
#define METAL_SYSTEM		"freertos"
#define METAL_SYSTEM_FREERTOS

/** Processor type (arm, x86_64, ...). */
#define METAL_PROCESSOR		"aarch64"
#define METAL_PROCESSOR_AARCH64

/** Machine type (zynq, zynqmp, ...). */
#define METAL_MACHINE		"ma35"
#define METAL_MACHINE_MA35

#define HAVE_STDATOMIC_H
/* #undef HAVE_FUTEX_H */
/* #undef HAVE_PROCESSOR_ATOMIC_H */
#define HAVE_PROCESSOR_CPU_H

#ifdef __cplusplus
}
#endif

#endif /* __METAL_CONFIG__H__ */
