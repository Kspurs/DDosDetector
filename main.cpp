#include<unordered_map>
#include<string>
#include"MurmurHash3.h"
#include<math.h>
#include<fstream>
#include<unordered_set>
#include <iostream>
#include <string.h>
#include"HLLCM.cpp"
using namespace std;
class Detector{
    private:
    char* bit_array;
    double sampling_probability;
    int abs_threshold;
    int bit_array_cnt;
    int bit_array_length;
    int seed1;
    int seed2=10000;
    unordered_map<string,int> offline_hashmap_src;
    unordered_map<string,int> offline_hashmap_dst;
    public:
Detector(int length,int abs_threshold,int seed1,int seed2,double p)
    {
        
        this->bit_array_length=length;
        this->abs_threshold=abs_threshold;
        this->seed1=seed1;
        this->seed2=seed2;
        this->bit_array=(char*)malloc(length/8);
        this->sampling_probability=p;
        this->bit_array_cnt=0;
    }
    void insert(string src,string dst)
    {
        double cur_sampling_probility=this->sampling_probability*this->bit_array_length/(this->bit_array_length-this->bit_array_cnt);
        unsigned int hash_value1=0;
        unsigned int hash_value2=0;
        string key=(src+dst);
        MurmurHash3_x86_32(key.data(),key.size(),this->seed1,&hash_value1);
        MurmurHash3_x86_32(key.data(),key.size(),this->seed2,&hash_value2);
        hash_value2=hash_value2%this->bit_array_length;
        int byte=hash_value2/8;
        int in_byte_idx=hash_value2%8;
        int is_sampled=hash_value1<cur_sampling_probility*pow(2,32);
        char read_byte=0;  
        memcpy(&read_byte,this->bit_array+byte,1);
        int bit_value=read_byte&(1<<in_byte_idx);
        if(bit_value==0)
        {
            this->bit_array_cnt+=1;
            memset(this->bit_array+byte,read_byte|(1<<in_byte_idx),1);
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
    double get_spread(string dst)
    {
        return this->offline_hashmap_dst[dst]/this->sampling_probability;
    }
    
};
int main()
{
    ifstream read_file_normal;
    ifstream read_file_ddos;
    read_file_normal.open("processed/normal_traffic.txt",ios::in);
    unordered_set<string> all_dst,actual_victim,detected_victim;
    string src,dst,timestamp;
    Detector* detector=new Detector(8000000,1000,1232,1545,0.1);
    // HLLCM* detector=new HLLCM();
    time_t start;
    time(&start);
    unsigned int total_pkt=0;
    while(read_file_normal>>src&&read_file_normal>>dst)
    {
        all_dst.insert(dst);
        detector->insert(src,dst);
        total_pkt++;
    }
    read_file_normal.close();
    read_file_ddos.open("processed/processed_traffic.txt",ios::in);
    while(read_file_ddos>>timestamp&&read_file_ddos>>src&&read_file_ddos>>dst)
    {
        all_dst.insert(dst);
        detector->insert(src,dst);
        total_pkt++;
        actual_victim.insert(dst);
    }
    time_t end;
    time(&end);
    unsigned int process_time=(end-start);
    cout<<"process_time: "<<process_time<<endl;
    cout<<"total pkt: "<<total_pkt<<endl;
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
    ofstream write_result;
    write_result.open("estimation_result.txt",ios::out);
    for(auto dst:all_dst)
    {
        write_result<<dst<<" "<<detector->get_spread(dst)<<endl;
    }
    write_result.close();
    cout<<true_positive<<" "<<true_negative<<" "<<false_positive<<" "<<false_negative<<endl;
    double accuary=(true_positive+true_negative)/(true_positive+true_negative+false_positive+false_negative);
    double recall=true_positive/(true_positive+false_negative);
    double precision=true_positive/(true_positive+false_positive);
    double f1=2*precision*recall/(precision+recall);
    cout<<accuary<<" "<<recall<<" "<<precision<<" "<<f1<<endl;
}