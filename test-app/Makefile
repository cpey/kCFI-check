# Build for Linux desktop:
# ARCH=x86 HOST=linux make
#
# Build for mobile on Linux host:
# HOST=linux make
#
# Build for mobile on macOS:
# make

ifeq ($(ARCH),x86)
CC = gcc
DEFS = -DARCH_X86
FLAGS = -m64
else
ifeq ($(HOST),linux)
export NDK=$(HOME)/Android/Sdk/ndk/21.3.6528147/
export TOOLCHAIN=$(NDK)/toolchains/llvm/prebuilt/linux-x86_64
export TARGET=aarch64-linux-android
export API=21
else
export NDK=$(HOME)/Library/Android/sdk/ndk-bundle/
export TOOLCHAIN=$(NDK)/toolchains/llvm/prebuilt/darwin-x86_64
export TARGET=aarch64-linux-android
export API=21
endif
CC = $(TOOLCHAIN)/bin/$(TARGET)$(API)-clang
DEST = /data/local/tmp
endif

kcfi-test: kcfi-test.c helpers.c
	$(CC) -o $@ $^
	adb push $@ $(DEST)

clean: 
	rm -f test-kcfi-user
