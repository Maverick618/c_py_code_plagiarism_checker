from Homework import Homework
from Solution import get_similarity
p_1 = r'D:\Program_Code\WorkPlace\Python\c_py_code_plagiarism_checker\data\cpp_test_data\5'
p_2 = r'D:\Program_Code\WorkPlace\Python\c_py_code_plagiarism_checker\data\cpp_test_data\6'
hw_1 = Homework(p_1, False)
hw_2 = Homework(p_2, False)

pre_seq_1, post_seq_1 = hw_1.get_ast_seq()
pre_seq_2, post_seq_2 = hw_2.get_ast_seq()

res, idx_1, idx_2, idx_3, idx_4 = get_similarity(pre_seq_1, post_seq_1, pre_seq_2, post_seq_2)

text_1 = hw_1.get_raw_text(idx_1)
text_2 = hw_2.get_raw_text(idx_2)

print("-------------------------")
print(text_1, end="")
print("-------------------------")
print(text_2, end="")
print("-------------------------")
