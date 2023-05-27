#define COMMAND_STR_LENGTH 32
#define BUF_SIZE 256

#define COMMAND_PRINT "print"
#define COMMAND_SORT "sort"
#define COMMAND_RETURN "return"

#define PORT 9999

struct message {
    char command[COMMAND_STR_LENGTH];
    char buf[BUF_SIZE];
};

#define MAX_NUMBERS 128
struct sort_command_payload {
    int len;
    int numbers[MAX_NUMBERS];
};

#define MAX_PRINT_CHARS 128
struct print_command_payload {
    int len;
    char string_to_print[MAX_PRINT_CHARS];
};

enum return_codes {
    RET_SUCCESS = 0,
    RET_ERROR = 400
};

struct return_command_payload {
    int return_code;
};