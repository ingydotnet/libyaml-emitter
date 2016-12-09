#include <yaml.h>

#include <stdlib.h>
#include <stdio.h>

#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>

void print_escaped(yaml_char_t* str, size_t length);

int main(int argc, char *argv[]) {
  int max = (argc < 2 ? 2 : argc);
  int number;

  for (number = 1; number < max; number++) {
    FILE *file;
    yaml_parser_t parser;
    yaml_event_t event;
    int done = 0;
    int count = 0;
    int error = 0;

    /* printf("[%d] Parsing '%s': ", number, argv[number]); */
    fflush(stdout);

    if (argc < 2)
      file = fopen("/dev/stdin", "rb");
    else
      file = fopen(argv[number], "rb");
    assert(file);

    assert(yaml_parser_initialize(&parser));

    yaml_parser_set_input_file(&parser, file);


    while (!done) {
      yaml_event_delete(&event);

      count ++;
    }

    assert(!fclose(file));

    /* printf("%s (%d events)\n", (error ? "FAILURE" : "SUCCESS"), count); */
  }

  return 0;
}

void print_escaped(yaml_char_t* str, size_t length) {
  int number;
  char c;

  for (number = 0; number < length; number++) {
    c = *(str++);
    switch(c) {
      case '\\':
        printf("\\\\");
        break;
      case '\0':
        printf("\\0");
        break;
      case '\r':
        printf("\\r");
        break;
      case '\n':
        printf("\\n");
        break;
      default:
        printf("%c", c);
        break;
    }
  }
}
