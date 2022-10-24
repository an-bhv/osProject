#include <stdio.h> 
#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#define MAXN 50

int N, StartX, StartY;
int dx[] = { 2, 1, -1, -2, -2, -1, 1, 2 };
int dy[] = { 1, 2, 2, 1, -1, -2, -2, -1 };
int grid[MAXN][MAXN];

typedef struct {
    int x, y;
} pair;

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

bool done = false;
// backtracking main part
void dfs(int cnt, int x, int y) {
    order[cnt - 1].x = x;
    order[cnt - 1].y = y;

    grid[x][y] = cnt;
    // grid is filled
    if (cnt == N * N) {
        print_path(order, cnt);
        done = 1;
        exit(0);
    }

    // finding for all valid neigbours the number of moves they can make and sorting
    pair nxt[8];
    int len = 0;
    for (int i = 0; i < 8; ++i) {
        int X = x + dx[i], Y = y + dy[i];
        if (!valid(X, Y) || grid[X][Y]) continue;
        nxt[len].x = moves(X, Y);
        nxt[len].y = i;
        len++;
    }

    if (len > 0) {
        qsort((void*)nxt, len, sizeof(nxt[0]), comp);
        for (int i = 0; i < len; ++i)
            dfs(cnt + 1, x + dx[nxt[i].y], y + dy[nxt[i].y]);
    }
    grid[x][y] = 0;
}

void dfs_close(int x, int y, int cnt) {
    grid[x][y] = cnt;

    // grid is filled
    if (cnt == N * N && !done) {
        if (done) return;

        bool isclosed = false;
        int stx = 0, sty = 0, endx = 0, endy = 0;
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                if (grid[i][j] == 1) {
                    stx = i;
                    sty = j;
                }
                if (grid[i][j] == N * N) {
                    endx = i;
                    endy = j;
                }
            }
        }

        for (int i = 0; i < 8; i++) {
            int nx = stx + dx[i], ny = sty + dy[i];
            if (nx == endx && ny == endy)
                isclosed = true;
        }

        if (isclosed) {
            done = 1;
            //printf("Starting at %d %d\n" , stx , sty);
            int offset = grid[StartX][StartY] - 1;
            for (int i = 0; i < N; i++) {
                for (int j = 0; j < N; j++) {
                    int final = grid[i][j] - offset + (grid[i][j] > offset ? 0 : N * N);
                    order[final - 1].x = i;
                    order[final - 1].y = j;
                }
            }
            print_path(order, cnt);
            exit(0);
        }
    }

    pair nxt[8];
    int len = 0;
    for (int i = 0; i < 8; ++i) {
        int X = x + dx[i], Y = y + dy[i];
        if (!valid(X, Y) || grid[X][Y]) continue;
        nxt[len].x = moves(X, Y);
        nxt[len].y = i;
        len++;
    }

    if (len > 0) {
        qsort((void*)nxt, len, sizeof(nxt[0]), comp);
        cnt = cnt + 1;
        x = x + dx[nxt[0].y];
        y = y + dy[nxt[0].y];
        dfs_close(x, y, cnt);
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

    if (N % 2) {
        if (((StartX + StartY) % 2) || N <= 4)
            printf("No Possible Tour\n");
        else {
            dfs(1, StartX, StartY);
            if (!done)
                printf("No Possible Tour\n");
        }
    } else {
        if (N <= 4) {
            printf("No Possible Tour\n");
            exit(0);
        }

        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                memset(grid, 0, sizeof(grid));
                dfs_close(i, j, 1);
            }
        }
    }

    return 0;
}
