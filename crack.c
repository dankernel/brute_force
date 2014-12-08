/*
 * crack.c
 *   Multithreaded brute-force password hash cracker.
 *   usage: crack threads keysize target
 *
 *   Nathan Bossart
 *   Sept 25, 2013
 */

#define _GNU_SOURCE
#include "dkh/md5.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <crypt.h>
#include <math.h>

char GET_HASH[10][33] = { "414d161f5c3c37f34aaf70f114f3918c",
  "3fde6bb0541387e4ebdadf7c2ff31123",
  "bf0fc9ec54bf143b948bb8b8fd30dba7",
  "e7df3ae8d0b2c67b9ca57905bd2aac89",
  "eb4fb89a42b18740d073dae7c6f46e2b",
  "da3a6e38587cdb1502bbca38cd162b01",
  "a4d3b161ce1309df1c4e25df28694b7b",
  "be35da39d5271c87e327609a0f152db3",
  "3186d68ed459909f4d5ce3af8547eb75",
  "deab299b21307c8953f7381038989cf0"};

char TARGET[33];

typedef struct seg seg;
struct seg {
  double start, end;
};

static char *get_hash_cpy(char *ret, char *test)
{
	md5_state_t state;
	md5_byte_t digest[16];
	char hex_output[16*2 + 1];
	int di;

	md5_init(&state);
	md5_append(&state, (const md5_byte_t *)test, strlen(test));
	md5_finish(&state, digest);

	for (di = 0; di < 16; ++di)
		sprintf(hex_output + di * 2, "%02x", digest[di]);

  strncpy(ret, hex_output, 33);
	return ret;
}

// gets the nth possible string
char* getNthString(double num) {

  // calculate the string
  char* str = malloc(20*sizeof(char));
  memset(str, '\0', sizeof(str));
  int i = 0;
  do {
    double rem = fmod(num, 57);
    str[i++] = 'A' + (int) rem;
    num = (num-rem)/57;
  } while (num > 0);

  // reverse the string
  i = 0;
  int len = strlen(str);
  while (i<len/2) {
    str[len-1-i] = str[len-1-i] ^ str[i];
    str[i]       = str[len-1-i] ^ str[i];
    str[len-1-i] = str[len-1-i] ^ str[i];
    ++i;
  }
  return str;
}

// increments the string
void incrementString(char *curr_string) {
  int i = strlen(curr_string)-1;
  while (++curr_string[i]=='{') {
    curr_string[i--] = 'A';
    if (i==-1) {
      strcat(curr_string, "A");
      break;
    }
  }
}

// each thread tries a range of strings
void *crack(seg* range) {
  double i = range->start;
  char *hash;
  char *guess = getNthString(i);
  struct crypt_data data;
  data.initialized = 0;
  int loop = 0;

  printf("mk th %f \n", i);

  hash = malloc(sizeof(char) * 100);
  while (i++ < range->end) {
    /* hash = crypt_r(guess, "ee", &data); */
    get_hash_cpy(hash, guess);
    /* printf("%s %s %s\n", guess, hash, TARGET); */

    for (loop = 0; loop < 10; loop++) {
      if (strcmp(hash, GET_HASH[loop])==0) {
        printf("%s %s %s\n", guess, hash, TARGET);
        printf("Fin..!\n");
      }
    }
    incrementString(guess);
  }
  printf("return\n");
  return 0;
}

int main(int argc, char* argv[]) {

  // find number of possible passwords
  int i = atoi(argv[2]);
  double possibilities = 0;
  int       threads   = 0;
  double    perThread = 0;
  seg       range[threads];
  pthread_t thread_id[threads];

  int ret = 0;

  while (i>0) {
    possibilities += pow(57, i--);
  }

  threads   = atoi(argv[1]);
  perThread = floor(possibilities/threads);

  // prepare thread data and start threads
  /* strcpy(TARGET, argv[3]); */
  while (i<threads) {
    range[i].start = perThread * i;
    if (i==threads-1) {
      range[i].end = possibilities;
    } else {
      range[i].end = range[i].start + perThread;
    }
    if (pthread_create(&thread_id[i], NULL, crack, &range[i])) {
      perror("Creating thread");
      /* exit(1); */
    }
    ++i;
  }

  // join threads
  i = 0;
  do {
    pthread_join(thread_id[i++], (void **)&ret);
    printf("ret : %d\n", ret);
  } while (i<threads);
  printf("=== End ===\n");

  exit(0);

}
