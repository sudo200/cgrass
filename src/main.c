#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/ioctl.h>

#define __noreturn __attribute__((noreturn))

#define EQUALS(x, y)  (strcmp(x, y) == 0)

#define VERSION "v1.0.0"

__noreturn static void help(void) {
  puts(
    "Usage: cgrass [OPTION]...\n"
    "\n"
    "cgrass is the UNIX way of touching grass.\n"
    "It generates random amounts of grass in your terminal.\n"
    "\n"
    "Options:\n"
    "\t-w <UINT>\tForce a width\n"
    "\n"
    "\t-c <STR>\tThe characters to use for drawing grass\n"
    "\n"
    "\t-s <UINT>\tSeed for the RNG\n"
    "\n"
    "\t-m <UINT>\tThe maximum height of the grass\n"
    "\n"
    "\t-p <BYTE>\tThe probability the generate grass in a cell\n"
    "\n"
    "\t-i <BYTE>\tThe value to increment probability with for each row\n"
    "\n"
    "\t-h\t\tShow help\n"
    "\n"
    "\t-v\t\tShow version\n"
    "\n"
    "sudo200 and contributors\n"
  );
  exit(EXIT_FAILURE);
}

__noreturn static void version(void) {
  puts("cgrass " VERSION);
  exit(EXIT_FAILURE);
}

static void setcolor(void) {
  printf("%c[0;32m", 0x1B);
}

static void resetcolor(void) {
  printf("%c[0m", 0x1B);
}

unsigned int get_uint(const char *str) {
  char *t;
  unsigned long num = strtoul(str, &t, 0);

  if(num == 0 && str == t) {
    fputs("Not a valid integer!\n", stderr);
    exit(EXIT_FAILURE);
  }

  if(num > UINT_MAX) {
    fputs("Integer out of bounds!\n", stderr);
    exit(EXIT_FAILURE);
  }

  return (unsigned int)num;
}

static unsigned int get_percent(const char *str) {
  unsigned int percent = get_uint(str);
  if(percent > 100) {
    fputs("Not a valid percent value!\n", stderr);
    exit(EXIT_FAILURE);
  }
  return percent;
}

int main(int argc, char **argv)
{
  const char *grass_chars = "|";
  unsigned int seed = *(unsigned int *)&argv ^ getpid(),
               max_height = 7U,
               propability = 10, // %
               increment = 10;

  struct winsize w;
  memset(&w, 0, sizeof(w));

  // Argument parsing
  for(int i = 1; i < argc; i++) {
    if(EQUALS(argv[i], "-v") || EQUALS(argv[i], "--version"))
      version();
    else if(EQUALS(argv[i], "-h") || EQUALS(argv[i], "--help"))
      help();
    else if(EQUALS(argv[i], "-w"))
      w.ws_col = get_uint(argv[++i]);
    else if(EQUALS(argv[i], "-c"))
      grass_chars = argv[++i];
    else if(EQUALS(argv[i], "-s"))
      seed = get_uint(argv[++i]);
    else if(EQUALS(argv[i], "-m"))
      max_height = get_uint(argv[++i]);
    else if(EQUALS(argv[i], "-p"))
      propability = get_percent(argv[++i]);
    else if(EQUALS(argv[i], "-i"))
      increment = get_percent(argv[++i]);
  }

  if(w.ws_col == 0U) { // If not set, get from term.
    if(!isatty(fileno(stdout))) {
      fputs("-w is required when not attached to a terminal!\n", stderr);
      return EXIT_FAILURE;
    }
    ioctl(fileno(stdout), TIOCGWINSZ, &w);
  }

  srand(seed);
  const size_t grass_chars_len = strlen(grass_chars);
  w.ws_row = max_height;

  // Alloc buffer
  char screen[w.ws_row][w.ws_col];
  for(unsigned int i = 0U; i < w.ws_row; i++)
    memset(screen[i], ' ', sizeof(**screen) * w.ws_col);
  

  // Generate random grass image
  for(size_t y = 0UL; y < w.ws_row; y++) {
    for(size_t x = 0UL; x < w.ws_col; x++) {
      if(screen[(y - 1) % w.ws_row][x] != ' ') {
        screen[y][x] = grass_chars[rand() % grass_chars_len];
        continue;
      }

      if((rand() % 101) + 1 <= propability)
        screen[y][x] = grass_chars[rand() % grass_chars_len];
    }

    if(propability < 100)
      propability += increment;
  }

  // Print out buffer
  setcolor();
  for(size_t y = 0UL; y < w.ws_row; y++) {
    fwrite(screen[y], sizeof(**screen), w.ws_col, stdout);
    putchar('\n');
  }
  resetcolor();

  return 0;
}

