#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <cmath>

#include "TFile.h"
#include "TTree.h"
#include "TH2D.h"

struct Hit {
    long long eventID;
    int copyNo;
    double x, y, z;
};

bool isRightFront(int copyNo) {
    return copyNo >= 0 && copyNo <= 12;
}
bool isRightBack(int copyNo) {
    return copyNo >= 100 && copyNo <= 112;
}
bool isLeftFront(int copyNo) {
    return copyNo >= 200 && copyNo <= 212;
}
bool isLeftBack(int copyNo) {
    return copyNo >= 300 && copyNo <= 312;
}

int main() {
    std::ifstream inFile("FilteredEvents.dat");
    if (!inFile.is_open()) {
        std::cerr << "Error opening FilteredEvents.dat" << std::endl;
        return 1;
    }

    TFile* outFile = new TFile("ReconstructedVertices.root", "RECREATE");
    TTree* tree = new TTree("Vertices", "Extrapolated vertex positions");

    double y_proj, z_proj;
    long long eventID;

    tree->Branch("eventID", &eventID, "eventID/L");
    tree->Branch("y", &y_proj, "y/D");
    tree->Branch("z", &z_proj, "z/D");

    TH2D* h2_yz = new TH2D("h2_yz", "Extrapolated Vertices (Y vs Z);Y (cm);Z (cm)", 
                           200, -100, 100, 
                           200, -100, 100);

    // Set axis divisions for more tick marks
    h2_yz->GetXaxis()->SetNdivisions(20, kTRUE);  // Ticks every 5 units on Y axis
    h2_yz->GetYaxis()->SetNdivisions(20, kTRUE);  // Ticks every 5 units on Z axis

    std::unordered_map<long long, std::vector<Hit>> eventHits;

    std::string line;
    while (std::getline(inFile, line)) {
        std::istringstream iss(line);
        std::vector<std::string> fields;
        std::string word;
        while (iss >> word) fields.push_back(word);

        if (fields.size() < 17) continue;

        Hit hit;
        hit.eventID = std::stoll(fields[0]);
        hit.copyNo = std::stoi(fields[7]);
        hit.x = std::stod(fields[9]);
        hit.y = std::stod(fields[10]);
        hit.z = std::stod(fields[11]);

        eventHits[hit.eventID].push_back(hit);
    }

    for (const auto& [evtID, hits] : eventHits) {
        Hit front, back;
        bool found = false;

        for (const auto& h1 : hits) {
            for (const auto& h2 : hits) {
                if (h1.copyNo == h2.copyNo) continue;

                if (isLeftFront(h1.copyNo) && isLeftBack(h2.copyNo)) {
                    front = h1;
                    back = h2;
                    found = true;
                }
                else if (isRightFront(h1.copyNo) && isRightBack(h2.copyNo)) {
                    front = h1;
                    back = h2;
                    found = true;
                }

                if (found) break;
            }
            if (found) break;
        }

        if (found) {
            double x1 = front.x, y1 = front.y, z1 = front.z;
            double x2 = back.x, y2 = back.y, z2 = back.z;
            double dx = x2 - x1;

            if (std::abs(dx) < 1e-6) continue;

            double dy = y2 - y1;
            double dz = z2 - z1;

            double t = (-8.0 - x1) / dx;

            y_proj = y1 + t * dy;
            z_proj = z1 + t * dz;
            eventID = evtID;

            tree->Fill();
            h2_yz->Fill(y_proj, z_proj);
        }
    }

    tree->Write();
    h2_yz->Write();
    outFile->Close();

    std::cout << "ROOT file 'ReconstructedVertices.root' with 2D histogram written successfully.\n";
    return 0;
}
