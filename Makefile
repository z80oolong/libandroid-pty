## The dynamic library "libandroid-pty.so" is based on the following code of "bionic" library, which is standard C library for Android OS
## by The Android Open Source Project, and fixed by Z.OOL. (NAKATSUKA, Yukitaka).
## 
## - https://raw.githubusercontent.com/android/platform_bionic/30a36273ab761fb07892f7816b4873b6c48a0cea/libc/bionic/pty.cpp
## - https://raw.githubusercontent.com/android/platform_bionic/30a36273ab761fb07892f7816b4873b6c48a0cea/libc/private/ThreadLocalBuffer.h
## 
## libandroid-pty.so is distributed under the same license of "bionic" library.
## 
## ----
## 
## Copyright (C) 2008 The Android Open Source Project
## Copyright (C) 2017 Z.OOL. (NAKATSUKA, Yukitaka <zool@zool.jpn.org>)
## 
## All rights reserved.
##
## Redistribution and use in source and binary forms, with or without
## modification, are permitted provided that the following conditions
## are met:
##  * Redistributions of source code must retain the above copyright
##    notice, this list of conditions and the following disclaimer.
##  * Redistributions in binary form must reproduce the above copyright
##    notice, this list of conditions and the following disclaimer in
##    the documentation and/or other materials provided with the
##    distribution.
##
## THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
## "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
## LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
## FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
## COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
## INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
## BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
## OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
## AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
## OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
## OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
## SUCH DAMAGE.

# install directory.
INSTALL_PREFIX := /usr/local

# source file, header file, object file, and map file.
LIBANDROID_PTY_C   = libandroid-pty.c
LIBANDROID_PTY_H   = libandroid-pty.h
LIBANDROID_PTY_SO  = libandroid-pty.so
LIBANDROID_PTY_MAP = libandroid-pty.map

# shell commands.
MKDIR   = mkdir
INSTALL = install
CP      = cp
RM      = rm

# compile commands and flags.
GCC     = gcc
STRIP   = strip
CFLAGS  += -fPIC -shared -Wall -Os -Wl,--version-script,$(LIBANDROID_PTY_MAP)
LDFLAGS += -lpthread

# compile architecture.
ARCH    := arm

ifeq ($(ARCH), arm)
  GCC    := arm-linux-gnueabihf-$(GCC)
  STRIP  := arm-linux-gnueabihf-$(STRIP)
endif
ifeq ($(ARCH), x86-32)
  GCC    := x86_64-linux-gnu-$(GCC)
  STRIP  := x86_64-linux-gnu-$(STRIP)
  CFLAGS := -m32 $(CFLAGS)
endif
ifeq ($(ARCH), x86-64)
  GCC    := x86_64-linux-gnu-$(GCC)
  STRIP  := x86_64-linux-gnu-$(STRIP)
endif

# build task.
$(LIBANDROID_PTY_SO): $(LIBANDROID_PTY_C) $(LIBANDROID_PTY_H) $(LIBANDROID_PTY_MAP)
	$(GCC) -o $(LIBANDROID_PTY_SO) $(CFLAGS) $(LIBANDROID_PTY_C) $(LDFLAGS)
	$(STRIP) $(LIBANDROID_PTY_SO)
	$(CP) -p $(LIBANDROID_PTY_SO) $(LIBANDROID_PTY_SO).$(ARCH)

install: $(LIBANDROID_PTY_SO) $(LIBANDROID_PTY_H)
	[ -d $(INSTALL_PREFIX)/lib ]     || $(MKDIR) -p $(INSTALL_PREFIX)lib
	[ -d $(INSTALL_PREFIX)/include ] || $(MKDIR) -p $(INSTALL_PREFIX)/include
	$(INSTALL) -m 0700 $(LIBANDROID_PTY_SO) $(INSTALL_PREFIX)/lib
	$(INSTALL) -m 0666 $(LIBANDROID_PTY_H)  $(INSTALL_PREFIX)/include

clean:
	$(RM) -f $(LIBANDROID_PTY_SO)

distclean:
	$(RM) -f $(LIBANDROID_PTY_SO) $(LIBANDROID_PTY_SO).*
