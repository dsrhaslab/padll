# Current Limitations in the System

### Relative *vs.* Absolute Paths
For path-based operations, such as `open`, `rename`, `getxattr`, ..., the system only considers the absolute path.
This is because PADLL needs to check if the path belongs to the targeted mountpoint.
For example, if the path is `/mnt/data/file.txt` and the target mountpoint is `/mnt`, it will be considered as a valid path;
however, if the path is `file.txt`, it will be considered as an invalid path.

This has repercussions not only for the path-based operations, but also for those that are dependent on the returned file descriptor.


### Close for unregisted file descriptors
One of the microbenchmarks is to replay the trace of the `close` system call in the ABCI supercomputer.
However, if the trace replayer only submits "dumb" `close`operations (*i.e.,* random file descriptors), the operation will not be successfully enforced, since at some point in the internal operation flow, it will generate an invalid `workflow identifier` (`static_cast<uint32_t> (-1)`).


### File descriptor based operations when `open` is not handled
If the `open` system call is not handled in the system (`libc_calls::open = false`), then all file descriptor based operations such as `read`, `write`, `close`, ... will not be enforced, since the `MountPointEntry` is not registered.

