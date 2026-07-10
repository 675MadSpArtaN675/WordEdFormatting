import sys
import time as t
import pathlib as pl
import subprocess as s
import threading as th

import collections as cn

import colorama as c

def decoding(line: str | bytes):
    return line if type(line) == str else line.decode()

def is_colored(input_val: str, is_col: bool):
    return input_val if is_col else ""

def print_remains_text(proc, stream, errors, out, title: str, _is_colored: bool):
    out_text = out.readlines()
    is_colored_func = lambda x: is_colored(x, _is_colored)
    remains = "".join(map(lambda x: is_colored_func(c.Fore.BLUE) + f"[{title}]: " + is_colored_func(c.Fore.LIGHTGREEN_EX) + decoding(x), out_text))

    remains_errors = str()
    if errors is not None:
        error_text = errors.readlines()
        remains_errors = "".join(map(lambda x: is_colored_func(c.Fore.RED) + "[ERROR]: " + is_colored_func(c.Fore.LIGHTRED_EX) + decoding(x), error_text))

    result_remains = remains + (remains_errors if errors is not None else "")
    print(result_remains.strip(), file=stream)

def PrintErrors(proc: s.Popen, title: str, to_file_write_too: bool = False):
    streamOptions = cn.namedtuple("StreamOptions", ["stream", "is_colored"])
    streams = [streamOptions(sys.stdout, True)]

    out = proc.stdout
    errors = proc.stderr

    file_of = None
    if to_file_write_too:
        file_of = open("logs.txt")
        streams.append(streamOptions(file_of, False))

    while proc.poll() is None:
        out_line = out.readline()

        if out_line:
            line = decoding(out_line).strip()

            for stream in streams:
                print(is_colored(c.Fore.BLUE, stream.is_colored) + f"[{title}]: " + is_colored(c.Fore.LIGHTGREEN_EX, stream.is_colored) + line, file=stream.stream)

        if not out_line:
            t.sleep(0.3)

    for stream_opts in streams:
        print_remains_text(proc, stream_opts.stream, out, errors, title, stream_opts.is_colored)

    out.close()

    if errors is not None:
        errors.close()

class CMakeInteractor:
    _build_dir: str = None

    def __init__(self, build_dir: str = "build"):
        self._build_dir = pl.Path(build_dir).absolute()

    def configure(self, boost_path: str):
        self._configure_subproc(["cmake", "-B", self._build_dir, "-G", "MinGW Makefiles", f"-DBoost_DIR={boost_path}"], "CONFIGURE")

    def build(self):
        self._configure_subproc(["cmake", "--build", self._build_dir], "BUILD")

    def _configure_subproc(self, args: list[str], title: str):
        configure_proc = s.Popen(args, stdout=s.PIPE, stderr=s.PIPE)
        self._configure_subproc_obj(configure_proc, title)

    def _configure_subproc_obj(self, configure_proc: s.Popen, title: str):
        t.sleep(5)

        print_th = th.Thread(target=PrintErrors, args=[configure_proc, title])
        print_th.start()

        configure_proc.wait()
        print_th.join()

    def copy_test_files(self, include_files_patterns: list[str]):
        build_dir_obj = pl.Path(self._build_dir)
        cwd = pl.Path.cwd()

        files_to_perform = [concret_file for file in include_files_patterns for concret_file in cwd.glob(file)]

        for file_ in build_dir_obj.rglob(r"*_tests.exe"):
            parent_dir = file_.parent
            for file in files_to_perform:
                file.copy_into(parent_dir)

    def start_tests(self, test_files_exclude: list[str] = ()):
        build_dir_obj = pl.Path(self._build_dir)
        print(c.Fore.GREEN + "Valid path: " + c.Fore.LIGHTCYAN_EX + str(build_dir_obj))

        cwd = pl.Path.cwd()
        valid_files = []
        for file in build_dir_obj.rglob(r"*_tests.exe"):
            file_name = file.name
            if file_name not in test_files_exclude and file.is_file():
                print(c.Fore.GREEN + "\nRunning test: " + c.Fore.LIGHTGREEN_EX + file_name + "\n")

                command = f"cd {file.parent} && {file} --log_level=message"
                print(f"Ready command: {c.Fore.CYAN}{command}")
                test_process = s.Popen(command.split(), shell=True, encoding='utf-8', cwd=cwd, stdout=s.PIPE, stderr=s.STDOUT)
                self._configure_subproc_obj(test_process, "TEST")
                valid_files.append(file)

        return valid_files


def main():
    c.init(autoreset=True)
    build_dir = "D:\\Programming\\CPP\\ReportWrapper\\build"
    boost_directory = "C:\\Lib\\boost\\lib\\cmake\\Boost-1.91.0"

    try:
        interactor = CMakeInteractor(build_dir)
        interactor.configure(boost_directory)
        interactor.build()

        print(c.Fore.GREEN + "\nCopying test files...")
        interactor.copy_test_files(["*.docx"])

        print(c.Fore.BLUE + "\nStarting tests...")
        interactor.start_tests([])

    except Exception as ex:
        print(f"Error of building project: {ex}")

if __name__ == "__main__":
    main()