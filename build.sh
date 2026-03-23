#!/bin/bash

FORCE_BUILD=false
while [[ "$#" -gt 0 ]]; do
    case $1 in
        -f|--force) FORCE_BUILD=true; shift ;;
        *) shift ;;
    esac
done

clear
mkdir -p build bin
echo "crescent - compiling..."

INCLUDES="-I./include $(pkg-config --cflags raylib)"
DEFINES="-D_CRT_SECURE_NO_WARNINGS -DIMGUI_HAS_DOCK"
WARNINGS="-Wall -Wextra"
LIBS="$(pkg-config --libs raylib) -lm"

NEWEST_HEADER=$(find src include -type f -name '*.h' -exec stat -c %Y {} + 2>/dev/null | sort -n | tail -1)
NEWEST_HEADER=${NEWEST_HEADER:-0}

NEEDS_LINKING=false

while IFS= read -r -d '' file; do

    rel_path="${file#src/}"
    obj_rel_path="${rel_path%.c}.o"
    obj_path="build/${obj_rel_path}"

    mkdir -p "$(dirname "$obj_path")"

    compile=$FORCE_BUILD

    if [ "$compile" = false ]; then
        if [ ! -f "$obj_path" ]; then
            compile=true
        else
            obj_time=$(stat -c %Y "$obj_path")
            c_time=$(stat -c %Y "$file")
            if [ "$c_time" -gt "$obj_time" ] || [ "$NEWEST_HEADER" -gt "$obj_time" ]; then
                compile=true
            fi
        fi
    fi

    if [ "$compile" = true ]; then
        echo "[+] Compiling: $rel_path..."
        clang -x c -c "$file" -O3 $INCLUDES $DEFINES $WARNINGS -o "$obj_path"
        NEEDS_LINKING=true
    fi

done < <(find src -type f -name '*.c' -print0)

if [ "$NEEDS_LINKING" = true ] || [ ! -f "bin/crescent" ]; then
    echo "[~] linking crescent..."

    ALL_OBJS=$(find build -type f -name '*.o')

    if [ -n "$ALL_OBJS" ]; then
        clang $ALL_OBJS $LIBS -o bin/crescent
        if [ $? -ne 0 ]; then
            echo "linking failed."
            exit 1
        fi
    else
        echo "No object files found to link."
        exit 1
    fi
else
    echo "[v] code up to date, skipping compilation."
fi

if [ -d "assets" ]; then
    echo "[~] syncing assets..."
    mkdir -p bin/assets
    rsync -a --delete assets/ bin/assets/
fi

WIDTH=$(tput cols)
GREEN=$(tput setaf 2)
BLACK=$(tput setaf 0)
BG_GREEN=$(tput setab 2)
RESET=$(tput sgr0)
LINE=$(printf "%${WIDTH}s" | tr " " "=")

echo "${GREEN}${LINE}${RESET}"
echo -e "${BG_GREEN}${BLACK}BUILD SUCCESSFUL${RESET}"
echo "${GREEN}${LINE}${RESET}"
./bin/crescent
