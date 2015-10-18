/*
 * Focal, 1981.
 * Operating system dependent code.
 * Most has something to do with the syntax of
 * a file name or the format of the directory of the
 * disc; most of this is in the library command.
 */
#include "focal.h"

#ifdef DIR_SUPPORT
# ifdef	DOS
#  include <dos.h>
# else
#  include <sys/types.h>
#  include <sys/stat.h>
#  include <fcntl.h>
#  include <unistd.h>
#  ifdef DIRENT
#   include <dirent.h>
#   define	direct	dirent
#  else
#   include <sys/dir.h>
#  endif
struct stat	sb;
struct direct	db;
# endif
#endif

/*
 * Process the library command.
 * This command (sadly) requires a rather
 * detailed knowledge of the file system of
 * the operating system.
 */
void library(void)
{
	char	*p;
	int	c;
	int	d;
	char	*sctp;
	struct line	*lp1;
	struct line	*lp2;
	FILE	*fp;

	c = getnb();
	if (c!='c' && c!='s' && c!='l' && c!='d')
		diag("Bad library command");
	while (isalpha(*ctp))
		++ctp;
	while ((d = *ctp)==' ' || d=='\t')
		++ctp;
	if (c!='l' && d==0)
		diag("Missing file name");
	p = ctp;
	while (*ctp != 0)
		++ctp;
	switch (c) {

	case 'c':
		if ((fp=fopen(p, "r")) == NULL)
			diag("Cannot open");
		lp1 = line;
		while (lp1 != NULL) {
			lp2 = lp1->l_fp;
			free ((char *) lp1);
			lp1 = lp2;
		}
		line = NULL;
		sctp = ctp;
		while (focal_getline(abuf, fp) != 0) {
			ctp = abuf;
			if ((c=getnb()) != 0) {
				if (isdigit(c) == 0)
					diag("Direct line in call");
				inject(c);
			}
		}
		fclose(fp);
		ctp = sctp;
		break;

	case 'd':
#ifdef	vax
		if (delete(p) < 0)
#else
		if (remove(p) < 0)
#endif
			diag("Cannot delete");
		break;

	case 'l':
#ifndef	DIR_SUPPORT
		diag("Library list not implemented");
#else
	{
#ifdef	DOS
		int	attr = _A_RDONLY | _A_ARCH;
		int	rc;
		static struct find_t	de;
		static char	fname[FILENAME_MAX];
		char	*s;
		
		if (d == 0)
			strcpy(fname, "*.*");
		else {
			strcpy(fname, p);
			s = fname + strlen(fname) - 1;
			if (*s == '\\')
				strcat(fname, "*.*");
			else if (*s == '.')
				strcat(fname, "\\*.*");
		}
		rc = _dos_findfirst(fname, attr, &de);
		while (!rc) {
			if ((de.attrib & _A_SUBDIR) == 0
			    && strcmp(de.name, ".") != 0
			    && strcmp(de.name, "..") != 0)
				printf("%-13s\n", de.name);
			rc = _dos_findnext(&de);
		}
#else
#ifdef DIRENT
		struct dirent *de;
		DIR *dp;

		if (d == 0)
			p = ".";
		if ((dp = opendir(p)) == NULL) {
			perror("focal");
			diag("Bad directory");
		}
		while ((de = readdir(dp)) != NULL) {
			if (de->d_ino == 0
			    ||  strcmp(de->d_name, ".") == 0
			    ||  strcmp(de->d_name, "..") == 0)
				continue;
			printf("%s\n", de->d_name);
		}
		closedir(dp);
#else
		int	fd = 0;
		
		if (d == 0)
			p = ".";
		if (stat(p, &sb) < 0
		|| (sb.st_mode&S_IFMT) != S_IFDIR
		|| (fd = open(p, 0)) < 0)
			diag("Bad directory");
		while (read(fd, &db, sizeof(db)) == sizeof(db)) {
			if (db.d_ino == 0
			||  strncmp(db.d_name, ".",  DIRSIZ) == 0
			||  strncmp(db.d_name, "..", DIRSIZ) == 0)
				continue;
			printf("%.*s\n", DIRSIZ, db.d_name);
		}
		close(fd);
#endif
#endif
	}
#endif
		break;

	case 's':
		if ((fp=fopen(p, "w")) == NULL)
			diag("Cannot create");
		save(NULL, fp);
		fclose(fp);
		break;
	}
}

/*
 * Set up to catch the user's
 * ^C interrupt.
 */
void catchcc(void)
{
	signal(SIGINT, (void (*) (int))onintr);
}

/*
 * This routine is called by
 * the ^C signal handler. All it does
 * is set a flag, which is looked at
 * by the dispatcher.
 */
void onintr(void)
{
	++intflag;
}
