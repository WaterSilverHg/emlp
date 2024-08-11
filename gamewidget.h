#ifndef GAMEWIDGET_H
#define GAMEWIDGET_H

#include <QWidget>
#include<QTcpSocket>
#include<QTcpServer>
#include<QMessageBox>
#include<QBitArray>
#include <QRandomGenerator>

#include"setting.hpp"
#include"player.hpp"

QT_BEGIN_NAMESPACE
namespace Ui { class GameWidget; }
QT_END_NAMESPACE

class GameWidget : public QWidget
{
    Q_OBJECT

public:
    GameWidget(QWidget *parent = nullptr);
    ~GameWidget();

private:
    Player p1;//1
    Player p2;//0
    int round;
    QBitArray qtpool;//道具池
    QVector<bool> bullets;//弹夹
    QString message;//消息

    Ui::GameWidget *ui;

    QTcpServer* tcpserver;
    QTcpSocket* tcpsocket;

    bool isServer = 0;

    void startGame();//游戏开始，负责初始化
    void initSocket();
    void initPlayer();//初始化玩家属性
    void chooseFirst();//选择先后手
    void setQToolspool();
    void setBullets(const QString& = QString());
    void makeQTools(const QString& = QString());
    void useQTools(const QString& = QString());
    void useQTools(QTool*);
    void shoot(const QString&);
    void useQGun(const QString& = QString());
    void showMessage();
    void showBlood();
    void gameover();
    void showWin();
    void showDead();
    void dealDatagram();

};
#endif // GAMEWIDGET_H
