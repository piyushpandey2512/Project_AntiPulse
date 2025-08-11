


/***This one is working better */

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <cmath>
#include <algorithm> // For std::minmax_element

#include "TFile.h"
#include "TTree.h"
#include "TH2D.h"
#include "TVector3.h" // Use TVector3 for clarity and its helpful methods

// --- Data Structures ---
struct Hit {
    long long eventID;
    int       trackID;
    int       copyNo;
    TVector3  position; // Use a TVector3 to hold coordinates
};

// --- Geometry Helper Functions ---
// These functions correctly identify which part of the detector was hit.
bool isFront(int c) { return (c >= 200 && c <= 212) || (c >= 0 && c <= 12); }
bool isBack(int c)  { return (c >= 300 && c <= 312) || (c >= 100 && c <= 112); }


int main() {
    // --- SETUP ---
    std::ifstream inFile("FilteredEvent_20250808_145228_3PtSource_new_1Cr.dat");
    if (!inFile.is_open()) {
        std::cerr << "Error opening data file." << std::endl;
        return 1;
    }

    // --- GEOMETRY DEFINITION ---
    // According to the Geant4 code, the sources are on the YZ-plane (x=0).
    const double targetX = -8.0;

    // --- ROOT FILE SETUP ---
    TFile* outFile = new TFile("ReconstructedVertices_IMPROVED.root", "RECREATE");
    TTree* tree = new TTree("Vertices", "Extrapolated vertex YZ positions");

    double y_proj, z_proj;
    long long eventID;
    tree->Branch("eventID", &eventID, "eventID/L");
    tree->Branch("y", &y_proj, "y/D");
    tree->Branch("z", &z_proj, "z/D");

    TH2D* h2_yz = new TH2D("h2_yz", "Extrapolated Vertices (Y vs Z);Y (cm);Z (cm)", 200, -100, 100, 200, -100, 100);

    // --- PARSING ---
    std::unordered_map<long long, std::vector<Hit>> eventHits;
    std::string line;
    std::cout << "Reading and parsing data file...\n";
    while (std::getline(inFile, line)) {
        std::istringstream iss(line);
        std::vector<std::string> fields;
        std::string word;
        while (iss >> word) fields.push_back(word);

        if (fields.size() < 17) continue;

        Hit hit;
        hit.eventID = std::stoll(fields[0]);
        hit.trackID = std::stoi(fields[3]);
        hit.copyNo  = std::stoi(fields[7]);
        hit.position = TVector3(std::stod(fields[9]), std::stod(fields[10]), std::stod(fields[11]));

        eventHits[hit.eventID].push_back(hit);
    }
    std::cout << "Finished reading file. Found " << eventHits.size() << " unique events.\n";


    // --- RECONSTRUCTION ---
    int vertexCount = 0;
    std::cout << "Finding tracks and extrapolating to source plane...\n";
    for (const auto& [evtID, hits] : eventHits) {
        std::unordered_map<int, std::vector<Hit>> tracks;
        for (const auto& h : hits) tracks[h.trackID].push_back(h);

        for (const auto& [trkID, trkHits] : tracks) {
            if (trkHits.size() < 2) continue;

            // CORRECTED LOGIC: Find the hits with the minimum and maximum X coordinates
            auto minmax_hits = std::minmax_element(trkHits.begin(), trkHits.end(),
                [](const Hit& a, const Hit& b) {
                    return a.position.X() < b.position.X();
                });

            const Hit& minX_hit = *minmax_hits.first;
            const Hit& maxX_hit = *minmax_hits.second;

            // Sanity Check: Ensure this is a valid, forward-going track
            if (isFront(minX_hit.copyNo) && isBack(maxX_hit.copyNo)) {
                
                TVector3 p1 = minX_hit.position;
                TVector3 p2 = maxX_hit.position;
                TVector3 dir = p2 - p1;

                // Check for division by zero (track is parallel to the YZ plane)
                if (std::abs(dir.X()) > 1e-6) {
                    
                    // This is the parameter 't' in the line equation P(t) = P1 + t * Dir
                    double t = (targetX - p1.X()) / dir.X();

                    // Calculate the y and z coordinates at that point
                    y_proj = p1.Y() + t * dir.Y();
                    z_proj = p1.Z() + t * dir.Z();
                    
                    // Save the result
                    eventID = evtID;
                    tree->Fill();
                    h2_yz->Fill(y_proj, z_proj);
                    vertexCount++;
                }
            }
        }
    }

    // --- FINALIZATION ---
    std::cout << "Track extrapolation completed.\n";
    std::cout << "Total extrapolated vertices found: " << vertexCount << "\n";
    outFile->cd();
    tree->Write();
    h2_yz->Write();
    outFile->Close();

    return 0;
}







/* Working code (old)**/

// #include <iostream>
// #include <fstream>
// #include <sstream>
// #include <string>
// #include <vector>
// #include <unordered_map>
// #include <cmath>

// #include "TFile.h"
// #include "TTree.h"
// #include "TH2D.h"

// struct Hit {
//     long long eventID;
//     int copyNo;
//     int trackID;
//     double x, y, z;
// };

// bool isRightFront(int copyNo) { return copyNo >= 0 && copyNo <= 12; }
// bool isRightBack(int copyNo)  { return copyNo >= 100 && copyNo <= 112; }
// bool isLeftFront(int copyNo)  { return copyNo >= 200 && copyNo <= 212; }
// bool isLeftBack(int copyNo)   { return copyNo >= 300 && copyNo <= 312; }

// int main() {
//     std::ifstream inFile("FilteredEvent_20250627_214333_1Cr.dat");
//     if (!inFile.is_open()) {
//         std::cerr << "Error opening FilteredEvent_20250627_214333_1Cr.dat" << std::endl;
//         return 1;
//     }

//     TFile* outFile = new TFile("ReconstructedVertices_20250627_214333_1Cr.root", "RECREATE");
//     TTree* tree = new TTree("Vertices", "Extrapolated vertex positions");

//     double y_proj, z_proj;
//     long long eventID;

//     tree->Branch("eventID", &eventID, "eventID/L");
//     tree->Branch("y", &y_proj, "y/D");
//     tree->Branch("z", &z_proj, "z/D");

//     TH2D* h2_yz = new TH2D("h2_yz", "Extrapolated Vertices (Y vs Z);Y (cm);Z (cm)",
//                            200, -100, 100, 200, -100, 100);

//     std::unordered_map<long long, std::vector<Hit>> eventHits;

//     std::string line;
//     while (std::getline(inFile, line)) {
//         std::istringstream iss(line);
//         std::vector<std::string> fields;
//         std::string word;
//         while (iss >> word) fields.push_back(word);

//         if (fields.size() < 17) continue;

//         Hit hit;
//         hit.eventID = std::stoll(fields[0]);
//         hit.copyNo  = std::stoi(fields[7]);
//         hit.x       = std::stod(fields[9]);
//         hit.y       = std::stod(fields[10]);
//         hit.z       = std::stod(fields[11]);
//         hit.trackID = std::stoi(fields[3]); // Use trackID for matching

//         eventHits[hit.eventID].push_back(hit);
//     }

//     // For each event, process all valid track-matched pairs
//     for (const auto& [evtID, hits] : eventHits) {
//         // Group hits by trackID
//         std::unordered_map<int, std::vector<Hit>> tracks;
//         for (const auto& h : hits) {
//             tracks[h.trackID].push_back(h);
//         }

//         for (const auto& [trkID, trkHits] : tracks) {
//             // Look for left and right pairs separately
//             Hit front, back;
//             bool foundLeft = false, foundRight = false;

//             // Left
//             for (const auto& h1 : trkHits)
//                 if (isLeftFront(h1.copyNo)) front = h1;
//             for (const auto& h2 : trkHits)
//                 if (isLeftBack(h2.copyNo)) back = h2;
//             if (isLeftFront(front.copyNo) && isLeftBack(back.copyNo)) {
//                 double x1 = front.x, y1 = front.y, z1 = front.z;
//                 double x2 = back.x, y2 = back.y, z2 = back.z;
//                 double dx = x2 - x1;
//                 if (std::abs(dx) > 1e-6) {
//                     double dy = y2 - y1;
//                     double dz = z2 - z1;
//                     double t = (-8.0 - x1) / dx;
//                     y_proj = y1 + t * dy;
//                     z_proj = z1 + t * dz;
//                     eventID = evtID;
//                     tree->Fill();
//                     h2_yz->Fill(y_proj, z_proj);
//                 }
//             }

//             // Right
//             front = back = Hit();
//             for (const auto& h1 : trkHits)
//                 if (isRightFront(h1.copyNo)) front = h1;
//             for (const auto& h2 : trkHits)
//                 if (isRightBack(h2.copyNo)) back = h2;
//             if (isRightFront(front.copyNo) && isRightBack(back.copyNo)) {
//                 double x1 = front.x, y1 = front.y, z1 = front.z;
//                 double x2 = back.x, y2 = back.y, z2 = back.z;
//                 double dx = x2 - x1;
//                 if (std::abs(dx) > 1e-6) {
//                     double dy = y2 - y1;
//                     double dz = z2 - z1;
//                     double t = (-8.0 - x1) / dx;
//                     y_proj = y1 + t * dy;
//                     z_proj = z1 + t * dz;
//                     eventID = evtID;
//                     tree->Fill();
//                     h2_yz->Fill(y_proj, z_proj);
//                 }
//             }
//         }
//     }

//     tree->Write();
//     h2_yz->Write();
//     outFile->Close();

//     std::cout << "ROOT file 'ReconstructedVertices.root' with 2D histogram written successfully.\n";
//     return 0;
// }


