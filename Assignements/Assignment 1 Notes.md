
* Aim: `create a shell` interface that 
	* accepts user commands
	* executes each command in separate process
	> [[Difference between shell and kernel]]

* support `input & output redirection`
	> [[Input & Output Redirection in Command-Line]]

* supports `pipe as a form of IPC` between a pair of command
>	[[IPC (Inter-Process Communication)]]

```
This will involve following UNIX system calls
	1. fork()
	2. exec()
	3. wait()
	4. dup2()
	5. pipe()
```
				
[[Meaning of these system calls]]

* This assignment can be completed on any Linux, UNIX, macOS system.
