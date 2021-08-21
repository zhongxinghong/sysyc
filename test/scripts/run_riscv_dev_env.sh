#!/bin/sh

ROOT_DIR=$(cd $(dirname $0); pwd)

sudo docker run -it -v $ROOT_DIR/../../:/root/sysyc riscv-dev-env /bin/bash
