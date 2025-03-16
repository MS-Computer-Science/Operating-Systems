#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <dirent.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <string.h>
#include <unistd.h>

// Function to get file permissions in "rwxr-xr-x" format
void get_permissions(mode_t mode, char *perm_str) {
    const char *chars = "rwxrwxrwx";
    for (int i = 0; i < 9; i++) {
        perm_str[i] = (mode & (1 << (8 - i))) ? chars[i] : '-';
    }
    perm_str[9] = '\0';
}

// Function to determine file type (d, -, l)
char get_file_type(mode_t mode) {
    if (S_ISDIR(mode)) return 'd';  // Directory
    if (S_ISLNK(mode)) return 'l';  // Symlink
    return '-';                     // Regular file
}

int main() {
    struct dirent *entry;
    struct stat file_stat;
    char permissions[10];
    char mod_time[20];

    // Open current directory
    DIR *dir = opendir(".");
    if (!dir) {
        perror("opendir");
        return EXIT_FAILURE;
    }

    // Read directory entries (including hidden files)
    while ((entry = readdir(dir)) != NULL) {
        if (stat(entry->d_name, &file_stat) == -1) {
            perror("stat");
            continue;
        }

        // Get file type and permissions
        get_permissions(file_stat.st_mode, permissions);
        char file_type = get_file_type(file_stat.st_mode);

        // Get owner and group names
        struct passwd *pw = getpwuid(file_stat.st_uid);
        struct group *gr = getgrgid(file_stat.st_gid);

        // Get modification time
        struct tm *time_info = localtime(&file_stat.st_mtime);
        strftime(mod_time, sizeof(mod_time), "%b %d %H:%M", time_info);

        // Print file details
        printf("%c%s %3lu %-8s %-8s %8ld %s %s\n",
            file_type,
            permissions,
            file_stat.st_nlink,
            pw ? pw->pw_name : "unknown",
            gr ? gr->gr_name : "unknown",
            file_stat.st_size,
            mod_time,
            entry->d_name);
    }

    closedir(dir);
    return 0;
}
