/*-
 * Copyright (c) 2020 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by 
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE NETBSD FOUNDATION, INC. AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "sandbox.h"

static void 
usage(void)
{
    fprintf(stderr, 
            "usage: sandbox-exec [OPTION] script prog prog-args...\n"
            "\n"
            "  options:\n"
            "    -h\n"
            "      display this help message\n"
            "    -k\n"
            "      if process attempts a denied operation, kill the process\n");
    exit(1);
}

int
main(int argc, char *argv[])
{
    int error = 0;
    int c = 0;
    int flags = 0;

    opterr = 0;
    while ((c = getopt(argc, argv, "hk")) != -1) {
        switch (c) {
        case 'k':
            flags |= SANDBOX_ON_DENY_KILL;
            break;
        case 'h':
            usage();
        case '?':
            fprintf(stderr, "unknown option '%c'\n", (char)optopt);
            exit(1);
        default:
            usage();
        }
    }
    argc -= optind;
    argv += optind;

    if (argc < 2)
        usage();

    error = sandbox_from_file(argv[0], flags);
    if (error == -1) {
        fprintf(stderr, "sandbox_from_file('%s') failed: '%s'\n", argv[0], strerror(errno));
        error = 1;
        goto fail;
    }

    error = execv(argv[1], &argv[1]);
    if (error == -1) {
        fprintf(stderr, "execv('%s', ...) failed: '%s'\n", argv[1], strerror(errno));
        error = 1;
        goto fail;
    }

fail:
    return (error);
}
