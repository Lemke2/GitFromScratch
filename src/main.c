#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "utils.c"

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t   i8;
typedef int16_t  i16;
typedef int32_t  i32;
typedef int64_t  i64;

typedef float    f32;
typedef double   f64;

typedef int32_t b32;

#define true 1
#define false 0



b32 streq(const char* s1, const char* s2){
    return strcmp(s1, s2) == 0;
}

b32 directory_exists(const char* path){
    struct stat st;
    
    if(stat(path, &st) != 0){
        return false;
    }
    
    return S_ISDIR(st.st_mode);
}

b32 create_directory(const char* path){
    return mkdir(path, 0755) == 0;
}

b32 file_exists(const char* path){
    struct stat st;
    return stat(path, &st) == 0 && S_ISREG(st.st_mode);
}

void hash_file(const char* filepath, char* hash_out){
    FILE* f = fopen(filepath, "rb");
    if(!f){
        printf("Failed to open file: %s\n", filepath);
        return;
    }

    fseek(f, 0, SEEK_END);
    u32 fsize = ftell(f);
    fseek(f, 0, SEEK_SET);

    unsigned char* content = malloc(fsize);
    fread(content, 1, fsize, f);
    fclose(f);
    
    // 2. Compute SHA-1
    unsigned char hash[20];  // 20 bytes
    SHA1((char*)hash, (char*)content, fsize);
    
    // 3. Convert to hex string
    for(int i = 0; i < 20; i++){
        sprintf(hash_out + (i * 2), "%02x", hash[i]);
    }
    hash_out[40] = '\0';  // Null terminate
    
    free(content);
}

void store_blob(const char* filepath, const char* hash){
    FILE* f = fopen(filepath, "rb");
    if(!f){
        printf("Failed to open file: %s\n", filepath);
        return;
    }

    char shaPath[256];
    sprintf(shaPath, ".gitFromAlibaba/objects/blobs/%s", hash);


    fseek(f, 0, SEEK_END);
    u32 fsize = ftell(f);
    fseek(f, 0, SEEK_SET);

    unsigned char* content = malloc(fsize);
    fread(content, 1, fsize, f);
    fclose(f);

    FILE* fout = fopen(shaPath, "wb");
    if(!fout){
        printf("Failed to open file: %s\n", filepath);
        return;
    }

    fwrite(content, 1, fsize, fout);
    fclose(fout);
}

void add_to_index(const char* filepath, const char* hash){
    FILE* f = fopen(".gitFromAlibaba/index", "a");
    fprintf(f, "%s %s\n", filepath, hash);
    fclose(f);
}

int main(int argc, char *argv[]){
    if(argc == 1){
        printf("Expected argument like: 'help', 'init', 'add'...\n");
        return 0;
    }

    char* command = argv[1];

    if(streq(command, "init") == 1){
        if(argc > 2){
            printf("Too many args for 'init' command.\n");
            return 1;
        }

        if(directory_exists(".gitFromAlibaba")){
            printf("Repository already exists.\n");
            return 1;
        }

        create_directory(".gitFromAlibaba");
        create_directory(".gitFromAlibaba/objects");
        create_directory(".gitFromAlibaba/objects/blobs");
        create_directory(".gitFromAlibaba/refs");
        printf("Initialized empty gitFromAlibaba repository\n");
    }

    if(streq(command, "add")){
        if(argc < 3){
            printf("Usage: git add <file>\n");
            return 1;
        }

        char* filepath = argv[2];
    
        if(!file_exists(filepath)){
            printf("File not found: %s\n", filepath);
            return 1;
        }
        char hash[41];
        hash_file(filepath, hash);
        printf("THE HASHED FILE IS: %s", hash);
        store_blob(filepath, hash);
        add_to_index(filepath, hash);
        printf("Added %s\n", filepath);
    }

    return 0;
}