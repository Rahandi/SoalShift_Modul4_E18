#define FUSE_USE_VERSION 28
#include <fuse.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <sys/time.h>

static const char *dirpath = "/home/randi/Downloads";

static int E18_getattr(const char *path, struct stat *stbuf){
    int result;
    char fullpath[100];
    sprintf(fullpath, "%s%s". dirpath, path);
    result = lstat(fullpath, stbuf);
    if(res == -1){
        return -errno;
    }
    return 0;
}

static struct fuse_operations E18_oper = {
    .getattr    =   E18_getattr,
    .getdir     =   E18_getdir,
    .mknod      =   E18_mknod,
    .chmod      =   E18_chmod,
    .open       =   E18_open,
    .read       =   E18_read,
    .write      =   E18_write
};