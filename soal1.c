#define FUSE_USE_VERSION 28
#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <sys/time.h>
#include <stdlib.h>

static const char *dirpath = "/home/ghifarozarrr/Documents";

static int xmp_getattr(const char *path, struct stat *stbuf)
{
  int res;
	char fpath[1000];
	sprintf(fpath,"%s%s",dirpath,path);
	res = lstat(fpath, stbuf);

	if (res == -1) return -errno;

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
	if (dp == NULL) return -errno;

	while ((de = readdir(dp)) != NULL) 
	{
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
	int res = 0, fd = 0, i;
	sprintf(fpath, "%s%s",dirpath,path);
	
	(void) fi;

	char ext[1000];
	for(i=0; i<strlen(fpath) && fpath[i]!='.'; i++);
	strcpy(ext, fpath+i);
	fd = open(fpath, O_RDONLY);
	char pindah[1000], asal[1000], tujuan[1000]; 
	if (fd == -1) return -errno;
	else
	{
		if(strcmp(ext, ".pdf") == 0 || strcmp(ext, ".doc") == 0 || strcmp(ext, ".txt") == 0|| strcmp(ext, ".ditandai") == 0)
		{	
			system("zenity --width 400 --error --title 'Error' --text 'Terjadi Kesalahan! File berisi konten berbahaya.'");
			sprintf(asal, "%s", fpath);
                        sprintf(tujuan, "%s.ditandai", fpath);
			sprintf(pindah, "mv %s %s", asal, tujuan); //biar gampang ntar no 2nya
			system(pindah);
			return -errno;
                }
		res = pread(fd, buf, size, offset);
		if (res == -1) res = -errno;

		close(fd);
	}
	return res;
}


static struct fuse_operations xmp_oper = 
{
	.getattr	= xmp_getattr,
	.readdir	= xmp_readdir,
	.read		= xmp_read,
};

int main(int argc, char *argv[])
{
	umask(0);
	return fuse_main(argc, argv, &xmp_oper, NULL);
}
