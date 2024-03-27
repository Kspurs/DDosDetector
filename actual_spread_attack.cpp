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
int main()
{
    // ifstream inf;
    // inf.open("processed/processed_traffic.txt",ios::in);
    // unordered_map<string,int> vis;
    // unordered_map<string,int> actual_spread;
    // string timestamp,src,dst;
    // while (inf>>timestamp&&inf>>src&&inf>>dst)
    // {
    //     if(!vis[src+dst])
    //     {
    //         actual_spread[dst]+=1;
    //         vis[src+dst]=1;
    //     }
    // }
    // ofstream out;
    // out.open("actual_spread_attack.txt",ios::out);
    // vector<pair<int,string>> all_spread;
    // for(auto[dst,spread]:actual_spread)
    // {
    //     all_spread.emplace_back(spread,dst);
    // }
    // sort(all_spread.begin(),all_spread.end());
    // for(auto p:all_spread)
    // {
    //     out<<"key:"<<p.second<<" "<<"spread:"<<p.first<<endl;
    // }
    // out.close();
    // inf.close();
    ifstream inf;
    inf.open("../processed/normal_traffic.txt",ios::in);
    unordered_map<string,int> vis;
    unordered_map<string,int> actual_spread;
    string timestamp,src,dst;
    while (inf>>src&&inf>>dst)
    {
        if(!vis[src+dst])
        {
            actual_spread[dst]+=1;
            vis[src+dst]=1;
        }
    }
    ofstream out;
    out.open("actual_spread_normal.txt",ios::out);
    vector<pair<int,string>> all_spread;
    for(auto[dst,spread]:actual_spread)
    {
        all_spread.emplace_back(spread,dst);
    }
    sort(all_spread.begin(),all_spread.end());
    for(auto p:all_spread)
    {
        out<<"key:"<<p.second<<" "<<"spread:"<<p.first<<endl;
    }
    out.close();
    inf.close();
}