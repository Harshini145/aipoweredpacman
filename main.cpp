#include <iostream>
#include <conio.h>
#include <windows.h>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>
using namespace std;

struct Position {
    int y, x;
    bool operator==(const Position& other) const {
        return y == other.y && x == other.x;
    }
    bool operator!=(const Position& other) const {
        return !(*this == other);
    }
};

enum Direction { UP, DOWN, LEFT, RIGHT, NONE };

struct Ghost {
    Position pos;
    char symbol;
    string name;
    Direction dir;
};

vector<string> easyMaze = {
    "####################",
    "#P...     #     ...#",
    "# # # ### # ### # #",
    "# #     #   #     #",
    "# ### ##### # ### #",
    "#     #   # #     #",
    "# ##### # # ##### #",
    "#     #   #   #   #",
    "# ... ##### ### ...#",
    "####################"
};

vector<string> mediumMaze = {
    "####################",
    "#P..#    #     .. .#",
    "# ## ### # ### ## ##",
    "#    #       #     #",
    "# ### ####### ### ##",
    "#     #   #   #    #",
    "# ### # # # ### ## #",
    "#   #     #     #  #",
    "# ...### ##### ...##",
    "####################"
};

vector<string> hardMaze = {
    "####################",
    "#P.#   #   #  ...# #",
    "# ## # # # # ### # #",
    "#  # #   #   #    ##",
    "## ### ##### ### ###",
    "#     #   #   #    #",
    "# ### # # # ### ## #",
    "#  #       #    #  #",
    "# ...##### ### ...##",
    "####################"
};

vector<string> maze;
Position pacman;
int score = 0;
vector<Ghost> ghosts;

bool isValidMove(int y, int x) {
    return y >= 0 && y < (int)maze.size() && x >= 0 && x < (int)maze[0].size() && maze[y][x] != '#';
}

Position moveInDirection(Position pos, Direction d) {
    Position np = pos;
    switch (d) {
        case UP: np.y--; break;
        case DOWN: np.y++; break;
        case LEFT: np.x--; break;
        case RIGHT: np.x++; break;
        default: break;
    }
    return np;
}

void findPacman() {
    for (int y = 0; y < (int)maze.size(); y++) {
        for (int x = 0; x < (int)maze[y].size(); x++) {
            if (maze[y][x] == 'P') {
                pacman = { y, x };
                return;
            }
        }
    }
}

bool dotsLeft() {
    for (auto &row : maze)
        for (char c : row)
            if (c == '.') return true;
    return false;
}

Position bfsNextStep(Position start, Position target) {
    int H = (int)maze.size();
    int W = (int)maze[0].size();
    vector<vector<bool>> visited(H, vector<bool>(W, false));
    vector<vector<Position>> parent(H, vector<Position>(W, {-1, -1}));

    vector<Position> q;
    q.push_back(start);
    visited[start.y][start.x] = true;

    int front = 0;
    bool found = false;

    int dy[4] = { -1, 1, 0, 0 };
    int dx[4] = { 0, 0, -1, 1 };

    while (front < (int)q.size()) {
        Position cur = q[front++];
        if (cur == target) {
            found = true;
            break;
        }
        for (int i = 0; i < 4; i++) {
            int ny = cur.y + dy[i];
            int nx = cur.x + dx[i];
            if (ny >= 0 && ny < H && nx >= 0 && nx < W && !visited[ny][nx] && maze[ny][nx] != '#') {
                visited[ny][nx] = true;
                parent[ny][nx] = cur;
                q.push_back({ ny, nx });
            }
        }
    }

    if (!found) return start;

    Position cur = target;
    Position prev = parent[cur.y][cur.x];
    while (prev != start) {
        cur = prev;
        prev = parent[cur.y][cur.x];
    }
    return cur;
}


void moveBlinky(Ghost &g) {
    if (rand() % 100 < 30) return; 
    Position nextPos = bfsNextStep(g.pos, pacman);
    g.pos = nextPos;
}

void moveGreeny(Ghost &g, Direction pacDir) {
    if (rand() % 100 < 30) return;
    Position target = pacman;
    for (int i = 0; i < 4; i++) {
        Position next = moveInDirection(target, pacDir);
        if (isValidMove(next.y, next.x)) target = next;
        else break;
    }
    Position nextPos = bfsNextStep(g.pos, target);
    g.pos = nextPos;
}

void moveInky(Ghost &g, Position blinkyPos, Direction pacDir) {
    Position tileAhead = pacman;
    for (int i = 0; i < 2; i++) {
        Position next = moveInDirection(tileAhead, pacDir);
        if (isValidMove(next.y, next.x)) tileAhead = next;
        else break;
    }
    int targetY = tileAhead.y + (tileAhead.y - blinkyPos.y);
    int targetX = tileAhead.x + (tileAhead.x - blinkyPos.x);

    if (targetY < 0) targetY = 0;
    if (targetY >= (int)maze.size()) targetY = (int)maze.size() - 1;
    if (targetX < 0) targetX = 0;
    if (targetX >= (int)maze[0].size()) targetX = (int)maze[0].size() - 1;

    Position target = { targetY, targetX };
    if (!isValidMove(target.y, target.x)) target = pacman;

    Position nextPos = bfsNextStep(g.pos, target);
    g.pos = nextPos;
}

void moveClyde(Ghost &g) {
    int distY = abs(g.pos.y - pacman.y);
    int distX = abs(g.pos.x - pacman.x);
    int dist = distY + distX;

    if (dist > 8) {
        if (rand() % 100 < 60) {
            Position nextPos = bfsNextStep(g.pos, pacman);
            g.pos = nextPos;
            return;
        }
    }

    if (rand() % 100 < 75) {
        vector<Position> candidates;
        int dy[4] = { -1, 1, 0, 0 };
        int dx[4] = { 0, 0, -1, 1 };
        for (int i = 0; i < 4; i++) {
            int ny = g.pos.y + dy[i];
            int nx = g.pos.x + dx[i];
            if (isValidMove(ny, nx)) candidates.push_back({ ny, nx });
        }
        if (!candidates.empty()) {
            int idx = rand() % candidates.size();
            g.pos = candidates[idx];
            return;
        }
    }
    
}

void moveGhosts(Direction pacDir, int tick) {
    if (tick % 2 != 0) return; 

    Position blinkyPos;
    for (auto &gl : ghosts) if (gl.name == "Blinky") blinkyPos = gl.pos;

    for (auto &g : ghosts) {
        if (g.name == "Blinky") moveBlinky(g);
        else if (g.name == "Greeny") moveGreeny(g, pacDir);
        else if (g.name == "Inky") moveInky(g, blinkyPos, pacDir);
        else if (g.name == "Clyde") moveClyde(g);
    }
}

bool checkCollision() {
    for (auto &g : ghosts)
        if (g.pos == pacman) return true;
    return false;
}

void clearScreen() {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    DWORD count;
    DWORD cellCount;
    COORD homeCoords = {0, 0};

    if (hConsole == INVALID_HANDLE_VALUE) return;
    if (!GetConsoleScreenBufferInfo(hConsole, &csbi)) return;

    cellCount = csbi.dwSize.X * csbi.dwSize.Y;

    FillConsoleOutputCharacter(hConsole, (TCHAR) ' ', cellCount, homeCoords, &count);
    FillConsoleOutputAttribute(hConsole, csbi.wAttributes, cellCount, homeCoords, &count);
    SetConsoleCursorPosition(hConsole, homeCoords);
}

void setColor(int color) {
    static HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, color);
}

void drawMaze() {
    vector<string> display = maze; 

    display[pacman.y][pacman.x] = 'P';
    for (auto &g : ghosts)
        display[g.pos.y][g.pos.x] = g.symbol;

    clearScreen();

    cout << "Score: " << score << "\n\n";

    for (int y = 0; y < (int)display.size(); y++) {
        for (int x = 0; x < (int)display[y].size(); x++) {
            char c = display[y][x];
            switch (c) {
                case '#': setColor(1); cout << "\xDB"; break;  
                case 'P': setColor(14); cout << "P"; break;     
                case 'B': setColor(12); cout << "B"; break;
                case 'G': setColor(10); cout << "G"; break;
                case 'I': setColor(11); cout << "I"; break;
                case 'C': setColor(13); cout << "C"; break;
                case '.': setColor(7); cout << "."; break;
                case ' ': setColor(0); cout << " "; break;
                default: setColor(7); cout << c; break;
            }
        }
        cout << "\n";
    }
    setColor(7); 
}

int main() {
    srand((unsigned)time(NULL));

    cout << "Select difficulty level (e - easy, m - medium, h - hard): ";
    char lvl; cin >> lvl;

    if (lvl == 'e' || lvl == 'E') maze = easyMaze;
    else if (lvl == 'm' || lvl == 'M') maze = mediumMaze;
    else maze = hardMaze;

    findPacman();

    ghosts.clear();
    ghosts.push_back({ {8, 9}, 'B', "Blinky", NONE });
    ghosts.push_back({ {8, 10}, 'G', "Greeny", NONE });
    ghosts.push_back({ {1, 18}, 'I', "Inky", NONE });
    ghosts.push_back({ {1, 9}, 'C', "Clyde", NONE });

    drawMaze();

    Direction pacDir = NONE;
    int tick = 0;

    while (true) {
        if (_kbhit()) {
            int ch = _getch();

            if (ch == 0 || ch == 224) {
                int arrow = _getch();
                switch (arrow) {
                    case 72: pacDir = UP; break;
                    case 80: pacDir = DOWN; break;
                    case 75: pacDir = LEFT; break;
                    case 77: pacDir = RIGHT; break;
                }
            } else {
                if (ch == 'q') break;
            }

            Position nextPos = moveInDirection(pacman, pacDir);
            if (isValidMove(nextPos.y, nextPos.x)) {
                if (maze[nextPos.y][nextPos.x] == '.') {
                    score += 10;
                    maze[nextPos.y][nextPos.x] = ' ';
                }
                pacman = nextPos;
            }
            drawMaze();
        }

        moveGhosts(pacDir, tick);

        if (checkCollision()) {
            drawMaze();
            cout << "\nGame Over! You were caught by a ghost.\nFinal Score: " << score << "\n";
            break;
        }

        if (!dotsLeft()) {
            drawMaze();
            cout << "\nCongratulations! You ate all the dots.\nFinal Score: " << score << "\n";
            break;
        }

        drawMaze();
        tick++;
        Sleep(200);
    }

    return 0;
}
