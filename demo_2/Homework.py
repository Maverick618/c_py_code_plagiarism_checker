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
        self.pre_location = [[]]
        # self.post_location = []

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

        pre_seq = [[]]
        # post_seq = [[]]
        self.pre_location = [[]]
        # self.post_location = [[]]
        for filepath in self._files_path:
            tu = index.parse(filepath)
            assert isinstance(tu, TranslationUnit)
            self.__clang_ast_travers(tu.cursor, pre_seq, pre_seq[0], self.pre_location, self.pre_location[0])

        return pre_seq  # , post_seq

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

    def __clang_ast_travers(self, cursor, pre_seq, pre_base_seq, loc_seq, loc_base_seq, in_func=False):
        """
        ast pre & post travers
        done:
            1. pre & post traver (now only use the pre)
            todo: 2. record the location information
            3: functionDecl ast
        """
        # pre ast in base seq
        name_kind = str(cursor.kind)[11:]
        pre_base_seq.append(name_kind)
        # location in base location seq
        temp = str(cursor.location).split()
        loc = [temp[2][1:-2], int(temp[4][:-1])]
        loc_base_seq.append(loc)

        if not in_func and name_kind == 'FUNCTION_DECL':
            pre_temp = []
            loc_temp = []
            # post_temp = []
            self.__clang_ast_travers(cursor, pre_temp, pre_temp, loc_temp, loc_temp, True)
            pre_seq.append(pre_temp)
            loc_seq.append(loc_temp)
            # post_seq.append(post_temp)
        else:
            for node in cursor.get_children():
                self.__clang_ast_travers(node, pre_seq, pre_base_seq, loc_seq, loc_base_seq)
        # post_base_seq.append(name_kind)
        # self.post_location.append(loc)

    def get_raw_text(self, loc_idx, index):
        """
        get the raw text in the original text at lines [begin, end]

        get file path, begin and end from the list of index

        the element of index:
        [file path, line number]
        """

        # get the file path and line_nums
        for loc in self.pre_location:
            print(loc)
        dic = {}
        for filepath in index:
            # Notice: 'None'[1:-2] == 'on', line 114
            file = self.pre_location[loc_idx][filepath][0]
            assert isinstance(file, str)
            if filepath != -1 and file != 'on':
                line_num = self.pre_location[loc_idx][filepath][1]
                if file not in dic:
                    dic[file] = [line_num, line_num]
                else:
                    dic[file][0] = min(dic[file][0], line_num)
                    dic[file][1] = max(dic[file][1], line_num)

        # todo: be a list for each file ?
        texts = []

        for file_path in dic:
            text = ''
            with open(file_path, 'r') as file:
                start, end = dic[file_path][0], dic[file_path][1]
                assert isinstance(start, int)
                assert isinstance(end, int)
                line_num = 0
                while line_num <= end:
                    line = file.readline()
                    line_num += 1
                    if line_num >= start:
                        text += line
            texts.append(text)
        return texts
    pass
