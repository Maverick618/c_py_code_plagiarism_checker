from clang.cindex import Config
from clang.cindex import TranslationUnit
from clang.cindex import Index
import ast
import os

libclangPATH = r"C:\Program Files\LLVM\bin\libclang.dll"

class PyVisitor(ast.NodeVisitor):
    def __init__(self, pre_seq, post_seq):
        self._pre_seq = pre_seq
        self._post_seq = post_seq

    def generic_visit(self, node):
        self._pre_seq.append(type(node).__name__)
        ast.NodeVisitor.generic_visit(self, node)
        self._post_seq.append(type(node).__name__)
        pass
    pass


class Homework:
    def __init__(self, path, code_type, opensource=None):
        # error if path isn't a dir
        if opensource is None:
            opensource = []
        self._opensource = opensource
        assert os.path.isdir(path)
        self._files_path = []
        self._type = code_type
        self.pre_location = []
        self.post_location = []

        for root, subdir, files in os.walk(path):
            for filename in files:
                # get all the python c/c++ language file in current path
                if filename in self._opensource:
                    pass
                elif code_type and filename.endswith('.py'):
                    self._files_path.append(os.path.join(path, os.path.join(root, filename)))
                # get all the c/c++ language file in current path
                elif filename.endswith('.cpp') or \
                        filename.endswith('.c') or \
                        filename.endswith('.c++') or \
                        filename.endswith('.h'):
                    self._files_path.append(os.path.join(path, os.path.join(root, filename)))

    @property
    def files_path(self):
        # only readable
        return self._files_path

    @property
    def type(self):
        """
         only readable
         the val of code_type means:
         True: Python
         False: C/C++
        """
        return self._type

    def get_ast_seq(self):
        """
        getAstSeq(): get all files ast sequence in the homework, include preorder_travel and postorder_travel
        """
        if self._type:
            return self.__get_py_ast_seq()
        else:
            return self.__get_clang_ast_seq()

    def __get_clang_ast_seq(self):
        """
        the c/c++ language version of get ast seq
        """
        if not Config.loaded:
            Config.set_library_file(libclangPATH)
        index = Index.create()
        assert isinstance(index, Index)

        pre_seq = []
        post_seq = []
        for filepath in self._files_path:
            tu = index.parse(filepath)
            assert isinstance(tu, TranslationUnit)
            self.__clang_ast_travers(tu.cursor, pre_seq, post_seq)
        return pre_seq, post_seq

    def __get_py_ast_seq(self):
        """
        the python language version of get ast seq
        todo: record the location information
        """
        pre_seq = []
        post_seq = []
        visitor = PyVisitor(pre_seq, post_seq)
        for filepath in self._files_path:
            file = open(filepath, encoding='utf-8').read()
            code = ast.parse(file)
            visitor.visit(code)
        return pre_seq, post_seq
        pass

    def __clang_ast_travers(self, cursor, pre_seq, post_seq):
        """
        ast pre & post travers
        done:
            1. pre & post traver
            2. record the location information
        """
        pre_seq.append(str(cursor.kind)[11:])
        temp = str(cursor.location).split()
        loc = [temp[2][1:-2], int(temp[4][:-1])]
        self.pre_location.append(loc)
        for node in cursor.get_children():
            self.__clang_ast_travers(node, pre_seq, post_seq)
        post_seq.append(str(cursor.kind)[11:])
        self.post_location.append(loc)

    def get_raw_text(self, index):
        """
        get the raw text in the original text at lines [begin, end]

        get file path, begin and end from the list of index

        the element of index:
        [file path, line number]
        """

        # get the file path and line_nums
        dic = {}
        for filepath in index:
            if filepath != -1 and self.pre_location[filepath][0] != 'on':  # Notice: 'None'[1:-2] == 'on', line 114
                if self.pre_location[filepath][0] not in dic:
                    dic[self.pre_location[filepath][0]] = {self.pre_location[filepath][1]}
                else:
                    dic[self.pre_location[filepath][0]].add(self.pre_location[filepath][1])

        # todo: be a list for each file ?
        text = ""

        for path in dic:
            with open(path, 'r') as file:
                start, end = min(dic[path]), max(dic[path])
                line_num = 0
                while line_num <= end:
                    line = file.readline()
                    line_num += 1
                    if line_num >= start:
                        text += line
        return text
    pass
