from clang.cindex import Config
from clang.cindex import TranslationUnit
from clang.cindex import Index
from Homework import Homework

# 以函数为块对ast遍历按块进行区分

libclangPATH = r"C:\Program Files\LLVM\bin\libclang.dll"

if not Config.loaded:
    Config.set_library_file(libclangPATH)

index = Index.create()
assert isinstance(index, Index)
filepath = r'a.c'
tu = index.parse(filepath)
assert isinstance(tu, TranslationUnit)

pre_seq = [[]]
base_seq = pre_seq[0]

def pre_ast_traver(cursor, base_seq, seq, fun=False):
    base_seq.append(str(cursor.kind)[11:])
    if not fun and str(cursor.kind) == "CursorKind.FUNCTION_DECL":
        temp = []
        pre_ast_traver(cursor, temp, temp, True)
        seq.append(temp)
    else:
        for child in cursor.get_children():
            pre_ast_traver(child, base_seq, seq)


pre_ast_traver(tu.cursor, base_seq, pre_seq)

for seq in pre_seq:
    print(seq)


print("---------------------------------------")
hw = Homework('./', False)
pre_seq_1 = hw.get_ast_seq()
print("---------------------------------------")
for seq in pre_seq_1:
    print(seq)
print("---------------------------------------")

print(pre_seq == pre_seq_1)

print("---------------------------------------")
idx = [3, 4, 5]
texts = hw.get_raw_text(1, idx)
for text in texts:
    print(text)
# post_seq = [[]]
# base_post_seq = post_seq[0]
#
# def post_ast_traver(cursor, base_seq, seq, fun=False):
#     if not fun and str(cursor.kind) == "CursorKind.FUNCTION_DECL":
#         temp = []
#         post_ast_traver(cursor, temp, temp, True)
#         seq.append(temp)
#     else:
#         for child in cursor.get_children():
#             post_ast_traver(child, base_seq, seq)
#     base_seq.append(str(cursor.kind)[11:])
#
#
# post_ast_traver(tu.cursor, base_post_seq, post_seq)
#
# for seq in post_seq:
#     print(seq)