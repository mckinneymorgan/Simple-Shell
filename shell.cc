// Original author: Morgan McKinney 4/2020

#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "shell.h"

using namespace std;
const int STDIN_FD = 0;
const int STDOUT_FD = 1;

// Code from lab 3, changed as needed
void call_redirected(const command *cmd) {
	pid_t pid = fork();

	if (pid < 0) {
		perror("could not create a child process");
		return;
	} else if (pid == 0) {
		if (cmd->in_redir) {
			int fd = open(cmd->in_redir, O_RDONLY | O_CREAT | O_TRUNC, 0666);

			if (fd < 0) {
				perror("could not open file for redirection");
				exit(1);
			}
			if (dup2(fd, STDIN_FD) < 0) {
				perror("could not duplicate file descriptor");
				exit(2);
			}
		}
		if (cmd->out_redir) {
			int fd = open(cmd->out_redir, O_WRONLY | O_CREAT | O_TRUNC, 0666);

			if (fd < 0) {
				perror("could not open file for redirection");
				exit(1);
			}
			if (dup2(fd, STDOUT_FD) < 0) {
				perror("could not duplicate file descriptor");
				exit(2);
			}
		}
		execvp(cmd->args[0], cmd->args);
		perror("could not execute process");
		exit(3);
	} else {
		int status = 0;
		if (wait(&status) < 0) {
			perror("could not wait for child");
			return;
		}
		if (WIFEXITED(status)) {
			if (WEXITSTATUS(status) != 0) {
				cerr << "Command exited with status " << WEXITSTATUS(status) << endl;
			}
		}
		if (WIFSIGNALED(status)) {
			if (WTERMSIG(status) != 0) {
				cerr << "Command exited due to signal " << WTERMSIG(status) << endl;
			}
		}
	}	
}

int main(int argc, char **argv) {
	ifstream scriptf;
	bool from_script = false;

	if (argc > 2) {
		cerr << "Usage: " << argv[0] << " [<scriptfile>]\n";
		exit(1);
	} else if (argc == 2) {
		from_script = true;
		scriptf.open(argv[1]);
		if (scriptf.bad()) {
			cerr << "Could not open input file " << argv[1] << ": "
			     << strerror(errno) << endl;
			exit(1);
		}
	}

	istream &inf = (argc == 2) ? scriptf : cin;

	string line;
	if (!from_script) {
		cerr << "shell> " << flush;
	}
	while (getline(inf, line)) {
		command *cmd = parse_command(line.c_str());
		
		if (cmd->args[0]) {
			if (string(cmd->args[0]) == "cd") {
				int ret;
				if (cmd->args[1] != nullptr) {
					// Change to specified working directory
					const char* directory = cmd->args[1];
					ret = chdir(directory);
					if (ret == -1) { // Unsuccessful completion
						cerr << "Could not chdir: " << strerror(errno) << endl;
					}
				}
				else {
					// Change to home directory
					const char* homeDirectory = getenv("HOME");
					ret = chdir(homeDirectory);
					if (homeDirectory == NULL) {
						cerr << "No home directory?" << strerror(errno) << endl;
					}
					else if (ret == -1) { // Unsuccessful completion
						cerr << "Could not chdir: " << strerror(errno) << endl;
					}
				}
			} else if (string(cmd->args[0]) == "setenv") {
				int ret;
				if (cmd->args[1] != nullptr) {
					if (cmd->args[2] != nullptr) {
						// Two arguments given
						ret = setenv(cmd->args[1], cmd->args[2], 1);
						if (ret == -1) {
							cerr << "Could not setenv: " << strerror(errno) << endl;
						}

					}
					else if {
						// One argument given, unset variable
						ret = unsetenv(cmd->args[1]);
						if (ret == -1) {
							cerr << "Could not unsetenv: " << strerror(errno) << endl;
						}
					}
				}
				else {
					// Report error, but do not call setenv
					cerr << "No arguments supplied" << endl;
				}
			} else if (string(cmd->args[0]) == "exit") {
				// Exit with success
				exit(0);
			} else {
				// External command
				call_redirected(cmd);
			}
		} // Else, empty command: do nothing

		free_command(cmd);
		if (!from_script) { 
			cerr << "shell> " << flush; 
		}
	}

	if (inf.bad()) {
		cerr << "Error reading input: " << strerror(errno) << endl;
		exit(1);
	}
	exit(0);
}
