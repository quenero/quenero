#!/bin/bash

set -e

UPSTREAM_VER="$1"
LIBQUENERO_VER="${UPSTREAM_VER/[^0-9.]*/}"
if ! grep -q "^Package: libquenero$LIBQUENERO_VER\$" debian/control; then
    echo -e "\nError: debian/control doesn't contain the correct libquenero$LIBQUENERO_VER version; you should run:\n\n    ./debian/update-libquenero-ver.sh\n"
    exit 1
fi

for sublib in "" "-wallet"; do
    if ! [ -f debian/libquenero$sublib$LIBQUENERO_VER ]; then
        rm -f debian/libquenero$sublib[0-9]*.install
        sed -e "s/@LIBQUENERO_VER@/$LIBQUENERO_VER/" debian/libquenero$sublib.install.in >debian/libquenero$sublib$LIBQUENERO_VER.install
    fi
done
