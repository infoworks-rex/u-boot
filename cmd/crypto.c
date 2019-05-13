// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2019 Fuzhou Rockchip Electronics Co., Ltd
 */

#include <common.h>
#include <command.h>
#include <crypto.h>
#include <dm.h>
#include <u-boot/md5.h>
#include <u-boot/sha1.h>
#include <u-boot/sha256.h>

static u8 foo_data[] = {
	0x52, 0x53, 0x41, 0x4b, 0x00, 0x00, 0x00, 0x00, 0x23, 0x00, 0x00, 0x00,
	0xda, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xd5, 0xf2, 0xfc, 0xbb,
	0x1a, 0x39, 0x61, 0xf5, 0x63, 0x7f, 0xa6, 0xeb, 0x5d, 0xc5, 0x22, 0xe2,
	0x65, 0x03, 0xcc, 0x61, 0x92, 0x60, 0x4c, 0x5f, 0x4e, 0x81, 0x96, 0x3d,
	0x89, 0x1d, 0xa3, 0x5c, 0xb7, 0x88, 0xc3, 0x75, 0xcd, 0x71, 0xf0, 0x16,
	0x04, 0x5e, 0xef, 0x85, 0xdf, 0x30, 0x57, 0x7e, 0x06, 0x0d, 0x44, 0x3b,
	0x80, 0x22, 0x41, 0xcd, 0xba, 0x40, 0xb5, 0xfa, 0xbe, 0x8f, 0xa9, 0x61,
	0x4d, 0xc6, 0x2a, 0xd9, 0x96, 0x57, 0x57, 0xc6, 0xc0, 0x99, 0xc0, 0x94,
	0x57, 0x21, 0x0a, 0xad, 0xde, 0x4c, 0xe1, 0xaa, 0x6e, 0x11, 0xc6, 0xeb,
	0xa2, 0x09, 0x65, 0xd9, 0xd5, 0xc6, 0x8f, 0x8d, 0xb6, 0xec, 0xb3, 0xca,
	0x29, 0xa4, 0x9f, 0xab, 0x90, 0x7e, 0xf2, 0x86, 0x92, 0x25, 0x71, 0xf4,
	0x17, 0x70, 0x92, 0xe0, 0xd0, 0xe2, 0xe1, 0x57, 0xc4, 0xca, 0x44, 0xbb,
	0x14, 0xdc, 0xf8, 0x1a, 0x27, 0x00, 0x8f, 0x30, 0xcd, 0x0a, 0x4a, 0x26,
	0x42, 0x07, 0xa1, 0x1e, 0x69, 0x7e, 0xfd, 0xc4, 0x38, 0xf8, 0x53, 0xfe,
	0xcd, 0xa7, 0xfc, 0x7d, 0xa1, 0x87, 0xe8, 0xec, 0x52, 0x01, 0x37, 0x43,
	0x0c, 0xf2, 0x2e, 0x89, 0x3d, 0xaa, 0xed, 0x9d, 0x83, 0x13, 0x8f, 0x79,
	0xec, 0xb3, 0x7f, 0xbe, 0xf9, 0x36, 0x99, 0x8d, 0x93, 0xc2, 0x4d, 0x8f,
	0x8f, 0x18, 0x1e, 0x94, 0x25, 0xe7, 0x0b, 0xdc, 0xd9, 0x85, 0x88, 0x77,
	0x7e, 0x25, 0x80, 0x10, 0xde, 0xb6, 0xdd, 0xe6, 0xb6, 0xba, 0xe9, 0x1d,
	0xac, 0xd2, 0xd7, 0x1c, 0x8e, 0x86, 0x5e, 0xd1, 0xc0, 0x97, 0x59, 0x79,
	0x7e, 0x1a, 0x96, 0x6a, 0xa1, 0x09, 0xc0, 0x45, 0xb0, 0x71, 0x2d, 0xd1,
	0x87, 0x6f, 0xbe, 0x31, 0x21, 0xcb, 0x52, 0xde, 0x48, 0x41, 0x73, 0x7e,
	0x4a, 0x2f, 0xf8, 0x95, 0x0c, 0x2d, 0xb4, 0x9d, 0x53, 0x70, 0xcd, 0xa5,
	0x01, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0xa5, 0x0d, 0xc2, 0xc6, 0xf1, 0x08, 0x95, 0x55,
	0x1d, 0xb3, 0xf9, 0x43, 0xb0, 0x7b, 0x5b, 0x96, 0xa0, 0x72, 0xe8, 0xd6,
	0x95, 0xd6, 0x98, 0xec, 0x53, 0x6b, 0x85, 0x77, 0x74, 0x21, 0xf1, 0x22,
	0x5c, 0x4d, 0x56, 0x55, 0x89, 0x0d, 0x60, 0xa5, 0x2b, 0x68, 0xf8, 0x71,
	0x6d, 0x72, 0xed, 0x67, 0x55, 0x3e, 0x5f, 0xd1, 0x9e, 0x21, 0xa7, 0x6e,
	0x5c, 0xf2, 0xac, 0xe8, 0x66, 0x1d, 0x4e, 0xe5, 0x58, 0x76, 0xed, 0x2b,
	0xfe, 0x2b, 0xc9, 0xf6, 0x7f, 0xb7, 0x02, 0x27, 0xfd, 0x25, 0x3f, 0x2d,
	0x7c, 0x1c, 0x37, 0x80, 0x29, 0x7f, 0x12, 0xd2, 0xd5, 0xae, 0x27, 0xc6,
	0x68, 0x24, 0x83, 0xc5, 0x61, 0x8b, 0xea, 0x1a, 0xbd, 0x07, 0x96, 0xe4,
	0xd1, 0xc5, 0xd4, 0x21, 0x39, 0x4a, 0xaa, 0x66, 0xd2, 0x3f, 0xd2, 0x67,
	0x55, 0x39, 0xbf, 0x40, 0x66, 0x66, 0x5a, 0x92, 0xdd, 0x8d, 0xda, 0x5a,
	0x89, 0x7e, 0x78, 0xd9, 0x5f, 0x42, 0xe4, 0x16, 0xdb, 0x76, 0x49, 0xe0,
	0x71, 0x98, 0x8e, 0xed, 0x61, 0xd9, 0x63, 0x0a, 0x11, 0xf6, 0x24, 0x7f,
	0x9b, 0xf2, 0xee, 0xfc, 0xa4, 0x4c, 0x98, 0x81, 0xfb, 0x2d, 0xd3, 0x8f,
	0x29, 0x77, 0x63, 0xd9, 0x51, 0xfc, 0xe1, 0x17, 0x3f, 0xc3, 0xce, 0xca,
	0xbe, 0xec, 0xf2, 0xc9, 0x07, 0x48, 0x74, 0x54, 0xbb, 0x1d, 0x02, 0xe7,
	0xaf, 0x18, 0x27, 0x39, 0xb8, 0xc4, 0x90, 0x05, 0x20, 0x5b, 0x38, 0x7b,
	0x3a, 0x15, 0x94, 0x7e, 0xe8, 0xc1, 0xa5, 0x8c, 0xc0, 0x6c, 0x86, 0x96,
	0xeb, 0x46, 0xfa, 0xad, 0x2e, 0x69, 0x12, 0x56, 0xb9, 0x51, 0x91, 0x9c,
	0xea, 0x83, 0x4b, 0xd3, 0x5f, 0xb7, 0xdd, 0x29, 0x99, 0xb9, 0x66, 0x65,
	0x5e, 0xe2, 0x59, 0x16, 0x20, 0xa2, 0xda, 0x0e, 0x8e, 0xd4, 0x41, 0xdc,
	0x1e, 0x75, 0x32, 0xf8, 0x73, 0xfa, 0x22, 0x7b, 0x22, 0x7c, 0x7d, 0x9d,
	0x03, 0x89, 0x54, 0x27, 0x8f, 0xbb, 0x12, 0xbb, 0x29, 0x0a, 0xe0, 0x41,
	0x4e, 0x80, 0x1b, 0x3f, 0x9c, 0x6d, 0xc1, 0x79, 0xdb, 0x3c, 0xc4, 0xd2,
	0x65, 0x99, 0xd7, 0x16, 0xff, 0xff, 0xff, 0xff, 0xd1, 0x95, 0x46, 0x66,
	0x83, 0xe2, 0x1b, 0xd4, 0x35, 0x1f, 0x5d, 0xf2, 0x1e, 0xb5, 0x43, 0x22,
	0xe4, 0x41, 0xe6, 0x6a, 0x46, 0x93, 0x24, 0x89, 0x1b, 0x74, 0x3c, 0xa0,
	0x20, 0x3b, 0x4e, 0x64, 0xff, 0xff, 0xff, 0xff,
};

static u8 rsa2048_n[] = {
	0xd5, 0xf2, 0xfc, 0xbb, 0x1a, 0x39, 0x61, 0xf5, 0x63, 0x7f, 0xa6, 0xeb,
	0x5d, 0xc5, 0x22, 0xe2, 0x65, 0x03, 0xcc, 0x61, 0x92, 0x60, 0x4c, 0x5f,
	0x4e, 0x81, 0x96, 0x3d, 0x89, 0x1d, 0xa3, 0x5c, 0xb7, 0x88, 0xc3, 0x75,
	0xcd, 0x71, 0xf0, 0x16, 0x04, 0x5e, 0xef, 0x85, 0xdf, 0x30, 0x57, 0x7e,
	0x06, 0x0d, 0x44, 0x3b, 0x80, 0x22, 0x41, 0xcd, 0xba, 0x40, 0xb5, 0xfa,
	0xbe, 0x8f, 0xa9, 0x61, 0x4d, 0xc6, 0x2a, 0xd9, 0x96, 0x57, 0x57, 0xc6,
	0xc0, 0x99, 0xc0, 0x94, 0x57, 0x21, 0x0a, 0xad, 0xde, 0x4c, 0xe1, 0xaa,
	0x6e, 0x11, 0xc6, 0xeb, 0xa2, 0x09, 0x65, 0xd9, 0xd5, 0xc6, 0x8f, 0x8d,
	0xb6, 0xec, 0xb3, 0xca, 0x29, 0xa4, 0x9f, 0xab, 0x90, 0x7e, 0xf2, 0x86,
	0x92, 0x25, 0x71, 0xf4, 0x17, 0x70, 0x92, 0xe0, 0xd0, 0xe2, 0xe1, 0x57,
	0xc4, 0xca, 0x44, 0xbb, 0x14, 0xdc, 0xf8, 0x1a, 0x27, 0x00, 0x8f, 0x30,
	0xcd, 0x0a, 0x4a, 0x26, 0x42, 0x07, 0xa1, 0x1e, 0x69, 0x7e, 0xfd, 0xc4,
	0x38, 0xf8, 0x53, 0xfe, 0xcd, 0xa7, 0xfc, 0x7d, 0xa1, 0x87, 0xe8, 0xec,
	0x52, 0x01, 0x37, 0x43, 0x0c, 0xf2, 0x2e, 0x89, 0x3d, 0xaa, 0xed, 0x9d,
	0x83, 0x13, 0x8f, 0x79, 0xec, 0xb3, 0x7f, 0xbe, 0xf9, 0x36, 0x99, 0x8d,
	0x93, 0xc2, 0x4d, 0x8f, 0x8f, 0x18, 0x1e, 0x94, 0x25, 0xe7, 0x0b, 0xdc,
	0xd9, 0x85, 0x88, 0x77, 0x7e, 0x25, 0x80, 0x10, 0xde, 0xb6, 0xdd, 0xe6,
	0xb6, 0xba, 0xe9, 0x1d, 0xac, 0xd2, 0xd7, 0x1c, 0x8e, 0x86, 0x5e, 0xd1,
	0xc0, 0x97, 0x59, 0x79, 0x7e, 0x1a, 0x96, 0x6a, 0xa1, 0x09, 0xc0, 0x45,
	0xb0, 0x71, 0x2d, 0xd1, 0x87, 0x6f, 0xbe, 0x31, 0x21, 0xcb, 0x52, 0xde,
	0x48, 0x41, 0x73, 0x7e, 0x4a, 0x2f, 0xf8, 0x95, 0x0c, 0x2d, 0xb4, 0x9d,
	0x53, 0x70, 0xcd, 0xa5,
};

static u8 rsa2048_e[] = {
	0x01, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
};

static u8 rsa2048_c[] = {
	0xa5, 0x0d, 0xc2, 0xc6, 0xf1, 0x08, 0x95, 0x55, 0x1d, 0xb3, 0xf9, 0x43,
	0xb0, 0x7b, 0x5b, 0x96, 0xa0, 0x72, 0xe8, 0xd6, 0x95, 0xd6, 0x98, 0xec,
	0x53, 0x6b, 0x85, 0x77, 0x74, 0x21, 0xf1, 0x22, 0x5c, 0x4d, 0x56, 0x55,
	0x89, 0x0d, 0x60, 0xa5, 0x2b, 0x68, 0xf8, 0x71, 0x6d, 0x72, 0xed, 0x67,
	0x55, 0x3e, 0x5f, 0xd1, 0x9e, 0x21, 0xa7, 0x6e, 0x5c, 0xf2, 0xac, 0xe8,
	0x66, 0x1d, 0x4e, 0xe5, 0x58, 0x76, 0xed, 0x2b, 0xfe, 0x2b, 0xc9, 0xf6,
	0x7f, 0xb7, 0x02, 0x27, 0xfd, 0x25, 0x3f, 0x2d, 0x7c, 0x1c, 0x37, 0x80,
	0x29, 0x7f, 0x12, 0xd2, 0xd5, 0xae, 0x27, 0xc6, 0x68, 0x24, 0x83, 0xc5,
	0x61, 0x8b, 0xea, 0x1a, 0xbd, 0x07, 0x96, 0xe4, 0xd1, 0xc5, 0xd4, 0x21,
	0x39, 0x4a, 0xaa, 0x66, 0xd2, 0x3f, 0xd2, 0x67, 0x55, 0x39, 0xbf, 0x40,
	0x66, 0x66, 0x5a, 0x92, 0xdd, 0x8d, 0xda, 0x5a, 0x89, 0x7e, 0x78, 0xd9,
	0x5f, 0x42, 0xe4, 0x16, 0xdb, 0x76, 0x49, 0xe0, 0x71, 0x98, 0x8e, 0xed,
	0x61, 0xd9, 0x63, 0x0a, 0x11, 0xf6, 0x24, 0x7f, 0x9b, 0xf2, 0xee, 0xfc,
	0xa4, 0x4c, 0x98, 0x81, 0xfb, 0x2d, 0xd3, 0x8f, 0x29, 0x77, 0x63, 0xd9,
	0x51, 0xfc, 0xe1, 0x17, 0x3f, 0xc3, 0xce, 0xca, 0xbe, 0xec, 0xf2, 0xc9,
	0x07, 0x48, 0x74, 0x54, 0xbb, 0x1d, 0x02, 0xe7, 0xaf, 0x18, 0x27, 0x39,
	0xb8, 0xc4, 0x90, 0x05, 0x20, 0x5b, 0x38, 0x7b, 0x3a, 0x15, 0x94, 0x7e,
	0xe8, 0xc1, 0xa5, 0x8c, 0xc0, 0x6c, 0x86, 0x96, 0xeb, 0x46, 0xfa, 0xad,
	0x2e, 0x69, 0x12, 0x56, 0xb9, 0x51, 0x91, 0x9c, 0xea, 0x83, 0x4b, 0xd3,
	0x5f, 0xb7, 0xdd, 0x29, 0x99, 0xb9, 0x66, 0x65, 0x5e, 0xe2, 0x59, 0x16,
	0x20, 0xa2, 0xda, 0x0e, 0x8e, 0xd4, 0x41, 0xdc, 0x1e, 0x75, 0x32, 0xf8,
	0x73, 0xfa, 0x22, 0x7b,
};

/* RSA2048-SHA256 sign data of foo_data[] */
static u8 rsa2048_sha256_sign[] = {
	0xd4, 0x05, 0x21, 0x34, 0x2c, 0x92, 0xaf, 0x73, 0x72, 0x13, 0x54, 0x92,
	0x9b, 0x57, 0x63, 0x82, 0xd4, 0x7c, 0x41, 0xba, 0x58, 0x3e, 0xa6, 0x82,
	0xe8, 0x56, 0x9f, 0x1c, 0xc2, 0x6d, 0x07, 0x6d, 0xe0, 0xca, 0x4c, 0x93,
	0xb4, 0xa4, 0x82, 0x53, 0x41, 0x58, 0xeb, 0x77, 0xd5, 0x2c, 0xf5, 0x3f,
	0x3d, 0x1b, 0x54, 0x1b, 0x6a, 0x18, 0x30, 0xc1, 0x93, 0x14, 0x8f, 0xdd,
	0x77, 0xe4, 0xed, 0xb8, 0xd3, 0xfc, 0x63, 0xed, 0x88, 0x2f, 0xf4, 0x35,
	0xc4, 0xdb, 0x89, 0x94, 0x4a, 0xcd, 0x88, 0x88, 0x4f, 0xc3, 0xeb, 0x14,
	0x03, 0xd9, 0x85, 0x2e, 0x7a, 0x27, 0xbd, 0xc9, 0x19, 0xbf, 0x55, 0x50,
	0x2a, 0x29, 0x23, 0x02, 0xe4, 0xaf, 0x16, 0x5c, 0x17, 0x81, 0x38, 0x79,
	0xf1, 0x2b, 0x9a, 0xc7, 0xd2, 0x9d, 0x66, 0xca, 0xf7, 0x6b, 0xf0, 0x83,
	0x81, 0x57, 0xfc, 0x56, 0xae, 0x6d, 0x98, 0xae, 0xe2, 0xec, 0x00, 0x19,
	0x6b, 0x75, 0x06, 0xdc, 0x1c, 0x6e, 0xbd, 0x23, 0x15, 0xb1, 0x3a, 0x43,
	0x7e, 0x3c, 0x8b, 0x32, 0x9c, 0x4d, 0xa2, 0xcf, 0x14, 0xee, 0xd8, 0x7a,
	0x12, 0x90, 0x70, 0xd2, 0x9f, 0xd5, 0x6d, 0x50, 0x67, 0x35, 0x93, 0x97,
	0x2e, 0xff, 0x8f, 0xbf, 0x34, 0x4c, 0xa9, 0xee, 0x6a, 0xaa, 0x87, 0x09,
	0x1f, 0x6b, 0xcc, 0x3d, 0xeb, 0x88, 0xca, 0x8f, 0x45, 0x86, 0xb7, 0xb2,
	0xd6, 0xd8, 0xe5, 0x3e, 0x94, 0x02, 0x4b, 0xa7, 0x74, 0xb8, 0x5f, 0x1f,
	0xe3, 0xbc, 0x95, 0x72, 0x3b, 0x51, 0xd4, 0xec, 0x8d, 0x87, 0xca, 0xb0,
	0x69, 0xca, 0xa9, 0x68, 0xfc, 0x34, 0xf9, 0xb5, 0x67, 0xef, 0xac, 0xc2,
	0x10, 0x1a, 0x39, 0x56, 0xcb, 0x38, 0x45, 0x92, 0x74, 0x2e, 0x24, 0x1c,
	0x4e, 0x31, 0x80, 0xd9, 0x2d, 0x5f, 0x74, 0x48, 0xfe, 0x46, 0x4e, 0xf6,
	0x19, 0x9a, 0x1d, 0x32,
};

static void dump(const char *title, void *hard_d, void *soft_d, u32 nbits)
{
	int i, same;
	char *buf;

	same = !memcmp(hard_d, soft_d, BITS2BYTE(nbits));
	printf("[%s]: %s\n", title, same ? "OK" : "FAIL");

	printf("    hard: ");
	buf = hard_d;
	for (i = 0; i < BITS2BYTE(nbits); i++)
		printf("%02x ", buf[i]);

	printf("\n    soft: ");
	buf = soft_d;
	for (i = 0; i < BITS2BYTE(nbits); i++)
		printf("%02x ", buf[i]);

	printf("\n\n");
}

static int do_crypto(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	struct udevice *dev;
	sha_context csha_ctx;
	sha1_context sha1_ctx;
	sha256_context sha256_ctx;
	rsa_key rsa_key;
	u8 sha256_out0[32];
	u8 sha256_out1[32];
	u8 sha1_out0[20];
	u8 sha1_out1[20];
	u8 rsa_out[32];
	u8 md5_out0[16];
	u8 md5_out1[16];
	u32 cap;

	cap = CRYPTO_MD5 | CRYPTO_SHA1 | CRYPTO_SHA256 | CRYPTO_RSA2048;
	dev = crypto_get_device(cap);
	if (!dev) {
		printf("Can't find crypto device for expected capability\n");
		return -ENODEV;
	}

	/* MD5 */
	csha_ctx.algo = CRYPTO_MD5;
	csha_ctx.length = sizeof(foo_data);
	crypto_sha_csum(dev, &csha_ctx, (char *)foo_data,
			sizeof(foo_data), md5_out0);
	md5(foo_data, sizeof(foo_data), md5_out1);
	dump("MD5", md5_out0, md5_out1, crypto_algo_nbits(csha_ctx.algo));

	/* SHA1 */
	csha_ctx.algo = CRYPTO_SHA1;
	csha_ctx.length = sizeof(foo_data);
	crypto_sha_csum(dev, &csha_ctx, (char *)foo_data,
			sizeof(foo_data), sha1_out0);
	sha1_starts(&sha1_ctx);
	sha1_update(&sha1_ctx, (const u8 *)foo_data, sizeof(foo_data));
	sha1_finish(&sha1_ctx, sha1_out1);
	dump("SHA1", sha1_out0, sha1_out1, crypto_algo_nbits(csha_ctx.algo));

	/* SHA256 */
	csha_ctx.algo = CRYPTO_SHA256;
	csha_ctx.length = sizeof(foo_data);
	crypto_sha_csum(dev, &csha_ctx, (char *)foo_data,
			sizeof(foo_data), sha256_out0);
	sha256_starts(&sha256_ctx);
	sha256_update(&sha256_ctx, (const u8 *)foo_data, sizeof(foo_data));
	sha256_finish(&sha256_ctx, sha256_out1);
	dump("SHA256", sha256_out0, sha256_out1,
	     crypto_algo_nbits(csha_ctx.algo));

	/* RSA2048-SHA256 */
	rsa_key.algo = CRYPTO_RSA2048;
	rsa_key.n = (u32 *)&rsa2048_n;
	rsa_key.e = (u32 *)&rsa2048_e;
	rsa_key.c = (u32 *)&rsa2048_c;
	crypto_rsa_verify(dev, &rsa_key, rsa2048_sha256_sign, rsa_out);
	dump("RSA2048-SHA256", rsa_out,
	     sha256_out1, crypto_algo_nbits(csha_ctx.algo));

	return 0;
}

U_BOOT_CMD(
	crypto, 1, 1, do_crypto,
	"crypto test",
	""
);
