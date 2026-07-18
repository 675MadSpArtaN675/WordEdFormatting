import sys, os
import time as t
import pathlib as pl
import subprocess as s
import threading as th

import collections as cn
import icecream as ic

import colorama as c

def decoding(line: str | bytes):
    return line if type(line) == str else line.decode()

def is_colored(input_val: str, is_col: bool):
    return input_val if is_col else ""

def print_remains_text(stream, errors, out, title: str, _is_colored: bool):
    is_colored_func = lambda x: is_colored(x, _is_colored)
    remains, remains_errors = str(), str()
    if out is not None:
        out_text = out.readlines()

        remains = "".join(map(lambda x: is_colored_func(c.Fore.BLUE) + f"[{title}]: " + is_colored_func(c.Fore.LIGHTGREEN_EX) + decoding(x), out_text))
        remains_errors = str()

    if errors is not None:
        error_text = errors.readlines()
        remains_errors = "".join(map(lambda x: is_colored_func(c.Fore.RED) + "[ERROR]: " + is_colored_func(c.Fore.LIGHTRED_EX) + decoding(x), error_text))

    result_remains = remains + (remains_errors if errors is not None else "")
    print(result_remains.strip(), file=stream)

def PrintErrors(proc: s.Popen, title: str, to_file_write_too: bool = True):
    streamOptions = cn.namedtuple("StreamOptions", ["stream", "is_colored"])
    streams = [streamOptions(sys.stdout, True)]

    file_of = None
    if to_file_write_too:
        file_of = open("logs.txt", mode="w", encoding='utf-8')
        streams.append(streamOptions(file_of, False))

    out = proc.stdout
    errors = proc.stderr

    while proc.poll() is None:
        out_line = out.readline()

        if out_line:
            line = decoding(out_line).strip()

            for stream in streams:
                print(is_colored(c.Fore.BLUE, stream.is_colored) + f"[{title}]: " + is_colored(c.Fore.LIGHTGREEN_EX, stream.is_colored) + line, file=stream.stream)

        if not out_line:
            t.sleep(0.3)

    for stream_opts in streams:
        print_remains_text(stream_opts.stream, out, errors, title, stream_opts.is_colored)

    out.close()

    if errors is not None:
        errors.close()

class CMakeInteractor:
    _build_dir: str = None
    _build_type: str = None
    _last_return_code: int = None

    _file_find_pattern = "*_tests.exe" if sys.platform == "win32" else "*_tests"

    def __init__(self, build_type: str | None = None, build_dir: str = "build"):
        self._build_dir = pl.Path(build_dir).absolute()
        self._build_type = build_type

    def configure(self, is_debug: bool, **kwargs):
        build_type_ = []

        if self._build_type is not None:
            build_type_.extend(["-G", self._build_type])

        if is_debug:
            build_type_.append("-DDEBUG_LIB=ON")

        for path_of_lib in ["boost_root", "boost_path", "duckx_path", "pugixml_path"]:
            if not kwargs.get(path_of_lib):
                kwargs[path_of_lib] = "/usr/local" if sys.platform == "linux" else "C:/Lib/"

            print(f"Path of {path_of_lib}: {kwargs.get(path_of_lib)}")

        lib_dirs_ = [f"-DBoost_ROOT={kwargs["boost_root"]}", f"-DBoost_DIR={kwargs["boost_path"]}", f"-DDUCKX_DIRECTORY={kwargs["duckx_path"]}",f"-DPUGIXML_DIRECTORY={kwargs["pugixml_path"]}"]

        return self._configure_subproc(["cmake", "-B", str(self._build_dir), *build_type_, *lib_dirs_], "CONFIGURE")

    def build(self):
        self._configure_subproc(["cmake", "--build", str(self._build_dir)], "BUILD")

    def _configure_subproc(self, args: list[str], title: str):
        print("[SYSTEM]: Subproc args: ", " ".join(args))
        configure_proc = s.Popen(args, stdout=s.PIPE, stderr=s.PIPE)
        self._configure_subproc_print_obj(configure_proc, title)

    def _configure_subproc_print_obj(self, configure_proc: s.Popen, title: str):
        t.sleep(5)

        print_th = th.Thread(target=PrintErrors, args=[configure_proc, title])
        print_th.start()

        configure_proc.wait()
        print_th.join()

        self._last_return_code = configure_proc.returncode
        if self.return_code != 0:
            raise Exception(f"Failed on stage: '{title}'")

    @property
    def return_code(self):
        return self._last_return_code

    def copy_test_files(self, include_files_patterns: list[str]):
        build_dir_obj = pl.Path(self._build_dir)
        cwd = pl.Path.cwd()

        files_to_perform = [concret_file for file in include_files_patterns for concret_file in cwd.glob(file)]

        for file_ in build_dir_obj.rglob(self._file_find_pattern):
            parent_dir = file_.parent
            for file in files_to_perform:
                file.copy_into(parent_dir)

    def start_tests(self, test_files_exclude: list[str] = ()):
        build_dir_obj = pl.Path(self._build_dir)
        print(c.Fore.GREEN + "Valid path: " + c.Fore.LIGHTCYAN_EX + str(build_dir_obj))

        cwd = pl.Path.cwd()
        valid_files = []
        for file in build_dir_obj.rglob(self._file_find_pattern):
            file_name = file.name
            if file_name not in test_files_exclude and file.is_file():
                print(c.Fore.GREEN + "\nRunning test: " + c.Fore.LIGHTGREEN_EX + file_name + "\n")

                command = f"{file} --log_level=message"
                print(f"Ready command: {c.Fore.CYAN}{command}")
                test_process = s.Popen(command, shell=True, encoding='utf-8', cwd=str(file.parent), stdout=s.PIPE, stderr=s.STDOUT)
                self._configure_subproc_print_obj(test_process, "TEST")
                valid_files.append(file)

        return valid_files

def choose_path(first_path: str, second_path: str = "/usr/local"):
    return first_path if sys.platform == "win32" else second_path

def main():
    c.init(autoreset=True)
    build_dir = pl.Path("build").absolute()

    boost_directory = choose_path("C:/Lib/boost", "/usr/local/Boost")
    duckx_directory = choose_path("C:/Lib/duckx")
    pugixml_directory = choose_path("C:/Lib/pugixml")

    build_type = None if sys.platform == "linux" else "MinGW Makefiles"

    try:
        interactor = CMakeInteractor(build_type, build_dir=build_dir)
        interactor.configure(
            True,
            boost_root=boost_directory,
            boost_path=boost_directory + "/lib/cmake/Boost-1.91.0",
            duckx_path=duckx_directory,
            pugixml_path=pugixml_directory
        )

        interactor.build()

        print(c.Fore.GREEN + "\nCopying test files...")
        interactor.copy_test_files(["*.docx"])

        print(c.Fore.BLUE + "\nStarting tests...")
        interactor.start_tests([])

    except Exception as ex:
        print(f"Error of building project: {ex}")

if __name__ == "__main__":
    main()