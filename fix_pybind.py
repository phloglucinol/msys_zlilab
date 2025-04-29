import os
import re

def process_file(filepath):
    with open(filepath, "r", encoding="utf-8") as f:
        lines = f.readlines()

    new_lines = []
    inside_pybind11_func = False
    brace_depth = 0
    func_start_line = 0

    # 识别 pybind11 风格函数的正则
    func_pattern = re.compile(
        r'''^\s*(?:static\s+)?(?:pybind11::)?(handle|object)\s+(\w+)\s*\([^)]*\)\s*\{''')

    for idx, line in enumerate(lines):
        if not inside_pybind11_func:
            m = func_pattern.match(line)
            if m:
                inside_pybind11_func = True
                brace_depth = 1
                func_start_line = idx
                new_lines.append(line)
                continue
            else:
                new_lines.append(line)
                continue
        else:
            brace_depth += line.count('{')
            brace_depth -= line.count('}')
            # 只在 pybind11 函数体里替换
            replaced_line = re.sub(
                r'\breturn\s+NULL\s*;',
                'return pybind11::none();',
                line
            )
            new_lines.append(replaced_line)
            if brace_depth == 0:
                inside_pybind11_func = False

    with open(filepath, "w", encoding="utf-8") as f:
        f.writelines(new_lines)

def main():
    for root, dirs, files in os.walk("."):
        for name in files:
            if name.endswith(".cxx"):
                path = os.path.join(root, name)
                print(f"Processing {path}")
                process_file(path)

if __name__ == "__main__":
    main()
