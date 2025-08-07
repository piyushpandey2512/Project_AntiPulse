#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <map>
#include <algorithm>

int main() {
    std::ifstream inFile("PionInteractions_20250807_152941_Testing_No_STL.dat");
    std::ofstream outFile("FilteredEvent_20250807_152941_Testing_No_STL.dat");

    if (!inFile.is_open() || !outFile.is_open()) {
        std::cerr << "Error opening files!" << std::endl;
        return 1;
    }

    std::unordered_map<long long, std::vector<std::string>> eventMap;

    std::string line;
    while (std::getline(inFile, line)) {
        std::istringstream iss(line);
        std::vector<std::string> fields;
        std::string word;

        // Split the line into fields
        while (iss >> word) {
            fields.push_back(word);
        }

        // Filter conditions
        if (fields.size() >= 17 &&
            fields[6] == "Scintillator" &&
            (
                fields[5] == "pi+" ||
                fields[5] == "pi-" ||
                fields[5] == "kaon+" ||
                fields[5] == "kaon-"
            ) && fields[8] == "in" &&
            (fields[9] == "14.550" || fields[9] == "24.550")) {

            long long eventID = std::stoll(fields[0]);
            eventMap[eventID].push_back(line);
        }
    }

    // Sort event IDs numerically
    std::vector<long long> sortedEventIDs;
    for (const auto& [eventID, _] : eventMap) {
        sortedEventIDs.push_back(eventID);
    }
    std::sort(sortedEventIDs.begin(), sortedEventIDs.end());

    // Write filtered lines in sorted event order
    for (const auto& eventID : sortedEventIDs) {
        const auto& lines = eventMap.at(eventID);
        size_t count = lines.size();
        if (count <= 1) continue;
        if (count % 2 != 0) count--;  // Remove last if odd

        for (size_t i = 0; i < count; ++i) {
            outFile << lines[i] << '\n';
        }
    }

    inFile.close();
    outFile.close();

    std::cout << "Filtered and sorted data written to FilteredEvents.dat" << std::endl;
    return 0;
}
