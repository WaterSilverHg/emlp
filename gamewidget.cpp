#include "gamewidget.h"
#include "ui_gamewidget.h"

GameWidget::GameWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::GameWidget)
{
    ui->setupUi(this);

    tcpsocket = nullptr;
    tcpserver = nullptr;

    connect(ui->login_PB,&QPushButton::clicked,[this](){//输入昵称
        p1.username = ui->login_LE->text();
        if(p1.username.isEmpty()){
            QMessageBox::information(this,"提示","昵称不能为空");
            return;
        }
        ui->stackedWidget->setCurrentIndex(page_s::SELECT);
    });
    connect(ui->s_server_PB,&QPushButton::clicked,[this](){//被动连接
        tcpserver = new QTcpServer(this);
        tcpserver->listen(QHostAddress::Any,server_s::PORT);
        connect(tcpserver,&QTcpServer::newConnection,[this](){
            if(tcpsocket != nullptr){
                tcpsocket->close();
                delete tcpsocket;
                tcpsocket = nullptr;
            }
            tcpsocket = tcpserver->nextPendingConnection();
            //QMessageBox::information(this,"提示","连接成功");
            ui->stackedWidget->setCurrentIndex(page_s::GAME);
            startGame();
        });
        ui->stackedWidget->setCurrentIndex(page_s::SERVER);
    });
    connect(ui->serverRe_PB,&QPushButton::clicked,[this](){
        if(tcpserver != nullptr){
            tcpserver->close();
            delete tcpserver;
            tcpserver = nullptr;
        }
        ui->stackedWidget->setCurrentIndex(page_s::SELECT);
    });
    connect(ui->s_client_PB,&QPushButton::clicked,[this](){//主动连接
        if(tcpsocket != nullptr){
            tcpsocket->close();
        }
        else tcpsocket = new QTcpSocket(this);
        ui->stackedWidget->setCurrentIndex(page_s::CLIENT);
    });
    connect(ui->client_PB,&QPushButton::clicked,[this](){//发起连接
        tcpsocket->connectToHost(QHostAddress(ui->client_LE->text()),server_s::PORT);
        connect(tcpsocket,&QTcpSocket::connected,[this](){
            //QMessageBox::information(this,"提示","连接成功");
            ui->stackedWidget->setCurrentIndex(page_s::GAME);
            startGame();
        });
        connect(tcpsocket,&QTcpSocket::errorOccurred,[this](){
            QMessageBox::information(this,"提示","连接失败");
        });
    });
    connect(ui->clientRe_PB,&QPushButton::clicked,[this](){
        if(tcpsocket != nullptr){
            tcpsocket->close();
            delete tcpsocket;
            tcpsocket = nullptr;
        }
        ui->stackedWidget->setCurrentIndex(page_s::SELECT);
    });
    connect(ui->shootp1_PB,&QPushButton::clicked,[this](){
        shoot("1");
    });
    connect(ui->shootp2_PB,&QPushButton::clicked,[this](){
        shoot("0");
    });
}

GameWidget::~GameWidget()
{
    if(tcpserver != nullptr){
        tcpserver->close();
        delete tcpserver;
        tcpserver = nullptr;
    };
    if(tcpsocket != nullptr){
        tcpsocket->close();
        delete tcpsocket;
        tcpsocket = nullptr;
    }
    delete ui;
}

void GameWidget::startGame()
{
    initSocket();
    initPlayer();
    setQToolspool();
    if(tcpserver){
        chooseFirst();
        setBullets();//makeQTools放到了里面
        //tcpserver->close();
        //delete tcpserver;
        //tcpserver = nullptr;
    }
    //makeQTools();
}

void GameWidget::initSocket()
{
    if(tcpsocket == nullptr)std::terminate();
    connect(tcpsocket,&QTcpSocket::readyRead,[this](){
        dealDatagram();
    });
    connect(tcpsocket,&QTcpSocket::disconnected,[this](){
        QMessageBox::information(this,"提示","对方断开连接");
        if(tcpserver != nullptr){
            tcpserver->close();
            delete tcpserver;
            tcpserver = nullptr;
        }
        ui->stackedWidget->setCurrentIndex(page_s::SELECT);
    });
    _sleep(100);
    tcpsocket->write((operation_s::sendname + p1.username + '|').toUtf8());
}

void GameWidget::initPlayer()
{
    p1.blood = p2.blood = player_s::blood;
    p1.tools.resize(player_s::maxtools);
    for(auto& t:p1.tools)
        if(t){
            delete t;
            t = nullptr;
        }
    p2.tools.resize(player_s::maxtools);
    for(auto& t:p2.tools)
        if(t){
            delete t;
            t = nullptr;
        }
    p1.nexthurt = p2.nexthurt = 1;
    ui->p1name_LE->setText(p1.username);
}

void GameWidget::chooseFirst()
{
    srand(time(0));
    int first = rand()%2;
    tcpsocket->write((operation_s::choosefirst+QString(first?"0":"1")+'|').toUtf8());//0 1 倒转
    round = first;
    message = first?"你先手":"对面先手";
    showMessage();
}

void GameWidget::setQToolspool()
{
    qtpool.resize(qtoolspool_s::maxtools);
    qtpool[qtoolspool_s::Beer] = 1;
    qtpool[qtoolspool_s::Cigarette] = 1;
    qtpool[qtoolspool_s::Saw] = 1;
    qtpool[qtoolspool_s::Glass] = 1;
    qtpool[qtoolspool_s::Handcuffs] = 1;
    qtpool[qtoolspool_s::Drug] = 0;
    qtpool[qtoolspool_s::Phone] = 0;
    qtpool[qtoolspool_s::Reverser] = 0;
    qtpool[qtoolspool_s::Epinephrine] = 0;
}

void GameWidget::setBullets(const QString &content)
{
    int t_b;
    int num;
    if(content.isEmpty()){
        QRandomGenerator *random = QRandomGenerator::global();
        num = random->bounded(bullet_s::minbullet,bullet_s::maxbullet + 1);
        bullets.clear();
        bullets.resize(num);
        t_b = num/2;//实弹的数量
        for(int i = 0;i<t_b;++i){
            int index = random->bounded(num);
            if(!bullets[index]){
                bullets[index] = true;
            }
        }
        QString newop = operation_s::setbullets + QString::number(num);
        for(auto tmp:bullets){
            if(tmp)newop+='1';
            else newop+='0';
        }
        newop+='|';
        tcpsocket->write(newop.toUtf8());
    }
    else{
        num = content[0].digitValue();
        t_b = num/2;
        bullets.resize(num);
        for(int i = 0;i<num;++i){
            bullets[i] = content[i+1].digitValue();
        }
    }
    message = "装弹：共"+QString::number(num)+"发子弹，其中有"+QString::number(t_b)+"发实弹";

    makeQTools();
    showMessage();
}

void GameWidget::makeQTools(const QString &content)
{
    if(round){
        ui->shootp1_PB->setEnabled(true);
        ui->shootp2_PB->setEnabled(true);
    }
    else{
        ui->shootp1_PB->setEnabled(false);
        ui->shootp2_PB->setEnabled(false);
    }
    if(content.isEmpty()){
        QRandomGenerator *random = QRandomGenerator::global();
        int num = bullets.size()/2;
        for(int i = 0;i<num;){
            int emptyindex = -1;
            for(int i = 0;i<player_s::maxtools;++i){
                if(p1.tools[i] == nullptr){
                    emptyindex = i;
                    break;
                }
            }
            if(-1 == emptyindex)return;
            int poolindex = random->bounded(qtoolspool_s::maxtools);
            //qDebug()<<poolindex;
            if(qtpool.at(poolindex)){
                if(poolindex == qtoolspool_s::Beer){
                    p1.tools[emptyindex] = new QToolBeer(emptyindex,1,ui->p1tools_F);
                }
                else if(poolindex == qtoolspool_s::Cigarette){
                    p1.tools[emptyindex] = new QToolCigarette(emptyindex,1,ui->p1tools_F);
                }
                else if(poolindex == qtoolspool_s::Drug){
                    p1.tools[emptyindex] = new QToolDrug(emptyindex,1,ui->p1tools_F);
                }
                else if(poolindex == qtoolspool_s::Epinephrine){
                    p1.tools[emptyindex] = new QToolEpinephrine(emptyindex,1,ui->p1tools_F);
                }
                else if(poolindex == qtoolspool_s::Glass){
                    p1.tools[emptyindex] = new QToolGlass(emptyindex,1,ui->p1tools_F);
                }
                else if(poolindex == qtoolspool_s::Handcuffs){
                    p1.tools[emptyindex] = new QToolHandcuffs(emptyindex,1,ui->p1tools_F);
                }
                else if(poolindex == qtoolspool_s::Phone){
                    p1.tools[emptyindex] = new QToolPhone(emptyindex,1,ui->p1tools_F);
                }
                else if(poolindex == qtoolspool_s::Reverser){
                    p1.tools[emptyindex] = new QToolReverser(emptyindex,1,ui->p1tools_F);
                }
                else if(poolindex == qtoolspool_s::Saw){
                    p1.tools[emptyindex] = new QToolSaw(emptyindex,1,ui->p1tools_F);
                }


                connect(p1.tools[emptyindex],&QPushButton::clicked,[=](){
                    //QMessageBox::information(this,"提示","点击了道具");
                    if(round)useQTools(p1.tools[emptyindex]);
                });
                ++i;
                tcpsocket->write((operation_s::makeqtool+QString::number(poolindex)+'|').toUtf8());
            }
        }
    }
    else{
        int poolindex = content[0].digitValue();
        int emptyindex = -1;
        for(int i = 0;i<player_s::maxtools;++i){
            if(p2.tools[i] == nullptr){
                emptyindex = i;
                break;
            }
        }
        if(poolindex == qtoolspool_s::Beer){
            p2.tools[emptyindex] = new QToolBeer(emptyindex,0,ui->p2tools_F);
        }
        else if(poolindex == qtoolspool_s::Cigarette){
            p2.tools[emptyindex] = new QToolCigarette(emptyindex,0,ui->p2tools_F);
        }
        else if(poolindex == qtoolspool_s::Drug){
            p2.tools[emptyindex] = new QToolDrug(emptyindex,0,ui->p2tools_F);
        }
        else if(poolindex == qtoolspool_s::Epinephrine){
            p2.tools[emptyindex] = new QToolEpinephrine(emptyindex,0,ui->p2tools_F);
        }
        else if(poolindex == qtoolspool_s::Glass){
            p2.tools[emptyindex] = new QToolGlass(emptyindex,0,ui->p2tools_F);
        }
        else if(poolindex == qtoolspool_s::Handcuffs){
            p2.tools[emptyindex] = new QToolHandcuffs(emptyindex,0,ui->p2tools_F);
        }
        else if(poolindex == qtoolspool_s::Phone){
            p2.tools[emptyindex] = new QToolPhone(emptyindex,0,ui->p2tools_F);
        }
        else if(poolindex == qtoolspool_s::Reverser){
            p2.tools[emptyindex] = new QToolReverser(emptyindex,0,ui->p2tools_F);
        }
        else if(poolindex == qtoolspool_s::Saw){
            p2.tools[emptyindex] = new QToolSaw(emptyindex,0,ui->p2tools_F);
        }

        connect(p2.tools[emptyindex],&QPushButton::clicked,[=](){
            if(round)useQTools(p2.tools[emptyindex]);
        });
    }
}

void GameWidget::useQTools(const QString & content)
{
    int who = content[0].digitValue();
    int index = content[1].digitValue();
    Player& p = who?p1:p2;
    useQTools(p.tools[index]);
}

void GameWidget::useQTools(QTool * t_tool)
{
    if(round != t_tool->who){//如果不是自己用自己的道具，那么拜拜
        return;
    }
    message = (t_tool->who?"你":"对面");
    if(typeid(t_tool).hash_code() == typeid(QToolBeer).hash_code()){
        if(bullets.isEmpty())return;
        bool b = bullets.back();
        bullets.pop_back();
        QMessageBox::information(this,"提示","经过此处");
        message += QString("使用了香烟，退掉了一颗") + (b?"实弹":"虚弹");
    }
    else if(typeid(t_tool).hash_code() == typeid(QToolCigarette).hash_code()){
        if(t_tool->who == 1){
            if(p1.blood >= player_s::blood)return;//如果满血，不用
            p1.blood++;
        }
        else{
            if(p2.blood >= player_s::blood)return;
            p2.blood++;
        }
        message += "使用了香烟，回复了1点血";
    }
    else if(typeid(t_tool).hash_code() == typeid(QToolDrug).hash_code()){
        return;//未启用
    }
    else if(typeid(t_tool).hash_code() == typeid(QToolEpinephrine).hash_code()){
        return;//未启用
    }
    else if(typeid(&t_tool).hash_code() == typeid(QToolGlass).hash_code()){
        if(bullets.isEmpty())return;
        message += "使用了放大镜";
        if(t_tool->who){
            message+="下一颗子弹是" + QString(bullets.back()?"实弹":"虚弹");
        }
    }
    else if(typeid(t_tool).hash_code() == typeid(QToolHandcuffs).hash_code()){
        if(t_tool->who){
            p2.handcuffed = 2;
        }
        else{
            p1.handcuffed = 2;
        }
        message += "使用了手铐，" + QString(t_tool->who?"对面":"你") + "无法行动一次";
    }
    else if(typeid(t_tool).hash_code() == typeid(QToolPhone).hash_code()){
        return;//未开发
    }
    else if(typeid(t_tool).hash_code() == typeid(QToolReverser).hash_code()){
        return;//未开发
    }
    else if(typeid(t_tool).hash_code() == typeid(QToolSaw).hash_code()){
        if(t_tool->who){
            if(p1.nexthurt == 1){
                p1.nexthurt = 2;
            }
            else return;
        }
        else{
            if(p2.nexthurt == 1){
                p2.nexthurt = 2;
            }
            else return;
        }
        message += "使用了手锯，下次开枪造成的伤害+1";
    }
    else if(typeid(t_tool).hash_code() == typeid(QTool).hash_code()){
        QMessageBox::information(this,"提示","空道具");
        return;
    }
    else{
        return;
    }

    if(t_tool->who == 1){
        p1.tools[t_tool->index] = nullptr;
    }
    else if(t_tool->who == 0){
        p2.tools[t_tool->index] = nullptr;
    }
    if(round) tcpsocket->write((operation_s::useqtool + QString::number(t_tool->who?0:1) + QString::number(t_tool->index)).toUtf8());
    delete t_tool;
    showMessage();
}

void GameWidget::shoot(const QString & content)
{
    if(round && !bullets.isEmpty())
        useQGun(content);
}

void GameWidget::useQGun(const QString & content)
{
    int shootwho = content[0].digitValue();
    bool b = bullets.back();
    bullets.pop_back();

    if(round)tcpsocket->write((operation_s::useqgun+QString::number(shootwho?0:1) + '|').toUtf8());//0 1 倒转

    if(round){//自己开枪
        message = "你开枪射向了";
        if(shootwho){//射向自己
            message+="自己,";
            if(b){
                p1.blood-=p1.nexthurt;
                round = 0;
            }
            //虚弹不变
        }
        else{
            message+="对面,";
            if(b){
                p2.blood-=p1.nexthurt;
            }
            round = 0;
        }
        if(b){
            message+="是实弹，造成了" + QString::number(p1.nexthurt) + "点伤害";
            if(p2.handcuffed == 2)round = 1;
        }
        else{
            message+="是虚弹";
        }
        p1.nexthurt = 1;
        if(p2.handcuffed>0)p2.handcuffed--;
    }
    else{
        message = "对方开枪射向了";
        if(shootwho){//射向自己
            message+="你,";
            if(b){
                p1.blood-=p2.nexthurt;
            }
            round = 1;
        }
        else{
            message+="他自己,";
            if(b){
                p2.blood-=p2.nexthurt;
                round = 1;
            }
            //虚弹不变
        }
        if(b){
            message+="是实弹，造成了" + QString::number(p2.nexthurt) + "点伤害";
        }
        else{
            message+="是虚弹";
            if(p1.handcuffed == 2)round = 0;
        }
        p2.nexthurt = 1;
        if(p1.handcuffed>0)p1.handcuffed--;
    }
    if(round){
        ui->shootp1_PB->setEnabled(true);
        ui->shootp2_PB->setEnabled(true);
    }
    else{
        ui->shootp1_PB->setEnabled(false);
        ui->shootp2_PB->setEnabled(false);
    }
    showMessage();
    if(round && bullets.isEmpty()){
        setBullets();
        makeQTools();
    }
}

void GameWidget::showMessage()
{
    ui->messagebox->insertHtml(QString::number(round) + "<p>" + message + "<p/><dr/>");
    showBlood();
}

void GameWidget::showBlood()
{
    ui->bloodbox->setHtml("你的血量：" + QString::number(p1.blood) + "| 对面血量：" + QString::number(p2.blood));
}

void GameWidget::dealDatagram()
{
    QList<QByteArray> arraylist = tcpsocket->readAll().split('|');
    for(QByteArray& array:arraylist){
        if(array.isEmpty())return;
        QString op = array.mid(0,3);
        QString content = array.mid(3);
        if(op == operation_s::useqtool){
            useQTools(content);
        }
        else if(op == operation_s::useqgun){
            useQGun(content);
        }
        else if(op == operation_s::makeqtool){
            makeQTools(content);
        }
        else if(op == operation_s::setbullets){
            setBullets(content);
        }
        else if(op == operation_s::choosefirst){
            round = content[0].digitValue();
            message = round?"你先手":"对面先手";
            showMessage();
        }
        else if(op == operation_s::sendname){
            p2.username = content;
            ui->p2name_LE->setText(p2.username);
        }
        else{
            QMessageBox::information(this,"警告",array);
            std::terminate();
        }
    }
}

