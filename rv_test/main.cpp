#include <cstdlib>
#include <getopt.h>
#include <iostream>
#include <optional>
#include <stdexcept>

#include "lib/tracker.h"

using namespace NTestTracker;

int InitProgramParams(
    int argc,
    char *argv[],
    std::string& videoPath,
    std::string& modelPath,
    std::optional<size_t>& averageWindowSize,
    std::optional<size_t>& medianWindowSize)
{
    char* video_file = nullptr;
    char* model_file = nullptr;
    static struct option long_options[] = {
        {"video", required_argument, nullptr, 'v'},
        {"model", required_argument, nullptr, 'm'},
        {"frame_averaging_window", required_argument, nullptr, 'w'}, // the width of the frame averaging window
        {"median_window", required_argument, nullptr, 'k'}, // size of kernel of median filter
        {"help", no_argument, nullptr, 'h'},
        {nullptr, no_argument, nullptr, 0}
    };

    int opt;
    int option_index = 0;
    const static char options[] = "v:m:w:k:b:h";
    while ((opt = getopt_long(argc, argv, options, long_options, &option_index)) != -1) {
        switch (opt) {
            case 'v':
                video_file = optarg;
                break;
            case 'm':
                model_file = optarg;
                break;
            case 'w':
                try {
                    averageWindowSize = std::make_optional<size_t>(std::stoul(optarg));
                } catch (...) {
                    std::cerr << "Invalid number for --frame_averaging_window: " << optarg << "\n";
                    return 1;
                }

                break;
            case 'k':
                try {
                    medianWindowSize = std::make_optional<size_t>(std::stoul(optarg));
                } catch (...) {
                    std::cerr << "Invalid number for --median_window: " << optarg << "\n";
                    return 1;
                }


                break;
            case 'h':
                std::cout << "Использование: " << argv[0] << " [OPTIONS]\n";
                std::cout << "  -v, --video FILE                    Входной файл с видео\n";
                std::cout << "  -m, --model FILE                    Файл с моделью\n";
                std::cout << "  -w, --frame_averaging_window [N]    Число кадров для усреднения\n";
                std::cout << "  -k, --median_window [N]             Размер ядра фильтра скользящей медианы\n";
                std::cout << "  -h, --help                          Показать справку\n";
                return 1;
            default:
                std::cerr << "Используйте " << argv[0] << " --help для справки\n";
                return 1;
        }
    }

    // Checking required arguments
    if (video_file == nullptr) {
        std::cerr << "The input file is not specified\n";
        return 1;
    } else {
        videoPath = video_file;
    }

    if (model_file == nullptr) {
        std::cerr << "The model file is not specified\n";
        return 1;
    } else {
        modelPath = model_file;
    }

    return 0;
}

int main(int argc, char *argv[]) {
    std::string videoPath;
    std::string modelPath;
    std::optional<size_t> averageWindowSize;
    std::optional<size_t> medianWindowSize;
    std::optional<size_t> bilateralWindowSize;
    if (InitProgramParams(
        argc,
        argv,
        videoPath,
        modelPath,
        averageWindowSize,
        medianWindowSize) == 0)
    {
        TTestTracker trackerJob(videoPath, modelPath, averageWindowSize, medianWindowSize);
        return trackerJob.Run();
    }

    return 0;
}
