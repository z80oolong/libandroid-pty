/*
 * The dynamic library "libandroid-pty.so" is based on the following code of "bionic" library, which is standard C library for Android OS
 * by The Android Open Source Project, and fixed by Z.OOL. (NAKATSUKA, Yukitaka).
 * 
 * - https://raw.githubusercontent.com/android/platform_bionic/30a36273ab761fb07892f7816b4873b6c48a0cea/libc/bionic/pty.cpp
 * - https://raw.githubusercontent.com/android/platform_bionic/30a36273ab761fb07892f7816b4873b6c48a0cea/libc/private/ThreadLocalBuffer.h
 * 
 * libandroid-pty.so is distributed under the same license of "bionic" library.
 * 
 * ----
 * 
 * Copyright (C) 2008 The Android Open Source Project
 * Copyright (C) 2017 Z.OOL. (NAKATSUKA, Yukitaka <zool@zool.jpn.org>)
 * 
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#define _GNU_SOURCE
#define _XOPEN_SOURCE 600

#include <malloc.h>
#include <pthread.h>
#include <errno.h>
#include <fcntl.h>
#include <pty.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>
#include <utmp.h>

// Functions about pty libraries.

extern int getpt(void);
extern int grantpt(int fd);
extern int posix_openpt(int flags);
extern char* ptsname(int fd);
extern int ptsname_r(int fd, char* buf, size_t len);
extern char* ttyname(int fd);
extern int ttyname_r(int fd, char* buf, size_t len);
extern int unlockpt(int fd);
extern int openpty(int* master, int* slave, char* name, const struct termios* t, const struct winsize* ws);
extern int forkpty(int* amaster, char* name, const struct termios* t, const struct winsize* ws);
extern int login_tty(int fd);
