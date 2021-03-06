/*
 * Recording process, third try: use pseudo-terminals,
 * copying the mode of the pty slave from the tty on stdin.
 * We also set the mode of the tty on stdin to raw.
 */

main(argc, argv, envp)
int	argc;
char	**argv;
char	**envp;
{
	int	master_fd, slave_fd, childpid;

	if (!isatty(0) || !isatty(1))
		err_quit("stdin and stdout must be a terminal");

	if ( (master_fd = pty_master()) < 0)
		err_sys("can't open master pty");
	if (tty_getmode(0) < 0)
		err_sys("can't get tty mode of standard input");

	if ( (childpid = fork()) < 0)
		err_sys("can't fork");
	else if (childpid == 0) {	/* child process */
		if ( (slave_fd = pty_slave(master_fd)) < 0)
			err_sys("can't open pty slave");
		close(master_fd);
		if (tty_setmode(slave_fd) < 0)
			err_sys("can't set tty mode of pty slave");

		exec_shell(slave_fd, argv, envp);
			/* NOTREACHED */
	}

	if (tty_raw(0) < 0)			/* set stdin tty to raw mode */
		err_sys("tty_raw error");

	pass_all(master_fd, childpid);

	if (tty_reset(0) < 0)			/* reset stdin mode */
		err_sys("tty_reset error");
	exit(0);
}
