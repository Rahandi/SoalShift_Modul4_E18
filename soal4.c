#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <sys/statfs.h>

static const char *dirpath = "/home/randi/Downloads";

static int E18_getattr(const char *path, struct stat *stbuf){
    int result;
    char fullpath[100];
    sprintf(fullpath, "%s%s", dirpath, path);
    result = lstat(fullpath, stbuf);
    if(result == -1){
        return -errno;
    }
    return 0;
}

static int E18_getdir(const char *path, fuse_dirh_t h, fuse_dirfil_t filler){
    DIR *dp;
    struct dirent *de;
    int result = 0;
    char fullpath[100];
    sprintf(fullpath, "%s%s", dirpath, path);
    dp = opendir(fullpath);
    if(dp == NULL){
        return -errno;
    }
    while((de=readdir(dp)) != NULL){
        result = filler(h, de->d_name, de->d_type);
        if(result != 0){
            break;
        }
    }
    closedir(dp);
    return result;
}

static int E18_mknod(const char *path, mode_t mode, dev_t rdev){
    int result;
    char fullpath[100];
    sprintf(fullpath, "%s%s", dirpath, path);
    result = mknod(fullpath, mode, rdev);
    if(result == -1){
        return -errno;
    }
    return 0;
}

static int E18_chmod(const char *path, mode_t mode){
    int result;
    char fullpath[100];
    sprintf(fullpath, "%s%s", dirpath, path);
    result = chmod(fullpath, mode);
    if(result == -1){
        return -errno;
    }
    return 0;
}

static int E18_open(const char *path, int flags){
    int result;
    char fullpath[100];
    sprintf(fullpath, "%s%s", dirpath, path);
    result = open(fullpath, flags);
    if(result == -1){
        return -errno;
    }
    close(result);
    return 0;
}

static int E18_read(const char *path, char *buf, size_t size, off_t offset)
{
    int fd;
    int result;
    char fullpath[100];
    sprintf(fullpath, "%s%s", dirpath, path);
    fd = open(fullpath, O_RDONLY);
    if(fd == -1){
        return -errno;
    }
    result = pread(fd, buf, size, offset);
    if(result == -1){
        result = -errno;
    }
    close(fd);
    return result;
}

static int E18_write(const char *path, const char *buf, size_t size, off_t offset)
{
    int fd;
    int result;
    int result1;
    char fullpath[100];
    sprintf(fullpath, "%s%s.copy", dirpath, path);
    fd = open(fullpath, O_WRONLY);
    if(fd == -1){
        return -errno;
    }
    result = pwrite(fd, buf, size, offset);
    if(result == -1)
        result = -errno;
    result1 = chmod(fullpath, 0000);
    if(result1 == -1)
        result1 = -errno;
    close(fd);
    return result;
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

int main(int argc, char *argv[])
{
    fuse_main(argc, argv, &E18_oper);
    return 0;
}