import matplotlib.pyplot as plt

n = 50 # ファイルの分割数

def plot_values(filename):
    x = []  # x軸の値
    y = []  # y軸の値

    with open(filename, 'r') as file:
        lines = file.readlines()

        # ファイルをn分割して処理
        chunk_size = len(lines) // n
        for i in range(0, len(lines), chunk_size):
            chunk = lines[i:i+chunk_size]

            x.clear()
            y.clear()

            for j, line in enumerate(chunk):
                value = int(line.strip())  # 一行を整数値として読み取る
                x.append(i + j + 1)       # x軸の値を連続して増やす
                y.append(value)           # y軸の値として追加

            # グラフの描画
            plt.figure(figsize=(12, 6))  # 図のサイズを調整
            
            plt.plot(x, y)
            plt.xlabel('sample_index')
            plt.ylabel('trigger_success')
            plt.title('R-peak detection')
            plt.show()

# 使用例
filename = 'out.txt'  # テキストファイルのパスを指定
plot_values(filename)
