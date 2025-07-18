/* user/find.c */
#include "types.h"
#include "stat.h"
#include "fcntl.h"
#include "user.h"
#include "fs.h"

void find(char *path, char *target)
{
  char buf[512], *p;
  int fd;
  struct dirent de;
  struct stat st;

  if((fd = open(path, 0)) < 0){
    printf(2, "find: cannot open %s\n", path);
    return;
  }
  if(fstat(fd, &st) < 0){
    printf(2, "find: cannot stat %s\n", path);
    close(fd);
    return;
  }
  if(st.type != T_DIR){
    close(fd);
    return;
  }

  if(strlen(path) + 1 + DIRSIZ + 1 > sizeof(buf)){
    printf(2, "find: path too long\n");
    close(fd);
    return;
  }

  strcpy(buf, path);
  p = buf + strlen(buf);
  *p++ = '/';

  while(read(fd, &de, sizeof(de)) == sizeof(de)){
    if(de.inum == 0) continue;

    memmove(p, de.name, DIRSIZ);
    p[DIRSIZ] = 0;

    if(stat(buf, &st) < 0){
      printf(2, "find: cannot stat %s\n", buf);
      continue;
    }

    if(st.type == T_DIR){
      if(strcmp(de.name, ".") && strcmp(de.name, ".."))
        find(buf, target);
    } else if(st.type == T_FILE){
      if(strcmp(de.name, target) == 0)
        printf(1, "%s\n", buf);     // 1 = stdout
    }
  }
  close(fd);
}

int
main(int argc, char *argv[])
{
  if(argc != 3){
    printf(2, "usage: find path filename\n");
    exit();
  }
  find(argv[1], argv[2]);
  exit();
}
