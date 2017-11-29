# dedup_redup
File Deduplication / Reduplication C++11 solution

Deduplication is a specialized form of compression for when the granularity of redundancy is large. Deduplication comes in many shapes and forms, but one of the simplest implementations involves the following three steps:

Pick the size of a chunk, which is the granularity (e.g., 1KB) at which you want to deduplicate a file.
Inspect every non-overlapping chunk in the file (e.g., 0th KB, 1st KB, ..., 99th KB), and identify the unique ones.
For each unique chunk, take note of where they are found in the file (e.g., 0, 1, 2, ..., 99).
