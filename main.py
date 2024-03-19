import mmh3
import os
class Detector:
    def __init__(self,m,p,t):
        self.bit_array=[0]*m
        self.samping_probability=p
        self.offline_hashmap_src={}
        self.offline_hashmap_dst={}
        self.detect_threshold=t
        self.cnt=0
        self.seed1=1000
        self.m=m
        self.seed2=3341
        self.total_dst_flow_spread_estimation=0
    def insert(self,src,dst):
        sampling_probability=self.samping_probability*self.m/(self.m-self.cnt)
        hash_result_1=mmh3.hash(src+dst,self.seed1,False)
        hash_result_2=mmh3.hash(src+dst,self.seed2,False)%self.m
        is_sampled=hash_result_1<sampling_probability*2**32
        if self.bit_array[hash_result_2]==0:
            self.cnt+=1
            self.bit_array[hash_result_2]=1
            if is_sampled:
                if src in self.offline_hashmap_src:
                    self.offline_hashmap_src[src]+=1
                else:
                    self.offline_hashmap_src[src]=1
                if dst in self.offline_hashmap_dst:
                    self.offline_hashmap_dst[dst]+=1
                    self.total_dst_flow_spread_estimation+=1/self.samping_probability
                else:
                    self.offline_hashmap_dst[dst]=1
    def detect(self,src,dst):
        if dst not in self.offline_hashmap_dst:
            return False
        src_flow_spread_estimation=0
        if dst in self.offline_hashmap_src:
            src_flow_spread_estimation=self.offline_hashmap_src[dst]/self.samping_probability
        dst_flow_spread_estimation=self.offline_hashmap_dst[dst]/self.samping_probability
        return dst_flow_spread_estimation-src_flow_spread_estimation>self.detect_threshold*dst_flow_spread_estimation and dst_flow_spread_estimation>0.01*self.total_dst_flow_spread_estimation
dataset=open("normal_traffic.txt")
detector=Detector(1000000,0.1,0.7)
ddos_dataset=open("./CAIDA2007/ddos_traffic.txt")
filenames=os.listdir("./CAIDA2007")
curfileidx=0
alldst={}
detected_victim={}
actual_victim={}
current_time=0
idx=0
max_time=0
while 1:
    readstr=dataset.readline()
    if not readstr:
        break
    src,dst=readstr[:-1].split(" ")
    detector.insert(src,dst)
    alldst[dst]=1
dataset.close()
while 1:
    readstr=ddos_dataset.readline()
    if not readstr:
        ddos_dataset.close()
        curfileidx+=1
        if curfileidx==5:
            break
        ddos_dataset=open("./CAIDA2007/"+filenames[curfileidx])
        continue
    parameters=readstr[:-1].split(" ")
    if len(parameters)!=3:
        continue
    timestamp,src,dst=parameters
    if len(timestamp.split("."))!=2:
        continue
    timestamp=float(timestamp)
    max_time=max(max_time,int(timestamp))
    if timestamp-current_time>60:
        current_time=timestamp
        idx+=1
        actual_victim[dst+str(idx)]=1
    dst+=str(idx)
    alldst[dst]=1
    detector.insert(src,dst)
for dst in alldst:
    if detector.detect('',dst):
        actual_victim[dst]=1
true_positive=0
false_positive=0
false_negative=0
true_negative=0
print(detector.total_dst_flow_spread_estimation)
for victim in actual_victim:
    print(detector.offline_hashmap_dst[victim])
for dst in alldst:
    if dst not in actual_victim:
        if dst in detected_victim:
            false_positive+=1
        else:
            true_negative+=1
    else:
        if dst in detected_victim:
            true_positive+=1
        else:
            false_negative+=1
print(len(alldst))
print(true_positive,false_positive,false_negative,true_negative)
accuary=(true_positive+true_negative)/(true_positive+true_negative+false_positive+false_negative)
recall=true_positive/(true_positive+false_negative)
precision=true_positive/(true_positive+false_positive)
f1=2*precision*recall/(precision+recall)
print("accuary:",accuary)
print("recall:",recall)
print("precision:",precision)
print("f1:",f1)
