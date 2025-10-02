#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"

#define MAXVISITED 22


int visited_inodes[MAXVISITED];
int visited_count = 0;

int 
already_visited(int ino) 
{
  for(int i = 0; i < visited_count; i++) {
    if(visited_inodes[i] == ino)
      return 1;   // found
  }
  return 0;
}

void 
mark_visited(int ino) 
{
  if(visited_count < MAXVISITED)
    visited_inodes[visited_count++] = ino;
}

// KMP algorithim
char* 
find(const char *haystack, const char *needle) 
{
    int len = strlen(needle);
    // empty string
    if (len == 0) return (char*)haystack;

    for (int i = 0; haystack[i]; i++) {
        int j = 0;
        while (j < len && haystack[i+j] == needle[j]) {
            j++;
        }
        if (j == len) {
            return (char*)&haystack[i]; // found
        }
    }
    return 0; // not found
}


void
count_in_file(char *word, char *path, int fd)
{
        char buf[512];
        int n, m;
        int word_count = 0;
        int len = strlen(word);
	char *p, *q;
        
	m = 0;
	// printf("searching for: [%s]\n", word);
        while((n = read(fd, buf + m, sizeof(buf) - m -1)) > 0) {
		m += n;
		buf[m] = '\0';
		p = buf;
		// check each character till newline char
		while ((q = strchr(p, '\n')) != 0) {
			// changes newline to null terminator
			*q = 0;
			for (char *s = p; (s = find(s, word)) != 0; s++){
				word_count++;
				s += len - 1;
			} 

			// restore newline
			*q = '\n';
			// move to next line
			p = q + 1;
		}
		// handle leftover memory
       		if (m > 0) {
                	m -= p - buf;
                	memmove(buf, p, m);
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
		if (already_visited(st.ino)) {
    		// for cases as . or .. infinite looping can occur
		// check if dir was vister already or not
    			close(fd);
    			return;
  		}
 		mark_visited(st.ino);
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

			// check path to avoid console
			struct stat st_entry;
       	 		if (stat(buf, &st_entry) < 0)
            		continue;

        		if (st_entry.type == T_DEVICE)
            		continue;

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


