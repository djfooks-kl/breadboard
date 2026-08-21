#!/usr/bin/env python3
import os
import sys

def convert_indentation(line, tab_width=4):
    """Convert the leading whitespace of a line to spaces.

    Any existing mix of leading tabs/spaces is first expanded into an
    equivalent column count, then re-encoded as spaces
    with any remainder kept as spaces.
    """
    stripped = line.lstrip(" \t")
    leading = line[: len(line) - len(stripped)]

    col = 0
    for ch in leading:
        if ch == "\t":
            col += tab_width - (col % tab_width)
        else:
            col += 1

    tabs = col // tab_width
    spaces = col % tab_width

    return (" " * tabs * tab_width) + (" " * spaces) + stripped

def sort_include_blocks(lines):
    """Sort each consecutive run of '#include' lines alphabetically."""
    result = []
    block = []

    def flush_block():
        if block:
            block.sort(key=lambda l: l.strip())
            result.extend(block)
            block.clear()

    for line in lines:
        content = line.strip()
        if content.startswith("#include"):
            block.append(line)
        else:
            flush_block()
            result.append(line)

    flush_block()
    return result

def process_file(path, tab_width=4):
    try:
        with open(path, "r", encoding="utf-8", errors="surrogateescape", newline="") as f:
            content = f.read()
    except OSError:
        return False

    raw_lines = content.splitlines(keepends=True)

    indented_lines = [convert_indentation(l, tab_width) for l in raw_lines]
    final_lines = sort_include_blocks(indented_lines)

    new_content = "".join(final_lines)

    if new_content != content:
        with open(path, "w", encoding="utf-8", errors="surrogateescape", newline="") as f:
            f.write(new_content)
        print(f"Updated: {path}")
        return True
    return False

def iter_files(folder, extensions=None):
    for root, _, files in os.walk(folder):
        for name in files:
            if extensions and not any(name.endswith(ext) for ext in extensions):
                continue
            yield os.path.join(root, name)

def main():
    changed = 0
    total = 0
    for path in iter_files("src", [".cpp", ".h"]):
        total += 1
        if process_file(path, tab_width=4):
            changed += 1

    print(f"\nProcessed {total} file(s), Updated {changed}.")

if __name__ == "__main__":
    main()