#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<limits.h>
#define EUCLID(X,Y) ((X)*(X))+((Y)*(Y))

#ifndef NDEBUG
/* Debug only code */
#endif

typedef struct
{
    int X;
    int Y;
    int Cluster;
} Point;

void KMeans_Inital_Forgy(int P,int K);
int KMeans_GeneratePoints(int N);
int KMeans_Assign();
void KMeans_Update();
int KMeans_Algorithm(int P,int K);

Point MyPoints[1024];
Point MyCentroids[1024];
int PointNumber;
int ClusterNumber=5;



int MyCMP (const void * A, const void * B)
{
    return ( ((Point*)A)->Cluster - ((Point*)B)->Cluster );
}

void KMeans_Inital_RandomPartition(int P,int K)
{
    int I,J,L;
    PointNumber=P;
    ClusterNumber=K;
    Point MyTemp;
    for (I=0; I<P; I++)
    {
        MyPoints[I].Cluster=rand()%K;
    }
    KMeans_Update();
}

void KMeans_Inital_Forgy(int P,int K)
{
    int I,J,L;
    PointNumber=P;
    ClusterNumber=K;
    Point MyTemp;
    for (I=0; I<P; I++)
    {
        L=rand()%P;
        MyTemp=MyPoints[I];
        MyPoints[I]=MyPoints[L];
        MyPoints[L]=MyTemp;
    }

    for (I=0; I<K; I++)
    {
        MyCentroids[I]=MyPoints[I];
    }
}

int KMeans_GeneratePoints(int N)
{
    int I,J,K;
    srand(time(NULL));
    for (I=0; I<N&I<1024; I++)
    {
        MyPoints[I].X=rand()%1000;
        MyPoints[I].Y=rand()%1000;
        MyPoints[I].Cluster=INT_MAX;
    }
    return I;
}

int KMeans_Assign()
{
    int I,J,K;
    int Min,Sum;
    for (I=0,Sum=0; I<PointNumber; I++)
    {
        Min=INT_MAX;
        for (J=0; J<ClusterNumber; J++)
        {
            if (Min>EUCLID(MyPoints[I].X-MyCentroids[J].X,MyPoints[I].Y-MyCentroids[J].Y))
            {
                Min=EUCLID(MyPoints[I].X-MyCentroids[J].X,MyPoints[I].Y-MyCentroids[J].Y);
                MyPoints[I].Cluster=J;
            }
        }
        Sum+=Min;
    }
    return Sum;
}

void KMeans_Update()
{
    int I,J,K;
    int SumY,SumX;
    qsort(MyPoints,PointNumber,sizeof(Point),MyCMP);
    SumX=MyPoints[0].X;
    SumY=MyPoints[0].Y;
    for (I=0,J=1,K=1; I<ClusterNumber; I++)
    {
        while(MyPoints[J-1].Cluster==MyPoints[J].Cluster)
        {
            SumX+=MyPoints[J].X;
            SumY+=MyPoints[J].Y;
            K++;
            J++;
        }
        MyCentroids[I].X=(double)SumX/K; //??
        MyCentroids[I].Y=(double)SumY/K; //??
        SumX=MyPoints[J].X;
        SumY=MyPoints[J].Y;
        K=1;
        J++;
    }
}

int KMeans_Algorithm(int P,int K)
{
    int Sum,OldSum=INT_MAX;
    srand(time(NULL));
    //KMeans_Inital_RandomPartition(P,K);
    while (1)
    {
        Sum=KMeans_Assign();
        if (Sum==OldSum)
        {
            break;
        }
        KMeans_Update();
        OldSum=Sum;
    }
    return 0;
}
