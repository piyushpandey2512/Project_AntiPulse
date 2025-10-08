
/**************************************** Best Code for Moire gratings ******************************************/

// #include <iostream>
// #include <fstream>
// #include <vector>
// #include <cmath>
// #include <unordered_map>
// #include <sstream>
// #include <algorithm>
// #include <string>
// #include <optional>
// #include <limits>

// // ROOT Headers
// #include "TVector3.h"
// #include "TFile.h"
// #include "TTree.h"
// #include "TH3D.h"
// #include "TMath.h"

// // --- Unit Definitions ---
// static const double cm = 1.0;
// static const double micrometer = 0.0001 * cm;

// // --- Data Structures ---
// struct Hit {
//     long long eventID;
//     int       trackID;
//     int       copyNo;
//     TVector3  position;
// };

// struct SourceBox {
//     TVector3 center;
//     double   halfWidthX;
//     double   halfWidthY;
//     double   halfWidthZ;
// };

// // --- Geometry Helper Functions (Unchanged) ---
// // These are still valid for identifying hits in the scintillator modules.
// bool isLeftFront(int copyNo)  { return copyNo >= 200 && copyNo < 300; }
// bool isLeftBack(int copyNo)   { return copyNo >= 300 && copyNo < 400; }
// bool isRightFront(int copyNo) { return copyNo >= 0   && copyNo < 100; }
// bool isRightBack(int copyNo)  { return copyNo >= 100 && copyNo < 200; }

// // --- Mathematical Functions ---
// std::pair<TVector3, TVector3> getTrack(const Hit& frontHit, const Hit& backHit) {
//     TVector3 p_front = frontHit.position;
//     TVector3 p_back  = backHit.position;
//     TVector3 dir = (p_back - p_front).Unit();
//     return {p_front, dir};
// }

// // --- STANDARD, PHYSICALLY CORRECT EXTRAPOLATION FUNCTION ---
// std::optional<TVector3> extrapolateToBox(const TVector3& trackPoint, const TVector3& trackDir, const SourceBox& box) {
//     double xmin = box.center.X() - box.halfWidthX, xmax = box.center.X() + box.halfWidthX;
//     double ymin = box.center.Y() - box.halfWidthY, ymax = box.center.Y() + box.halfWidthY;
//     double zmin = box.center.Z() - box.halfWidthZ, zmax = box.center.Z() + box.halfWidthZ;
//     double t_near = -std::numeric_limits<double>::infinity(), t_far  =  std::numeric_limits<double>::infinity();
//     if (std::abs(trackDir.X()) < 1e-9) { if (trackPoint.X() < xmin || trackPoint.X() > xmax) return std::nullopt; }
//     else { double t1 = (xmin - trackPoint.X()) / trackDir.X(), t2 = (xmax - trackPoint.X()) / trackDir.X(); if (t1 > t2) std::swap(t1, t2); t_near = std::max(t_near, t1); t_far  = std::min(t_far, t2); }
//     if (std::abs(trackDir.Y()) < 1e-9) { if (trackPoint.Y() < ymin || trackPoint.Y() > ymax) return std::nullopt; }
//     else { double t1 = (ymin - trackPoint.Y()) / trackDir.Y(), t2 = (ymax - trackPoint.Y()) / trackDir.Y(); if (t1 > t2) std::swap(t1, t2); t_near = std::max(t_near, t1); t_far  = std::min(t_far, t2); }
//     if (std::abs(trackDir.Z()) < 1e-9) { if (trackPoint.Z() < zmin || trackPoint.Z() > zmax) return std::nullopt; }
//     else { double t1 = (zmin - trackPoint.Z()) / trackDir.Z(), t2 = (zmax - trackPoint.Z()) / trackDir.Z(); if (t1 > t2) std::swap(t1, t2); t_near = std::max(t_near, t1); t_far  = std::min(t_far, t2); }
    
//     // Failure conditions: geometric miss OR intersection is in front of the detector.
//     if (t_near > t_far || t_far < 0) {
//         return std::nullopt;
//     }
    
//     // Success: Return the vertex at the CLOSEST intersection point (t_far).
//     return trackPoint + trackDir * t_far;
// }

// // --- Main Program ---
// int main() {
//     // Make sure this path points to the data file from your NEW simulation
//     std::ifstream inFile("../build/PionInteractions_20250917_160608.dat");
//     if (!inFile) { std::cerr << "Error: Could not open input file.\n"; return 1; }
    
//     // =======================================================================
//     // --- MODIFICATION: Define Source Boxes to Match the Gratings ---
//     // =======================================================================
//     // These dimensions and positions are taken directly from your DetectorConstruction
//     const TVector3 basePosition(0.1*cm, 0.0*cm, 0.55*cm);
//     const double gratingHalfX = 7.0 * cm / 2.0;
//     const double gratingHalfY = 7.0 * cm / 2.0;
//     const double gratingHalfZ = 250.0 * micrometer / 2.0;

//     // Create a list of the two gratings which are the potential sources of pions
//     std::vector<SourceBox> sourceBoxes;
    
//     // Grating 1 (Copy Number 1)
//     TVector3 center_grating1 = basePosition;
//     sourceBoxes.push_back({center_grating1, gratingHalfX, gratingHalfY, gratingHalfZ});

//     // Grating 2 (Copy Number 2)
//     TVector3 center_grating2 = basePosition + TVector3(0, 0, -45 * cm);
//     sourceBoxes.push_back({center_grating2, gratingHalfX, gratingHalfY, gratingHalfZ});
//     // =======================================================================

    
//     TFile* outFile = new TFile("ExtrapolatedVertices_FromGratings1.root", "RECREATE");
//     TTree* tree = new TTree("ExtrapolatedVertexTree", "Pion Origins Extrapolated to Gratings");
//     long long eventID; int trackID; double vx, vy, vz;
//     tree->Branch("eventID", &eventID, "eventID/L"); tree->Branch("trackID", &trackID, "trackID/I");
//     tree->Branch("vx", &vx, "vx/D"); tree->Branch("vy", &vy, "vy/D"); tree->Branch("vz", &vz, "vz/D");
//     TH3D* h3_vertex = new TH3D("h3_extrap_vertex", "Extrapolated Pion Origins;X (cm);Y (cm);Z (cm)",
//                             50, -5.0, 5.0, 
//                             50, -5.0, 5.0, 
//                             101, -50.0, 5.0); // Zoom in on the Z-axis for the gratings

//     // --- FILE PARSING (Unchanged) ---
//     std::unordered_map<long long, std::vector<Hit>> events;
//     std::string line;
//     std::cout << "Reading and parsing data file...\n";
//     while (std::getline(inFile, line)) {
//         if (line.empty()) continue;
//         std::istringstream iss(line); Hit hit;
//         std::string p, n, v, io; double x,y,z; long long pid,sid; double t,e,px,py,pz;
//         if (!(iss>>hit.eventID>>pid>>sid>>hit.trackID>>p>>n>>v>>hit.copyNo>>io>>x>>y>>z>>t>>e>>px>>py>>pz)) continue;
//         if (io == "in") { hit.position=TVector3(x,y,z); events[hit.eventID].push_back(hit); }
//     }
//     inFile.close();
//     std::cout << "Finished reading file. " << events.size() << " events found.\n";

//     // --- RECONSTRUCTION LOGIC (Unchanged logic, new source boxes) ---
//     int vertexCount = 0;
//     std::cout << "Finding pion tracks and extrapolating to gratings...\n";
//     for (const auto& [current_eventID, all_hits_in_event] : events) {
//         std::unordered_map<int, std::vector<Hit>> tracks_in_event;
//         for (const auto& hit : all_hits_in_event) { tracks_in_event[hit.trackID].push_back(hit); }

//         for (const auto& [current_trackID, hits_for_this_track] : tracks_in_event) {
//             if (hits_for_this_track.size() < 2) continue;
            
//             std::optional<Hit> leftFrontHit, leftBackHit, rightFrontHit, rightBackHit;
//             for (const auto& hit : hits_for_this_track) {
//                 if (isLeftFront(hit.copyNo))  leftFrontHit = hit;
//                 if (isLeftBack(hit.copyNo))   leftBackHit = hit;
//                 if (isRightFront(hit.copyNo)) rightFrontHit = hit;
//                 if (isRightBack(hit.copyNo))  rightBackHit = hit;
//             }
            
//             std::optional<std::pair<TVector3, TVector3>> track_to_extrapolate;
//             if (leftFrontHit && leftBackHit) {
//                 track_to_extrapolate = getTrack(*leftFrontHit, *leftBackHit);
//             } else if (rightFrontHit && rightBackHit) {
//                 track_to_extrapolate = getTrack(*rightFrontHit, *rightBackHit);
//             }

//             if (track_to_extrapolate) {
//                 // Loop over the two gratings to find the first match
//                 for (const auto& box : sourceBoxes) {
//                     auto found_vertex = extrapolateToBox(track_to_extrapolate->first, track_to_extrapolate->second, box);
//                     if (found_vertex) {
//                         eventID = current_eventID; trackID = current_trackID;
//                         vx = found_vertex->X(); vy = found_vertex->Y(); vz = found_vertex->Z();
//                         tree->Fill();
//                         h3_vertex->Fill(vx, vy, vz);
//                         vertexCount++;
//                         break; // "First Match Wins" logic
//                     }
//                 }
//             }
//          }
//     }
    
//     std::cout << "Track extrapolation completed.\n";
//     std::cout << "Total extrapolated vertices found: " << vertexCount << "\n";
//     outFile->cd();
//     tree->Write();
//     h3_vertex->Write();
//     outFile->Close();
//     delete outFile;

//     return 0;   
// }






/**************************************** Best Code for Moire Source ******************************************/

#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <unordered_map>
#include <sstream>
#include <algorithm>
#include <string>
#include <optional>
#include <limits>

// ROOT Headers
#include "TVector3.h"
#include "TFile.h"
#include "TTree.h"
#include "TH3D.h"
#include "TMath.h"

// --- Unit Definitions ---
static const double cm = 1.0;
static const double micrometer = 0.0001 * cm;

// --- Data Structures ---
struct Hit {
    long long eventID;
    int       trackID;
    int       copyNo;
    TVector3  position;
};

struct SourceBox {
    TVector3 center;
    double   halfWidthX;
    double   halfWidthY;
    double   halfWidthZ;
};

// --- Geometry Helper Functions ---
bool isLeftFront(int copyNo)  { return copyNo >= 200 && copyNo < 300; }
bool isLeftBack(int copyNo)   { return copyNo >= 300 && copyNo < 400; }
bool isRightFront(int copyNo) { return copyNo >= 0   && copyNo < 100; }
bool isRightBack(int copyNo)  { return copyNo >= 100 && copyNo < 200; }

// --- Mathematical Functions ---
std::pair<TVector3, TVector3> getTrack(const Hit& frontHit, const Hit& backHit) {
    TVector3 p_front = frontHit.position;
    TVector3 p_back  = backHit.position;
    TVector3 dir = (p_back - p_front).Unit();
    return {p_front, dir};
}

// --- CORRECTED EXTRAPOLATION FUNCTION ---
std::optional<TVector3> extrapolateToBox(const TVector3& trackPoint, const TVector3& trackDir, const SourceBox& box) {
    double xmin = box.center.X() - box.halfWidthX, xmax = box.center.X() + box.halfWidthX;
    double ymin = box.center.Y() - box.halfWidthY, ymax = box.center.Y() + box.halfWidthY;
    double zmin = box.center.Z() - box.halfWidthZ, zmax = box.center.Z() + box.halfWidthZ;
    double t_near = -std::numeric_limits<double>::infinity(), t_far  =  std::numeric_limits<double>::infinity();
    if (std::abs(trackDir.X()) < 1e-9) { if (trackPoint.X() < xmin || trackPoint.X() > xmax) return std::nullopt; }
    else { double t1 = (xmin - trackPoint.X()) / trackDir.X(), t2 = (xmax - trackPoint.X()) / trackDir.X(); if (t1 > t2) std::swap(t1, t2); t_near = std::max(t_near, t1); t_far  = std::min(t_far, t2); }
    if (std::abs(trackDir.Y()) < 1e-9) { if (trackPoint.Y() < ymin || trackPoint.Y() > ymax) return std::nullopt; }
    else { double t1 = (ymin - trackPoint.Y()) / trackDir.Y(), t2 = (ymax - trackPoint.Y()) / trackDir.Y(); if (t1 > t2) std::swap(t1, t2); t_near = std::max(t_near, t1); t_far  = std::min(t_far, t2); }
    if (std::abs(trackDir.Z()) < 1e-9) { if (trackPoint.Z() < zmin || trackPoint.Z() > zmax) return std::nullopt; }
    else { double t1 = (zmin - trackPoint.Z()) / trackDir.Z(), t2 = (zmax - trackPoint.Z()) / trackDir.Z(); if (t1 > t2) std::swap(t1, t2); t_near = std::max(t_near, t1); t_far  = std::min(t_far, t2); }
    
    // The only failure condition is if the ray misses the box entirely.
    if (t_near > t_far) {
        return std::nullopt;
    }
    
    // We want the vertex, which is the FIRST intersection point (t_near).
    return trackPoint + trackDir * t_near;
}

// --- Main Program ---
int main() {
    std::ifstream inFile("../build/PionInteractions_20250924_094041.dat");
    if (!inFile) { std::cerr << "Error: Could not open input file.\n"; return 1; }
    
    const TVector3 stlPosition(-8.0 * cm, 3.5 * cm, 0.0 * cm);
    const double reconBoxHalfX = 5.0 * cm; 
    const double reconBoxHalfY = 5.0 * cm; 
    const double boxHalfZ = 250.0 * micrometer / 2.0;

    SourceBox centerBox = {stlPosition + TVector3(0, 0, 0),    reconBoxHalfX, reconBoxHalfY, boxHalfZ};
    SourceBox rightBox  = {stlPosition + TVector3(0, 0, 45 * cm),  reconBoxHalfX, reconBoxHalfY, boxHalfZ};
    SourceBox leftBox   = {stlPosition + TVector3(0, 0, -45 * cm), reconBoxHalfX, reconBoxHalfY, boxHalfZ};
    
    TFile* outFile = new TFile("ExtrapolatedVertices_Final_CorrectedMathTest.root", "RECREATE");
    TTree* tree = new TTree("ExtrapolatedVertexTree", "3D Extrapolated Track Origins");
    long long eventID; int trackID; double vx, vy, vz;
    tree->Branch("eventID", &eventID, "eventID/L"); tree->Branch("trackID", &trackID, "trackID/I");
    tree->Branch("vx", &vx, "vx/D"); tree->Branch("vy", &vy, "vy/D"); tree->Branch("vz", &vz, "vz/D");
    TH3D* h3_vertex = new TH3D("h3_extrap_vertex", "Extrapolated Vertex Origins;X (cm);Y (cm);Z (cm)",
                            50, -12.0, -4.0, 
                            50, -2.0, 8.0, 
                            101, -65.6, 65.6);
    std::unordered_map<long long, std::vector<Hit>> events;
    std::string line;
    std::cout << "Reading and parsing data file...\n";
    while (std::getline(inFile, line)) {
        if (line.empty()) continue;
        std::istringstream iss(line); Hit hit;
        std::string p, n, v, io; double x,y,z; long long pid,sid; double t,e,px,py,pz;
        if (!(iss>>hit.eventID>>pid>>sid>>hit.trackID>>p>>n>>v>>hit.copyNo>>io>>x>>y>>z>>t>>e>>px>>py>>pz)) continue;
        if (io == "in") { hit.position=TVector3(x,y,z); events[hit.eventID].push_back(hit); }
    }
    inFile.close();
    std::cout << "Finished reading file. " << events.size() << " events found.\n";

    int vertexCount = 0;
    std::cout << "Finding tracks and extrapolating to source...\n";
    for (const auto& [current_eventID, all_hits_in_event] : events) {
        std::unordered_map<int, std::vector<Hit>> tracks_in_event;
        for (const auto& hit : all_hits_in_event) { tracks_in_event[hit.trackID].push_back(hit); }

        for (const auto& [current_trackID, hits_for_this_track] : tracks_in_event) {
            if (hits_for_this_track.size() < 2) continue;
            
            std::optional<Hit> leftFrontHit, leftBackHit, rightFrontHit, rightBackHit;
            for (const auto& hit : hits_for_this_track) {
                if (isLeftFront(hit.copyNo))  leftFrontHit = hit;
                if (isLeftBack(hit.copyNo))   leftBackHit = hit;
                if (isRightFront(hit.copyNo)) rightFrontHit = hit;
                if (isRightBack(hit.copyNo))  rightBackHit = hit;
            }
            
            std::optional<TVector3> found_vertex;

            if (leftFrontHit && leftBackHit) {
                auto track = getTrack(*leftFrontHit, *leftBackHit);
                found_vertex = extrapolateToBox(track.first, track.second, leftBox);
                if (!found_vertex) {
                    found_vertex = extrapolateToBox(track.first, track.second, centerBox);
                }
            } else if (rightFrontHit && rightBackHit) {
                auto track = getTrack(*rightFrontHit, *rightBackHit);
                found_vertex = extrapolateToBox(track.first, track.second, rightBox);
                if (!found_vertex) {
                    found_vertex = extrapolateToBox(track.first, track.second, centerBox);
                }
            }

            if (found_vertex) {
                eventID = current_eventID; trackID = current_trackID;
                vx = found_vertex->X(); vy = found_vertex->Y(); vz = found_vertex->Z();
                tree->Fill();
                h3_vertex->Fill(vx, vy, vz);
                vertexCount++;
            }
         }
    }
    
    std::cout << "Track extrapolation completed.\n";
    std::cout << "Total extrapolated vertices found: " << vertexCount << "\n";
    outFile->cd();
    tree->Write();
    h3_vertex->Write();
    outFile->Close();
    delete outFile;

    return 0;   
}