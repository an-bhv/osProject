#include <stdio.h> 
#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>
#include <pthread.h>
#include <sys/wait.h>
#include <string.h>
#define MAXN 50

int N;
// direction of moves
int dx[] = { 2, 1, -1, -2, -2, -1, 1, 2 };
int dy[] = { 1, 2, 2, 1, -1, -2, -2, -1 };
int grid[MAXN][MAXN];

// Helper struct, can utilize any other custom structs if needed 
typedef struct {
    int x, y;
} pair;

/*
Custom comparator for sorting

int comparator(const void* p1, const void* p2);
Return value meaning
<0 The element pointed by p1 goes before the element pointed by p2
0  The element pointed by p1 is equivalent to the element pointed by p2
>0 The element pointed by p1 goes after the element pointed by p2

*/
int comp(const void* p, const void* q) {
    int l = ((pair*)p)->x, r = ((pair*)q)->x;
    if (l == r) {
        l = ((pair*)p)->y, r = ((pair*)q)->y;
        return l - r;
    }
    return l - r;
}

pair order[MAXN * MAXN];

// check if cell is valid or not
bool valid(int x, int y) {
    return 0 <= x && x < N && 0 <= y && y < N;
}

// calculates number of moves that can be made from this cell
int moves(int x, int y) {
    int cnt = 0;
    for (int i = 0; i < 8; ++i) {
        int X = x + dx[i], Y = y + dy[i];
        if (valid(X, Y) && !grid[X][Y]) cnt++;
    }
    return cnt;
}

// prints path and tests if output is valid or not
void print_path(pair path[], int n) {
    // remove this later just testing not in template code
    {
        bool vis[N][N];
        for (int i = 0; i < n; ++i) {
            // checking if each coordinate is valid or not
            assert(valid(path[i].x, path[i].y));

            vis[path[i].x][path[i].y] = true;

            // checking if difference in moves is valid or not
            if (i > 0) {
                int X = path[i].x - path[i - 1].x;
                int Y = path[i].y - path[i - 1].y;

                bool seen = false;
                for (int j = 0; j < 8; ++j) {
                    if (X == dx[j] && Y == dy[j]) {
                        seen = true;
                        break;
                    }
                }

                assert(seen);
            }
        }

        // checking if all cells are visited or not
        for (int i = 0; i < N; ++i)
            for (int j = 0; j < N; ++j)
                assert(vis[i][j]);
    }

    for (int i = 0; i < n; i++) {
        printf("%d,%d|", path[i].x, path[i].y);
    }
}

typedef struct {
    int x, y, cnt, ** grid;
    // pair* order;
} state;

bool done = false;
pthread_mutex_t lock;

// backtracking main part
void* dfs(void* input) {
    if (done) return NULL;
    state* now = (state*)input;
    now->grid[now->x][now->y] = now->cnt;

    // grid is filled
    if (now->cnt == N * N && !done) {
        pthread_mutex_lock(&lock);
        if (done) return NULL;
        done = 1;

        // for (int i = 0; i < N; ++i) {
        //     for (int j = 0; j < N; ++j) {
        //         printf("%d ", now->grid[i][j]);
        //     }
        //     printf("\n");
        //     fflush(stdout);
        // }

        for (int i = 0; i < N; ++i) {
            for (int j = 0; j < N; ++j) {
                order[now->grid[i][j] - 1].x = i;
                order[now->grid[i][j] - 1].y = j;
            }
        }

        print_path(order, now->cnt);
        pthread_mutex_unlock(&lock);
        pthread_exit(NULL);
        exit(0);
    }

    // finding for all valid neigbours the number of moves they can make and sorting
    pair nxt[8];
    int len = 0;
    for (int i = 0; i < 8; ++i) {
        int X = now->x + dx[i], Y = now->y + dy[i];
        if (!valid(X, Y) || now->grid[X][Y]) continue;
        nxt[len].x = moves(X, Y);
        nxt[len].y = i;
        len++;
    }

    if (len > 0) {
        qsort((void*)nxt, len, sizeof(nxt[0]), comp);
        if (now->cnt % 10 == 0) {
            pthread_t ttid[len];
            for (int i = 0; i < len; ++i) {
                pthread_t tid;

                state* lol = (state*)malloc(sizeof(state*));

                lol->x = now->x + dx[nxt[i].y];
                lol->y = now->y + dy[nxt[i].y];
                lol->cnt = now->cnt + 1;

                lol->grid = (int**)malloc(N * sizeof(int*));
                for (int i = 0; i < N; ++i) {
                    lol->grid[i] = (int*)malloc(N * sizeof(int));
                    for (int j = 0; j < N; ++j)
                        lol->grid[i][j] = now->grid[i][j];
                }

                // lol->grid = now->grid;
                // dfs(lol);
                pthread_create(&tid, NULL, dfs, (void*)lol);
                // wait(NULL);
                ttid[i] = tid;
            }

            for (int i = 0; i < len; ++i)
                pthread_join(ttid[i], NULL);
        } else {
            for (int i = 0; i < len; ++i) {
                state* lol = (state*)malloc(sizeof(state*));

                lol->x = now->x + dx[nxt[i].y];
                lol->y = now->y + dy[nxt[i].y];
                lol->cnt = now->cnt + 1;
                
                lol->grid = (int**)malloc(N * sizeof(int*));
                for (int i = 0; i < N; ++i) {
                    lol->grid[i] = (int*)malloc(N * sizeof(int));
                    for (int j = 0; j < N; ++j)
                        lol->grid[i][j] = now->grid[i][j];
                }

                dfs((void*)lol);
            }
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        printf("Usage: ./Knight.out grid_size StartX StartY");
        exit(-1);
    }

    N = atoi(argv[1]);
    int StartX = atoi(argv[2]);
    int StartY = atoi(argv[3]);

    state* Start = (state*)malloc(sizeof(state*));

    Start->x = StartX;
    Start->y = StartY;
    Start->cnt = 1;

    Start->grid = (int**)malloc(N * sizeof(int*));
    for (int i = 0; i < N; ++i)
        Start->grid[i] = (int*)malloc(N * sizeof(int));

    pthread_mutex_init(&lock, NULL);

    dfs(Start);

    if (!done)
        printf("No Possible Tour\n");
    return 0;
}


