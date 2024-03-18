import mmh3
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
        hash_result_1=mmh3.hash64(src^dst,self.seed1)
        hash_result_2=mmh3.hash64(src^dst,self.seed2)%self.m
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

if __name__=="main":
    dataset=[]
    ddos_dataset=[]
    detected_victim=[]
    actual_victim=[]
    

