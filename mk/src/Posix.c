/*
Copyright © 1998, 1999 Lucent Technologies Inc.  All rights reserved.  
Revisions Copyright © 1999, 2000 Vita Nuova Limited.  All rights reserved.
Revisions Copyright © 2001 Norman Ramsey.  All rights reserved.  
*/
#include	"mk.h"
#include	<dirent.h>
#include	<signal.h>
#include	<sys/wait.h>
#include	<utime.h>

char 	*shell =	"/bin/sh";
char 	*shellname =	"sh";

extern char **environ;

void
readenv(void)
{
	char **p, *s;
	Word *w;

	for(p = environ; *p; p++){
		s = shname(*p);
		if(*s == '=') {
			*s = 0;
			w = newword(s+1);
		} else
			w = newword("");
		if (symlook(*p, S_INTERNAL, 0))
			continue;
		s = strdup(*p);
		setvar(s, (void *)w);
		symlook(s, S_EXPORTED, (void*)"")->value = (void*)"";
	}
}

/*
 *	done on child side of fork, so parent's env is not affected
 *	and we don't care about freeing memory because we're going
 *	to exec immediately after this.
 */
void
exportenv(Envy *e)
{
	int i;
	char **p;
	char buf[8192];

	p = 0;
	for(i = 0; e->name; e++, i++) {
		p = (char**) Realloc(p, (i+2)*sizeof(char*));
		if(e->values)
			sprint(buf, "%s=%s", e->name,  wtos(e->values, IWS));
		else
			sprint(buf, "%s=", e->name);
		p[i] = strdup(buf);
	}
	p[i] = 0;
	environ = p;
}

void
dirtime(char *dir, char *path)
{
	DIR *dirp;
	struct dirent *dp;
	Dir d;
	void *t;
	char buf[8192];

	dirp = opendir(dir);
	if(dirp) {
		while((dp = readdir(dirp)) != 0){
			sprint(buf, "%s%s", path, dp->d_name);
			if(dirstat(buf, &d) < 0)
				continue;
			t = (void *)d.mtime;
			if (!t)			/* zero mode file */
				continue;
			if(symlook(buf, S_TIME, 0))
				continue;
			symlook(strdup(buf), S_TIME, t)->value = t;
		}
		closedir(dirp);
	}
}

int
waitfor(char *msg)
{
	int status;
	int pid;

	*msg = 0;
	pid = wait(&status);
	if(pid > 0) {
		if(status&0x7f) {
			if(status&0x80)
				snprint(msg, ERRLEN, "signal %d, core dumped", status&0x7f);
			else
				snprint(msg, ERRLEN, "signal %d", status&0x7f);
		} else if(status&0xff00)
			snprint(msg, ERRLEN, "exit(%d)", (status>>8)&0xff);
	}
	return pid;
}

void
expunge(int pid, char *msg)
{
	if(strcmp(msg, "interrupt"))
		kill(pid, SIGINT);
	else
		kill(pid, SIGHUP);
}

int
execsh(char *args, char *cmd, Bufblock *buf, Envy *e)
{
	char *p;
	int tot, n, pid, in[2], out[2];

	if(buf && pipe(out) < 0){
		perror("pipe");
		Exit();
	}
	pid = fork();
	if(pid < 0){
		perror("mk fork");
		Exit();
	}
	if(pid == 0){
		if(buf)
			close(out[0]);
		if(pipe(in) < 0){
			perror("pipe");
			Exit();
		}
		pid = fork();
		if(pid < 0){
			perror("mk fork");
			Exit();
		}
		if(pid != 0){
			dup2(in[0], 0);
			if(buf){
				dup2(out[1], 1);
				close(out[1]);
			}
			close(in[0]);
			close(in[1]);
			if (e)
				exportenv(e);
			if(shflags)
				execl(shell, shellname, shflags, args, 0);
			else
				execl(shell, shellname, args, 0);
			perror(shell);
			_exits("exec");
		}
		close(out[1]);
		close(in[0]);
		if(DEBUG(D_EXEC))
			printf("starting: %s\n", cmd);
		p = cmd+strlen(cmd);
		while(cmd < p){
			n = write(in[1], cmd, p-cmd);
			if(n < 0)
				break;
			cmd += n;
		}
		close(in[1]);
		_exits(0);
	}
	if(buf){
		close(out[1]);
		tot = 0;
		for(;;){
			if (buf->current >= buf->end)
				growbuf(buf);
			n = read(out[0], buf->current, buf->end-buf->current);
			if(n <= 0)
				break;
			buf->current += n;
			tot += n;
		}
		if (tot && buf->current[-1] == '\n')
			buf->current--;
		close(out[0]);
	}
	return pid;
}
int
pipecmd(char *cmd, Envy *e, int *fd)
{
	int pid, pfd[2];

	if(DEBUG(D_EXEC))
		printf("pipecmd='%s'", cmd);/**/

	if(fd && pipe(pfd) < 0){
		perror("pipe");
		Exit();
	}
	pid = fork();
	if(pid < 0){
		perror("mk fork");
		Exit();
	}
	if(pid == 0){
		if(fd){
			close(pfd[0]);
			dup2(pfd[1], 1);
			close(pfd[1]);
		}
		if(e)
			exportenv(e);
		if(shflags)
			execl(shell, shellname, shflags, "-c", cmd, 0);
		else
			execl(shell, shellname, "-c", cmd, 0);
		perror(shell);
		_exits("exec");
	}
	if(fd){
		close(pfd[1]);
		*fd = pfd[0];
	}
	return pid;
}

void
Exit(void)
{
	while(wait(0) >= 0)
		;
	exits("error");
}

static	struct
{
	int	sig;
	char	*msg;
}	sigmsgs[] =
{
	SIGALRM,	"alarm",
	SIGFPE,		"sys: fp: fptrap",
	SIGPIPE,	"sys: write on closed pipe",
	SIGILL,		"sys: trap: illegal instruction",
	SIGSEGV,	"sys: segmentation violation",
	0,		0
};

static void
notifyf(int sig)
{
	int i;
	char *p;

	for(i = 0; sigmsgs[i].msg; i++)
		if(sigmsgs[i].sig == sig)
			killchildren(sigmsgs[i].msg);

	/* should never happen */
	signal(sig, SIG_DFL);
	kill(getpid(), sig);
}

void
catchnotes()
{
	int i;

	for(i = 0; sigmsgs[i].msg; i++)
		signal(sigmsgs[i].sig, notifyf);
}

char*
maketmp(void)
{
	static char temp[L_tmpnam];

	return tmpnam(temp);
}

int
chgtime(char *name)
{
	Dir sbuf;
	struct utimbuf u;

	if(dirstat(name, &sbuf) >= 0) {
		u.actime = sbuf.atime;
		u.modtime = time(0);
		return utime(name, &u);
	}
	return close(create(name, OWRITE, 0666));
}

void
rcopy(char **to, Resub *match, int n)
{
	int c;
	char *p;

	*to = match->s.sp;		/* stem0 matches complete target */
	for(to++, match++; --n > 0; to++, match++){
		if(match->s.sp && match->e.ep){
			p = match->e.ep;
			c = *p;
			*p = 0;
			*to = strdup(match->s.sp);
			*p = c;
		}
		else
			*to = 0;
	}
}

int
mkdirstat(char *name, Dir *buf)
{
	return dirstat(name, buf);
}
