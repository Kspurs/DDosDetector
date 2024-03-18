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
    def insert(self,src,dst):
        sampling_probability=self.samping_probability*self.m/(self.m-self.cnt)
        hash_result_1=mmh3.hash64(src+dst,self.seed1)
        hash_result_2=mmh3.hash64(src+dst,self.seed2)%self.m
        is_sampled=hash_result_1<sampling_probability*2**64
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
                else:
                    self.offline_hashmap_dst[dst]=1
    def detect(self,src,dst):
        if src not in self.offline_hashmap_src or dst not in self.offline_hashmap_dst:
            return False
        src_flow_spread_estimation=self.offline_hashmap_src[src]/self.samping_probability
        dst_flow_spread_estimation=self.offline_hashmap_dst[dst]/self.samping_probability
        return dst_flow_spread_estimation-src_flow_spread_estimation>self.detect_threshold*dst_flow_spread_estimation
dataset=open("130200.txt")
detector=Detector(100000,0.01,0.1)
ddos_dataset=open("./CAIDA2007/20070804_134936.txt")
filenames=os.listdir("./CAIDA2007")
curfileidx=0
alldst={}
detected_victim=[]
actual_victim=[]
current_time=0
idx=0
while 1:
    readstr=dataset.readline()
    if not readstr:
        break
    src,dst=readstr.split()[1:3]
    detector.insert(src,dst)
    alldst[dst]=1
    if detector.detect(src,dst):
        detected_victim.append(dst)
dataset.close()
while 1:
    readstr=ddos_dataset.readline()
    if not readstr:
        ddos_dataset.close()
        curfileidx+=1
        ddos_dataset=open("./CAIDA2007/"+filenames[curfileidx])
        break
    timestamp,src,dst=readstr.split()
    timestamp=float(timestamp)
    alldst[dst]=1
    if timestamp-current_time>60:
        current_time=timestamp
        idx+=1
        dst+=str(idx)
        actual_victim.append(dst)
    detector.insert(src,dst)
    if detector.detect(src,dst):
        detected_victim.append(dst)
true_positive=0
false_positive=0
false_negative=0
true_negative=0
for dst in alldst:
    if dst in detected_victim and dst in actual_victim:
        true_positive+=1
    elif dst in detected_victim and dst not in actual_victim:
        false_positive+=1
    elif dst not in detected_victim and dst in actual_victim:
        false_negative+=1
    else:
        true_negative+=1
accuary=(true_positive+true_negative)/(true_positive+true_negative+false_positive+false_negative)
recall=true_positive/(true_positive+false_negative)
precision=true_positive/(true_positive+false_positive)
f1=2*precision*recall/(precision+recall)
print("accuary:",accuary)
print("recall:",recall)
print("precision:",precision)
print("f1:",f1)

