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
    int trackID;
    double x, y, z;
};

bool isRightFront(int copyNo) { return copyNo >= 0 && copyNo <= 12; }
bool isRightBack(int copyNo)  { return copyNo >= 100 && copyNo <= 112; }
bool isLeftFront(int copyNo)  { return copyNo >= 200 && copyNo <= 212; }
bool isLeftBack(int copyNo)   { return copyNo >= 300 && copyNo <= 312; }

int main() {
    std::ifstream inFile("FilteredEvent_20250627_214333_1Cr.dat");
    if (!inFile.is_open()) {
        std::cerr << "Error opening FilteredEvent_20250627_214333_1Cr.dat" << std::endl;
        return 1;
    }

    TFile* outFile = new TFile("ReconstructedVertices_20250627_214333_1Cr.root", "RECREATE");
    TTree* tree = new TTree("Vertices", "Extrapolated vertex positions");

    double y_proj, z_proj;
    long long eventID;

    tree->Branch("eventID", &eventID, "eventID/L");
    tree->Branch("y", &y_proj, "y/D");
    tree->Branch("z", &z_proj, "z/D");

    TH2D* h2_yz = new TH2D("h2_yz", "Extrapolated Vertices (Y vs Z);Y (cm);Z (cm)",
                           200, -100, 100, 200, -100, 100);

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
        hit.copyNo  = std::stoi(fields[7]);
        hit.x       = std::stod(fields[9]);
        hit.y       = std::stod(fields[10]);
        hit.z       = std::stod(fields[11]);
        hit.trackID = std::stoi(fields[3]); // Use trackID for matching

        eventHits[hit.eventID].push_back(hit);
    }

    // For each event, process all valid track-matched pairs
    for (const auto& [evtID, hits] : eventHits) {
        // Group hits by trackID
        std::unordered_map<int, std::vector<Hit>> tracks;
        for (const auto& h : hits) {
            tracks[h.trackID].push_back(h);
        }

        for (const auto& [trkID, trkHits] : tracks) {
            // Look for left and right pairs separately
            Hit front, back;
            bool foundLeft = false, foundRight = false;

            // Left
            for (const auto& h1 : trkHits)
                if (isLeftFront(h1.copyNo)) front = h1;
            for (const auto& h2 : trkHits)
                if (isLeftBack(h2.copyNo)) back = h2;
            if (isLeftFront(front.copyNo) && isLeftBack(back.copyNo)) {
                double x1 = front.x, y1 = front.y, z1 = front.z;
                double x2 = back.x, y2 = back.y, z2 = back.z;
                double dx = x2 - x1;
                if (std::abs(dx) > 1e-6) {
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

            // Right
            front = back = Hit();
            for (const auto& h1 : trkHits)
                if (isRightFront(h1.copyNo)) front = h1;
            for (const auto& h2 : trkHits)
                if (isRightBack(h2.copyNo)) back = h2;
            if (isRightFront(front.copyNo) && isRightBack(back.copyNo)) {
                double x1 = front.x, y1 = front.y, z1 = front.z;
                double x2 = back.x, y2 = back.y, z2 = back.z;
                double dx = x2 - x1;
                if (std::abs(dx) > 1e-6) {
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
        }
    }

    tree->Write();
    h2_yz->Write();
    outFile->Close();

    std::cout << "ROOT file 'ReconstructedVertices.root' with 2D histogram written successfully.\n";
    return 0;
}
