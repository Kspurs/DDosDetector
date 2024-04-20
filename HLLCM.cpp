#include<unordered_map>
#include<string>
#include"MurmurHash3.h"
#include<math.h>
#include<fstream>
#include<unordered_set>
#include <iostream>
#include <string.h>
using namespace std;
class HLLCM
{
private:
    int random_number[256];
    int CM[3][28];
    int HLL_array[1600000];     
public:
    HLLCM(){
        memset(CM,0,sizeof(CM));
        for(int i=0;i<256;i++)
        {
            random_number[i]=rand();
        }
        memset(HLL_array,0,sizeof(HLL_array));
    };
    void insertCM(string src,string dst)
    {
        for(int i=0;i<3;i++)
        {
            unsigned int row_idx=0;
            MurmurHash3_x86_32(dst.data(),dst.size(),random_number[i],&row_idx);
            row_idx=row_idx%28;
            this->CM[i][row_idx]+=1;
        }
    }
    void insertHLL(string src,string dst)
    {
        unsigned int key=0;
        MurmurHash3_x86_32(src.data(),src.size(),40000,&key);
        int leading_zero=1;
        for(int i=31;i>=0;i--)
        {
            if((1<<i)&key)
            {
                break;
            }
            else
            {
                leading_zero++;
            }
        }
        for(int i=0;i<256;i++)
        {
            unsigned int HLL_idx=0;
            MurmurHash3_x86_32(dst.data(),dst.size(),random_number[i],&HLL_idx);
            HLL_idx=HLL_idx%256;
            this->HLL_array[HLL_idx]=max(this->HLL_array[HLL_idx],leading_zero);
        }
    }
    int getCMEstimation(string dst)
    {
        int min_val=INT32_MAX;
        for(int i=0;i<3;i++)
        {
            unsigned int row_idx=0;
            MurmurHash3_x86_32(dst.data(),dst.size(),random_number[i],&row_idx);
            row_idx=row_idx%28;
            min_val=min(min_val,this->CM[i][row_idx]);
        }
        return min_val;
    }
    double getHLLEstimation(string dst)
    {
        double a=0.7213/(1+1.079/256);
        double sum=0;
        for(int i=0;i<256;i++)
        {
            unsigned int HLL_idx=0;
            MurmurHash3_x86_32(dst.data(),dst.size(),random_number[i],&HLL_idx);
            HLL_idx=HLL_idx%256;
            sum+=1/pow(2,this->HLL_array[HLL_idx]);

        }
        return a*256*256*1/sum;
    }
    void insert(string src,string dst)
    {
        this->insertCM(src,dst);
        this->insertHLL(src,dst);
    }
    int detect(string dst)
    {
        return this->getCMEstimation(dst)>100000&&this->getHLLEstimation(dst)>1000;
    }
};

