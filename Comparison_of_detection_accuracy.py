def compare_files(file1_path, file2_path):
    with open(file1_path, 'r') as file1, open(file2_path, 'r') as file2:
        lines_file1 = file1.readlines()
        lines_file2 = file2.readlines()

    total_lines = len(lines_file1)
    matching_lines = 0
    not_matching_count = 0
    for line_num, (line1, line2) in enumerate(zip(lines_file1, lines_file2), 1):
        # 改行文字を取り除いてから比較する
        if line1.strip() == line2.strip():
            matching_lines += 1
        #else:
            #print(f"行 {line_num}:")
            #print(f"  ファイル1: {line1.strip()}")
            #print(f"  ファイル2: {line2.strip()}")
            #not_matching_count += 1

    similarity_percentage = (matching_lines / total_lines) * 100
    return similarity_percentage, not_matching_count

#if __name__ == "__main__":
file1_path = "out.txt"  # 1つ目のファイルのパスを指定してください
file2_path = "approximate_out.txt"  # 2つ目のファイルのパスを指定してください
similarity_percentage, not_matching_count = compare_files(file1_path, file2_path)
#print(f"ファイルの行の値が一致しなかった回数: {not_matching_count}")
print(f"ファイルの一致度: {similarity_percentage:.2f}%")
