#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define WINDOWSIZE 20    // Integrator window size, in samples.
#define NOSAMPLE -32000  // Indicator for no more samples.
#define FS 360           // Sampling frequency.

void readFile(const char *filename, int **data, int *length);
void detectRPeaks(int *data, int length, int **rPeaks, int *numRPeaks);
void calculateRRIntervals(int *rPeaks, int numRPeaks, int **rrIntervals, int *numRRIntervals);
void classifyBeats(int *rrIntervals, int numRRIntervals, int **classifications);
void writeResults(const char *filename, int *classifications, int numRRIntervals);

void readFile(const char *filename, int **data, int *length) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("ファイルを開くことができません");
        exit(EXIT_FAILURE);
    }

    // Allocate memory for data array (assuming max length based on file size)
    *data = (int *)malloc(650000 * sizeof(int)); // As the input file has about 650000 lines
    if (*data == NULL) {
        perror("メモリの割り当てに失敗しました");
        fclose(file);
        exit(EXIT_FAILURE);
    }

    // Read data into the array
    *length = 0;
    while (fscanf(file, "%d", &(*data)[*length]) != EOF) {
        (*length)++;
    }

    fclose(file);
}

void detectRPeaks(int *data, int length, int **rPeaks, int *numRPeaks) {
    // Allocate memory for rPeaks array
    *rPeaks = (int *)malloc(length * sizeof(int));
    if (*rPeaks == NULL) {
        perror("メモリの割り当てに失敗しました");
        exit(EXIT_FAILURE);
    }

    *numRPeaks = 0;
    for (int i = 0; i < length; i++) {
        if (data[i] == 1) {
            (*rPeaks)[*numRPeaks] = i;
            (*numRPeaks)++;
        }
    }

    // Resize rPeaks array to fit the exact number of peaks found
    *rPeaks = (int *)realloc(*rPeaks, *numRPeaks * sizeof(int));
    if (*rPeaks == NULL && *numRPeaks > 0) {
        perror("メモリの割り当てに失敗しました");
        exit(EXIT_FAILURE);
    }
}

void calculateRRIntervals(int *rPeaks, int numRPeaks, int **rrIntervals, int *numRRIntervals) {
    *numRRIntervals = numRPeaks - 1;
    *rrIntervals = (int *)malloc(*numRRIntervals * sizeof(int));
    if (*rrIntervals == NULL) {
        perror("メモリの割り当てに失敗しました");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < *numRRIntervals; i++) {
        double intervalInSeconds = (double)(rPeaks[i + 1] - rPeaks[i]) / FS;
        (*rrIntervals)[i] = (int)(intervalInSeconds * 1000);
    }
}

void classifyBeats(int *rrIntervals, int numRRIntervals, int **classifications) {
    *classifications = (int *)malloc(numRRIntervals * sizeof(int));
    if (*classifications == NULL) {
        perror("メモリの割り当てに失敗しました");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < numRRIntervals; i++) {
        (*classifications)[i] = 1; // Initialize as normal

        if (i > 1) {
            int RR1 = rrIntervals[i - 2];
            int RR2 = rrIntervals[i - 1];
            int RR3 = rrIntervals[i];

            // Classification rules
            if ((RR1 < 1.8 * RR2 && RR2 < 600) || ((rrIntervals[i - 1] + rrIntervals[i] + rrIntervals[i + 1]) < 1700)) {
                int j = i;
                int VFCount = 0;
                while (j < numRRIntervals && ((rrIntervals[j - 1] < 700 && rrIntervals[j] < 700 && rrIntervals[j + 1] < 700) || (rrIntervals[j - 1] + rrIntervals[j] + rrIntervals[j + 1]) < 1700)) {
                    (*classifications)[j] = 3;
                    j++;
                    VFCount++;
                }
                if (VFCount < 4) {
                    for (int k = i; k < j; k++) {
                        (*classifications)[k] = 1;
                    }
                }
                i = j - 1;
            } else if ((RR1 < 1.15 * RR2 && RR3 < 1.15 * RR2) ||
                       (fabs(RR1 - RR2) < 300 && RR1 > 800 && RR2 > 800 && 1.2 * (RR1 + RR2) < 2 * RR3) ||
                       (fabs(RR2 - RR3) < 300 && RR2 > 800 && RR3 > 800 && 1.2 * (RR2 + RR3) < 2 * RR1)) {
                (*classifications)[i - 1] = 2;
            } else if (RR2 > 2200 && RR2 < 3000 && (fabs(RR1 - RR2) < 200 || fabs(RR2 - RR3) < 200)) {
                (*classifications)[i - 1] = 4;
            }
        }
    }
}

void writeResults(const char *filename, int *classifications, int numRRIntervals) {
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        perror("ファイルを開くことができません");
        exit(EXIT_FAILURE);
    }

    //fprintf(file, "R波の位置:\n");
    //for (int i = 0; i < numRPeaks; i++) {
    //    fprintf(file, "%d\n", rPeaks[i]);
    //}
    //fprintf(file, "\nRR間隔:\n");
    //for (int i = 0; i < numRRIntervals; i++) {
    //    fprintf(file, "%d\n", rrIntervals[i]);
    //}

    // Write classifications to file
    fprintf(file, "心拍分類:\n");
    for (int i = 0; i < numRRIntervals; i++) {
        fprintf(file, "%d\n", classifications[i]);
    }

    fclose(file);
}

int main() {
    const char *filename = "out.txt";
    int *data = NULL;
    int length = 0;

    // ファイルからデータを読み込む
    readFile(filename, &data, &length);

    // R波の位置を検出
    int *rPeaks = NULL;
    int numRPeaks = 0;
    detectRPeaks(data, length, &rPeaks, &numRPeaks);

    // RR間隔を計算
    int *rrIntervals = NULL;
    int numRRIntervals = 0;
    calculateRRIntervals(rPeaks, numRPeaks, &rrIntervals, &numRRIntervals);

    // 心拍を分類
    int *classifications = NULL;
    classifyBeats(rrIntervals, numRRIntervals, &classifications);

    // 結果をテキストファイルに出力
    writeResults("results.txt", classifications, numRRIntervals);

    // メモリの解放
    free(data);
    free(rPeaks);
    free(rrIntervals);
    free(classifications);

    return 0;
}
