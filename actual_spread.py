traffic=open("normal_traffic.txt",'r')
readstr=traffic.readline().strip('\n')
vis={}
per_dst_spread={}
per_src_spread={}
packet_num=0
while readstr:
    packet_num+=1
    src,dst=readstr.split(" ")
    if src+dst not in vis:
        vis[src+dst]=1
        if dst not in per_dst_spread:
            per_dst_spread[dst]=0
        if src not in per_src_spread:
            per_src_spread[src]=0
        per_dst_spread[dst]+=1
        per_src_spread[src]+=1
    readstr=traffic.readline().strip('\n')
traffic.close()
print(packet_num)
print(sum(per_dst_spread.values()))
alldst=list(per_dst_spread.keys())
alldst.sort(key=lambda t:per_dst_spread[t],reverse=True)
for key in alldst[:10]:
    print(key,per_dst_spread[key],end=" ")
    if key in per_src_spread:
        print(per_src_spread[key],end="")
    print()
print("---------------")
allsrc=list(per_src_spread.keys())
allsrc.sort(key=lambda t:per_src_spread[t],reverse=True)
for key in allsrc[:10]:
    print(key,per_src_spread[key])