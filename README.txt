Author: Morgan McKinney 4/2020
Contents:
	shell.cc: Implementation of main and call_redirected().
	Makefile: Builds shell executable.
	README.txt: This file.
	shell.h: Definition of command structure, and prototypes for parse and free commands.
	parser.c: Implementation of parse_command and free_command.
	tests/: Subdirectory containing test inputs, expected outputs, and helper commands.
	run-tests: Sh script to run tests and compare shell output to expected output.
Running:
	May compile and build shell using "make shell" in command prompt.
Implementation Notes:
	Regarding my design decisions, I tried keeping all changes within shell.cc
	in order to keep things simple when debugging. The shell determines whether
	a command is built in or not using if statements matching a certain command.
Limitations:
	Provided parser supports only 30 arguments, thus longer command lines cannot
	be used. Also, I had failing test cases for setenv, setenv-unset, redir-out-then-in,
	and redir-both. They occur in my implementations of setenv and redir-in. I tried
	to solve them by discussing with peers, reading the textbook for clues, and checking
	the class notes. If I had more time, I'd likely go to office hours to fix those issues.
References:
	The implementation of cmd->in_redir benefitted from discussions with
	Nich Kiem and Brooke Kalous, who suggested basing it off redir_out but
	changing the flags. The reuse of:
	if (!from_script) { cerr << "shell> " << flush; }
	also benefitted from discussions with Nich Kiem who suggested putting it
	in the main while loop again to fix script command inputs.
	 
