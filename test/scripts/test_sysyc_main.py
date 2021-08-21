#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#------------------------------------------------------------------------------
#  A SysY Compiler for PKU Compiling Principles Practice Course (2021 Spring)
#------------------------------------------------------------------------------
#  Project: sysyc
#  File: test_sysyc_main.py
#  Created Date: 2021-05-24
#  Author: Zhong Xinghong (1700012608)
#  Email: zxh2017@pku.edu.cn
#------------------------------------------------------------------------------
#  Copyright (c) 2021 Zhong Xinghong
#------------------------------------------------------------------------------

import os
import re
import shutil
import subprocess
from const import ROOT_DIR, SYSYC_CMD, SYSY_TESTCASE_DIR
from utils import mkdir, sysy_input_generator, print_proc_return

OUTPUT_DIR = os.path.join(ROOT_DIR, "../output/test_sysyc_main/")

def test_sysyc_main():
    cflags_suffix_list = [
        ("", ".sysy.sy", "sysy"),
        ("-S -e", ".eeyore.S", "eeyore"),
        ("-S -t", ".tigger.S", "tigger"),
        ("-S", ".riscv.S", "riscv"),
    ]

    re_sysy_rel_dir = re.compile(
        pattern=r'^section\d[/\\]+(functional|performance)_test[/\\]*$'
    )

    for src_filepath, rel_dir in sysy_input_generator(SYSY_TESTCASE_DIR):
        src_dir = os.path.dirname(src_filepath)
        src_filename = os.path.basename(src_filepath)
        src_filepath = os.path.abspath(src_filepath)
        src_basename = os.path.splitext(src_filename)[0]
        output_dir = os.path.join(OUTPUT_DIR, rel_dir)

        mkdir(output_dir)
        shutil.copyfile(src_filepath, os.path.join(output_dir, src_filename))

        mat = re_sysy_rel_dir.match(rel_dir)

        for cflags, suffix, lang in cflags_suffix_list:
            if lang in ("eeyore", "tigger", "riscv"):
                if lang == "riscv":
                    lang = "tigger"

                lang_ext = '.' + lang
                lang_src_dir = os.path.join(SYSY_TESTCASE_DIR, "../", lang, mat.group(1))
                lang_src_filepath = os.path.join(lang_src_dir, src_basename + lang_ext)

                if not os.path.exists(lang_src_filepath):
                    continue

            dst_filename = src_basename + suffix
            dst_filepath = os.path.join(output_dir, dst_filename)
            dst_filepath = os.path.abspath(dst_filepath)

            print("BUILD: %s" % os.path.normpath(os.path.join(rel_dir, dst_filename)))

            ret = subprocess.run(
                '{} {} {} -o {}'.format(
                    SYSYC_CMD,
                    cflags,
                    src_filepath,
                    dst_filepath,
                ),
                shell=True,
                stdout=subprocess.PIPE,
                stderr=subprocess.PIPE,
            )

            if ret.returncode != 0:
                print_proc_return(ret)
                print(ret.args)
                return

def main():
    test_sysyc_main()

if __name__ == "__main__":
    main()
