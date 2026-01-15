#include <cstdlib>
#include <getopt.h>
#include <iostream>
#include <optional>
#include <stdexcept>

#include "lib/tracker.h"

using namespace NTestTracker;

/**
 * @brief Parses command-line arguments to configure the tracking application.
 *
 * This function uses getopt_long to parse both short (-v) and long (--video) options.
 * It populates the provided reference parameters with the parsed values.
 *
 * @param argc The argument count from main().
 * @param argv The argument vector from main().
 * @param[out] videoPath Reference to a string that will store the path to the video file.
 * @param[out] modelPath Reference to a string that will store the path to the model file.
 * @param[out] averageWindowSize Optional reference to store the frame averaging window size.
 * @param[out] medianWindowSize Optional reference to store the median filter window size.
 * @return int Returns 0 on successful parsing, and a non-zero value on error or if help is displayed.
 */
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
        {"frame_averaging_window", required_argument, nullptr, 'w'},
        {"median_window", required_argument, nullptr, 'k'},
        {"help", no_argument, nullptr, 'h'},
        {nullptr, no_argument, nullptr, 0}
    };

    int opt;
    int option_index = 0;

    // This string defines the short options. A colon (:) after a character means it requires an argument.
    const static char options[] = "v:m:w:k:h";
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
                std::cout << "Usage: " << argv[0] << " [OPTIONS]\n\n";
                std::cout << "Required arguments:\n";
                std::cout << "  -v, --video FILE                    Input video file path.\n";
                std::cout << "  -m, --model FILE                    Path to the ONNX model file.\n\n";
                std::cout << "Optional arguments:\n";
                std::cout << "  -w, --frame_averaging_window N      Number of frames for the moving average filter.\n";
                std::cout << "  -k, --median_window N               Kernel size for the median filter (must be an odd number > 1).\n";
                std::cout << "  -h, --help                          Show this help message and exit.\n";
                return 1; // Return 1 to indicate that the program should exit.
            default: // Handles unknown options
                std::cerr << "Try '" << argv[0] << " --help' for more information.\n";
                return 1;
        }
    }

    // Checking required arguments
    if (video_file == nullptr) {
        std::cerr << "Error: The input video file is a required argument.\n";
        std::cerr << "Use --video <path_to_file>.\n";
        return 1;
    } else {
        videoPath = video_file;
    }

    if (model_file == nullptr) {
        std::cerr << "Error: The model file is a required argument.\n";
        std::cerr << "Use --model <path_to_file>.\n";
        return 1;
    } else {
        modelPath = model_file;
    }

    return 0; // Success
}

/**
 * @brief Main entry point of the application.
 */
 int main(int argc, char *argv[]) {
    // Variables to hold the application's configuration.
    std::string videoPath;
    std::string modelPath;
    std::optional<size_t> averageWindowSize;
    std::optional<size_t> medianWindowSize;
    std::optional<size_t> bilateralWindowSize;

    // Parse command-line arguments.
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
