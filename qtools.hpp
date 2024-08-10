#ifndef QTOOLS_H
#define QTOOLS_H

#include<QPushButton>
//#include<QFrame>
#include"setting.hpp"

class QTool:public QPushButton{
public:
    int index;//道具坐标
    int who;//道具所有者
    QTool(int i,int w,QWidget *parent = nullptr):index(i),who(w),QPushButton(parent){
        if(parent == nullptr){
            qDebug()<<"parent is null";
            std::terminate();
        }
        this->resize(qtoolway::w,qtoolway::h);
        this->move(qtoolway::way[index],0);
        this->show();
    }
};

class QToolSaw:public QTool{
public:
    QToolSaw(int i,int w,QWidget *parent = nullptr):QTool(i,w,parent){
        setText("锯子");
    }
};

class QToolPhone:public QTool{
public:
    QToolPhone(int i,int w,QWidget *parent = nullptr):QTool(i,w,parent){
        setText("手机");
    }
};

class QToolHandcuffs:public QTool{
public:
    QToolHandcuffs(int i,int w,QWidget *parent = nullptr):QTool(i,w,parent){
        setText("手铐");
    }
};

class QToolBeer:public QTool{
public:
    QToolBeer(int i,int w,QWidget *parent = nullptr):QTool(i,w,parent){
        setText("啤酒");
    }
};

class QToolCigarette:public QTool{
public:
    QToolCigarette(int i,int w,QWidget *parent = nullptr):QTool(i,w,parent){
        setText("香烟");
    }
};

class QToolGlass:public QTool{
public:
    QToolGlass(int i,int w,QWidget *parent = nullptr):QTool(i,w,parent){
        setText("放大镜");
    }
};

class QToolReverser:public QTool{
public:
    QToolReverser(int i,int w,QWidget *parent = nullptr):QTool(i,w,parent){
        setText("转换器");
    }
};

class QToolEpinephrine:public QTool{
public:
    QToolEpinephrine(int i,int w,QWidget *parent = nullptr):QTool(i,w,parent){
        setText("肾上腺素");
    }
};

class QToolDrug:public QTool{
public:
    QToolDrug(int i,int w,QWidget *parent = nullptr):QTool(i,w,parent){
        setText("药品");
    }
};

#endif // QTOOLS_H
