#!/usr/bin/sh

# get the dir of this script file
DIR_CURRENT="$(cd "$(dirname "$0")" && pwd)"

DIRS=$(ls -d */)

# print them one on each line
echo "Directories:"
for dir in $DIRS; do
    echo "$dir"
done

# for eahc dir filtered to the first five dirs
DIRS_LIMITED=$(echo "$DIRS" | head -n 1)

NEW_DIR="$DIR_CURRENT/_NEW_DIR"
if [ -d "$NEW_DIR" ]; then
    rm -rf "$NEW_DIR"
fi
mkdir "$NEW_DIR"

for dir in $DIRS_LIMITED; do
    echo "Processing directory: $dir"

    cp -r "$dir" "$NEW_DIR/$dir"

    new_dir_destination="$NEW_DIR/$dir"

    # in the hls.tcl file find the line "add_files -tb src/[lindex $argv 0]_tb.cpp"
    # keep that line but add a new line under it that says "add_files -tb data/"
    sed -i -e '/^add_files -tb src\/\[lindex \$argv 0\]_tb\.cpp$/a\add_files -tb data/' "$new_dir_destination/hls.tcl"

    # in src/*.cpp (there should be one file like this),
    # comment out this exact section:

    # // get environment variable PRJ_PATH
    # char* prj_path_c_str = getenv("PRJ_PATH");
    # if (prj_path_c_str == NULL) {
    #   std::cerr << "Environment variable PRJ_PATH not set" << std::endl;
    #   return 1;
    # }

    tb_fp=""
    found_files=0
    for file in "$new_dir_destination/src/"*_tb.cpp; do
        if [ -f "$file" ]; then
            if [ "$found_files" -ge 1 ]; then
                echo "Error: expected exactly one src/*_tb.cpp but found multiple files" >&2
                exit 1
            fi
            tb_fp="$file"
            found_files=$((found_files + 1))
        fi
    done

    if [ "$found_files" -eq 0 ]; then
        echo "Error: expected exactly one src/*_tb.cpp but found none" >&2
        exit 1
    fi

    if [ -z "$tb_fp" ]; then
        echo "Error: expected exactly one src/*_tb.cpp but found none" >&2
        exit 1
    fi

    # be sure to comment all the way in including the closing }
    sed -i '
        /char\* *prj_path_c_str *= *getenv("PRJ_PATH")/,/^[[:space:]]*}/{
            s|^|// |                                  # comment every line in the block
            /^[[:space:]]*}/a char* prj_path_c_str = ".";  # append new line after the closing brace
        }
    ' "$tb_fp"

done
