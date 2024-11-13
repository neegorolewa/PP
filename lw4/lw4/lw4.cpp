#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <fstream>
#include <opencv2/opencv.hpp>
#include <windows.h>

struct ThreadData
{
    cv::Mat* image;
    int startCol;
    int endCol;
    int threadId;
    std::ofstream* logFile;
    std::chrono::high_resolution_clock::time_point startTime;
    std::vector<std::chrono::high_resolution_clock::time_point>* timestamps;
};

DWORD WINAPI BlurImagePart(LPVOID lpParam)
{
    ThreadData* data = (ThreadData*)lpParam;
    cv::Mat roi = (*data->image)(cv::Range::all(), cv::Range(data->startCol, data->endCol));


    //for (int i = 0; i < 20; i++)
    //{
    //    cv::blur(roi, roi, cv::Size(5, 5));

    //    auto endTime = std::chrono::high_resolution_clock::now();
    //    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);

    //    //(*data->logFile) << "Thread " << data->threadId << " processed in " << duration << " ms" << std::endl;
    //    data->timestamps->push_back(duration);

    //}
    

    for (int y = 1; y < roi.rows - 1; ++y) 
    {
        for (int x = 1; x < roi.cols - 1; ++x) 
        {

            cv::Rect pixels(x - 1, y - 1, 3, 3);
            cv::Mat smallImage = roi(pixels);

            for (int i = 0; i < 100; i++)
            {
                cv::blur(smallImage, smallImage, cv::Size(3, 3));
            }

            smallImage.copyTo(roi(pixels));

            auto endTime = std::chrono::high_resolution_clock::now();

            //(*data->logFile) << "Thread " << data->threadId << " processed in " << duration << " ms" << std::endl;
            data->timestamps->push_back(endTime);
        }
    }

    ExitThread(0);
}

void ProcessImage(const std::string& inputPath, const std::string& outputPath, int numThreads, int numCores)
{
    // Загрузка изображения
    cv::Mat image = cv::imread(inputPath, cv::IMREAD_COLOR);
    if (image.empty()) {
        std::cerr << "Could not open or find the image: " << inputPath << std::endl;
        return;
    }

    // Разделение изображения на вертикальные полосы
    int width = image.cols;
    int stripWidth = width / numThreads;

    std::vector<HANDLE> threads(numThreads);
    std::vector<ThreadData> threadData(numThreads);
    std::vector<std::vector<std::chrono::high_resolution_clock::time_point>> timestamps(numThreads);

    std::ofstream logFile("processing_log.txt");

    if (!logFile.is_open())
    {
        std::cerr << "Could not open log file for writing." << std::endl;
        return;
    }

    auto startTime = std::chrono::high_resolution_clock::now();

    // Создание потоков для обработки каждой полосы
    for (int i = 0; i < numThreads; ++i) 
    {
        threadData[i].image = &image;
        threadData[i].startCol = i * stripWidth;
        threadData[i].endCol = (i == numThreads - 1) ? width : (i + 1) * stripWidth;

        threadData[i].threadId = i;
        threadData[i].logFile = &logFile;
        threadData[i].startTime = startTime; // Передаем время начала обработки
        threadData[i].timestamps = &timestamps[i]; // Передаем вектор временных меток

        threads[i] = CreateThread(NULL, 0, BlurImagePart, &threadData[i], 0, NULL);

        if (threads[i] == NULL)
        {
            std::cerr << "Failed to create thread " << i << std::endl;
            return;
        }

        // Установка маски сходства для эмуляции количества ядер
        SetThreadAffinityMask(threads[i], 1 << (i % numCores));

        int priority;
        switch (i)
        {
        case 0:
            priority = THREAD_PRIORITY_ABOVE_NORMAL;
            break;
        case 1:
            priority = THREAD_PRIORITY_NORMAL;
            break;
        case 2:
            priority = THREAD_PRIORITY_BELOW_NORMAL;
            break;
        default:
            priority = THREAD_PRIORITY_NORMAL;
            break;
        }

        if (!SetThreadPriority(threads[i], priority))
        {
            std::cerr << "Failed to set thread priority for thread " << i << std::endl;
            return;
        }
    }

    for (const HANDLE& thread : threads)
    {
        if (ResumeThread(thread) == static_cast<DWORD>(-1))
        {
            std::cerr << "Error: unable to resume thread" << std::endl;
            return;
        }
    }

    // Ожидание завершения всех потоков
    WaitForMultipleObjects(numThreads, threads.data(), TRUE, INFINITE);

    // Закрытие дескрипторов потоков
    for (int i = 0; i < numThreads; ++i)
    {
        CloseHandle(threads[i]);
    }

    /*auto endTime = std::chrono::high_resolution_clock::now();
    auto totalDuration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();*/

    for (int i = 0; i < numThreads; ++i)
    {
        for (const auto& timestamp : timestamps[i])
        {
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(timestamp - startTime).count();
            //запись в файлы
            logFile << i << ' ' << duration << std::endl;
        }
    }

    // Сохранение обработанного изображения
    if (!cv::imwrite(outputPath, image))
    {
        std::cerr << "Could not write the image to: " << outputPath << std::endl;
    }

    logFile.close();
}

int main(int argc, char* argv[])
{
    if (argc < 5)
    {
        std::cerr << "Usage: " << argv[0] << " <input_file> <output_file> <num_threads> <num_cores>" << std::endl;
        return 1;
    }

    std::string inputPath = argv[1];
    std::string outputPath = argv[2];
    int numThreads = std::stoi(argv[3]);
    int numCores = std::stoi(argv[4]);

    ProcessImage(inputPath, outputPath, numThreads, numCores);

    return 0;
}