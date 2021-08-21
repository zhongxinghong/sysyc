#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#------------------------------------------------------------------------------
#  A SysY Compiler for PKU Compiling Principles Practice Course (2021 Spring)
#------------------------------------------------------------------------------
#  Project: sysyc
#  File: const.py
#  Created Date: 2021-05-02
#  Author: Zhong Xinghong (1700012608)
#  Email: zxh2017@pku.edu.cn
#------------------------------------------------------------------------------
#  Copyright (c) 2021 Zhong Xinghong
#------------------------------------------------------------------------------

import os
import sys

ROOT_DIR = os.path.dirname(__file__)

SYSY_TESTCASE_DIR = os.path.join(ROOT_DIR, "../open-test-cases/sysy/")
EEYORE_TESTCASE_DIR = os.path.join(ROOT_DIR, "../open-test-cases/eeyore/")
TIGGER_TESTCASE_DIR = os.path.join(ROOT_DIR, "../open-test-cases/tigger/")
EXTRA_TESTCASE_DIR = os.path.join(ROOT_DIR, "../extra-test-cases/")

SYSYC_TEST_BIN_DIR = os.path.join(ROOT_DIR, "../../build/test/")

SYLIB_H = os.path.join(SYSY_TESTCASE_DIR, "sylib.h")
SYLIB_C = os.path.join(SYSY_TESTCASE_DIR, "sylib.c")
SYLIB_INCLUDE = SYSY_TESTCASE_DIR

MINIVM_CMD = os.path.join(ROOT_DIR, "../MiniVM/build/minivm")
SYSYC_CMD = os.path.join(ROOT_DIR, "../../build/sysyc")
