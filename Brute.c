  #include <stdio.h> 
  #include <stdbool.h>
  #include <stdlib.h>
  #include <assert.h>
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

  Source: http://www.cplusplus.com/reference/cstdlib/qsort/
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

  // backtracking main part
  void dfs(int cnt, int x, int y) {
      order[cnt - 1].x = x;
      order[cnt - 1].y = y;

      grid[x][y] = cnt;
      // grid is filled
      if (cnt == N * N) {
          print_path(order, cnt);
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

  int main(int argc, char* argv[]) {
      if (argc != 4) {
          printf("Usage: ./Knight.out grid_size StartX StartY");
          exit(-1);
      }

      N = atoi(argv[1]);
      int StartX = atoi(argv[2]);
      int StartY = atoi(argv[3]);
      dfs(1, StartX, StartY);

      printf("No Possible Tour\n");
      return 0;
  }
