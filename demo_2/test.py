from clang.cindex import Config
from clang.cindex import TranslationUnit
from clang.cindex import Index

# 以函数为块对ast遍历按块进行区分

libclangPATH = r"C:\Program Files\LLVM\bin\libclang.dll"

if not Config.loaded:
    Config.set_library_file(libclangPATH)

index = Index.create()
assert isinstance(index, Index)
filepath = r'./a.c'
tu = index.parse(filepath)
assert isinstance(tu, TranslationUnit)

pre_seq = [[]]
base_seq = pre_seq[0]

def pre_ast_traver(cursor, base_seq, seq, fun=False):
    base_seq.append(cursor.kind)
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


post_seq = [[]]
base_post_seq = post_seq[0]

def post_ast_traver(cursor, base_seq, seq, fun=False):
    if not fun and str(cursor.kind) == "CursorKind.FUNCTION_DECL":
        temp = []
        post_ast_traver(cursor, temp, temp, True)
        seq.append(temp)
    else:
        for child in cursor.get_children():
            post_ast_traver(child, base_seq, seq)
    base_seq.append(cursor.kind)


post_ast_traver(tu.cursor, base_post_seq, post_seq)

for seq in post_seq:
    print(seq)
