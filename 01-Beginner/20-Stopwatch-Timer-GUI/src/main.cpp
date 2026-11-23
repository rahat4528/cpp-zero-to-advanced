#include "StopwatchTimer.h"

#include <chrono>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>

void printHelp() {
    std::cout << "Stopwatch Timer GUI / TUI\n"
              << "Usage:\n"
              << "  StopwatchTimer                                   Interactive terminal stopwatch\n"
              << "  StopwatchTimer --scripted \"<action_list>\"        Automated headless test mode\n"
              << "  StopwatchTimer --help                            Show help\n"
              << "Scripted Actions: start, sleep:<ms>, lap, pause, resume, stop, reset\n"
              << "Example:\n"
              << "  StopwatchTimer --scripted \"start;sleep:100;lap;sleep:150;lap;stop\"\n";
}

void printLapsTable(const stopwatch_timer::StopwatchTimer& sw) {
    const auto& laps = sw.getLaps();
    if (laps.empty()) return;

    auto fastestOpt = sw.getFastestLapIndex();
    auto slowestOpt = sw.getSlowestLapIndex();

    std::cout << "\n----------------------------------------------------\n";
    std::cout << std::left << std::setw(8) << "LAP #"
              << std::setw(18) << "SPLIT TIME"
              << std::setw(16) << "LAP TIME"
              << "TAG\n";
    std::cout << "----------------------------------------------------\n";
    for (size_t i = 0; i < laps.size(); ++i) {
        std::string tag = "-";
        if (fastestOpt && *fastestOpt == i) tag = "[FASTEST]";
        if (slowestOpt && *slowestOpt == i && laps.size() > 1) tag = "[SLOWEST]";

        std::cout << std::left << std::setw(8) << laps[i].lapNumber
                  << std::setw(18) << stopwatch_timer::StopwatchTimer::formatDuration(laps[i].splitTimeSeconds)
                  << std::setw(16) << stopwatch_timer::StopwatchTimer::formatDuration(laps[i].lapDeltaSeconds)
                  << tag << "\n";
    }
    std::cout << "----------------------------------------------------\n";
}

int main(int argc, char* argv[]) {
    std::string scriptedActions;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--help" || arg == "-h") {
            printHelp();
            return 0;
        } else if (arg == "--scripted" && i + 1 < argc) {
            scriptedActions = argv[++i];
        }
    }

    stopwatch_timer::StopwatchTimer sw;

    if (!scriptedActions.empty()) {
        std::cout << "=== Stopwatch Timer Automated Test Run ===\n";
        std::stringstream ss(scriptedActions);
        std::string action;

        while (std::getline(ss, action, ';')) {
            if (action == "start") {
                sw.start();
                std::cout << ">> Stopwatch STARTED\n";
            } else if (action == "pause") {
                sw.pause();
                std::cout << ">> Stopwatch PAUSED at: " << sw.getFormattedTime() << "\n";
            } else if (action == "resume") {
                sw.resume();
                std::cout << ">> Stopwatch RESUMED\n";
            } else if (action == "stop") {
                sw.stop();
                std::cout << ">> Stopwatch STOPPED at: " << sw.getFormattedTime() << "\n";
            } else if (action == "reset") {
                sw.reset();
                std::cout << ">> Stopwatch RESET\n";
            } else if (action == "lap") {
                auto lap = sw.recordLap();
                if (lap) {
                    std::cout << ">> LAP #" << lap->lapNumber << " Recorded: "
                              << stopwatch_timer::StopwatchTimer::formatDuration(lap->lapDeltaSeconds)
                              << " (Total: " << stopwatch_timer::StopwatchTimer::formatDuration(lap->splitTimeSeconds) << ")\n";
                }
            } else if (action.rfind("sleep:", 0) == 0) {
                int ms = std::stoi(action.substr(6));
                std::this_thread::sleep_for(std::chrono::milliseconds(ms));
            }
        }

        std::cout << "\nFinal Elapsed Time: " << sw.getFormattedTime() << "\n";
        printLapsTable(sw);
        return 0;
    }

    std::cout << "=== Precision Stopwatch Timer ===\n";
    std::cout << "Press ENTER to start timer...";
    std::string line;
    std::getline(std::cin, line);
    sw.start();

    std::cout << "Running... Press ENTER to record Lap, or type 'q' to stop:\n";
    while (std::getline(std::cin, line) && line != "q") {
        auto lap = sw.recordLap();
        if (lap) {
            std::cout << "Lap " << lap->lapNumber << ": "
                      << stopwatch_timer::StopwatchTimer::formatDuration(lap->lapDeltaSeconds)
                      << " (Split: " << sw.getFormattedTime() << ")\n";
        }
    }

    sw.stop();
    std::cout << "\nStopwatch Final Time: " << sw.getFormattedTime() << "\n";
    printLapsTable(sw);

    return 0;
}
