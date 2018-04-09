#include<iostream>
#include<algorithm>
#include<limits>
#include<cassert>
#include<cmath>
#include<ctime>
#include<cstdlib>
#include<vector>
#include<iomanip>
#include"include/matrix.h"
#include"include/my_point.h"

using namespace std;
 
const int P=100;        //输入样本的数量
vector<Point> X(P);  //输入样本
Matrix<double> Y(P,1);        //输入样本对应的期望输出
const int M=10;         //隐藏层节点数目
vector<Point> center(M);       //M个Green函数的数据中心
vector<double> delta(M);        //M个Green函数的扩展常数
Matrix<double> Green(P,M);         //Green矩阵
Matrix<double> Weight(M,1);       //权值矩阵
 
/*Hermit多项式函数*/
inline double Hermit(double x, double x2){
    return 1.1*(1-x+2*x*x)*exp(-1*x*x/2) + 1.2*(1-x2+2*x2*x2)*exp(-1*x2*x2/2);
}
 
/*产生指定区间上均匀分布的随机数*/
inline double uniform(double floor,double ceil){
    return floor+1.0*rand()/RAND_MAX*(ceil-floor);
}
 
/*产生区间[floor,ceil]上服从正态分布N[mu,sigma]的随机数*/
inline double RandomNorm(double mu,double sigma,double floor,double ceil){
    double x,prob,y;
    do{
        x=uniform(floor,ceil);
        prob=1/sqrt(2*M_PI*sigma)*exp(-1*(x-mu)*(x-mu)/(2*sigma*sigma));
        y=1.0*rand()/RAND_MAX;
    }while(y>prob);
    return x;
}
 
/*产生输入样本*/
//需要重写，不准
void generateSample(){
    for(int i=0;i<P;++i){
        double in=uniform(-4,4), in2 = uniform(-4,4); // 两个正态分布的坐标 -> 二维正态分布？ 可能不太对
        Point temp(in,in2);
        X[i] = temp;
        Y.put(i,0,Hermit(in, in2) + RandomNorm(0,0.1,-0.2,0.2));
    }
}
 
/*寻找样本离哪个中心最近*/
int nearest(const vector<Point>& center,Point sample){
    int rect=-1;
    double dist=numeric_limits<double>::max();
    for(int i=0;i<center.size();++i){
        if(sample.Point_dst(center[i])<dist){
            dist=sample.Point_dst(center[i]);
            rect=i;
        }
    }
    return rect;
}
 
/*计算簇的质心*/
Point calCenter(const vector<Point> &g){
    int len=g.size();
    double x_sum=0.0, y_sum=0.0;
    for(int i=0;i<len;++i)
        {
            x_sum+= g[i].getX();
            y_sum+= g[i].getY();
        }
    Point temp(x_sum/len,y_sum/len);
    return temp;
}
 
/*KMeans聚类法产生数据中心*/
void KMeans(){
    assert(P%M==0);
    vector<vector<Point> > group(M);          //记录各个聚类中包含哪些样本
    double gap=0.001;       //聚类中心的改变量小于为个值时，迭代终止
    for(int i=0;i<M;++i){   //从P个输入样本中随机选P个作为初始聚类中心
        center[i]=X[10*i+3];     //输入是均匀分布的，所以我们均匀地选取
    }
    while(1){
        for(int i=0;i<M;++i)
            group[i].clear();   //先清空聚类信息
        for(int i=0;i<P;++i){       //把所有输入样本归到对应的簇
            int c=nearest(center,X[i]);
            group[c].push_back(X[i]);
        }
        vector<Point> new_center(M);       //存储新的簇心
        for(int i=0;i<M;++i){
            vector<Point> g=group[i];
            new_center[i]=calCenter(g);
        }
        bool flag=false;
        for(int i=0;i<M;++i){       //检查前后两次质心的改变量是否都小于gap
            if(fabs(new_center[i].Point_dst(center[i]))>gap){
                flag=true;
                break;
            }
        }
        center = new_center;
        if(!flag)
            break;
    }
}
 
/*生成Green矩阵*/
void calGreen(){
    for(int i=0;i<P;++i){
        for(int j=0;j<M;++j){
            Green.put(i,j,exp(-1.0*(X[i].Point_dst(center[j]))*(X[i].Point_dst(center[j]))/(2*delta[j]*delta[j])));
        }
    }
}
 
/*求一个矩阵的伪逆*/
Matrix<double> getGereralizedInverse(const Matrix<double> &matrix){
    return (matrix.getTranspose()*matrix).getInverse()*(matrix.getTranspose());
}
 
/*利用已训练好的神经网络，由输入得到输出*/
double getOutput(Point x){
    double y=0.0;
    for(int i=0;i<M;++i)
        y+=Weight.get(i,0)*exp(-1.0*(x.Point_dst(center[i]))*(x.Point_dst(center[i]))/(2*delta[i]*delta[i]));
    return y;
}
 
int main(int argc,char *argv[]){
    srand(time(0));
    generateSample();       //产生输入和对应的期望输出样本
    KMeans();           //对输入进行聚类，产生聚类中心
    sort(center.begin(),center.end(),[](const Point &p1, const Point &p2)
    {
        if(p1.mod2() >= p2.mod2()) return true;
        else return false;
    });      //对聚类中心（point(x,y)转成模长一维数据）进行排序
     
    //根据聚类中心间的距离，计算各扩展常数
    delta[0]=center[1].Point_dst(center[0]);       
    delta[M-1]=center[M-1].Point_dst(center[M-2]);
    for(int i=1;i<M-1;++i){
        double d1=center[i].Point_dst(center[i-1]);
        double d2=center[i+1].Point_dst(center[i]);
        delta[i]=d1<d2?d1:d2;
    }
     
    calGreen();     //计算Green矩阵
    Weight=getGereralizedInverse(Green)*Y;      //计算权值矩阵
     
    //根据已训练好的神经网络作几组测试
    for(int x=-4;x<5;++x){
        for (int y = -4; y<5; ++y)
        {
            Point test_point(x,y);
            cout<<test_point<<"\t";
            cout<<setprecision(8)<<setiosflags(ios::left)<<setw(15);
            cout<<getOutput(test_point)<<Hermit(x,y)<<endl;      //先输出我们预测的值，再输出真实值
        }
    }
    return 0;
}