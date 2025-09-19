
#include <stdio.h>
#include <string.h>
#define MAX_LANGS 8
struct Language {
    char name[32];
    int enabled;
};
struct Language lang_db[MAX_LANGS] = {
    {"C", 1}, {"C++", 1}, {"Rust", 0}, {"Go", 0}, {"Python", 0}, {"Java", 0}, {"Assembly", 1}, {"Swift", 0}
};

void compiler_init() {
    printf("Compiler subsystem initialized\n");
}

int compiler_enable_lang(const char* lang) {
    for (int i = 0; i < MAX_LANGS; i++) {
        if (strcmp(lang_db[i].name, lang) == 0) {
            lang_db[i].enabled = 1;
            printf("Language enabled: %s\n", lang);
            return 0;
        }
    }
    printf("Language not found: %s\n", lang);
    return -1;
}

int compiler_compile(const char* src, const char* lang, int optimize) {
    for (int i = 0; i < MAX_LANGS; i++) {
        if (strcmp(lang_db[i].name, lang) == 0 && lang_db[i].enabled) {
            printf("Compiling %s with %s (optimize=%d)\n", src, lang, optimize);
            // Simulate error reporting
            if (strstr(src, "error")) {
                printf("Error: syntax error in %s\n", src);
                return -1;
            }
            printf("Compilation successful: %s\n", src);
            return 0;
        }
    }
    printf("Language not enabled: %s\n", lang);
    return -1;
}

void compiler_list_langs() {
    printf("Supported languages:\n");
    for (int i = 0; i < MAX_LANGS; i++) {
        printf("%s (enabled=%d)\n", lang_db[i].name, lang_db[i].enabled);
    }
}
