#ifdef READLINE
#include <readline/readline.h>
#include <readline/history.h>
#endif

#define DEBUG 0
#include "shell.h"

sigset_t sigchld_mask;

static void sigint_handler(int sig) {
  /* No-op handler, we just need break read() call with EINTR. */
  (void)sig;
}

/* Rewrite closed file descriptors to -1,
 * to make sure we don't attempt do close them twice. */
static void MaybeClose(int *fdp) {
  if (*fdp < 0)
    return;
  Close(*fdp);
  *fdp = -1;
}

/* Consume all tokens related to redirection operators.
 * Put opened file descriptors into inputp & output respectively. */
static int do_redir(token_t *token, int ntokens, int *inputp, int *outputp) {
  token_t mode = NULL; /* T_INPUT, T_OUTPUT or NULL */
  int n = 0;           /* number of tokens after redirections are removed */

  for (int i = 0; i < ntokens; i++) {
    /* TODO: Handle tokens and open files as requested. */
#ifdef STUDENT
    (void)mode;
    (void)MaybeClose;
    // sprawdzamy tokeny
    if (token[i] == T_INPUT) {
      // jesli token jest inputem to zamieniamy go na NULL
      token[i] = T_NULL;
      // jesli input jest otwarty to go zamykamy
      MaybeClose(inputp);
      // otwieramy plik(token[++i]) z flaga O_RDONLY(read only)
      *inputp = open(token[++i], O_RDONLY);
      // zamieniamy token na NULL
      token[i] = T_NULL;
    } else if (token[i] == T_OUTPUT) {
      // jesli token jest outputem to zamieniamy go na NULL
      token[i] = T_NULL;
      // jesli output jest otwarty to go zamykamy
      MaybeClose(outputp);
      // otwieramy plik(token[++i]) z flaga O_WRONLY(write only) oraz
      // O_CREAT(create file if it doesn't exist) oraz O_APPEND(append to the
      // end of file)
      *outputp = open(token[++i], O_WRONLY | O_CREAT | O_APPEND, 0644);
      // zamieniamy token na NULL
      token[i] = T_NULL;
    } else if (token[i] != T_NULL && token[i] != NULL) {
      // jesli token nie jest NULLem to zwiekszamy n
      n++;
    }
#endif /* !STUDENT */
  }

  token[n] = NULL;
  return n;
}

/* Execute internal command within shell's process or execute external command
 * in a subprocess. External command can be run in the background. */
static int do_job(token_t *token, int ntokens, bool bg) {
  int input = -1, output = -1;
  int exitcode = 0;

  ntokens = do_redir(token, ntokens, &input, &output);

  if (!bg) {
    if ((exitcode = builtin_command(token)) >= 0)
      return exitcode;
  }

  sigset_t mask;
  Sigprocmask(SIG_BLOCK, &sigchld_mask, &mask);

  /* TODO: Start a subprocess, create a job and monitor it. */
#ifdef STUDENT
  pid_t pid = fork();
  // jesli pid jest mniejszy od 0 to jest blad
  if (pid < 0)
    unix_error("Fork error");
  // jesli pid jest rowny 0 to jestesmy w dziecku
  if (pid == 0) {
    // jesli nie jestesmy w tle to ustawiamy proces jako foreground
    if (!bg) {
      setfgpgrp(getpid());
    }
    // sygnaly sa domyslnie blokowane, odblokowujemy je
    Sigprocmask(SIG_SETMASK, &mask, NULL);
    signal(SIGTSTP, SIG_DFL);
    signal(SIGTTIN, SIG_DFL);
    signal(SIGTTOU, SIG_DFL);

    // ustawiamy proces jako grupa procesow
    setpgid(pid, pid);

    // jesli input jest otwarty to zamieniamy go na stdin
    if (input != -1) {
      dup2(input, 0);
      // jesli input jest otwarty to go zamykamy
      MaybeClose(&input);
    }
    // jesli output jest otwarty to zamieniamy go na stdout
    if (output != -1) {
      dup2(output, 1);
      // jesli output jest otwarty to go zamykamy
      MaybeClose(&output);
    }
    external_command(token); // wywolujemy external command
  } else { // jesli pid jest wiekszy od 0 to jestesmy w rodzicu
    // ustawiamy proces jako grupa procesow
    setpgid(pid, pid);
    // dodajemy joba
    int j = addjob(pid, bg);
    // dodajemy proces do joba
    addproc(j, pid, token);
    // jesli nie jestesmy w tle to ustawiamy proces jako foreground
    if (!bg) {
      setfgpgrp(pid);
      // monitorujemy joba
      exitcode = monitorjob(&mask);
    } else {
      msg("[%d] running '%s'\n", j, jobcmd(j));
    }
  }
#endif /* !STUDENT */

  Sigprocmask(SIG_SETMASK, &mask, NULL);
  return exitcode;
}

/* Start internal or external command in a subprocess that belongs to pipeline.
 * All subprocesses in pipeline must belong to the same process group. */
static pid_t do_stage(pid_t pgid, sigset_t *mask, int input, int output,
                      token_t *token, int ntokens, bool bg) {
  ntokens = do_redir(token, ntokens, &input, &output);

  if (ntokens == 0)
    app_error("ERROR: Command line is not well formed!");

  /* TODO: Start a subprocess and make sure it's moved to a process group. */
  pid_t pid = Fork();
#ifdef STUDENT
  int exitcode = 0;
  if (pid < 0) // jesli pid jest mniejszy od 0 to jest blad
    unix_error("Fork error");
  if (pid == 0) {       // jesli pid jest rowny 0 to jestesmy w dziecku
    setpgid(pid, pgid); // ustawiamy proces jako grupa procesow
    if (!bg) { // jesli nie jestesmy w tle to ustawiamy proces jako foreground
      if (pgid == 0)
        setfgpgrp(getpid());
      else
        setfgpgrp(pgid);
    }
    // sygnaly sa domyslnie blokowane, odblokowujemy je
    Sigprocmask(SIG_SETMASK, mask, NULL);
    signal(SIGTSTP, SIG_DFL);
    signal(SIGTTIN, SIG_DFL);
    signal(SIGTTOU, SIG_DFL);

    if (input != -1) { // jesli input jest otwarty to zamieniamy go na stdin
      dup2(input, 0);
      MaybeClose(&input);
    }
    if (output != -1) { // jesli output jest otwarty to zamieniamy go na stdout
      dup2(output, 1);
      MaybeClose(&output);
    }
    if ((exitcode = builtin_command(token)) >=
        0) // jesli jest to builtin command
      exit(exitcode);

    external_command(token); // jesli nie to external command

  } else { // jesli pid jest wiekszy od 0 to jestesmy w rodzicu
    // jesli pgid jest rowny 0 to ustawiamy proces jako grupa procesow
    if (pgid == 0)
      setpgid(pid, pid);
    else
      setpgid(pid, pgid);
    // jesli nie jestesmy w tle to ustawiamy proces jako foreground
    if (!bg) {
      if (pgid == 0)
        setfgpgrp(pid);
      else
        setfgpgrp(pgid);
    }
  }
#endif /* !STUDENT */

  return pid;
}

static void mkpipe(int *readp, int *writep) {
  int fds[2];
  Pipe(fds);
  fcntl(fds[0], F_SETFD, FD_CLOEXEC);
  fcntl(fds[1], F_SETFD, FD_CLOEXEC);
  *readp = fds[0];
  *writep = fds[1];
}

/* Pipeline execution creates a multiprocess job. Both internal and external
 * commands are executed in subprocesses. */
static int do_pipeline(token_t *token, int ntokens, bool bg) {
  pid_t pid, pgid = 0;
  int job = -1;
  int exitcode = 0;

  int input = -1, output = -1, next_input = -1;

  mkpipe(&next_input, &output);

  sigset_t mask;
  Sigprocmask(SIG_BLOCK, &sigchld_mask, &mask);

  /* TODO: Start pipeline subprocesses, create a job and monitor it.
   * Remember to close unused pipe ends! */
#ifdef STUDENT
  (void)input;
  (void)job;
  (void)pid;
  (void)pgid;

  int i = 0;
  int ntokens_processed = 0;
  // pierwszy pipe
  for (; i < ntokens; i++) {
    if (token[i] == T_PIPE) { // jak natrafimy na T_PIPE
      token[i] = NULL;        // to zamieniamy go na NULL
      pgid = do_stage(pgid, &mask, input, output, token, i,
                      bg);       // ustawiamy pgid na pid pierwszego processu
      MaybeClose(&input);        // zamykamy input
      MaybeClose(&output);       // zamykamy output
      input = next_input;        // ustawiamy input na next_input
      job = addjob(pgid, bg);    // dodajemy job
      addproc(job, pgid, token); // dodajemy proces do joba
      i++;
      break; // wychodzimy z petli
    }
  }
  if (!bg) { // jesli nie jestesmy w tle to ustawiamy proces jako foreground
    setfgpgrp(pgid);
  }
  setpgid(pgid, pgid);
  // nastepne pipy
  int j = i;
  for (; i < ntokens; i++) {        // petla po tokenach
    if (token[i] == T_PIPE) {       // jesli natrafimy na T_PIPE
      token[i] = NULL;              // to zamieniamy go na NULL
      mkpipe(&next_input, &output); // tworzymy kolejny pipe
      pid = do_stage(pgid, &mask, input, output, token + j, ntokens_processed,
                     bg);           // tworzymy proces
      setpgid(pid, pgid);           // ustawiamy grupe dla procesu
      MaybeClose(&input);           // zamykamy input
      MaybeClose(&output);          // zamykamy output
      input = next_input;           // ustawiamy input na next_input
      addproc(job, pid, token + j); // dodajemy proces do joba
      ntokens_processed = 0;        // zerujemy licznik tokenow
      j = i + 1;                    // ustawiamy j na nastepny token
    } else {
      ntokens_processed++; // zwiekszamy licznik tokenow
    }
  }

  // ostatni pipe
  pid = do_stage(pgid, &mask, input, output, token + j, ntokens_processed,
                 bg);           // tworzymy proces
  MaybeClose(&input);           // zamykamy input
  MaybeClose(&output);          // zamykamy output
  input = next_input;           // ustawiamy input na next_input
  addproc(job, pid, token + j); // dodajemy proces do joba
  if (!bg) {
    exitcode = monitorjob(&mask); // monitorujemy job
  } else {
    msg("[%d] running '%s'\n", job, jobcmd(job));
  }
#endif /* !STUDENT */

  Sigprocmask(SIG_SETMASK, &mask, NULL);
  return exitcode;
}

static bool is_pipeline(token_t *token, int ntokens) {
  for (int i = 0; i < ntokens; i++)
    if (token[i] == T_PIPE)
      return true;
  return false;
}

static void eval(char *cmdline) {
  bool bg = false;
  int ntokens;
  token_t *token = tokenize(cmdline, &ntokens);

  if (ntokens > 0 && token[ntokens - 1] == T_BGJOB) {
    token[--ntokens] = NULL;
    bg = true;
  }

  if (ntokens > 0) {
    if (is_pipeline(token, ntokens)) {
      do_pipeline(token, ntokens, bg);
    } else {
      do_job(token, ntokens, bg);
    }
  }

  free(token);
}

#ifndef READLINE
static char *readline(const char *prompt) {
  static char line[MAXLINE]; /* `readline` is clearly not reentrant! */

  write(STDOUT_FILENO, prompt, strlen(prompt));

  line[0] = '\0';

  ssize_t nread = read(STDIN_FILENO, line, MAXLINE);
  if (nread < 0) {
    if (errno != EINTR)
      unix_error("Read error");
    msg("\n");
  } else if (nread == 0) {
    return NULL; /* EOF */
  } else {
    if (line[nread - 1] == '\n')
      line[nread - 1] = '\0';
  }

  return strdup(line);
}
#endif

int main(int argc, char *argv[]) {
  /* `stdin` should be attached to terminal running in canonical mode */
  if (!isatty(STDIN_FILENO))
    app_error("ERROR: Shell can run only in interactive mode!");

#ifdef READLINE
  rl_initialize();
#endif

  sigemptyset(&sigchld_mask);
  sigaddset(&sigchld_mask, SIGCHLD);

  if (getsid(0) != getpgid(0))
    Setpgid(0, 0);

  initjobs();

  struct sigaction act = {
    .sa_handler = sigint_handler,
    .sa_flags = 0, /* without SA_RESTART read() will return EINTR */
  };
  Sigaction(SIGINT, &act, NULL);

  Signal(SIGTSTP, SIG_IGN);
  Signal(SIGTTIN, SIG_IGN);
  Signal(SIGTTOU, SIG_IGN);

  while (true) {
    char *line = readline("# ");

    if (line == NULL)
      break;

    if (strlen(line)) {
#ifdef READLINE
      add_history(line);
#endif
      eval(line);
    }
    free(line);
    watchjobs(FINISHED);
  }

  msg("\n");
  shutdownjobs();

  return 0;
}
