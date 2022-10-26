#include <stdio.h> 
#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>
#include <pthread.h>
#include <sys/wait.h>
#include <string.h>
#include <semaphore.h>
#define MAXN 50
#define MAX_THREAD_CNT 200

int N;
// direction of moves
int dx[] = { 2, 1, -1, -2, -2, -1, 1, 2 };
int dy[] = { 1, 2, 2, 1, -1, -2, -2, -1 };

// Helper struct, can utilize any other custom structs if needed 
typedef struct {
    int x, y;
} pair;

int StartX, StartY;
int grid[MAX_THREAD_CNT][MAXN][MAXN];

int comp(const void* p, const void* q) {
    int l = ((pair*)p)->x, r = ((pair*)q)->x;
    if (l == r) {
        l = ((pair*)p)->y, r = ((pair*)q)->y;
        return l - r;
    }
    return l - r;
}

// check if cell is valid or not
bool valid(int x, int y) {
    return 0 <= x && x < N && 0 <= y && y < N;
}

// calculates number of moves that can be made from this cell
int moves(int x, int y, int id) {
    int cnt = 0;
    for (int i = 0; i < 8; ++i) {
        int X = x + dx[i], Y = y + dy[i];
        if (valid(X, Y) && !grid[id][X][Y]) cnt++;
    }
    return cnt;
}

// prints path and tests if output is valid or not
void print_path(pair path[], int n) {
    // printf("Thread %ld entered print path\n" , pthread_self());
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
    int x, y, cnt;
    int t_id;
} state;

bool done = false;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

void* dfs(void* input) {
    if (done) {
        pthread_exit(NULL);
        return NULL;
    }

    state* now = (state*)input;
    int x = now->x, y = now->y, cnt = now->cnt;
    int id = now->t_id;
    grid[id][x][y] = cnt;

    if (cnt == N * N && !done) {
        done = 1;

        pthread_mutex_lock(&lock);
        
        pair path[N * N];
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                path[grid[id][i][j] - 1].x = i;
                path[grid[id][i][j] - 1].y = j;
            }
        }

        print_path(path, N * N);
        
        pthread_exit(NULL);
        pthread_mutex_unlock(&lock);
        return NULL;
    }

    pair nxt[8];
    int len = 0;
    for (int i = 0; i < 8; ++i) {
        int X = now->x + dx[i], Y = now->y + dy[i];
        if (!valid(X, Y) || grid[id][X][Y]) continue;
        nxt[len].x = moves(X, Y, id);
        nxt[len].y = i;
        len++;
    }

    if (len > 0) {
        qsort((void*)nxt, len, sizeof(nxt[0]), comp);
        int sz = 1;
        for (int i = 1; i < len; ++i)
            if (nxt[i].x == nxt[0].x)
                sz++;
        int take = rand() % sz;
        now->cnt = cnt + 1;
        now->x = x + dx[nxt[take].y];
        now->y = y + dy[nxt[take].y];
        dfs(now);
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

    pthread_t threads[MAX_THREAD_CNT];
    int c = 0;
    while (c < MAX_THREAD_CNT) {
        state* Start = (state*)malloc(sizeof(state));
        Start->x = StartX;
        Start->y = StartY;
        Start->cnt = 1;
        Start->t_id = c;

        pthread_create(&threads[c], NULL, dfs, (void*)Start);
        c++;
    }

    for (int i = 0; i < MAX_THREAD_CNT; i++)
        pthread_join(threads[i], NULL);
    return 0;
}
