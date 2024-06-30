#
#   Ex3
#   main.py
#  	Itay Goldberg
# 	15.01.23
# 	Operating Systems 83381-01
# 
from typing import List

class Process:
    def __init__(self, t_start, t_left, priority=0, run_flag=True):
        self.t_start = t_start
        self.t_left = t_left
        self.priority = priority
        self.run_flag = run_flag

def print_processes(processes):
    for i, process in enumerate(processes):
        print(f"Process {i + 1}:")
        print(f"t_start: {process.t_start} t_left: {process.t_left} priority: {process.priority} run_flag: {process.run_flag}")

def read_processes(file_path: str,Processes) -> List[Process]:
    with open(file_path, 'r') as f:
        # Read the first line to get the size of the array
        size = int(f.readline())

        # Read the rest of the lines and create Process objects
        for i in range(size):
            line = str(f.readline().split())
            t_start = line.split(',')[0]
            t_start = int(t_start[2:len(line)])
            line = line[::-1]
            t_left = line.split(',')[0]
            t_left = t_left[2:len(line)]
            t_left = int(t_left[::-1])

            p = Process(t_start,t_left)
            processes.append(p)

    return processes

def FCFS(file_path: str , processes):
    processes = read_processes(file_path, processes)
    processes = sorted(processes, key=lambda process: process.t_start)
    clk = processes[0].t_start
    mean_turnaround = 0
    while processes[len(processes)-1].run_flag:
        for i in processes:
            if clk >= i.t_start and i.run_flag:
                if i.t_left > 0:
                    i.t_left -= 1
                    for j in processes:
                        if clk >= j.t_start and j.run_flag:
                            mean_turnaround += 1
                else:
                    i.run_flag = False
                    continue
                break


        clk += 1
    print(f"FCFS: mean turnaround = {mean_turnaround/len(processes)}")
    return

def LCFS_NP(file_path: str , processes):
    processes = read_processes(file_path, processes)
    for i in processes:
        i.priority = i.t_start
    processes = sorted(processes, key=lambda process: process.t_start)
    clk = 0
    last_jump_time = 0
    jump_width = 0
    mean_turnaround = 0
    all_done = 0
    while all_done < len(processes):
        for i in processes:
            if clk >= i.t_start and i.run_flag:
                for j in processes:
                    if j.priority > i.priority and clk >= j.t_start and j.run_flag:
                        i = j
                jump_width = i.t_left
                i.run_flag = False
                last_jump_time = clk
                if jump_width > 0:
                    clk += (jump_width - 1)
                mean_turnaround += jump_width
                all_done += 1
                break

        for i in processes:
            if clk >= i.t_start and i.run_flag:
                if i.t_start > last_jump_time:
                    mean_turnaround += (clk - i.t_start)
                else:
                    mean_turnaround += jump_width

        jump_width = 1
        last_jump_time = 0
        clk += 1
    print(f"LCFS_NP: mean turnaround = {mean_turnaround/len(processes)}")
    return

def LCFS_NP2(file_path: str , processes):
    processes = read_processes(file_path, processes)
    for i in processes:
        i.priority = i.t_start
    processes = sorted(processes, key=lambda process: process.t_start, reverse=True)
    clk = 0
    last_jump_time = 0
    jump_width = 0
    mean_turnaround = 0
    all_done = 0
    while all_done < len(processes):
        for i in processes:
            if clk >= i.t_start and i.run_flag:
                jump_width = i.t_left
                i.run_flag = False
                last_jump_time = clk
                clk += (jump_width - 1)
                mean_turnaround += jump_width
                all_done += 1
                break

        for i in processes:
            if clk >= i.t_start and i.run_flag:
                if i.t_start > last_jump_time:
                    mean_turnaround += (clk + 1 - i.t_start)
                else:
                    mean_turnaround += jump_width

        jump_width = 1
        last_jump_time = 0
        clk += 1
    print(f"LCFS_NP: mean turnaround = {mean_turnaround/len(processes)}")
    return

def LCFS_P(file_path: str , processes):
    processes = read_processes(file_path, processes)
    processes = sorted(processes, key=lambda process: process.t_start, reverse=True)
    for i in processes:
        i.priority = i.t_start
    clk = 0
    mean_turnaround = 0
    all_done = 0
    while all_done < len(processes):
        for i in processes:
            if clk >= i.t_start and i.run_flag:
                if i.t_left > 0:
                    i.t_left -= 1
                    break
                else:
                    i.run_flag = False
                    all_done += 1
                    continue

        for i in processes:
            if clk >= i.t_start and i.run_flag:
                mean_turnaround += 1
        clk += 1
    print(f"LCFS_P: mean turnaround = {mean_turnaround/len(processes)}")
    return

def Round_Robin(file_path: str , processes):
    processes = read_processes(file_path, processes)
    processes = sorted(processes, key=lambda process: process.t_start)
    clk = processes[0].t_start
    mean_turnaround = 0
    all_done = 0
    quantum = 2
    while all_done < len(processes):
        for i in processes:
            if clk >= i.t_start and i.run_flag:
                for j in processes:
                    if clk >= j.t_start and j.run_flag:
                        if clk == j.t_start:
                            mean_turnaround += 1
                        else:
                            mean_turnaround += quantum
                if i.t_left > 0:
                    i.t_left -= quantum
                else:
                    i.run_flag = False
                    all_done += 1
                    mean_turnaround -= quantum
        clk += quantum
    print(f"Round Robin: mean turnaround = {mean_turnaround/len(processes)}")
    return

def SJF(file_path: str , processes):
    processes = read_processes(file_path, processes)
    processes = sorted(processes, key=lambda process: process.t_left)
    for i in processes:
        i.priority = i.t_left
    clk = 0
    mean_turnaround = 0
    all_done = 0
    while all_done < len(processes):
        for i in processes:
            if clk >= i.t_start and i.run_flag:
                if i.t_left > 0:
                    for j in processes:
                        if clk >= j.t_start and j.run_flag:
                            mean_turnaround += 1
                    i.t_left -= 1
                    break
                else:
                    i.run_flag = False
                    all_done += 1
                    continue
        clk += 1
    print(f"SJF: mean turnaround = {mean_turnaround/len(processes)}\n")
    return

if __name__ == '__main__':
    processes = []
    #if (False):
    for txt_num in range(5):
        txt_num += 1
        #txt_num = 3
        file_path = "input_examples/input" + str(txt_num) + ".txt"
        print(f" * * Results for input{txt_num}.txt * *")
        FCFS(file_path, processes)
        processes.clear()
        LCFS_NP2(file_path, processes)
        processes.clear()
        LCFS_P(file_path, processes)
        processes.clear()
        Round_Robin(file_path, processes)
        processes.clear()
        SJF(file_path, processes)
        processes.clear()
    #txt_num = 5
    #file_path = "input_examples/input" + str(txt_num) + ".txt"
    #print(f"Results for input{txt_num}.txt")
    #LCFS_NP2(file_path, processes)

