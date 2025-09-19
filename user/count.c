#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"


void
count_in_file(char *word, char *path, int fd)
{
        char buf[512];
        int n;
        int word_count = 0;
        int len = strlen(word);

        while((n = read(fd, buf, sizeof(buf))) > 0) {
                for (int i = 0; i <= n - len; i++) {
                        if(strcmp(&buf[i], word) == 0) {
                                word_count++;
                        }
                }
        }
        printf("%s : %d\n", path, word_count);
}


void
count(char *word, char *path)
{
        char buf[512], *p;
        int fd;
        struct dirent de;
        struct stat st;

        // check if directory/file exists
        if((fd = open(path, O_RDONLY)) < 0){
                fprintf(2, "count: cannot open %s\n", path);
                return;
        }
        // stat the path
        if(fstat(fd, &st) < 0){
                fprintf(2, "count: cannot stat %s\n", path);
                close(fd);
                return;
        }

        // check if path contains file or directory
        switch(st.type){
        case T_DEVICE:
        case T_FILE:
                count_in_file(word, path, fd);
                break;
        case T_DIR:
                // constructing the path
                strcpy(buf, path);
                p = buf+strlen(buf);
                *p++ = '/';
                while(read(fd, &de, sizeof(de)) == sizeof(de)){
                        // skips empty slots
                        if(de.inum == 0)
                                continue;
                        memmove(p, de.name, DIRSIZ);
                        p[DIRSIZ] = 0;
                        // call count on the new path
                        count(word, buf);
                }
                break;
        }
        // close path
        close(fd);
}

// argc - argument count
// argv[1] - first command-line argument
int
main(int argc, char *argv[])
{
        char* word;

        if(argc <= 1){
                fprintf(2, "usage: count word [file/directory]\n");
                exit(1);
        }

        word = argv[1];

        // for one file/directory
        if(argc <= 2){
                count(word, argv[2]);
                exit(0);
        }

        // for multiple files/directory
        for(int i = 2; i < argc; i++){
                count(word, argv[i]);
        }
        exit(0);
}
