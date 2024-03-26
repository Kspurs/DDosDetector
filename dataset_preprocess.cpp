#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <deque>
using namespace std;
int main(){
    ofstream out;
    ifstream inf;
    string caida_dir_path = "ddos_dataset";
    out.open("processed/unprocessed_traffic.txt", ios::out);
    for (const auto& entry : filesystem::directory_iterator(caida_dir_path)) 
    {
        inf.open(entry.path(), ios::in);
        cout << "Reading " << entry.path() << endl;
        string timestamp,src_ip, dst_ip;
        while (inf >> timestamp && inf >> src_ip && inf >> dst_ip) {
            if(timestamp.size()!=20||timestamp.find_first_of(".")!=10)
            {
                continue;
            }
            out<<timestamp<<" "<<src_ip<<" "<<dst_ip<<endl;
        }
        inf.close();
            // break;
    }
    cout<<"preprocess phase 1 done"<<endl;
    out.close();
    inf.open("processed/unprocessed_traffic.txt", ios::in);
    out.open("processed/processed_traffic.txt", ios::out);
    double start_time=-1;
    double end_time=-1;
    string timestamp,src_ip, dst_ip;
    deque<vector<string>> data;
    while (inf >> timestamp && inf >> src_ip && inf >> dst_ip) {
        if(start_time==-1)
        {
            start_time=stod(timestamp);
            end_time=stod(timestamp)+120;
        }
        if(stod(timestamp)>end_time)
        {
            for(int i=0;i<data.size();i++)
            {
                out<<data[i][0]<<" "<<data[i][1]<<" "<<data[i][2]<<endl;
            }
            end_time+=1;
            start_time+=1;
            while(!data.empty()&&stod(data.front()[0])<start_time)
            {
                data.pop_front();
            }
        }
        vector<string> temp;
        temp.push_back(timestamp);
        temp.push_back(src_ip);
        temp.push_back(dst_ip);
        data.push_back(temp);
    }
    cout<<"preprocess phase 2 done"<<endl;
    out.close();
    inf.close();
}