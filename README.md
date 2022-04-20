# c_py_code_plagiarism_checker
## 版本文件信息
- version: demo v1
- HomeWork.py \
  主要用来获取作业的抽象语法树(AST)
- Solution.py \
  包含了可能用到的算法实现
- test_main.py \
  给定根目录和代码类别，给出相似度矩阵。目前因为下面想写test_location.py，更改了HomeWork里的__clang_get_ast_seq()方法，返回的值的个数变多了，test_main.py暂时未改变，因此运行此文件可能会报错。
  由于data\2020-linuxkernel-1里的代码相对复杂，计算时间比较久。
- test_location.py \
  测试相似片段原文定位，正在施工。
- data \
  测试数据目录
## 可能需要的库
- numpy
- ast
- math
- os
- sys
- threading
