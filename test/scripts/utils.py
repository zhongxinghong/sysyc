#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#------------------------------------------------------------------------------
#  A SysY Compiler for PKU Compiling Principles Practice Course (2021 Spring)
#------------------------------------------------------------------------------
#  Project: sysyc
#  File: utils.py
#  Created Date: 2021-05-02
#  Author: Zhong Xinghong (1700012608)
#  Email: zxh2017@pku.edu.cn
#------------------------------------------------------------------------------
#  Copyright (c) 2021 Zhong Xinghong
#------------------------------------------------------------------------------

import os
import shutil
import subprocess
from const import MINIVM_CMD, SYSY_TESTCASE_DIR, SYLIB_H, SYLIB_C, SYLIB_INCLUDE

def mkdir(dirpath):
    if not os.path.exists(dirpath):
        os.makedirs(dirpath)


def src_input_generator(ext, top_dir, rel_dir):
    for filename in sorted(os.listdir(top_dir)):
        filepath = os.path.join(top_dir, filename)
        if os.path.isdir(filepath):
            sub_rel_dir = os.path.join(rel_dir, filename)
            yield from src_input_generator(ext, filepath, sub_rel_dir)
        elif os.path.isfile(filepath) and filename.endswith(ext):
            yield filepath, rel_dir

def sysy_input_generator(top_dir, rel_dir=''):
    return src_input_generator('.sy', top_dir, rel_dir)

def eeyore_input_generator(top_dir, rel_dir=''):
    return src_input_generator('.eeyore', top_dir, rel_dir)

def tigger_input_generator(top_dir, rel_dir=''):
    return src_input_generator('.tigger', top_dir, rel_dir)


def print_proc_return(ret, print_stdout=True):
    print("Exit status %d" % ret.returncode)
    if print_stdout and len(ret.stdout) > 0:
        print(ret.stdout.decode('utf-8'))
    if len(ret.stderr) > 0:
        print(ret.stderr.decode('utf-8'))


def check_output(ret, target_output_filepath):
    current_output = ret.stdout.decode('utf-8') + str(ret.returncode & 0xff) + '\n'

    with open(target_output_filepath, 'r') as fp:
        target_output = fp.read()

    if ''.join(current_output.split()) != ''.join(target_output.split()):
        print("Incorrect output:")
        print("--------------------------------")
        print(current_output)
        print("--------------------------------")
        print(target_output)
        return False

    return True


def run_unit_test(cmd, src_filepath, dst_filepath, output_dir):
    ret = subprocess.run(
        '{} {} {}'.format(
            cmd,
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
        return False

    if len(ret.stdout) > 0:
        print(ret.stdout.decode('utf-8'))

    return True


def gcc_check(src_filepath, dst_filepath, output_dir, suffix):
    src_dir = os.path.dirname(src_filepath)
    src_filename = os.path.basename(src_filepath)
    src_basename = os.path.splitext(src_filename)[0]

    build_dir = os.path.join(output_dir, "build/")
    mkdir(build_dir)

    build_dst_filepath = os.path.join(build_dir, src_basename + ".%s" % suffix)
    build_dst_filepath = os.path.abspath(build_dst_filepath)

    sy_src_filepath = os.path.join(build_dir, src_basename + ".%s.c" % suffix)
    sy_src_filepath = os.path.abspath(sy_src_filepath)

    with open(sy_src_filepath, 'w') as wfp:
        wfp.write(
            '#include "sylib.h"\n'
            '\n'
        )
        with open(dst_filepath, 'r') as rfp:
            wfp.write(rfp.read())

    ret = subprocess.run(
        'gcc {} {} {} -I {} -o {}'.format(
            SYLIB_H,
            SYLIB_C,
            sy_src_filepath,
            SYLIB_INCLUDE,
            build_dst_filepath,
        ),
        shell=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
    )

    if ret.returncode != 0:
        print_proc_return(ret)
        return False

    data_in_filepath = os.path.join(src_dir, src_basename + ".in")
    data_out_filepath = os.path.join(src_dir, src_basename + ".out")

    fin = None

    if os.path.exists(data_in_filepath):
        fin = open(data_in_filepath, 'r')

    ret = subprocess.run(
        build_dst_filepath,
        shell=True,
        stdin=fin,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
    )

    if fin is not None:
        fin.close()

    if not check_output(ret, data_out_filepath):
        print(ret.args)
        return False

    return True


def minivm_check(src_filepath, dst_filepath):
    src_dir = os.path.dirname(src_filepath)
    src_filename = os.path.basename(src_filepath)
    src_basename = os.path.splitext(src_filename)[0]

    data_in_filepath = os.path.join(src_dir, src_basename + ".in")
    data_out_filepath = os.path.join(src_dir, src_basename + ".out")

    fin = None

    if os.path.exists(data_in_filepath):
        fin = open(data_in_filepath, 'r', encoding='utf-8')

    ret = subprocess.run(
        '{} {} {}'.format(
            MINIVM_CMD,
            dst_filepath,
            '-t' if dst_filepath.endswith('.tigger') else ''
        ),
        shell=True,
        stdin=fin,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
    )

    if fin is not None:
        fin.close()

    if len(ret.stderr) > 0:
        print("Exit status %d" % ret.returncode)
        print(ret.stderr.decode('utf-8'))
        print(ret.args)
        return False

    if not check_output(ret, data_out_filepath):
        return False

    return True


def riscv32_gcc_check(src_filepath, dst_filepath, output_dir, suffix):
    src_dir = os.path.dirname(src_filepath)
    src_filename = os.path.basename(src_filepath)
    src_basename = os.path.splitext(src_filename)[0]

    build_dir = os.path.join(output_dir, "build/")
    mkdir(build_dir)

    build_dst_filepath = os.path.join(build_dir, src_basename + ".%s" % suffix)
    build_dst_filepath = os.path.abspath(build_dst_filepath)

    riscv_src_filepath = os.path.join(build_dir, src_basename + ".%s.S" % suffix)
    riscv_src_filepath = os.path.abspath(riscv_src_filepath)

    shutil.copyfile(dst_filepath, riscv_src_filepath)

    ret = subprocess.run(
        'riscv32-unknown-linux-gnu-gcc {} -o {} -L/root -lsysy -static'.format(
            riscv_src_filepath,
            build_dst_filepath,
        ),
        shell=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
    )

    if ret.returncode != 0:
        print_proc_return(ret)
        return False

    data_in_filepath = os.path.join(src_dir, src_basename + ".in")
    data_out_filepath = os.path.join(src_dir, src_basename + ".out")

    fin = None

    if os.path.exists(data_in_filepath):
        fin = open(data_in_filepath, 'r')

    ret = subprocess.run(
        'qemu-riscv32-static {}'.format(
            build_dst_filepath
        ),
        shell=True,
        stdin=fin,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
    )

    if fin is not None:
        fin.close()

    if not check_output(ret, data_out_filepath):
        return False

    return True
