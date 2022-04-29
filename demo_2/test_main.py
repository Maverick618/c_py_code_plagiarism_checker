import os
import sys

import numpy as np

from Homework import Homework
from Solution import get_similarity

savedStdout = sys.stdout  # 保存标准输出流
print_log = open("./2020-LinuxKernel-1-log.txt", "w")
sys.stdout = print_log

root = r'D:\Program_Code\WorkPlace\Python\c_py_code_plagiarism_checker\data\2020-LinuxKernel-1'
assert os.path.isdir(root)

hw_paths = []
code_type = False
files = os.listdir(root)
for file in files:
    hw_paths.append(os.path.join(root, file))
    pass

assert len(files) == len(hw_paths)
print(len(files))
# print(hw_paths)
similarity_matrix = np.zeros((len(hw_paths), len(hw_paths)), dtype=float)
pre_seqs = [[] for x in range(len(hw_paths))]

for i in range(len(hw_paths)):
    j = i + 1
    p_1 = hw_paths[i]
    if not pre_seqs[i]:
        hw = Homework(p_1, code_type)
        pre_seqs[i] = hw.get_ast_seq()
    while j < len(hw_paths):
        p_2 = hw_paths[j]
        if not pre_seqs[j]:
            hw = Homework(p_2, code_type)
            pre_seqs[j] = hw.get_ast_seq()
        # todo: calculate the similarity, to rewrite the function of <get_similarity in the Solution.py or try to
        #  write a new function
        similarity_matrix[i][j] = get_similarity(pre_seqs[i], pre_seqs[j])
        similarity_matrix[j][i] = similarity_matrix[i][j]
        j += 1  # 差点又漏了

# output the result
print("\t", end='')
for file in files:
    print("\t\t", file[9:], end='')
    pass
print()
for i in range(len(files)):
    if len(files[i]) == 11:
        print(files[i][9:], end='\t')
    else:
        print(files[i][9:], end='')

    for j in range(len(files)):
        print("\t\t", similarity_matrix[i][j], end='')
    print()
    pass

sys.stdout = savedStdout  # 恢复标准输出流
print_log.close()