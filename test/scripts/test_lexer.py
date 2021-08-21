#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#------------------------------------------------------------------------------
#  A SysY Compiler for PKU Compiling Principles Practice Course (2021 Spring)
#------------------------------------------------------------------------------
#  Project: sysyc
#  File: test_lexer.py
#  Created Date: 2021-05-01
#  Author: Zhong Xinghong (1700012608)
#  Email: zxh2017@pku.edu.cn
#------------------------------------------------------------------------------
#  Copyright (c) 2021 Zhong Xinghong
#------------------------------------------------------------------------------

import os
import shutil
import subprocess
from const import ROOT_DIR, SYSYC_TEST_BIN_DIR, SYSY_TESTCASE_DIR
from utils import mkdir, sysy_input_generator, run_unit_test

OUTPUT_DIR = os.path.join(ROOT_DIR, "../output/test_lexer/")
CMD = os.path.abspath(os.path.join(SYSYC_TEST_BIN_DIR, "./test-lexer"))

def test_lexer():
    for src_filepath, rel_dir in sysy_input_generator(SYSY_TESTCASE_DIR):
        src_filename = os.path.basename(src_filepath)
        src_filepath = os.path.abspath(src_filepath)
        output_dir = os.path.join(OUTPUT_DIR, rel_dir)

        print("CASE: %s" % os.path.normpath(os.path.join(rel_dir, src_filename)))

        mkdir(output_dir)
        shutil.copyfile(src_filepath, os.path.join(output_dir, src_filename))

        dst_filename = os.path.splitext(src_filename)[0] + ".txt"
        dst_filepath = os.path.join(output_dir, dst_filename)
        dst_filepath = os.path.abspath(dst_filepath)

        if not run_unit_test(CMD, src_filepath, dst_filepath, output_dir):
            break

def main():
    test_lexer()

if __name__ == "__main__":
    main()
