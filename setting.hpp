#ifndef SETTING_HPP
#define SETTING_HPP

#include<QString>
#include<vector>

namespace page_s{
inline const int LOGIC = 0;
inline const int SELECT = 1;
inline const int SERVER = 2;
inline const int CLIENT = 3;
inline const int GAME = 4;
inline const int OVER = 5;
}

namespace server_s{
inline const int PORT = 8888;
}

namespace qtoolspool_s{
inline const int maxtools = 9;
inline const int Beer = 0;
inline const int Cigarette = 1;
inline const int Drug = 2;
inline const int Epinephrine = 3;
inline const int Glass = 4;
inline const int Handcuffs = 5;
inline const int Phone = 6;
inline const int Reverser = 7;
inline const int Saw = 8;
}

namespace bullet_s{
inline const int maxbullet = 8;
inline const int minbullet = 4;
}

namespace player_s{
inline const int blood = 5;
inline const int maxtools = 8;
}

namespace operation_s{
inline const QString choosefirst = "fir";
inline const QString sendname = "nam";
inline const QString makeqtool = "mak";
inline const QString useqtool = "tol";
inline const QString useqgun = "gun";
inline const QString setbullets = "bul";
}

namespace qtoolway{
inline std::vector<int> way = {0,50,100,150,200,250,300,350};
inline int w = 40;
inline int h = 60;
}

/*
 * 1 0都是以自己的视角看的
 * fir + 0/1 判断先后手 ，1 自己 0 他人
 * nam + QString 发送姓名
 * mak + 道具码;
 * tol + who + index使用道具
 * gun + shootwho
 * bul + 子弹数 + 子弹
 * gov + 0/1  游戏结束   /1自己失败，0对面失败
 *
 *
 *1.互发名字
 *2."被动连接"抽签
 *3.抽道具(根据子弹数)
 *4.开始对战
 *
 *
 *注意：主动造成的影响自己负责，被动造成的影响别人负责
 *比如说自己开枪造成弹夹空了，要负责装弹，反之不用
 *
 *各operation间用'|'分割
 *
 */



#endif // SETTING_HPP
