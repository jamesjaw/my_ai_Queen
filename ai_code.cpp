#include <iostream>
#include <fstream>
#include <array>
#include <vector>
#include <cstdlib>
#include <ctime>

#include <sstream>

using namespace std;


int value_map[64] = {90,-60,10,10,10,10,-60,90,
                     -60,-80,5,5,5,5,-80,-60,
                     10,5,1,1,1,1,5,10,
                     10,5,1,1,1,1,5,10,
                     10,5,1,1,1,1,5,10,
                     10,5,1,1,1,1,5,10,
                     -60,-80,5,5,5,5,-80,-60,
                     90,-60,10,10,10,10,-60,90};

//===================================================
struct Point {
    int x, y;
    Point() : Point(0, 0) {}
    Point(float x, float y) : x(x), y(y) {}
    bool operator==(const Point& rhs) const {
        return x == rhs.x && y == rhs.y;
    }
    bool operator!=(const Point& rhs) const {
        return !operator==(rhs);
    }
    Point operator+(const Point& rhs) const {
        return Point(x + rhs.x, y + rhs.y);
    }
    Point operator-(const Point& rhs) const {
        return Point(x - rhs.x, y - rhs.y);
    }
};

int player;
const int SIZE = 8;
std::array<std::array<int, SIZE>, SIZE> board;
std::vector<Point> next_valid_spots;

class OthelloBoard {
public:
    enum SPOT_STATE {
        EMPTY = 0,
        BLACK = 1,
        WHITE = 2
    };
    static const int SIZE = 8;
    const std::array<Point, 8> directions{{
        Point(-1, -1), Point(-1,0), Point(-1, 1),
        Point(0, -1)              , Point(0, 1),
        Point(1, -1),  Point(1, 0), Point(1, 1)
    }};
    std::array<std::array<int, SIZE>, SIZE> board;
    std::vector<Point> next_valid_spots;
    std::array<int, 3> disc_count;
    int cur_player;


    Point my_pick_is;

    OthelloBoard* parent = nullptr;
    std::vector<OthelloBoard> child;

private:
    int get_next_player(int player) const {
        return 3 - player;
    }
    bool is_spot_on_board(Point p) const {
        return 0 <= p.x && p.x < SIZE && 0 <= p.y && p.y < SIZE;
    }
    int get_disc(Point p) const {
        return board[p.x][p.y];
    }
    void set_disc(Point p, int disc) {
        board[p.x][p.y] = disc;
    }
    bool is_disc_at(Point p, int disc) const {
        if (!is_spot_on_board(p))
            return false;
        if (get_disc(p) != disc)
            return false;
        return true;
    }
    bool is_spot_valid(Point center) const {
        if (get_disc(center) != EMPTY)
            return false;
        for (Point dir: directions) {
            // Move along the direction while testing.
            Point p = center + dir;
            if (!is_disc_at(p, get_next_player(cur_player)))
                continue;
            p = p + dir;
            /// check is other side have my chest
            while (is_spot_on_board(p) && get_disc(p) != EMPTY) {
                if (is_disc_at(p, cur_player))
                    return true;
                p = p + dir;
            }
        }
        return false;
    }
    void flip_discs(Point center) {
        for (Point dir: directions) {
            // Move along the direction while testing.
            Point p = center + dir;
            if (!is_disc_at(p, get_next_player(cur_player)))
                continue;
            std::vector<Point> discs({p});
            p = p + dir;
            while (is_spot_on_board(p) && get_disc(p) != EMPTY) {
                if (is_disc_at(p, cur_player)) {
                    for (Point s: discs) {
                        set_disc(s, cur_player);
                    }
                    disc_count[cur_player] += discs.size();
                    disc_count[get_next_player(cur_player)] -= discs.size();
                    break;
                }
                discs.push_back(p);
                p = p + dir;
            }
        }
    }
public:
    OthelloBoard(){

    }
    //consturct
    OthelloBoard(std::array<std::array<int, SIZE>, SIZE> a) {
        for(int i=0;i<SIZE;i++){
            for(int j=0;j<SIZE;j++){
                board[i][j] = a[i][j];
                disc_count[board[i][j]]++;
            }
        }
        cur_player = (disc_count[WHITE] + disc_count[BLACK])%2 == 0? BLACK:WHITE;
        next_valid_spots = get_valid_spots();

    }

    //copy
    OthelloBoard(const OthelloBoard& a){
        for(int i=0;i<SIZE;i++){
            for(int j=0;j<SIZE;j++){
                board[i][j] = a.board[i][j];
            }
        }
        disc_count[EMPTY] = a.disc_count[EMPTY];
        disc_count[BLACK] = a.disc_count[BLACK];
        disc_count[WHITE] = a.disc_count[WHITE];
        cur_player = a.cur_player;


        //remember to update next_valid_spots

    }
    //count Q_value
    int set_Q_value(){
        int Q = 0;
        int my_Q = 0;
        int you_Q = 0;
        for(int i=0;i<SIZE;i++){
            for(int j=0;j<SIZE;j++){
                if(board[i][j] == EMPTY){
                    //do nothing
                }
                else if(board[i][j] == player){
                    my_Q += value_map[i*8 + j];
                }
                else{
                    you_Q += value_map[i*8 + j];
                }
            }
        }
        Q = my_Q - you_Q;
        //
        Q += next_valid_spots.size() * 3;
        //
        return Q;
    }

    std::vector<Point> get_valid_spots() const {
        std::vector<Point> valid_spots;
        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                Point p = Point(i, j);
                if (board[i][j] != EMPTY)
                    continue;
                if (is_spot_valid(p))
                    valid_spots.push_back(p);
            }
        }
        return valid_spots;
    }
    bool put_disc(Point p) {
        if(!is_spot_valid(p)) {
            return false;
        }
        set_disc(p, cur_player);
        disc_count[cur_player]++;
        disc_count[EMPTY]--;
        flip_discs(p);
        // Give control to the other player.
        cur_player = get_next_player(cur_player);
        // find where can put chese
        next_valid_spots = get_valid_spots();

        return true;
    }

};


//find leaf and count Q_value
int ABPminimax(OthelloBoard& node ,int depth,int A,int B, bool MorU){
    if(depth == 0 || node.next_valid_spots.empty()){
        cout<<node.set_Q_value()<<"\n";
        return node.set_Q_value();
    }
    //my turn
    if(MorU){
        int Q = -214700000;
        for(auto& it:node.next_valid_spots){
            OthelloBoard child(node);
            child.put_disc(it);
            int child_value = ABPminimax(child, depth - 1,A, B, false);
            Q = std::max(Q, child_value);

            A = std::max(A, child_value);
            if(B <= A){
                break;
            }
        }
        return Q;
    }

    //you turn
    else{
        int Q = 214700000;
        for(auto& it:node.next_valid_spots){
            OthelloBoard child(node);
            child.put_disc(it);
            int child_value = ABPminimax(child, depth - 1,A, B, true);
            Q = std::min(Q, child_value);

            B = std::min(B, child_value);
            if(B <= A){
                break;
            }
        }
        return Q;
    }

}

Point Queen(){
    Point p;
    //pick
    p = next_valid_spots[0];
    int maxQ = -214700000;
    for(auto& it:next_valid_spots){
        if((it.x == 0 && it.y == 0) || (it.x == 0 && it.y == 7) || (it.x == 7 && it.y == 0) || (it.x == 7 && it.y == 7)){
            return it;
        }
        OthelloBoard next(board);
        next.put_disc(it);
        int child_Q = ABPminimax(next,3,-214700000,214700000,false);
        if(maxQ <= child_Q){
            maxQ = child_Q;
            p = it;
        }
    }
    return p;
}





void read_board(std::ifstream& fin) {
    fin >> player;
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            fin >> board[i][j];
        }
    }
}

void read_valid_spots(std::ifstream& fin) {
    int n_valid_spots;
    fin >> n_valid_spots;
    float x, y;
    for (int i = 0; i < n_valid_spots; i++) {
        fin >> x >> y;
        next_valid_spots.push_back({x, y});
    }
}

void write_valid_spot(std::ofstream& fout) {
    int n_valid_spots = next_valid_spots.size();
    srand(time(NULL));
    // Choose random spot. (Not random uniform here)
    //int index = (rand() % n_valid_spots);
    //Point p = next_valid_spots[index];
    //fout << p.x << " " << p.y << std::endl;
    ///The Queen's Gambit

    OthelloBoard root(board);

    root.get_valid_spots();


    Point anw = Queen();
    fout << anw.x << " " << anw.y << std::endl;


    // Remember to flush the output to ensure the last action is written to file.
    //fout << p.x << " " << p.y << std::endl;


    fout.flush();
}


int main(int, char** argv) {
    std::ifstream fin(argv[1]);
    std::ofstream fout(argv[2]);
    read_board(fin);
    read_valid_spots(fin);
    write_valid_spot(fout);
    fin.close();
    fout.close();
    return 0;
}

/*
int main(){
    cin>>player;
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            cin >> board[i][j];
        }
    }
    OthelloBoard root(board);
    ABPminimax(root, 7, -214700000, 214700000, true);
    return 0;
}
*/
