/*-
 * Copyright (c) 1991, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * James W. Williams of NASA Goddard Space Flight Center.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <sys/cdefs.h>
#ifndef lint
__used static const char copyright[] =
"@(#) Copyright (c) 1991, 1993\n\r\
	The Regents of the University of California.  All rights reserved.\n\r";
#endif /* not lint */

#ifndef lint
#if 0
static char sccsid[] = "@(#)cksum.c	8.2 (Berkeley) 4/28/95";
#endif
#endif /* not lint */

#include <sys/cdefs.h>
__FBSDID("$FreeBSD: src/usr.bin/cksum/cksum.c,v 1.17 2003/03/13 23:32:28 robert Exp $");

#include <sys/types.h>

#include <err.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "extern.h"

static void usage(void);

int
chksum_main(int argc, char **argv)
{
	uint32_t val;
	int ch, fd, rval;
	off_t len;
	char *fn, *p;
	int (*cfncn)(int, uint32_t *, off_t *);
	void (*pfncn)(char *, u_int32_t, off_t);
	
	cfncn=NULL;

	if(*argv) {
	  if ((p = rindex(argv[0], '/')) == NULL)
	    p = argv[0];
	  else
	    ++p;
	  if (!strcmp(p, "sum")) {
	    cfncn = csum1;
	    pfncn = psum1;
	    ++argv;
	  }
	} 
	
	if(!cfncn) {
		cfncn = crc;
		pfncn = pcrc;

		while ((ch = getopt(argc, argv, "o:")) != -1)
			switch (ch) {
			case 'o':
				if (!strcmp(optarg, "1")) {
					cfncn = csum1;
					pfncn = psum1;
				} else if (!strcmp(optarg, "2")) {
					cfncn = csum2;
					pfncn = psum2;
				} else if (!strcmp(optarg, "3")) {
					cfncn = chksum_crc32;
					pfncn = pcrc;
				} else {
					warnx("illegal argument to -o option");
					usage(); return 0;
				}
				break;
			case '?':
			default:
				usage(); return 0;
			}
//		argc -= optind;
		argv += optind;
	}

	fd = STDIN_FILENO;
	fn = NULL;
	rval = 0;
	do {
		if (*argv) {
			fn = *argv++;
			if ((fd = open(fn, O_RDONLY, 0)) < 0) {
				warn("%s", fn);
                fprintf(stderr, "\r");
				rval = 1;
				continue;
			}
		}
		if (cfncn(fd, &val, &len)) {
			warn("%s", fn ? fn : "stdin");
            fprintf(stderr, "\r");
			rval = 1;
		} else
			pfncn(fn, val, len);
		(void)close(fd);
	} while (*argv);
	return(rval);
}

static void
usage(void)
{
	(void)fprintf(stderr, "\rusage: cksum [-o 1 | 2 | 3] [file ...]\n\r");
	(void)fprintf(stderr, "       sum [file ...]\n\r");
	// exit(1);
}
