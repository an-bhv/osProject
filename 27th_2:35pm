#include <stdio.h> 
#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>
#include <pthread.h>
#include <sys/wait.h>
#include <string.h>
#define MAXN 50
#define MAX_THREAD_CNT 4
#pragma GCC optimize("O3,unroll-loops")
 
short N;
// direction of moves
short dx[] = { 2, 1, -1, -2, -2, -1, 1, 2 };
short dy[] = { 1, 2, 2, 1, -1, -2, -2, -1 };
 
// Helper struct, can utilize any other custom structs if needed 
typedef struct {
    short x, y;
} pair;
 
short StartX, StartY;
short grid[MAX_THREAD_CNT][MAXN][MAXN];
 
// Custom Comparator to sort pairs
int comp(const void* p, const void* q) {
    short l = ((pair*)p)->x, r = ((pair*)q)->x;
    if (l == r) {
        l = ((pair*)p)->y, r = ((pair*)q)->y;
        return l - r;
    }
    return l - r;
}
 
// check if cell is valid or not
bool valid(short x, short y) {
    return 0 <= x && x < N && 0 <= y && y < N;
}
 
// calculates number of moves that can be made from this cell
short moves(short x, short y, short id) {
    short cnt = 0;
    for (short i = 0; i < 8; ++i) {
        short X = x + dx[i], Y = y + dy[i];
        if (valid(X, Y) && !grid[id][X][Y]) cnt++;
    }
    return cnt;
}
 
// prints path 
void print_path(pair path[], short n) {
    for (short i = 0; i < n; i++) {
        printf("%d,%d|", path[i].x, path[i].y);
    }
}
// Thread parameters
typedef struct {
    short x, y, cnt;
    short t_id;
} state;
 
// Global boolean to check if solution is found
bool done = false;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
 
// dfs
void* dfs(void* input) {
    short x = ((state*)input)->x, y = ((state*)input)->y, cnt = ((state*)input)->cnt;
    short id = ((state*)input)->t_id;
 
    while (true) {
        if (done) {
            pthread_exit(NULL);
            return NULL;
        }
        grid[id][x][y] = cnt;
        pthread_mutex_lock(&lock);
        if (cnt == N * N && !done) {
            done = 1;
 
            pair path[N * N];
            for (short i = 0; i < N; i++) {
                for (short j = 0; j < N; j++) {
                    path[grid[id][i][j] - 1].x = i;
                    path[grid[id][i][j] - 1].y = j;
                }
            }
 
            print_path(path, N * N);
            exit(0);
        }
 		pthread_mutex_unlock(&lock);
        pair nxt[8];
        short len = 0;
        for (short i = 0; i < 8; ++i) {
            short X = x + dx[i], Y = y + dy[i];
            if (!valid(X, Y) || grid[id][X][Y]) continue;
            nxt[len].x = moves(X, Y, id);
            nxt[len].y = i;
            len++;
        }
 
        if (len > 0) {
            qsort((void*)nxt, len, sizeof(nxt[0]), comp);
            short sz = 1;
            for (short i = 1; i < len; ++i)
                if (nxt[i].x == nxt[0].x)
                    sz++;
            // Picking a random minima
            short take = rand() % sz;
            cnt++;
            x += dx[nxt[take].y];
            y += dy[nxt[take].y];
        } else {
            pthread_exit(NULL);
            return NULL;
        }
    }
 
}
 
int main(int argc, char* argv[]) {
    if (argc != 4) {
        printf("Usage: ./Knight.out grid_size StartX StartY");
        exit(-1);
    }
 
    N = atoi(argv[1]);
    StartX = atoi(argv[2]);
    StartY = atoi(argv[3]);
 
    if (N == 1) {
        printf("0,0|");
        return 0;
    }
 
    if (N <= 4) {
        printf("No Possible Tour");
        return 0;
    }
 
    if (N % 2) {
        if (((StartX + StartY) % 2)) {
            printf("No Possible Tour");
            return 0;
        }
    }
 
    bool first = true;
    pthread_t threads[MAX_THREAD_CNT];
    
    // While solution is not found , create a batch of threads finding solution independently
    while (!done) {
        short c = 0;
        while (c < MAX_THREAD_CNT) {
            state* Start = (state*)malloc(sizeof(state));
            Start->x = StartX;
            Start->y = StartY;
            Start->cnt = 1;
            Start->t_id = c;
 
            if (!first) {
                for (short i = 0; i < N; ++i)
                    for (short j = 0; j < N; ++j)
                        grid[c][i][j] = 0;
            }
            pthread_create(&threads[c], NULL, dfs, (void*)Start);
            c++;
        }
 
        for (short i = 0; i < MAX_THREAD_CNT; i++)
            pthread_join(threads[i], NULL);
        first = false;
    }
    return 0;
}
