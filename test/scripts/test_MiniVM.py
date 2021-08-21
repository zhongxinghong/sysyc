#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#------------------------------------------------------------------------------
#  A SysY Compiler for PKU Compiling Principles Practice Course (2021 Spring)
#------------------------------------------------------------------------------
#  Project: sysyc
#  File: test_MiniVM.py
#  Created Date: 2021-05-23
#  Author: Zhong Xinghong (1700012608)
#  Email: zxh2017@pku.edu.cn
#------------------------------------------------------------------------------
#  Copyright (c) 2021 Zhong Xinghong
#------------------------------------------------------------------------------

import os
import itertools
from const import ROOT_DIR, EEYORE_TESTCASE_DIR, TIGGER_TESTCASE_DIR
from utils import eeyore_input_generator, tigger_input_generator, minivm_check

def test_MiniVM():
    all_src = itertools.chain(
        eeyore_input_generator(EEYORE_TESTCASE_DIR),
        tigger_input_generator(TIGGER_TESTCASE_DIR),
    )

    for src_filepath, rel_dir in all_src:
        src_filename = os.path.basename(src_filepath)
        src_filepath = os.path.abspath(src_filepath)

        if 'performance' in rel_dir:
            continue

        print("CASE: %s" % os.path.normpath(os.path.join(rel_dir, src_filename)))

        if not minivm_check(src_filepath, src_filepath):
            break

def main():
    test_MiniVM()

if __name__ == "__main__":
    main()

