"""
codegen_processor.py

Recursively scans .cpp/.h files for codegen blocks of the form:

    // -- [CODEGEN START] SomeFn(arg1, arg2, ...)
    ... existing content ...
    // -- [CODEGEN END]

For each block found, calls the matching Python function (looked up by name)
with the full file contents and the parsed arguments. The function's
return value replaces the content between the START and END markers.

Usage:
    python codegen_processor.py <folder> [--dry-run]

Adding a codegen function:
    Register a function in the CODEGEN_FUNCTIONS dict (or use @register).
    Signature: fn(file_content: str, content: str, *args: str) -> str
"""

import re
import sys
import argparse
from pathlib import Path
from typing import Callable

# ---------------------------------------------------------------------------
# Registry
# ---------------------------------------------------------------------------

CODEGEN_FUNCTIONS: dict[str, Callable] = {}
STRUCT_FIELDS_MAP: dict[dict[str, str]] = {}

def register(name: str | None = None):
    """Decorator to register a codegen function by name."""
    def decorator(fn: Callable) -> Callable:
        key = name if name else fn.__name__
        CODEGEN_FUNCTIONS[key] = fn
        return fn
    return decorator


def parse_struct(cpp_content: str) -> tuple[str, dict[str, str]]:
    """Parse the first C++ struct found and return (struct_name, name->type dict)."""
    pattern = r'struct\s+(\w+)\s*\{([^}]*)\}'
    match = re.search(pattern, cpp_content, re.DOTALL)

    if not match:
        raise ValueError("No struct found")

    struct_name = match.group(1)
    body = match.group(2)
    fields = {}

    field_pattern = r'(\w[\w\s:*<>,]*?)\s+(\w+)\s*(?:\[[^\]]*\])?\s*(?:=[^;]*|\([^)]*\))?\s*;'

    for field_match in re.finditer(field_pattern, body):
        field_type = field_match.group(1).strip()
        field_name = field_match.group(2).strip()
        if field_type not in ('return', 'struct', 'class', 'enum'):
            fields[field_name] = field_type

    STRUCT_FIELDS_MAP[struct_name] = fields
    return struct_name, fields

# ---------------------------------------------------------------------------
# Example codegen functions — replace / extend with your own
# ---------------------------------------------------------------------------

@register()
def UniformsHeader(cpp_content: str, *args: list[str]) -> str:
    struct_name, fields = parse_struct(cpp_content)

    for name in args:
        program_uniform = name[0].upper() + name[1:]
        fields[f'm_{program_uniform}'] = 'RUNTIME'

    lines = [f"struct {struct_name}Locations", "{"]
    for field_name in fields:
        lines.append(f"    GLint {field_name} = -1;")
    lines.append("};")

    return "\n".join(lines)

@register()
def UniformsDefinitions(cpp_content: str, struct_name: str) -> str:
    fields = STRUCT_FIELDS_MAP[struct_name.split('::')[-1]]

    lines = [f"void FillUniformLocations(const xc::ShaderProgram& program, {struct_name}Locations& out_uniformsLocations)", "{"]
    for field_name in fields:
        program_uniform = field_name.lstrip('m_')
        program_uniform = program_uniform[0].upper() + program_uniform[1:]
        lines.append(f'    out_uniformsLocations.{field_name} = program.GetUniformLocation("u_{program_uniform}");')
    lines.append("}")
    lines.append("")

    lines.extend([f"void SetUniformValues(const {struct_name}Locations& uniformsLocations, const {struct_name}& uniforms)", "{"])
    for (field_name, field_type) in fields.items():
        if field_type != 'RUNTIME':
            lines.append(f'    xg::SetUniform(uniformsLocations.{field_name}, uniforms.{field_name});')
    lines.append("}")

    return "\n".join(lines)


# ---------------------------------------------------------------------------
# Parsing
# ---------------------------------------------------------------------------

# Matches:  // -- [CODEGEN START] FnName(arg1, arg2)
START_RE = re.compile(
    r"(?P<indent>[ \t]*)//\s*--\s*\[CODEGEN START\]\s*"
    r"(?P<fn>\w+)\((?P<args>[^)]*)\)"
)
END_MARKER = "// -- [CODEGEN END]"


def parse_args_string(raw: str) -> list[str]:
    """Split 'arg1, arg2, arg3' into ['arg1', 'arg2', 'arg3'] (stripped)."""
    if not raw.strip():
        return []
    return [a.strip() for a in raw.split(",")]

def process_file_content(source: str, filepath: Path, dry_run: bool) -> tuple[str, int]:
    """
    Scan *source* for codegen blocks and call the registered functions.
    Each function receives the full file contents (not just the block content).

    Returns (new_source, number_of_blocks_processed).
    """
    lines = source.splitlines(keepends=True)
    output: list[str] = []
    i = 0
    blocks_processed = 0

    while i < len(lines):
        line = lines[i]
        m = START_RE.search(line)

        if m:
            fn_name = m.group("fn")
            raw_args = m.group("args")
            args = parse_args_string(raw_args)

            # Emit the START line unchanged
            output.append(line)
            i += 1

            # Skip over existing block content until END marker
            end_line: str | None = None
            while i < len(lines):
                if END_MARKER in lines[i]:
                    end_line = lines[i]
                    i += 1
                    break
                i += 1

            if end_line is None:
                print(f"  WARNING: No END marker found after START on line "
                      f"{len(output)} in {filepath}", file=sys.stderr)
                continue

            # Look up and call the function with the full file contents
            if fn_name not in CODEGEN_FUNCTIONS:
                print(f"  WARNING: No codegen function '{fn_name}' registered "
                      f"— skipping block in {filepath}", file=sys.stderr)
                output.append(end_line)
                continue

            fn = CODEGEN_FUNCTIONS[fn_name]
            new_content = fn(source, *args)
            blocks_processed += 1

            action = "Would replace" if dry_run else "Replacing"
            print(f"  {action} block [{fn_name}({', '.join(args)})] in {filepath}")

            # Re-indent every line of the generated content to match the START comment
            indent = m.group("indent")
            indented_content = "".join(
                indent + l if l.strip() else l
                for l in new_content.splitlines(keepends=True)
            )
            output.append(indented_content)
            output.append(f'\n{end_line}')

        else:
            output.append(line)
            i += 1

    return "".join(output), blocks_processed


# ---------------------------------------------------------------------------
# File I/O
# ---------------------------------------------------------------------------

EXTENSIONS = {".cpp", ".h", ".hpp", ".cc", ".cxx"}
HEADER_EXTS = {".h", ".hpp"}


def _file_sort_key(p: Path) -> tuple[int, str]:
    """Headers (0) before source files (1), alphabetical within each group."""
    return (0 if p.suffix in HEADER_EXTS else 1, str(p))


def process_folder(folder: Path, dry_run: bool) -> None:
    files = [
        p for p in folder.rglob("*")
        if p.is_file() and p.suffix in EXTENSIONS
    ]

    if not files:
        print(f"No {EXTENSIONS.join(',')} files found in {folder}")
        return

    total_blocks = 0
    total_files = 0

    for filepath in sorted(files, key=_file_sort_key):
        try:
            source = filepath.read_text(encoding="utf-8")
        except Exception as e:
            print(f"  ERROR reading {filepath}: {e}", file=sys.stderr)
            continue

        new_source, count = process_file_content(source, filepath, dry_run)

        if count == 0:
            continue  # Nothing to do in this file

        total_blocks += count
        total_files += 1

        if not dry_run and new_source != source:
            try:
                filepath.write_text(new_source, encoding="utf-8")
            except Exception as e:
                print(f"  ERROR writing {filepath}: {e}", file=sys.stderr)

    print(f"\n{'[DRY RUN] ' if dry_run else ''}Done — "
          f"{total_blocks} block(s) in {total_files} file(s).")

def main():
    parser = argparse.ArgumentParser(
        description="Process [CODEGEN START/END] blocks in .cpp/.h files."
    )
    parser.add_argument("folder", help="Root folder to scan recursively")
    parser.add_argument(
        "--dry-run", action="store_true",
        help="Print what would change without writing files"
    )
    args = parser.parse_args()

    folder = Path(args.folder)
    if not folder.is_dir():
        print(f"ERROR: '{folder}' is not a directory.", file=sys.stderr)
        sys.exit(1)

    print(f"Scanning: {folder.resolve()}")
    print(f"Registered functions: {', '.join(CODEGEN_FUNCTIONS) or '(none)'}\n")
    process_folder(folder, dry_run=args.dry_run)


if __name__ == "__main__":
    main()
