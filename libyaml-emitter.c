#include <yaml.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>

// void print_escaped(yaml_char_t* str, size_t length);
bool get_line(FILE *input, char *line);
char *get_anchor(char sigil, char *line, char *anchor);
char *get_tag(char *line, char *tag);
void get_value(char *line, char *value, int *style, int *offset);

int main(int argc, char *argv[]) {
  FILE *input;
  yaml_emitter_t emitter;
  yaml_event_t event;

  int canonical = 0;
  int unicode = 0;
  char line[1024];

  if (argc == 1)
    input = stdin;
  else if (argc == 2)
    input = fopen(argv[1], "rb");
  else {
    fprintf(stderr, "Usage: libyaml-emitter [<input-file>]\n");
    return 1;
  }
  assert(input);

  memset(&emitter, 0, sizeof(emitter));
  memset(&event, 0, sizeof(event));
  memset(&line, 0, sizeof(line));

  if (!yaml_emitter_initialize(&emitter)) {
    fprintf(stderr, "Could not initalize the emitter object\n");
    return 1;
  }
  yaml_emitter_set_output_file(&emitter, stdout);
  yaml_emitter_set_canonical(&emitter, canonical);
  yaml_emitter_set_unicode(&emitter, unicode);
  int implicit = 1;

  while (get_line(input, line)) {
    int ok;
    char anchor[256];
    char tag[256];

    if (strncmp(line, "+STR", 4) == 0) {
      ok = yaml_stream_start_event_initialize(
        &event,
        YAML_UTF8_ENCODING
      );
    }
    else if (strncmp(line, "-STR", 4) == 0) {
      ok = yaml_stream_end_event_initialize(&event);
    }
    else if (strncmp(line, "+DOC", 4) == 0) {
      implicit = 1;
      if (strncmp(line, "+DOC ---", 8) == 0) {
          implicit = 0;
      }
      ok = yaml_document_start_event_initialize(
        &event,
        NULL,
        NULL,
        NULL,
        implicit
      );
    }
    else if (strncmp(line, "-DOC", 4) == 0) {
      implicit = 1;
      if (strncmp(line, "-DOC ...", 8) == 0) {
          implicit = 0;
      }
      ok = yaml_document_end_event_initialize(&event, implicit);
    }
    else if (strncmp(line, "+MAP", 4) == 0) {
      ok = yaml_mapping_start_event_initialize(
        &event,
        (yaml_char_t *)get_anchor('&', line, anchor),
        (yaml_char_t *)get_tag(line, tag),
        0,
        0
      );
    }
    else if (strncmp(line, "-MAP", 4) == 0) {
      ok = yaml_mapping_end_event_initialize(&event);
    }
    else if (strncmp(line, "+SEQ", 4) == 0) {
      ok = yaml_sequence_start_event_initialize(
        &event,
        (yaml_char_t *)get_anchor('&', line, anchor),
        (yaml_char_t *)get_tag(line, tag),
        1,
        YAML_BLOCK_SEQUENCE_STYLE
      );
    }
    else if (strncmp(line, "-SEQ", 4) == 0) {
      ok = yaml_sequence_end_event_initialize(&event);
    }
    else if (strncmp(line, "=VAL", 4) == 0) {
      char value[1024];
      int style;
      char *tag_new = (yaml_char_t *)get_tag(line, tag);

      int offset = 5 + 2;
      if (tag_new) {
          offset += strlen((char *)tag_new);
      }

      get_value(line, value, &style, &offset);

      ok = yaml_scalar_event_initialize(
        &event,
        (yaml_char_t *)get_anchor('&', line, anchor),
        tag_new,
        (yaml_char_t *)value,
        -1,
        1,
        1,
        style
      );
    }
    else if (strncmp(line, "=ALI", 4) == 0) {
      ok = yaml_alias_event_initialize(
        &event,
        (yaml_char_t *)get_anchor('*', line, anchor)
      );
    }
    else {
      fprintf(stderr, "Unknown event: '%s'\n", line);
      fflush(stdout);
      return 1;
    }

    if (!ok)
      goto event_error;
    if (!yaml_emitter_emit(&emitter, &event))
      goto emitter_error;
  }

  assert(!fclose(input));
  yaml_emitter_delete(&emitter);
  fflush(stdout);

  return 0;

emitter_error:
  switch (emitter.error) {
    case YAML_MEMORY_ERROR:
      fprintf(stderr, "Memory error: Not enough memory for emitting\n");
      break;
    case YAML_WRITER_ERROR:
      fprintf(stderr, "Writer error: %s\n", emitter.problem);
      break;
    case YAML_EMITTER_ERROR:
      fprintf(stderr, "Emitter error: %s\n", emitter.problem);
      break;
    default:
      /* Couldn't happen. */
      fprintf(stderr, "Internal error\n");
      break;
  }
  yaml_emitter_delete(&emitter);
  return 1;

event_error:
  fprintf(stderr, "Memory error: Not enough memory for creating an event\n");
  yaml_emitter_delete(&emitter);
  return 1;
}

bool get_line(FILE *input, char *line) {
  char *newline;

  if (!fgets(line, 1024 - 1, input))
    return false;

  if ((newline = strchr(line, '\n')) == NULL) {
    fprintf(stderr, "Line too long: '%s'", line);
    abort();
  }
  *newline = '\0';

  return true;
}

char *get_anchor(char sigil, char *line, char *anchor) {
  char *start;
  char *end;
  if ((start = strchr(line, sigil)) == NULL)
    return NULL;
  start++;
  if ((end = strchr(start, ' ')) == NULL)
    end = line + strlen(line);
  memcpy(anchor, start, end - start);
  anchor[end - start] = '\0';
  return anchor;
}

char *get_tag(char *line, char *tag) {
  char *start;
  char *end;
  if ((start = strchr(line, '<')) == NULL)
    return NULL;
  if ((end = strchr(line, '>')) == NULL)
    return NULL;
  memcpy(tag, start + 1, end - start - 1);
  tag[end - start -1] = '\0';
  return tag;
}

void get_value(char *line, char *value, int *style, int *offset) {

  int i = 0;
  char *c;
  char *start;
  char *end;
  if ((start = strchr(line, ':')) != NULL)
    *style = YAML_PLAIN_SCALAR_STYLE;
  else if ((start = strchr(line, '\'')) != NULL)
    *style = YAML_SINGLE_QUOTED_SCALAR_STYLE;
  else if ((start = strchr(line, '"')) != NULL)
    *style = YAML_DOUBLE_QUOTED_SCALAR_STYLE;
  else if ((start = strchr(line, '|')) != NULL)
    *style = YAML_LITERAL_SCALAR_STYLE;
  else if ((start = strchr(line, '>')) != NULL)
    *style = YAML_FOLDED_SCALAR_STYLE;
  else
    abort();

  end = start++ + strlen(line);

  for (c = start; c < end; c++) {
    if (*c == '\\') {
      c++;
      if (*c == 'n')
        value[i++] = '\n';
      else
        abort();
    }
    else
      value[i++] = *c;
  }
}
