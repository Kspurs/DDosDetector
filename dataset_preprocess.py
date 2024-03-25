import os
dataset_files=os.listdir("ddos_dataset")
dataset_files.sort()
unprocessed_traffic=open("ddos_dataset/unprocessed_traffic.txt",'w')
for file in dataset_files:
    read_file=open("ddos_dataset/"+file,'r')
    read_str=read_file.readline()
    while read_str:
        timestamp,src,dst=read_str[:-1].split(" ")
        if len(timestamp)!=20 or len(timestamp.split('.'))!=2 or len(timestamp.split('.')[0])!=10:
            continue
        unprocessed_traffic.write(read_str)
        read_str=read_file.readline()
    read_file.close()
unprocessed_traffic.close()
unprocessed_traffic=open("ddos_dataset/unprocessed_traffic.txt",'r')
processed_traffic=open("ddos_dataset/processed_traffic.txt",'w')
readstr=unprocessed_traffic.readline()
start_time=float(readstr[:-1].split(" ")[0])
sliding_window=[]
current_window_idx=0
end_time=0
f=1
while readstr:
    timestamp=float(readstr[:-1].split(" ")[0])
    if timestamp-start_time>60:
        end_time=timestamp
        break
    sliding_window.append(readstr)
    readstr=unprocessed_traffic.readline()
while 1:
    for record in sliding_window:
        timestamp,src,dst=record[:-1].split(" ")
        dst+="_"+str(current_window_idx)+'\n'
        processed_traffic.write(" ".join([timestamp,src,dst]))
    end_time+=1
    start_time +=1
    current_window_idx+=1
    if len(readstr)==0:
        break
    while float(sliding_window[0][:-1].split(" ")[0])<start_time:
        sliding_window.pop(0)
    while readstr:
        timestamp = float(readstr[:-1].split(" ")[0])
        if timestamp>end_time:
            break
        sliding_window.append(readstr)
        readstr=unprocessed_traffic.readline()
processed_traffic.close()
unprocessed_traffic.close()


