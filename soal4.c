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

static int xmp_getattr(const char *path, struct stat *stbuf)
{
  int res;
    char fpath[1000];
    sprintf(fpath,"%s%s",dirpath,path);
    res = lstat(fpath, stbuf);

    if (res == -1)
        return -errno;

    return 0;
}

static int xmp_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi)
{
  char fpath[1000];
    if(strcmp(path,"/") == 0)
    {
        path=dirpath;
        sprintf(fpath,"%s",path);
    }
    else sprintf(fpath, "%s%s",dirpath,path);
    int res = 0;

    DIR *dp;
    struct dirent *de;

    (void) offset;
    (void) fi;

    dp = opendir(fpath);
    if (dp == NULL)
        return -errno;

    while ((de = readdir(dp)) != NULL) {
        struct stat st;
        memset(&st, 0, sizeof(st));
        st.st_ino = de->d_ino;
        st.st_mode = de->d_type << 12;
        res = (filler(buf, de->d_name, &st, 0));
            if(res!=0) break;
    }

    closedir(dp);
    return 0;
}

static int xmp_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi)
{
  char fpath[1000];
    if(strcmp(path,"/") == 0)
    {
        path=dirpath;
        sprintf(fpath,"%s",path);
    }
    else sprintf(fpath, "%s%s",dirpath,path);
    int res = 0;
  int fd = 0 ;

    (void) fi;
    fd = open(fpath, O_RDONLY);
    if (fd == -1)
        return -errno;

    res = pread(fd, buf, size, offset);
    if (res == -1)
        res = -errno;

    close(fd);
    return res;
}

static int xmp_mkdir(const char *path, mode_t mode)
{
    int result;
    char fpath[100];
    sprintf(fpath, "%s%s", dirpath, path);
    result = mkdir(fpath, mode);
    if(result == -1)
    {
        return -errno;
    }
    return 0;
}

static int xmp_write(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi)
{
    int fd;
    int res;
    char fpath[100], sesuatu[100], touch[100];
    const char *namafolder = "/simpanan";

    sprintf(sesuatu, "%s%s", dirpath, namafolder);
    mkdir(sesuatu, 0644);
    sprintf(touch, "touch %s%s.copy", sesuatu, path);
    system(touch);

    sprintf(fpath, "%s%s.copy", sesuatu, path);
    
    fd = open(fpath, O_WRONLY);
    if(fd == -1)
    {
        return -errno;
    }

    res = pwrite(fd, buf, size, offset);
    if(res == -1)
        res = -errno;

    close(fd);
    return res;
}

static int xmp_truncate(const char *path, off_t size)
{
    int res;
    char fpath[100];

    sprintf(fpath, "%s%s", dirpath, path);

    res = truncate(fpath, size);
    if(res == -1)
    {
        return -errno;
    }

    return 0;
}

static int xmp_open(const char *path, struct fuse_file_info *fi)
{
    int res;
    char fpath[100];

    sprintf(fpath, "%s%s", dirpath, path);

    res = open(path, fi->flags);
    if (res == -1)
        return -errno;

    close(res);
    return 0;
}

static struct fuse_operations xmp_oper = {
    .getattr    = xmp_getattr,
    .readdir    = xmp_readdir,
    .read       = xmp_read,
    .write      = xmp_write,
    .mkdir      = xmp_mkdir,
    .truncate   = xmp_truncate,
    .open       = xmp_open
};

int main(int argc, char *argv[])
{
    umask(0);
    return fuse_main(argc, argv, &xmp_oper, NULL);
}