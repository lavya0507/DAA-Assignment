#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#ifndef _WIN32
#include <time.h>
#endif
#define MAX_LINE 4096

typedef struct {
    int n;
    int capacity;
    int *weights;
    int *values;
} TestCase;

static void free_case(TestCase *tc) {
    free(tc->weights);
    free(tc->values);
    tc->weights = NULL;
    tc->values = NULL;
}

static int parse_cases(const char *filename, TestCase **cases_out, int *count_out) {
    FILE *fp = fopen(filename, "r");
    if(!fp)
    { perror("Could not open input file");
      return 0; }
    TestCase *cases = NULL;
    int count = 0, allocated = 0;
    char line[MAX_LINE];

    while (fgets(line, sizeof(line), fp))
    { char *p = line;
      while (isspace((unsigned char)*p)) p++;
      if(*p == '\0') continue;
      if(strncmp(p, "#Test", 5) == 0)
      { int n, capacity;
        do
        { if(!fgets(line, sizeof(line), fp)) goto invalid_input;
          p = line;
          while (isspace((unsigned char)*p)) p++;
        } while (*p == '\0' || *p == '\n' || *p == '\r');
        if(sscanf(p, "%d %d", &n, &capacity) != 2 || n < 0 || capacity < 0)
            goto invalid_input;
        TestCase tc;
        tc.n = n;
        tc.capacity = capacity;
        tc.weights = calloc((size_t)n, sizeof(int));
        tc.values = calloc((size_t)n, sizeof(int));
        if(n > 0 && (!tc.weights || !tc.values))
        { free_case(&tc);
          goto invalid_input; }
        for(int i=0; i<n; i++)
        { if(!fgets(line, sizeof(line), fp) || sscanf(line, "%d %d", &tc.weights[i], &tc.values[i]) != 2 ||
                    tc.weights[i] < 0)
          { free_case(&tc);
            goto invalid_input; }
        }
        if(count == allocated)
        { int new_size = allocated ? allocated * 2 : 8;
          TestCase *tmp = realloc(cases, (size_t)new_size * sizeof(TestCase));
          if(!tmp)
          { free_case(&tc);
            goto invalid_input; }
          cases = tmp;
          allocated = new_size;
        }
        cases[count++] = tc;
      }
    }
    fclose(fp);
    *cases_out = cases;
    *count_out = count;
    return 1;

invalid_input:
    fclose(fp);
    for (int i = 0; i < count; i++) free_case(&cases[i]);
    free(cases);
    fprintf(stderr, "Invalid input format. Each test case must be #Test, then n W, then n weight/value lines.\n");
    return 0;
}

static void print_table(int **dp, int rows, int capacity, const char *label) {
    printf("DP table after %s\n", label);
    printf("item/cap");
    for (int c = 0; c <= capacity; c++) printf("%5d", c);
    putchar('\n');

    for(int r=0; r<rows; r++)
    { printf("%8d", r);
      for(int c=0; c<=capacity; c++) printf("%5d", dp[r][c]);
      putchar('\n'); }
    putchar('\n');
}

static int knapsack(const TestCase *tc, int live, int delay_ms) {
    int n = tc->n, W = tc->capacity;
    int **dp = malloc((size_t)(n + 1) * sizeof(int *));
    if(!dp)
    { fprintf(stderr, "Memory allocation failed.\n");
      exit(EXIT_FAILURE); }
    for(int i=0; i<=n; i++)
    { dp[i] = calloc((size_t)W + 1, sizeof(int));
      if(!dp[i])
      { fprintf(stderr, "Memory allocation failed.\n");
        exit(EXIT_FAILURE); }
    }
    if(live)
    { printf("\033[2J\033[H");
      print_table(dp, 1, W, "initialization"); }
    for(int i=1; i<=n; i++)
    { int wt = tc->weights[i-1], val = tc->values[i - 1];
      for(int w=0; w<=W; w++)
      { dp[i][w] = dp[i - 1][w];
        if(wt<=w && dp[i-1][w-wt] + val > dp[i][w])
           dp[i][w] = dp[i - 1][w - wt] + val;
      }
    if(live)
    { printf("\033[2J\033[H");
      char label[100];
      snprintf(label, sizeof(label), "item %d (weight=%d, value=%d)", i, wt, val);
      print_table(dp, i + 1, W, label);
      printf("Current best value for capacity %d: %d\n", W, dp[i][W]);
      fflush(stdout);
#ifdef _WIN32
    if (delay_ms > 0)
    { clock_t start = clock();
      while((clock()-start) * 1000 / CLOCKS_PER_SEC < delay_ms) {} }
#else
     if (delay_ms > 0)
     { struct timespec ts;
       ts.tv_sec = delay_ms / 1000;
       ts.tv_nsec = (long)(delay_ms % 1000) * 1000000L;
       nanosleep(&ts, NULL);
     }
#endif
        }
    }
    int answer = dp[n][W];
    for (int i = 0; i <= n; i++) free(dp[i]);
    free(dp);
    return answer;
}

int main(int argc, char *argv[]) {
    const char *input_file = "input.txt";
    const char *result_file = "result.txt";
    int live=0, delay_ms = 350;
    for(int i=1; i<argc; i++)
    { if(strcmp(argv[i], "--live") == 0)
      { live = 1; }
      else if(strcmp(argv[i], "--delay") == 0 && i+1 < argc)
      { delay_ms = atoi(argv[++i]);
        if (delay_ms < 0) delay_ms = 0; }
      else if(strcmp(argv[i], "--result") == 0 && i+1 < argc)
      { result_file = argv[++i]; }
      else
      { input_file = argv[i]; }
    }

    TestCase *cases = NULL;
    int count=0;
    if(!parse_cases(input_file, &cases, &count)) return EXIT_FAILURE;
    FILE *out = fopen(result_file, "w");
    if(!out)
    { perror("Could not create result file");
      for(int i=0; i<count; i++) free_case(&cases[i]);
      free(cases);
      return EXIT_FAILURE; }
    for(int i=0; i<count; i++)
    { int answer = knapsack(&cases[i], live, delay_ms);
      printf("#Test %d (%d %d) %d\n", i + 1, cases[i].n, cases[i].capacity, answer);
      fprintf(out, "#Test %d (%d %d) %d\n", i + 1, cases[i].n, cases[i].capacity, answer);
      free_case(&cases[i]); }
    fclose(out);
    free(cases);
    printf("\nResults saved to %s\n", result_file);
    return 0;
}
