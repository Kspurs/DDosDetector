import mmh3
dataset=[]
attacker=[]
seed1=1000
seed2=3341
class Detector:
    def __init__(self,m,p):
        self.bit_array_src=[0]*m
        self.bit_array_dst=[0]*m
        self.samping_probability=p
        self.cnt1=0
        self.cnt2=0
    def insert(self,src,dst):
        hash_result=0
