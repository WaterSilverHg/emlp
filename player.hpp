#ifndef PLAYER_HPP
#define PLAYER_HPP
#include<QString>
#include<QVector>

#include"qtools.hpp"

class Player{
public:
    QString username;//名字
    int nexthurt;//下次开枪造成的伤害
    int blood;//当前血量
    int handcuffed = 0;//是否被拷住 2 正在被拷 1 无法被拷 0 可以被拷
    QVector<QTool*> tools;
};

#endif // PLAYER_HPP
