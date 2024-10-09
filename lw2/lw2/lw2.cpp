#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <windows.h>
#include <chrono>

using namespace cv;
using namespace std;

// Структура для передачи данных в поток
struct ThreadData {
    Mat* image;
    int startRow;
    int endRow;
};

// Функция для размытия части изображения
DWORD WINAPI blurPart(LPVOID lpParam) {
    ThreadData* data = (ThreadData*)lpParam;
    Mat part = (*data->image)(Range(data->startRow, data->endRow), Range::all());
    GaussianBlur(part, part, Size(21, 21), 0);
    return 0;
}

// Основная логика обработки изображения
void processImage(const string& inputFile, const string& outputFile, int numThreads, int numCores) {
    // Загрузка изображения
    Mat image = imread(inputFile, IMREAD_COLOR);
    if (image.empty()) {
        cout << "Could not open or find the image: " << inputFile << endl;
        return;
    }

    // Ограничение количества потоков по количеству ядер
    if (numThreads > numCores) {
        numThreads = numCores;
    }

    // Разделение изображения на горизонтальные полосы
    int rowsPerThread = image.rows / numThreads;

    // Создание вектора для хранения данных потоков
    vector<ThreadData> threadData(numThreads);
    vector<HANDLE> threads(numThreads);

    // Создание потоков
    for (int i = 0; i < numThreads; ++i) {
        threadData[i].image = &image;
        threadData[i].startRow = i * rowsPerThread;
        threadData[i].endRow = (i == numThreads - 1) ? image.rows : (i + 1) * rowsPerThread;
        threads[i] = CreateThread(NULL, 0, blurPart, &threadData[i], 0, NULL);
        if (threads[i] == NULL) {
            cout << "Failed to create thread " << i << endl;
            return;
        }
    }

    // Ожидание завершения всех потоков
    WaitForMultipleObjects(numThreads, threads.data(), TRUE, INFINITE);

    // Закрытие дескрипторов потоков
    for (int i = 0; i < numThreads; ++i) {
        CloseHandle(threads[i]);
    }

    // Сохранение результата
    bool success = imwrite(outputFile, image);
    if (!success) {
        cout << "Failed to save the image: " << outputFile << endl;
        return;
    }

    cout << "Image processing complete. Output saved as " << outputFile << endl;
}

int main(int argc, char* argv[]) {
    if (argc != 5) {
        cout << "Usage: " << argv[0] << " <input_file> <output_file> <num_threads> <num_cores>" << endl;
        return -1;
    }

    auto startTime = std::chrono::high_resolution_clock::now();

    // Получение параметров командной строки
    string inputFile = argv[1];
    string outputFile = argv[2];
    int numThreads = atoi(argv[3]);
    int numCores = atoi(argv[4]);

    // Вызов метода с основной логикой
    processImage(inputFile, outputFile, numThreads, numCores);

    auto endTime = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> duration = endTime - startTime;

    std::cout << "Execution time: " << duration.count() << " ms" << std::endl;

    return 0;
}