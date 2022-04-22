from Homework import Homework
import math
import numpy as np
import threading

def SmithWaterman(seq1, seq2, location_1, location_2, match, un_match, blank):
    seq1 = [' '] + seq1  # 前置空位
    seq2 = [' '] + seq2
    # location_1 = [' '] + location_1
    # location_2 = [' '] + location_2
    s_matrix = np.zeros([len(seq1), len(seq2)], int)
    path = {}

    for i in range(0, len(seq1)):
        for j in range(0, len(seq2)):
            if i == 0 or j == 0:
                path[i, j] = []
                s_matrix[i, j] = 0
            else:
                if seq1[i] == seq2[j]:  # 是否匹配 得分
                    l_matrix = s_matrix[i - 1, j - 1] + match
                else:
                    l_matrix = s_matrix[i - 1, j - 1] + un_match
                p_matrix = s_matrix[i - 1, j] - blank  # 插入或删除 得分
                q_matrix = s_matrix[i, j - 1] - blank  # 插入或删除 得分
                s_matrix[i, j] = max(0, l_matrix, p_matrix, q_matrix)  # 最终得分
                path[i, j] = []  # 路径来源选择
                if s_matrix[i, j] == math.floor(l_matrix):  # 包含优先级，若三者分值相同，优先选择斜向
                    path[i, j] = [i - 1, j - 1]
                elif s_matrix[i, j] == math.floor(p_matrix):
                    path[i, j] = [i - 1, j]
                elif s_matrix[i, j] == math.floor(q_matrix):
                    path[i, j] = [i, j - 1]
    # print("S = \n", S)
    # print("Path = \n", path)

    end = np.argwhere(s_matrix == s_matrix.max())
    # print("end = \n", end)

    for point in end:
        x = point[0]
        y = point[1]
        s1 = ""
        s2 = ""
        l1 = []
        l2 = []
        sim = 0
        while s_matrix[x, y] != 0:  # 回溯
            point = path[x, y]
            bx = point[0]
            by = point[1]
            # print("x = %d, y = %d" % (x,y))
            # print("bx = %d, by = %d" % (bx,by))
            if bx == x:
                s1 = '-' + ',' + s1
                s2 = seq2[y] + ',' + s2
                l1.insert(0, '-')
                l2.insert(0, location_2[y - 1])
                pass
            elif by == y:
                s1 = seq1[x] + ',' + s1
                s2 = '-' + ',' + s2
                l1.insert(0, location_1[x - 1])
                l2.insert(0, '-')
                pass
            else:
                s1 = seq1[x] + ',' + s1
                s2 = seq2[y] + ',' + s2
                l1.insert(0, location_1[x - 1])
                l2.insert(0, location_2[y - 1])
                sim += 1
            x = bx
            y = by
        print('The results of the most similar parts:')
        print('s1: %s' % list(s1.split(','))[:-1])
        print('s2: %s' % list(s2.split(','))[:-1])
        # # print ('sim = %d'%sim)
        # # 相似度计算
        # print('代码相似度为%.2f %%' % round(100 * 2 * sim / (len(seq1) + len(seq2) - 2), 4))
        res = round(100 * 2 * sim / (len(seq1) + len(seq2) - 2), 4)
        return res, l1, l2
    # 三个分值的设定问了一个专业的同学，说我所借鉴的代码是简化版的SmithWaterman，实际更为复杂
    # Smith_Waterman(test, original, 1, -1 / 3, 1)


class MyThread(threading.Thread):
    _result = None
    _l1 = None
    _l2 = None

    def __init__(self, func, args=()):
        super(MyThread, self).__init__()
        self.func = func
        self.args = args

    def run(self):
        self._result, self._l1, self._l2 = self.func(*self.args)

    @property
    def result(self):
        # noinspection PyBroadException
        try:
            return self._result, self._l1, self._l2
        except Exception:
            return None


def get_similarity(seq_1, seq_2, pre_location_1, post_location_1, seq_3, seq_4, pre_location_2, post_location_2):
    # todo: make similarity more accurate, we need a new method
    # 多线程计算
    # t1 = MyThread(SmithWaterman, args=(seq_1, seq_3, pre_location_1, pre_location_2, 1, -1 / 3, 1))
    # t2 = MyThread(SmithWaterman, args=(seq_2, seq_4, post_location_1, post_location_2, 1, -1 / 3, 1))
    # threads = [t1, t2]
    # t1.start()
    # t2.start()
    #
    # # 等待计算结束
    # for t in threads:
    #     t.join()

    res_1, l1, l2 = SmithWaterman(seq_1, seq_3, pre_location_1, pre_location_2, 1, -1 / 3, 1)
    res_2, l3, l4 = SmithWaterman(seq_2, seq_4, post_location_1, post_location_2, 1, -1 / 3, 1)
    res = (res_1 + res_2) / 2
    return round(res, 2), l1, l2, l3, l4


p_1 = r'D:\Program_Code\WorkPlace\Python\c_py_code_plagiarism_checker\data\cpp_test_data\5'
p_2 = r'D:\Program_Code\WorkPlace\Python\c_py_code_plagiarism_checker\data\cpp_test_data\6'
hw_1 = Homework(p_1, False)
hw_2 = Homework(p_2, False)

pre_seq_1, post_seq_1, pre_location_1, post_location_1 = hw_1.get_ast_seq()
pre_seq_2, post_seq_2, pre_location_2, post_location_2 = hw_2.get_ast_seq()

assert len(pre_seq_1) == len(pre_location_1)




res, l1, l2, l3, l4 = get_similarity(pre_seq_1, post_seq_1, pre_location_1, post_location_1,
                                     pre_seq_2, post_seq_2, pre_location_2, post_location_2)

print(res)

print(l1, l2)

print(l3, l4)
