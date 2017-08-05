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

// Include header file and define struct thread_local_string. 

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

typedef struct __thread_local_string {
        size_t size;
        pthread_key_t key;
} thread_local_string;

// Function about operation of struct thread_local_string.

static thread_local_string *create_thread_local_string(size_t size) {
	thread_local_string *result = NULL; pthread_key_t key = 0;  int error = 1;

	result = (thread_local_string *)calloc(1, sizeof(thread_local_string));
	if(result == NULL) {
		return NULL;
	}

	error = pthread_key_create(&key, free);

	if(error != 0) {
		return NULL;
	}

	result->size = size; result->key = key;

	return result;
}

static char *get_thread_local_string(thread_local_string *tls) {
	char *result = NULL; int error = 1;
	
	if(tls == NULL) {
		return NULL;
	}

	result = (char *)pthread_getspecific(tls->key);

	if(result == NULL) {
		result = (char *)calloc(1, tls->size);

		if(result == NULL) {
			return NULL;
		}

		error = pthread_setspecific(tls->key, (const void *)result);

		if(error != 0) {
			return NULL;
		}
	}
	
	return result;
}

static size_t sizeof_thread_local_string(thread_local_string *tls) {
	if(tls == NULL) {
		return 0;
	}

	return tls->size;
}

// Functions about pty libraries.

int getpt() {
	return posix_openpt(O_RDWR|O_NOCTTY);
}

int grantpt(int fd) {
	return 0;
}

int posix_openpt(int flags) {
	return open("/dev/ptmx", flags);
}

char* ptsname(int fd) {
	static thread_local_string *tls = NULL;
	char *buf = NULL; size_t size = 0; int error = -1;

	if(tls == NULL) {
		tls = create_thread_local_string(32);
	}

	buf = get_thread_local_string(tls);
	size = sizeof_thread_local_string(tls);
	error = ptsname_r(fd, buf, size);

	return (error == 0) ? buf : NULL;
}

int ptsname_r(int fd, char* buf, size_t len) {
	unsigned int pty_num;

	if(buf == NULL) {
		errno = EINVAL; return errno;
	}

	if(ioctl(fd, TIOCGPTN, &pty_num) != 0) {
		errno = ENOTTY; return errno;
	}

	if(snprintf(buf, len, "/dev/pts/%u", pty_num) >= (int)len) {
		errno = ERANGE; return errno;
	}

	return 0;
}

char* ttyname(int fd) {
	static thread_local_string *tls = NULL;
	char* buf = NULL; size_t size = 0; int error = 1;

	if(tls == NULL) {
		tls = create_thread_local_string(64);
	}

	buf = get_thread_local_string(tls);
	size = sizeof_thread_local_string(tls);
	error = ttyname_r(fd, buf, size);
	
	return (error == 0) ? buf : NULL;
}

int ttyname_r(int fd, char* buf, size_t len) {
	char path[64]; ssize_t count = -1;

	if(buf == NULL) {
		errno = EINVAL; return errno;
	}

	if(!isatty(fd)) {
		return errno;
	}

	snprintf(path, sizeof(path), "/proc/self/fd/%d", fd);

	if((count = readlink(path, buf, len)) == -1) {
		return errno;
	}

	if((size_t)count == len) {
		errno = ERANGE; return errno;
	}

	buf[count] = '\0';
	return 0;
}

int unlockpt(int fd) {
	int unlock = 0;
	return ioctl(fd, TIOCSPTLCK, &unlock);
}

int openpty(int* master, int* slave, char* name, const struct termios* t, const struct winsize* ws) {
	char buf[32];

	if((*master = getpt()) == -1) {
		return -1;
	}

	if(grantpt(*master) == -1 || unlockpt(*master) == -1) {
		close(*master);
		return -1;
	}

	if(name == NULL) {
		name = buf;
	}

	if(ptsname_r(*master, name, sizeof(buf)) != 0) {
		close(*master);
		return -1;
	}

	*slave = open(name, O_RDWR|O_NOCTTY);

	if(*slave == -1) {
		close(*master);
		return -1;
	}

	if(t != NULL) {
		tcsetattr(*slave, TCSAFLUSH, t);
	}

	if(ws != NULL) {
		ioctl(*slave, TIOCSWINSZ, ws);
	}

	return 0;
}

int forkpty(int* amaster, char* name, const struct termios* t, const struct winsize* ws) {
	int master; int slave; pid_t pid;

	if(openpty(&master, &slave, name, t, ws) == -1) {
		return -1;
	}

	if((pid = fork()) == -1) {
		close(master); close(slave);
		return -1;
	}

	if(pid == 0) {
		// Child.
		*amaster = -1; close(master);

		if(login_tty(slave) == -1) {
			_exit(1);
		}
		return 0;
	}

	// Parent.
	*amaster = master; close(slave);

	return pid;
}

int login_tty(int fd) {
	setsid();

	if(ioctl(fd, TIOCSCTTY, NULL) == -1) {
		return -1;
	}

	dup2(fd, STDIN_FILENO);
	dup2(fd, STDOUT_FILENO);
	dup2(fd, STDERR_FILENO);

	if(fd > STDERR_FILENO) {
		close(fd);
	}

	return 0;
}
