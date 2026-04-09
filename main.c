#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <windows.h>
#include <sys/stat.h>
#include <direct.h>
#define MAX_LINE_SIZE 256
#define GENERIC_MAX 512

#define AC_BLACK "\x1b[30m"
#define AC_RED "\x1b[31m"
#define AC_GREEN "\x1b[32m"
#define AC_YELLOW "\x1b[33m"
#define AC_BLUE "\x1b[34m"
#define AC_MAGENTA "\x1b[35m"
#define AC_CYAN "\x1b[36m"
#define AC_WHITE "\x1b[37m"
#define AC_NORMAL "\x1b[m"

void runCmd();
void execCmd(const char* cmd);
void ls_cmd(int argLength, char cmd[][MAX_LINE_SIZE]);
void cd_cmd(int argLength, char cmd[][MAX_LINE_SIZE]);
int getArgs(char args[][MAX_LINE_SIZE], const char* cmd);
int splitStr(char splitter, char strs[30][MAX_LINE_SIZE], const char* str);
void touch_cmd(int argLength, char cmd[][MAX_LINE_SIZE]);
void mkdir_cmd(int argLength, char cmd[][MAX_LINE_SIZE]);
void rm_cmd(int argLength, char cmd[][MAX_LINE_SIZE]);
void echo_cmd(int argLength, char cmd[][MAX_LINE_SIZE]);
void cat_cmd(int argLength, char cmd[][MAX_LINE_SIZE]);
void mv_cmd(int argLength, char cmd[][MAX_LINE_SIZE]);
void cp_cmd(int argLength, char cmd[][MAX_LINE_SIZE]);
int isValidPath(const char* path);
void clearInput();

char originalPath[GENERIC_MAX];
char currentPath[GENERIC_MAX];

int main() {
    printf("%s---STARTING BASH---%s\n", AC_CYAN, AC_NORMAL);

    if (getcwd(currentPath, sizeof(currentPath)) == NULL) {
        printf("ERROR: Failed to get current file path\n");
        exit(1);
    }

    strcpy(originalPath, currentPath);

    runCmd();
    return 0;
}

void runCmd() {

    printf("%sBASH: ", AC_CYAN);
    printf("%s<%s%s%s> ", AC_RED, AC_GREEN, currentPath, AC_RED);
    printf("%s>>%s ", AC_YELLOW, AC_NORMAL);
    fflush(stdout);
    char currentCmd[MAX_LINE_SIZE];

    fgets(currentCmd, MAX_LINE_SIZE, stdin);
    currentCmd[strcspn(currentCmd, "\n")] = '\0';

    execCmd(currentCmd);
    runCmd();
}

void execCmd(const char* cmd) {
    char args[10][MAX_LINE_SIZE];
    int argCount = getArgs(args, cmd);

    if (strcmp(args[0], "ls") == 0) {
        ls_cmd(argCount, args);
    }
    else if (strcmp(args[0], "exit") == 0) {
        printf("%s---EXITING BASH---%s\n", AC_CYAN, AC_NORMAL);
        exit(0);
    }
    else if (strcmp(args[0], "pwd") == 0) {
        printf("%sPath: %s%s%s\n", AC_CYAN, AC_YELLOW, currentPath, AC_NORMAL);
    }
    else if (strcmp(args[0], "cd") == 0) {
        cd_cmd(argCount, args);
    }
    else if (strcmp(args[0], "touch") == 0) {
        touch_cmd(argCount, args);
    }
    else if (strcmp(args[0], "mkdir") == 0) {
        mkdir_cmd(argCount, args);
    }
    else if (strcmp(args[0], "help") == 0) {
        printf("%sCommands: ls, exit, pwd, cd, touch, mkdir, rm, echo, cat, mv, cp%s\n", AC_YELLOW, AC_NORMAL);
    }
    else if (strcmp(args[0], "rm") == 0) {
        rm_cmd(argCount, args);
    }
    else if (strcmp(args[0], "echo") == 0) {
        echo_cmd(argCount, args);
    }
    else if (strcmp(args[0], "cat") == 0) {
        cat_cmd(argCount, args);
    }
    else if (strcmp(args[0], "mv") == 0) {
        mv_cmd(argCount, args);
    }
    else if(strcmp(args[0], "cp") == 0){
        cp_cmd(argCount, args);
    }
    else {
        printf("%sERROR: No such command!%s\n", AC_RED, AC_NORMAL);
    }

    runCmd();
}

void cp_cmd(int argLength, char cmd[][MAX_LINE_SIZE]) {
    if (argLength < 2) {
        printf("%sERROR: Invalid Arguments. cp -help%s\n", AC_RED, AC_NORMAL);
        return;
    }

    if (strcmp(cmd[1], "-help") == 0) {
        printf("%s--CP HELP MENU--%s\n", AC_CYAN, AC_NORMAL);
        printf("%s1. cp -P <path_src> <path_dest> -> copies the file to another dir%s\n", AC_YELLOW, AC_NORMAL);
        printf("%s2. cp <file> <new file> -> copies the file into the same dir%s\n", AC_YELLOW, AC_NORMAL);
        return;
    }

    if (cmd[1][0] == '-') {
        if (argLength < 4) {
            printf("%sERROR: Specify both paths!%s\n", AC_RED, AC_NORMAL);
            return;
        }

        if (CopyFile(cmd[2], cmd[3], FALSE) != 0) {
            printf("%sERROR: Couldn't Copy file!%s\n", AC_RED, AC_NORMAL);
        }

        return;
    }

    char tempPathSrc[MAX_LINE_SIZE];
    strcpy(tempPathSrc, currentPath);
    strcat(tempPathSrc, "\\");
    strcat(tempPathSrc, cmd[1]);


    char tempPathDest[MAX_LINE_SIZE];
    strcpy(tempPathDest, currentPath);
    strcat(tempPathDest, "\\");
    strcat(tempPathDest, cmd[2]);

    CopyFile(tempPathSrc, tempPathDest, FALSE);  
    return;
}

void mv_cmd(int argLength, char cmd[][MAX_LINE_SIZE]) {
    if (argLength < 2) {
        printf("%sERROR: Invalid Arguments. mv -help%s\n", AC_RED, AC_NORMAL);
        return;
    }

    if (strcmp(cmd[1], "-help") == 0) {
        printf("%s--MV HELP MENU--%s\n", AC_CYAN, AC_NORMAL);
        printf("%s1. mv <path_src> <path_dest> -> moves a file/dir to another place%s\n", AC_YELLOW, AC_NORMAL);
        printf("%s2. mv -r <old name> <new name> -> renames the file%s\n", AC_YELLOW, AC_NORMAL);
        return;
    }

    if (cmd[1][0] == '-') {
        if (argLength < 4) {
            printf("%sERROR: Specify old and new name!%s\n", AC_RED, AC_NORMAL);
            return;
        }

        char tempPathSrc[MAX_LINE_SIZE];
        strcpy(tempPathSrc, currentPath);
        strcat(tempPathSrc, "\\");
        strcat(tempPathSrc, cmd[2]);


        char tempPathDest[MAX_LINE_SIZE];
        strcpy(tempPathDest, currentPath);
        strcat(tempPathDest, "\\");
        strcat(tempPathDest, cmd[3]);

        if (rename(tempPathSrc, tempPathDest) != 0) {
            printf("%sERROR: Couldn't rename file!%s\n", AC_RED, AC_NORMAL);
        }

        return;
    }

    if (CopyFile(cmd[1], cmd[2], FALSE)) {
        remove(cmd[1]);
    }
    else {
        printf("%sERROR: Couldn't Copy file!%s\n", AC_RED, AC_NORMAL);
        return;
    }

}

void cat_cmd(int argLength, char cmd[][MAX_LINE_SIZE]) {
    if (argLength < 2) {
        printf("%sERROR: Not enough arguments. specify a file!%s\n", AC_RED, AC_NORMAL);
        return;
    }

    FILE* fptr;
    char tempPath[MAX_LINE_SIZE];
    strcpy(tempPath, currentPath);
    strcat(tempPath, "\\");
    strcat(tempPath, cmd[1]);

    fptr = fopen(tempPath, "r");

    char readFile[GENERIC_MAX];
    fread(readFile, sizeof(char), GENERIC_MAX, fptr);

    printf("%sDATA READ:%s", AC_YELLOW, AC_NORMAL);
    printf("\n%s", readFile);
    return;
}

void echo_cmd(int argLength, char cmd[][MAX_LINE_SIZE]) {
    if (argLength < 2) {
        printf("%sERROR: Invalid argument count. Do echo -help%s\n", AC_RED, AC_NORMAL);
        return;
    }

    if (argLength == 2) {
        if (strcmp(cmd[1], "-help") == 0) {
            printf("%s--ECHO HELP MENU--%s\n", AC_CYAN, AC_NORMAL);
            printf("%s1. echo <text> -> writes into console%s\n", AC_YELLOW, AC_NORMAL);
            printf("%s2. echo <text> > <file> -> overrites file with the text%s\n", AC_YELLOW, AC_NORMAL);
            printf("%s3. echo <text> >> <file> -> appends file text%s\n", AC_YELLOW, AC_NORMAL);
            return;
        }

        printf("%sECHO: %s%s\n", AC_CYAN, AC_NORMAL, cmd[1]);
        return;
    }

    if (argLength > 2) {
        if (cmd[1][0] == '"') {
            char textToPrint[MAX_LINE_SIZE];
            int lastArg = 1;
            for (int i = 1; i < argLength; i++) {
                strcat(textToPrint, " ");
                strcat(textToPrint, cmd[i]);
                if (cmd[i][strlen(cmd[i]) - 1] == '"') {
                    lastArg = i;
                    break;
                }
            }
            textToPrint[strcspn(textToPrint, "\"")] = ' ';
            textToPrint[strlen(textToPrint) - 1] = ' ';
            //textToPrint[strcspn(textToPrint, "\n")] = '\0';

            if (argLength - 1 == lastArg) {
                printf("%sECHO:%s%s\n", AC_CYAN, AC_NORMAL, textToPrint);
                return;
            }

            if (cmd[lastArg + 1][0] == '>') {
                if (lastArg + 2 >= argLength) {
                    printf("%sERROR: Specify a file to write to!%s\n", AC_RED, AC_NORMAL);
                    return;
                }

                char writeStyle[3] = "wb";
                if (cmd[lastArg + 1][1] == '>') strcpy(writeStyle, "ab");

                const char* file = cmd[lastArg + 2];

                FILE* fptr;

                char tempPath[MAX_LINE_SIZE];
                strcpy(tempPath, currentPath);
                strcat(tempPath, "\\");
                strcat(tempPath, file);
                fptr = fopen(tempPath, writeStyle);

                fwrite(textToPrint, sizeof(char), strlen(textToPrint), fptr);
                fwrite("\n", sizeof(char), 1, fptr);
                //fwrite(textToPrint, sizeof(char), strlen(textToPrint), fptr);
                fclose(fptr);
                return;

            }
            else {
                printf("%sERROR: Incorrect Syntax!%s\n", AC_RED, AC_NORMAL);
                return;
            }

            return;
        }


        return;
    }
}

void rm_cmd(int argLength, char cmd[][MAX_LINE_SIZE]) {
    if (argLength < 2) {
        printf("%sERROR: Invalid argument count. (specify name)%s\n", AC_RED, AC_NORMAL);
        return;
    }

    if (cmd[1][0] == '-') {
        if (strcmp(cmd[1], "-help") == 0) {
            printf("%s---RM HELP MENU---%s\n", AC_CYAN, AC_NORMAL);
            printf("%s1. rm <file> - removes a regular file %s\n", AC_YELLOW, AC_NORMAL);
            printf("%s2. rm -d <file> - removes a directory%s\n", AC_YELLOW, AC_NORMAL);
            return;
        }


        if (argLength < 3) {
            printf("%sERROR: Specify a file!%s\n", AC_RED, AC_NORMAL);
            return;
        }

        struct stat s;
        char temp_path[MAX_LINE_SIZE];
        strcpy(temp_path, currentPath);
        strcat(temp_path, "\\");
        strcat(temp_path, cmd[2]);

        if (stat(temp_path, &s) != 0) {
            printf("%sERROR: File Doesn't Exist!%s\n", AC_RED, AC_NORMAL);
            return;
        }

        if (s.st_mode & S_IFREG) {
            printf("%sWARNING: Specified path is not a directory! Use rm <file> instead.%s\n", AC_YELLOW, AC_NORMAL);
            return;
        }

        _rmdir(temp_path);
        return;
    }

    struct stat s;
    char temp_path[MAX_LINE_SIZE];
    strcpy(temp_path, currentPath);
    strcat(temp_path, "\\");
    strcat(temp_path, cmd[1]);

    if (stat(temp_path, &s) != 0) {
        printf("%sERROR: File Doesn't Exist!%s\n", AC_RED, AC_NORMAL);
        return;
    }

    if (s.st_mode & S_IFDIR) {
        printf("%sWARNING: Use rm -d <file> to remove directories%s\n", AC_YELLOW, AC_NORMAL);
        return;
    }

    remove(temp_path);
    return;
}

void ls_cmd(int argLength, char cmd[][MAX_LINE_SIZE]) {
    // Generic
    DIR* dr;
    struct dirent* en;

    dr = opendir(currentPath);
    if (dr) {
        while ((en = readdir(dr)) != NULL) {
            if (en->d_name[0] != '.') {
                if (argLength > 1 && strcmp(cmd[1], "-l") == 0) {
                    printf("%sFile %s-> %s%s, %llu%s\n", AC_CYAN, AC_YELLOW, AC_MAGENTA, en->d_name, en->d_ino, AC_NORMAL);
                }
                else {
                    printf("%sFile %s-> %s%s%s\n", AC_CYAN, AC_YELLOW, AC_MAGENTA, en->d_name, AC_NORMAL);
                }
            }
        }
    }
    else {
        printf("%sERROR: Couldn't Open Directory\nPath: {%s}%s\n", AC_RED, currentPath, AC_NORMAL);
        return;
    }

    return;


}

void touch_cmd(int argLength, char cmd[][MAX_LINE_SIZE]) {
    if (argLength < 2) {
        printf("%sERROR: Not Enough Arguments. Specify a name!%s\n", AC_RED, AC_NORMAL);
        return;
    }

    struct stat s;
    char temp_path[MAX_LINE_SIZE];
    strcpy(temp_path, currentPath);
    strcat(temp_path, "\\");
    strcat(temp_path, cmd[1]);

    if (argLength < 3) {
        strcat(temp_path, ".txt");
    }
    else {
        strcat(temp_path, ".");
        strcat(temp_path, cmd[2]);
    }



    if (stat(temp_path, &s) == 0) {
        printf("%sERROR: File Already Exists!%s\n", AC_RED, AC_NORMAL);
        return;
    }

    FILE* fptr;
    fptr = fopen(temp_path, "w");

    fclose(fptr);
}

void mkdir_cmd(int argLength, char cmd[][MAX_LINE_SIZE]) {
    if (argLength < 2) {
        printf("%sERROR: Not Enough Arguments. Specify a name!%s\n", AC_RED, AC_NORMAL);
        return;
    }

    struct stat s;
    char temp_path[MAX_LINE_SIZE];
    strcpy(temp_path, currentPath);
    strcat(temp_path, "\\");
    strcat(temp_path, cmd[1]);

    if (stat(temp_path, &s) == 0) {
        printf("%sERROR: Directory Already Exists!%s\n", AC_RED, AC_NORMAL);
        return;
    }

    _mkdir(temp_path);
}

void cd_cmd(int argLength, char cmd[][MAX_LINE_SIZE]) {
    if (argLength == 1) {
        strcpy(currentPath, "C:\\Users\\User");
        return;
    }

    if (strcmp(cmd[1], "..") == 0) {
        char splitPath[30][MAX_LINE_SIZE];
        int count = splitStr('\\', splitPath, currentPath);

        if (count == 1) return;

        char temp_str[MAX_LINE_SIZE];
        int l = count - 1;
        for (int i = 0; i < l; i++) {
            strcat(temp_str, splitPath[i]);
            if (i != (l - 1)) {
                strcat(temp_str, "\\");
            }
        }
        strcpy(currentPath, temp_str);
        return;
    }

    if (cmd[1][0] != '-') {
        char temp_str[MAX_LINE_SIZE];
        strcat(temp_str, currentPath);
        strcat(temp_str, "\\");
        strcat(temp_str, cmd[1]);

        if (isValidPath(temp_str) == 0) {
            strcpy(currentPath, temp_str);
        }
        else {
            printf("%sERROR: No such directory exists%s\n", AC_RED, AC_NORMAL);
        }
        return;
    }

    if (cmd[1][0] == '-') {
        if (strcmp(cmd[1], "-help") == 0) {
            printf("%s--CD HELP MENU--%s\n", AC_CYAN, AC_NORMAL);
            printf("%s1. cd .. -> goes to C:\\Users\\User%s\n", AC_YELLOW, AC_NORMAL);
            printf("%s2. cd <str> -> advances into a directory%s\n", AC_YELLOW, AC_NORMAL);
            printf("%s3. cd -og -> goes into the original .exe path%s\n", AC_YELLOW, AC_NORMAL);
            printf("%s4. cd -abs <path> -> goes to the path specified%s\n", AC_YELLOW, AC_NORMAL);
            return;
        }

        if (strcmp(cmd[1], "-og") == 0) {
            strcpy(currentPath, originalPath);
            return;
        }

        if (strcmp(cmd[1], "-abs") == 0) {
            if (argLength < 3) {
                printf("%sERROR: Not Enough Arguments (cd -help)%s\n", AC_RED, AC_NORMAL);
                return;
            }

            if (isValidPath(cmd[2]) == 0) {
                strcpy(currentPath, cmd[2]);
                return;
            }
            else {
                printf("%sERROR: Invalid Path%s\n", AC_RED, AC_NORMAL);
                return;
            }
        }
    }
}

/**
 * UTILITIES
 */

int isValidPath(const char* path) {
    DIR* dr;
    struct dirent* en;

    dr = opendir(path);

    if (dr) {
        return 0;
    }
    else {
        return 1;
    }
}

int splitStr(char splitter, char strs[30][MAX_LINE_SIZE], const char* str) {
    int argIndex = 0;
    int strIndex = 0;
    int writingStrIndex = 0;
    while (str[strIndex] != '\n' && str[strIndex] != '\0') {
        if (str[strIndex] != splitter) {
            strs[argIndex][writingStrIndex] = str[strIndex];
            writingStrIndex++;
        }
        else {
            strs[argIndex][writingStrIndex] = '\0';
            argIndex++;
            writingStrIndex = 0;
        }
        strIndex++;
    }

    strs[argIndex][writingStrIndex] = '\0';
    return (argIndex + 1);
}

int getArgs(char args[10][MAX_LINE_SIZE], const char* cmd) {
    int argIndex = 0;
    int strIndex = 0;
    int writingStrIndex = 0;
    while (cmd[strIndex] != '\n' && cmd[strIndex] != '\0') {
        if (cmd[strIndex] != ' ') {
            args[argIndex][writingStrIndex] = cmd[strIndex];
            writingStrIndex++;
        }
        else {
            args[argIndex][writingStrIndex] = '\0';
            argIndex++;
            writingStrIndex = 0;
        }
        strIndex++;

    }

    args[argIndex][writingStrIndex] = '\0';
    return (argIndex + 1);
}

void clearInput() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF)
        ;
}