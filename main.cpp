#include<unordered_map>
#include<string>
#include"MurmurHash3.h"
#include<math.h>
#include<fstream>
#include<unordered_set>
#include <iostream>
using namespace std;
class Detector{
    private:
    char* bit_array;
    double sampling_probability;
    int abs_threshold;
    int bit_array_cnt;
    int bit_array_length;
    int seed1;
    int seed2;
    unordered_map<string,int> offline_hashmap_src;
    unordered_map<string,int> offline_hashmap_dst;
    public:
    Detector(int length,int abs_threshold,int seed1,int seed2,int p)
    {
        this->bit_array_length=length;
        this->abs_threshold=abs_threshold;
        this->seed1=seed1;
        this->seed2=seed2;
        this->bit_array=(char*)malloc(length/8);
        this->sampling_probability=p;
    }
    void insert(string src,string dst)
    {
        double cur_sampling_probility=this->sampling_probability*this->bit_array_length/(this->bit_array_length-this->bit_array_cnt);
        int hash_value1=0;
        int hash_value2=0;
        string key=(src+dst);
        MurmurHash3_x64_128(key.data(),key.size(),this->seed1,&hash_value1);
        MurmurHash3_x64_128(key.data(),key.size(),this->seed2,&hash_value2);
        hash_value2=hash_value2%this->bit_array_length;
        int is_sampled=hash_value1<cur_sampling_probility*pow(2,32);
        if(this->bit_array[hash_value2]==0)
        {
            this->bit_array_cnt+=1;
            this->bit_array[hash_value2]=1;
            if(is_sampled)
            {
                this->offline_hashmap_src[src]+=1;
                this->offline_hashmap_dst[dst]+=1;
            }
        }
    }
    int detect(string dst)
    {
        double src_flow_spread=this->offline_hashmap_src[dst]/this->sampling_probability;
        double dst_flow_spread=this->offline_hashmap_dst[dst]/this->sampling_probability;
        return dst_flow_spread>this->abs_threshold&&dst_flow_spread-src_flow_spread>0.5*dst_flow_spread;

    }
    
};
int main()
{
    ifstream read_file_normal;
    ifstream read_file_ddos;
    read_file_normal.open("processed/normal_traffic.txt",ios::in);
    unordered_set<string> all_dst,actual_victim,detected_victim;
    string src,dst,timestamp;
    Detector* detector=new Detector(1000000,1000,1232,1545,0.3);
    while(read_file_normal>>src&&read_file_normal>>dst)
    {
        all_dst.insert(dst);
        detector->insert(src,dst);
    }
    read_file_normal.close();
    read_file_ddos.open("processed/processed_traffic.txt",ios::in);
    while(read_file_ddos>>timestamp&&read_file_ddos>>src&&read_file_ddos>>dst)
    {
        all_dst.insert(dst);
        detector->insert(src,dst);
        actual_victim.insert(dst);
    }
    double true_positive=0;
    double true_negative=0;
    double false_positive=0;
    double false_negative=0;
    for(auto dst:all_dst)
    {
        if(detector->detect(dst))
        {
            detected_victim.insert(dst);
        }
    }
    for(auto dst:all_dst)
    {
        if(actual_victim.find(dst)!=actual_victim.end())
        {
            if(detected_victim.find(dst)!=detected_victim.end())
            {
                true_positive++;
            }
            else
            {
                false_negative++;
            }
        }
        else
        {
            if(detected_victim.find(dst)!=detected_victim.end())
            {
                false_positive++;
            }
            else
            {
                true_negative++;
            }
        }
    }
    cout<<true_positive<<" "<<true_negative<<" "<<false_positive<<" "<<false_negative<<endl;
    double accuary=(true_positive+true_negative)/(true_positive+true_negative+false_positive+false_negative);
    double recall=true_positive/(true_positive+false_negative);
    double precision=true_positive/(true_positive+false_positive);
    double f1=2*precision*recall/(precision+recall);
    cout<<accuary<<" "<<recall<<" "<<precision<<" "<<f1<<endl;
}