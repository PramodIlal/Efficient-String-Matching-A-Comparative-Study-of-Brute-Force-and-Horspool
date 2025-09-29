#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

#define ALPHABET_SIZE 256

// Function to read the content of a file into a string
char* read_file(const char* filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Unable to open file");
        exit(EXIT_FAILURE);
    }
    
    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    char *content = (char*)malloc(length + 1);
    if (content) {
        fread(content, 1, length, file);
        content[length] = '\0';
    }
    
    fclose(file);
    return content;
}

// Function to highlight the key string in the command prompt using colors
void highlight_text(const char *text, const int *match_positions, int num_matches, int pattern_length) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
    WORD saved_attributes;

    // Save current attributes
    GetConsoleScreenBufferInfo(hConsole, &consoleInfo);
    saved_attributes = consoleInfo.wAttributes;

    int text_length = strlen(text);
    int current_match = 0;

    for (int i = 0; i < text_length; i++) {
        if (current_match < num_matches && i == match_positions[current_match]) {
            // Change color and print the match
            SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_INTENSITY);
            for (int j = 0; j < pattern_length; j++) {
                putchar(text[i + j]);
            }
            SetConsoleTextAttribute(hConsole, saved_attributes);
            i += pattern_length - 1;
            current_match++;
        } else {
            putchar(text[i]);
        }
    }
    printf("\n");
}

// Brute Force string matching algorithm
int brute_force(const char *text, const char *pattern, int *match_positions) {
    int n = strlen(text);
    int m = strlen(pattern);
    int match_count = 0;
    
    for (int i = 0; i <= n - m; i++) {
        int j=0;
        while(j<m && text[i+j]==pattern[j])
            j=j+1;
        if (j == m) {
            match_positions[match_count++] = i;
        }
    }
    return match_count;
}

// Function to preprocess the shift table for Horspool algorithm
void preprocess_shift_table(const char *pattern, int shift_table[]) {
    int m = strlen(pattern);
    for (int i = 0; i < ALPHABET_SIZE; i++) {
        shift_table[i] = m;
    }
    for (int j = 0; j < m - 1; j++) {
        shift_table[(unsigned char)pattern[j]] = m - 1 - j;
    }
}

// Horspool string matching algorithm
int horspool(const char *text, const char *pattern, int *match_positions) {
    int n = strlen(text);
    int m = strlen(pattern);
    int shift_table[ALPHABET_SIZE];
    int match_count = 0;
    
    preprocess_shift_table(pattern, shift_table);
    
    int i = m - 1;
    while (i < n) {
        int k = 0;
        while (k < m && pattern[m - 1 - k] == text[i - k]) {
            k++;
        }
        if (k == m) {
            match_positions[match_count++] = i - m + 1;
        }
        i += shift_table[(unsigned char)text[i]];
    }
    return match_count;
}

int main() {
    const char *filename = "input.txt";
    char *text = read_file(filename);
    
    char pattern[100];
    printf("Enter the search key: ");
    fgets(pattern, 100, stdin);

    // Remove trailing newline character from fgets
    size_t len = strlen(pattern);
    if (len > 0 && pattern[len - 1] == '\n') {
        pattern[len - 1] = '\0';
    }

    int match_positions[1000];
    int num_matches;

    printf("\nBrute Force Search:\n");
    num_matches = brute_force(text, pattern, match_positions);
    if (num_matches > 0) {
        highlight_text(text, match_positions, num_matches, strlen(pattern));
    } else {
        printf("Pattern not found using Brute Force Search.\n");
    }

    printf("\nHorspool Search:\n");
    num_matches = horspool(text, pattern, match_positions);
    if (num_matches > 0) {
        highlight_text(text, match_positions, num_matches, strlen(pattern));
    } else {
        printf("Pattern not found using Horspool Search.\n");
    }
    
    free(text);
    return 0;
}
