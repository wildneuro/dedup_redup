# dedup_redup
File Deduplication / Reduplication C++11 solution

# General
Deduplication is a specialized form of compression for when the granularity of redundancy is large. Deduplication comes in many shapes and forms, but one of the simplest implementations involves the following three steps:

Pick the size of a chunk, which is the granularity (e.g., 1KB) at which you want to deduplicate a file.
Inspect every non-overlapping chunk in the file (e.g., 0th KB, 1st KB, ..., 99th KB), and identify the unique ones.
For each unique chunk, take note of where they are found in the file (e.g., 0, 1, 2, ..., 99).

# Task Overview
In this challenge, you'll implement two functions: dedup() and redup(). The first function deduplicates a large input file to a smaller output file, which is then used by the second function to reconstruct (or reduplicate) the original file. Both functions take two arguments: the path of the file it needs to read, and the path of the file it needs to create. Both functions return nothing.

In addition to the code, we also expect you to write a short "design document" that describes your solution at a high level: how your algorithm works, how your output file is structured, difficulties you had to overcome, kinks you couldn't iron out, etc. Please include the design document as a comment block at the very top of your solution.
