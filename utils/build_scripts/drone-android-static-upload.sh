#!/usr/bin/env bash

# Script used with Drone CI to upload build artifacts (because specifying all this in
# .drone.jsonnet is too painful).



set -o errexit

if [ -z "$SSH_KEY" ]; then
    echo -e "\n\n\n\e[31;1mUnable to upload artifact: SSH_KEY not set\e[0m"
    # Just warn but don't fail, so that this doesn't trigger a build failure for untrusted builds
    exit 0
fi

echo "$SSH_KEY" >ssh_key

set -o xtrace  # Don't start tracing until *after* we write the ssh key

chmod 600 ssh_key

branch_or_tag=${DRONE_BRANCH:-${DRONE_TAG:-unknown}}

upload_to="quenero.rocks/${DRONE_REPO// /_}/${branch_or_tag// /_}"

tmpdir=android-deps-${DRONE_COMMIT}
mkdir -p $tmpdir/include $tmpdir/lib
cp src/wallet/api/wallet2_api.h $tmpdir/include

for android_abi in "$@"; do
    mkdir -p $tmpdir/lib/${android_abi}
    strip_arch=$android_abi-linux-android
    if [ "$android_abi" = "armeabi-v7a" ]; then strip_arch=arm-linux-androideabi
    elif [ "$android_abi" = "arm64-v8a" ]; then strip_arch=aarch64-linux-android
    elif [ "$android_abi" = "x86" ]; then strip_arch=i686-linux-android
    fi
    /usr/lib/android-sdk/ndk-bundle/toolchains/llvm/prebuilt/linux-x86_64/$strip_arch/bin/strip --strip-debug build-${android_abi}/src/wallet/api/libwallet_merged.a
    ln -s ../../../build-${android_abi}/src/wallet/api/libwallet_merged.a $tmpdir/lib/${android_abi}/libwallet_api.a
done

filename=android-deps-${DRONE_COMMIT}.tar.xz
XZ_OPT="--threads=6" tar --dereference -cJvf $filename $tmpdir

# sftp doesn't have any equivalent to mkdir -p, so we have to split the above up into a chain of
# -mkdir a/, -mkdir a/b/, -mkdir a/b/c/, ... commands.  The leading `-` allows the command to fail
# without error.
upload_dirs=(${upload_to//\// })
mkdirs=
dir_tmp=""
for p in "${upload_dirs[@]}"; do
    dir_tmp="$dir_tmp$p/"
    mkdirs="$mkdirs
-mkdir $dir_tmp"
done

sftp -i ssh_key -b - -o StrictHostKeyChecking=off drone@quenero.rocks <<SFTP
$mkdirs
put $filename $upload_to
SFTP

set +o xtrace

echo -e "\n\n\n\n\e[32;1mUploaded to https://${upload_to}/${filename}\e[0m\n\n\n"

