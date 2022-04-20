import math
import numpy as np
import threading

def SmithWaterman(seq1, seq2, match, un_match, blank):
    seq1 = [' '] + seq1  # 前置空位
    seq2 = [' '] + seq2
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
        # s1 = ""
        # s2 = ""
        sim = 0
        while s_matrix[x, y] != 0:  # 回溯
            point = path[x, y]
            bx = point[0]
            by = point[1]
            # print("x = %d, y = %d" % (x,y))
            # print("bx = %d, by = %d" % (bx,by))
            if bx == x:
                # s1 = '-' + ',' + s1
                # s2 = seq2[y] + ',' + s2
                pass
            elif by == y:
                # s1 = seq1[x] + ',' + s1
                # s2 = '-' + ',' + s2
                pass
            else:
                # s1 = seq1[x] + ',' + s1
                # s2 = seq2[y] + ',' + s2
                sim += 1
            x = bx
            y = by
        # print('The results of the most similar parts:')
        # print('s1: %s' % list(s1.split(','))[:-1])
        # print('s2: %s' % list(s2.split(','))[:-1])
        # # print ('sim = %d'%sim)
        # # 相似度计算
        # print('代码相似度为%.2f %%' % round(100 * 2 * sim / (len(seq1) + len(seq2) - 2), 4))
        res = round(100 * 2 * sim / (len(seq1) + len(seq2) - 2), 4)
        return res
    # 三个分值的设定问了一个专业的同学，说我所借鉴的代码是简化版的SmithWaterman，实际更为复杂
    # Smith_Waterman(test, original, 1, -1 / 3, 1)


class MyThread(threading.Thread):
    _result = None

    def __init__(self, func, args=()):
        super(MyThread, self).__init__()
        self.func = func
        self.args = args

    def run(self):
        self._result = self.func(*self.args)

    @property
    def result(self):
        # noinspection PyBroadException
        try:
            return self._result
        except Exception:
            return None


def get_similarity(seq_1, seq_2, seq_3, seq_4):
    # todo: make similarity more accurate, we need a new method
    # 多线程计算
    t1 = MyThread(SmithWaterman, args=(seq_1, seq_3, 1, -1 / 3, 1))
    t2 = MyThread(SmithWaterman, args=(seq_2, seq_4, 1, -1 / 3, 1))
    threads = [t1, t2]
    t1.start()
    t2.start()

    # 等待计算结束
    for t in threads:
        t.join()

    res_1 = t1.result
    res_2 = t2.result
    res = (res_1 + res_2) / 2
    return round(res, 2)
