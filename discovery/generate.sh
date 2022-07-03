#!/bin/bash -e

GSOAP_DIR=/usr/share/gsoap
GSOAP_IMPORT_DIR=$GSOAP_DIR/import

# -c++11 Generate C++ source code optimized for C++11 (compile with -std=c++11).
# -2     Generate SOAP 1.2 source code.
# -C     Generate client-side source code only.
# -x     Do not generate sample XML message files.
# -a     Use HTTP SOAPAction with WS-Addressing to invoke server-side operations.
# -L     Do not generate soapClientLib/soapServerLib.
# -pname Save files with new prefix name instead of soap.
# -Ipath Use path(s) for #import (paths separated with ':').
soapcpp2 -c++11 -2 -C -x -a -L -pwsdd -I $GSOAP_IMPORT_DIR $GSOAP_IMPORT_DIR/wsdd5.h
