#include "cachelab.h"
#include "string.h"
#include "stdlib.h"
#include "stdio.h"
#include "math.h"
#include "getopt.h"
#include "unistd.h"

const char *help = "\
Usage: ./csim-ref [-hv] -s <num> -E <num> -b <num> -t <file>\n\
Options:\n\
  -h         Print this help message.\n\
  -v         Optional verbose flag.\n\
  -s <num>   Number of set index bits.\n\
  -E <num>   Number of lines per set.\n\
  -b <num>   Number of block offset bits.\n\
  -t <file>  Trace file.\n\
\n\
Examples:\n\
  linux>  ./csim-ref -s 4 -E 1 -b 4 -t traces/yi.trace\n\
  linux>  ./csim-ref -v -s 8 -E 2 -b 4 -t traces/yi.trace\n";

typedef struct {
    int valid;
    int tag;
} Line;

typedef struct {
    int validNum;
    Line *lines;
} Set;

int S = -1, E = -1, B = -1;
char *file = NULL;
int hits = 0, misses = 0, evictions = 0;
int vFlag = 0;
Set *cache = NULL;

static void AccessLine(unsigned int index, unsigned int tag) {
    Line *lines = cache[index].lines;
    int target = -1;
    int i;
    for (i = 0; i < E; i++) {
        if (!lines[i].valid) break;
        if (lines[i].tag == tag) {
            target = i;
            break;
        }
    }
    if (target != -1) {
        // hit
        hits++;
        if (vFlag) printf(" hit");
    } else if (cache[index].validNum < E) {
        // load
        misses++;
        target = cache[index].validNum++;
        if (vFlag) printf(" miss");
    } else {
        // replace
        misses++;
        evictions++;
        target = E -1;
        if (vFlag) printf(" miss eviction");
    }
    // change the order
    for (i = target - 1; i >= 0; i--) {
        lines[i + 1] = lines[i];
    }
    lines[0].valid = 1;
    lines[0].tag = tag;
}

static void AccessMemory(char *data) {
    char op;
    unsigned int address;
    int size;
    sscanf(data, " %c %x,%d", &op, &address, &size);
    unsigned int index = address / B % S;
    unsigned int tag = address / B / S;
    if (vFlag) printf("%c %x,%d %d %d", op, address, size, tag, index);
    AccessLine(index, tag);
    if (op == 'M') {
        hits++;
        if (vFlag) printf(" hit");
    }
    if (vFlag) printf("\n");
}

int main(int argc, char *argv[])
{
    // Parse the command
    int i;
    const char *optstring = "hvs:E:b:t:";
    while ((i = getopt(argc, argv, optstring)) != -1) {
        switch (i) {
            case 'h':
            case '?':
                printf("%s", help);
                return 0;
            case 'v':
                vFlag = 1;
                break;
            case 's':
                S = pow(2, atoi(optarg));
                break;
            case 'E':
                E = atoi(optarg);
                break;
            case 'b':
                B = pow(2, atoi(optarg));
                break;
            case 't':
                file = optarg;
                break;
        }
    }
    // Check validity of arguments
    if (S < 0 || E < 0 || B < 0 || file == NULL) {
        printf("Lack of arguments or invalid arguments!\n");
        return -1;
    }
    // Check validity of input file
    FILE *fp = fopen(file, "r");
    if (fp == NULL) {
        printf("Can't open the file!\n");
        return -1;
    }
    // create the cache
    int j;
    cache = (Set*)malloc(sizeof(Set) * S);
    for (i = 0; i < S; i++) {
        cache[i].validNum = 0;
        cache[i].lines = (Line*)malloc(sizeof(Line) * E);
        for (j = 0; j < E; j++) {
            cache[i].lines[j].valid = 0;
        }
    }
    // read lines from the file
    char data[1024];
    char ch;
    i = 0;
    while (!feof(fp)) {
        ch = fgetc(fp);
        if (ch != '\n') {
            data[i++] = ch;
        } else {
            data[i] = '\0';
            i = 0;
            if (data[0] == ' ') {
                AccessMemory(data);
            }
        }
    }
    // close the file
    fclose(fp);
    printSummary(hits, misses, evictions);
    // free the cache
    for (i = 0; i < S; i++) {
        free(cache[i].lines);
    }
    free(cache);
    return 0;
}
