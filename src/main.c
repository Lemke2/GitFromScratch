#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/stat.h>
#include <sys/types.h>


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
#define S(value) string_from_cstring(value)

typedef struct String{
    int size;
    char* value;
} String;

String string_from_cstring(const char* cstr){
    String s;
    s.size = strlen(cstr);
    s.value = malloc(s.size * sizeof(char));
    memcpy(s.value, cstr, s.size);

    return s;
}

b32 streq(String string1, String string2){

    if(string1.size != string2.size){
        return false;
    }
    
    for(int i = 0; i < string1.size; i++){
        if(string1.value[i] != string2.value[i]){
           return false;
       }
    }

    return true;
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

int main(int argc, char *argv[]){
    if(argc == 1){
        printf("Expected argument like: 'help', 'init', 'add'...\n");
        return 0;
    }

    String command = S(argv[1]);

    if(streq(command, S("init")) == 1){
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
        create_directory(".gitFromAlibaba/refs");
        printf("Initialized empty gitFromAlibaba repository\n");
    }

    return 0;
}