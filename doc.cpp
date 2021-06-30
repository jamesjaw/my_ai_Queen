沿用助教的code (main+random)功能完整太棒了:
主教的code說明書:
0 = 空格
1 = 黑
2 = 白

public:
void rest() : 重置

vector<Point> get_valid_spots() const : 列出可以下的位置
    bool is_spot_valid(Point c) const : 檢查這個點c是不是可以下的位置
        int get_disc(Point p) const : 回覆p是黑or白or空位
        bool is_disc_at(Point p, int disc) : p 是不是disc（黑or白or空）
            bool is_spot_on_board(Point p) const : p是否在棋盤上

bool put_disc(Point p) : 在p下子，如果不合法就輸了
    void set_disc(Point p, int disc) : 在p下 disc(黑or白)
    disc_count[xxx]: 紀錄黑白空的數目
    void flip_discs(Point c) : 下了c子看看能不能翻別人的棋子
    int get_next_player(int p) const : 換人


ptt黑白棋基礎教學:
1.
https://www.ptt.cc/bbs/othello/M.1298698178.A.760.html
2.
https://www.ptt.cc/bbs/othello/M.1299304992.A.54A.html
3.
https://www.ptt.cc/bbs/othello/M.1300646760.A.440.html

abp參考:
https://www.youtube.com/watch?v=l-hh51ncgDI

估值方式參考:
https://zhuanlan.zhihu.com/p/35121997

我採用的估值方法:
估值表(前期,後期):評估場上局勢
活動度(前,中,後):雙方能下的位置差距(我-你)
穩定子:不能被翻轉的子很重要

搜尋樹使用阿發貝塔剪枝來避免無用之搜索
