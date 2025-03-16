### 1. `fork()`
`fork()` is used to create a new process by duplicating the current process. The new process is called the `child process`.

### 2. `exec()`
`exec()` is used to replace the current process image with a new process image. [[Process Image | Meaning of Process Image]]

### 3. `wait()`
`wait()` makes the parent process wait until all of its child processes have terminated.

### 4. `dup2()`
`dup2()` duplicates a file descriptor to a specified file descriptor number. [[Process Descriptors | Meaning of Process Descriptors]]

### 5. `pipe()`
`pipe()` creates a pair of file descriptors for inter-process communication (IPC).