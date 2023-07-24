import csv

def extract_column_to_text(input_file, output_file, column_index):
    try:
        # CSVファイルを読み込む
        with open(input_file, 'r', encoding='utf-8') as csvfile:
            reader = csv.reader(csvfile)
            # 出力ファイルを作成し、指定した列の要素を一行ずつ書き込む
            with open(output_file, 'w', encoding='utf-8') as outfile:
                for row in reader:
                    if len(row) > column_index:
                        outfile.write(row[column_index] + '\n')
                    else:
                        print(f"指定した列番号 {column_index} はCSVファイルの列数を超えています。")

        print("テキストファイルの生成が完了しました。")

    except FileNotFoundError:
        print("指定したCSVファイルが見つかりません。")

input_csv_file = "100.csv"   # 入力のCSVファイル名を指定してください
output_text_file = "ecg.txt"   # 出力のテキストファイル名を指定してください
target_column_index = 2   # 抽出したい列のインデックス（0から始まる列番号）を指定してください

extract_column_to_text(input_csv_file, output_text_file, target_column_index)
