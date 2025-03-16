#include <stdio.h>
#include <sys/stat.h>   // File information (struct stat)
#include <dirent.h>     // Directory handling (opendir, readdir, closedir)
#include <pwd.h>        // User ID to username conversion (getpwuid)
#include <grp.h>        // Group ID to group name conversion (getgrgid)
#include <time.h>       // Formatting time (strftime, localtime)

/* this will store file permission bits and print charachters accordingly*/
void print_permissions(mode_t mode) {    
    char perms[] = "rwxrwxrwx";   //permission characher array
    for (int i = 0; i < 9; i++)
        printf("%c", (mode & (1 << (8 - i))) ? perms[i] : '-'); //checks if the permission bit is set
}

int main() {
    struct dirent *entry;   // Pointer to directory entry
    struct stat file_stat;  // Structure to store file metadata
    DIR *dir = opendir("."); //open the currect directory

    if (!dir) {
        perror("opendir");
        return 1;
    }

    while ((entry = readdir(dir)) != NULL) {
        if (stat(entry->d_name, &file_stat) == -1) {
            perror("stat");  // Print error if stat fails
            continue;
        }

        printf("%c", S_ISDIR(file_stat.st_mode) ? 'd' : '-'); // File type
        print_permissions(file_stat.st_mode);                 // Permissions
        printf(" %2lu %s %s %6ld ", file_stat.st_nlink,
               getpwuid(file_stat.st_uid)->pw_name, //Converts user ID to username.
               getgrgid(file_stat.st_gid)->gr_name,  //Converts group ID to group name.
               file_stat.st_size);

        char time_str[16];
        strftime(time_str, sizeof(time_str), "%b %d %H:%M", localtime(&file_stat.st_mtime));
        printf("%s %s\n", time_str, entry->d_name);
    }

    closedir(dir);
    return 0;
}
