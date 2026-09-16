find . -name "*.c" -o -name "*.h" | sort | while read f; do
    echo "===== $f ====="
    cat "$f"
    echo
done > full_source_dump.txt
