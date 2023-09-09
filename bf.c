#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define DATA_SIZE 30000

/*
NOTES:

The user will pass into the program the <source_file> of the instructions
When ',' occurs in <source_file> then the user will input through the terminal

The tape size will be 30,000
Each cell is 8bit, that said each cell can support 0-255 (I am using uint8)
*/

int main(int argc, char** argv) {
    uint8_t data[DATA_SIZE] = {0}; // uint8 because we have 0-255
    uint8_t* data_ptr = data;
    int bracket_num = 0;

    char* instructions = NULL; // will be initialized later (with malloc)

    if (argc != 2) {
        fprintf(stderr, "Usage: bf.exe <file_path>"); 
        return EXIT_FAILURE;
    }
    char* file_name = argv[1]; // index in argv of file path
    
    FILE* fp = fopen(file_name, "rb");
    if (fp == NULL) {
        fprintf(stderr, "Cannot open file");
        return EXIT_FAILURE;
    }

    fseek(fp, 0, SEEK_END);
    const unsigned long file_size = ftell(fp); // get file size (to allocate memory later)
    rewind(fp);

    instructions = (char*)malloc(file_size + 1); // create char array of bf instructions (+1 because of NULL)

    size_t bytes_read = fread(instructions, 1, file_size, fp); // read the instructions from file
    if (bytes_read != file_size) {
        fprintf(stderr, "Could not read all file");
        free(instructions);
        return EXIT_FAILURE;
    }
    
    // instruction_ptr will point at the current instruction
    // I did not want to create another unsigned long variable to hold the index of current instruction
    // (it is not as cool as pointers lol + it means creating a variable which is more than 4bytes)
    for (char* instruction_ptr = instructions; (instruction_ptr - instructions) < file_size; instruction_ptr++) {
        switch (*instruction_ptr) {
            case '>': data_ptr++; break;    // move pointer to right by 1
            case '<': data_ptr--; break;    // move pointer to left by 1
            case '+': (*data_ptr)++; break; // increase value stored at the curr block
            case '-': (*data_ptr)--; break; // decrease value stored at the curr block
            
            case ',':
                *data_ptr = getchar();
                fflush(stdin);
                break;
            
            case '.': printf("%c", *data_ptr); break; // output a character
            
            case '[':
                if (*data_ptr == 0) { // jump to end of loop (because current data is 0)
                    bracket_num++;
                    instruction_ptr++;
                    for (; bracket_num > 0; instruction_ptr++) { // loop "forward" instructions until right ']'
                        if (*instruction_ptr == ']') bracket_num--;
                        else if (*instruction_ptr == '[') bracket_num++;
                    }
                }
                break;

            case ']':
                if (*data_ptr != 0) { // jump to start of loop (because curent data is not 0)
                    bracket_num++;
                    instruction_ptr--;
                    for (; bracket_num > 0; instruction_ptr--) { // loop "backward" instructions until right '['
                        if (*instruction_ptr == ']') bracket_num++;
                        else if (*instruction_ptr == '[') bracket_num--;
                    }
                }
                break;
            
            default:
                if (*instruction_ptr == '\r' || *instruction_ptr == '\n') break;
                fprintf(stderr, "Oops! This instruction:\n%c \nis not familiar...\n", *instruction_ptr);
                free(instructions);
                return EXIT_FAILURE;
            break;
        }
    }
    
    free(instructions);
    return EXIT_SUCCESS;
}

