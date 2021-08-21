#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#------------------------------------------------------------------------------
#  A SysY Compiler for PKU Compiling Principles Practice Course (2021 Spring)
#------------------------------------------------------------------------------
#  Project: sysyc
#  File: test_codegen_sysy_ast.py
#  Created Date: 2021-05-02
#  Author: Zhong Xinghong (1700012608)
#  Email: zxh2017@pku.edu.cn
#------------------------------------------------------------------------------
#  Copyright (c) 2021 Zhong Xinghong
#------------------------------------------------------------------------------

import os
import shutil
import subprocess
import itertools
from const import ROOT_DIR, SYSYC_TEST_BIN_DIR, SYSY_TESTCASE_DIR, EXTRA_TESTCASE_DIR
from utils import mkdir, sysy_input_generator, run_unit_test, gcc_check

OUTPUT_DIR = os.path.join(ROOT_DIR, "../output/test_codegen/sysy/")
CMD = os.path.abspath(os.path.join(SYSYC_TEST_BIN_DIR, "./test-codegen-sysy-ast"))
SUFFIX = "ast"

def test_codegen_sysy_ast():
    all_src = itertools.chain(
        sysy_input_generator(SYSY_TESTCASE_DIR),
        sysy_input_generator(EXTRA_TESTCASE_DIR, rel_dir="extra/")
    )

    for src_filepath, rel_dir in all_src:
        src_dir = os.path.dirname(src_filepath)
        src_filename = os.path.basename(src_filepath)
        src_filepath = os.path.abspath(src_filepath)
        src_basename = os.path.splitext(src_filename)[0]
        output_dir = os.path.join(OUTPUT_DIR, rel_dir)
        dst_filepath = os.path.join(output_dir, src_basename + ".%s.sy" % SUFFIX)
        dst_filepath = os.path.abspath(dst_filepath)

        if 'performance_test' in rel_dir:
            continue

        if src_filename in ['fft0.sy', 'fft1.sy', 'fft2.sy']:
            continue

        # if 'extra' not in rel_dir:
        #     continue

        print("CASE: %s" % os.path.normpath(os.path.join(rel_dir, src_filename)))

        mkdir(output_dir)
        shutil.copyfile(src_filepath, os.path.join(output_dir, src_filename))

        if not run_unit_test(CMD, src_filepath, dst_filepath, output_dir):
            break

        if not gcc_check(src_filepath, dst_filepath, output_dir, SUFFIX):
            break

def main():
    test_codegen_sysy_ast()

if __name__ == "__main__":
    main()
