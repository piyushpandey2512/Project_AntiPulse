
/*This one is working perfectly*/

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

// --- Geometry Helper Functions (Unchanged) ---
bool isLeftFront(int copyNo)  { return copyNo >= 200 && copyNo <= 212; }
bool isLeftBack(int copyNo)   { return copyNo >= 300 && copyNo <= 312; }
bool isRightFront(int copyNo) { return copyNo >= 0 && copyNo <= 12;   }
bool isRightBack(int copyNo)  { return copyNo >= 100 && copyNo <= 112; }

// --- Mathematical Functions ---
std::pair<TVector3, TVector3> getTrack(const Hit& frontHit, const Hit& backHit) {
    TVector3 p_front = frontHit.position;
    TVector3 p_back  = backHit.position;
    TVector3 dir = (p_back - p_front).Unit();
    return {p_front, dir};
}

std::optional<TVector3> extrapolateToBox(const TVector3& trackPoint, const TVector3& trackDir, const SourceBox& box) {
    double xmin = box.center.X() - box.halfWidthX, xmax = box.center.X() + box.halfWidthX;
    double ymin = box.center.Y() - box.halfWidthY, ymax = box.center.Y() + box.halfWidthY;
    double zmin = box.center.Z() - box.halfWidthZ, zmax = box.center.Z() + box.halfWidthZ;

    double t_near = -std::numeric_limits<double>::infinity();
    double t_far  =  std::numeric_limits<double>::infinity();

    // For X slabs
    if (std::abs(trackDir.X()) < 1e-9) { if (trackPoint.X() < xmin || trackPoint.X() > xmax) return std::nullopt; }
    else {
        double t1 = (xmin - trackPoint.X()) / trackDir.X(); double t2 = (xmax - trackPoint.X()) / trackDir.X();
        if (t1 > t2) std::swap(t1, t2);
        t_near = std::max(t_near, t1); t_far  = std::min(t_far, t2);
    }

    // For Y slabs
    if (std::abs(trackDir.Y()) < 1e-9) { if (trackPoint.Y() < ymin || trackPoint.Y() > ymax) return std::nullopt; }
    else {
        double t1 = (ymin - trackPoint.Y()) / trackDir.Y(); double t2 = (ymax - trackPoint.Y()) / trackDir.Y();
        if (t1 > t2) std::swap(t1, t2);
        t_near = std::max(t_near, t1); t_far  = std::min(t_far, t2);
    }

    // For Z slabs
    if (std::abs(trackDir.Z()) < 1e-9) { if (trackPoint.Z() < zmin || trackPoint.Z() > zmax) return std::nullopt; }
    else {
        double t1 = (zmin - trackPoint.Z()) / trackDir.Z(); double t2 = (zmax - trackPoint.Z()) / trackDir.Z();
        if (t1 > t2) std::swap(t1, t2);
        t_near = std::max(t_near, t1); t_far  = std::min(t_far, t2);
    }

    if (t_near > t_far || t_far < 0) {
        return std::nullopt;
    }

    return trackPoint + trackDir * t_far;
}


// --- Main Program ---
int main() {
    // --- SETUP ---
    // Make sure this path points to your latest 3-source data file
    std::ifstream inFile("../build/PionInteractions_20250825_055254.dat");
    if (!inFile) {
        std::cerr << "Error: Could not open input file. Please check path.\n";
        return 1;
    }
    
    // Define the source geometry using SourceBox and physical dimensions
    const TVector3 stlPosition(-8.0 * cm, 3.5 * cm, 0.0 * cm);
    const double boxHalfX = 7.0 * cm / 2.0;
    const double boxHalfY = 7.0 * cm / 2.0;
    const double boxHalfZ = 250.0 * micrometer / 2.0;

    std::vector<SourceBox> sourceBoxes;
    TVector3 center1 = stlPosition + TVector3(0, 0, 0);
    TVector3 center2 = stlPosition + TVector3(0, 0, 50 * cm);
    TVector3 center3 = stlPosition + TVector3(0, 0, -50 * cm);
    sourceBoxes.push_back({center1, boxHalfX, boxHalfY, boxHalfZ});
    sourceBoxes.push_back({center2, boxHalfX, boxHalfY, boxHalfZ});
    sourceBoxes.push_back({center3, boxHalfX, boxHalfY, boxHalfZ});
    
    // --- ROOT file setup ---
    TFile* outFile = new TFile("ExtrapolatedVertices_Corrected_only_left_source.root", "RECREATE");
    TTree* tree = new TTree("ExtrapolatedVertexTree", "3D Extrapolated Track Origins");
    long long eventID; int trackID; double vx, vy, vz;
    tree->Branch("eventID", &eventID, "eventID/L"); 
    tree->Branch("trackID", &trackID, "trackID/I");
    tree->Branch("vx", &vx, "vx/D"); 
    tree->Branch("vy", &vy, "vy/D"); 
    tree->Branch("vz", &vz, "vz/D");
    TH3D* h3_vertex = new TH3D("h3_extrap_vertex", "Extrapolated Vertex Origins;X (cm);Y (cm);Z (cm)", 50, -12.0, -4.0, 50, -2.0, 8.0, 100, -65.0, 65.0);

    // --- FILE PARSING (Unchanged) ---
    std::unordered_map<long long, std::vector<Hit>> events;  // key: eventID, value: vector of Hits
    std::string line; // to hold each line temporarily
    std::cout << "Reading and parsing data file...\n";
    while (std::getline(inFile, line)) { // read line by line
        if (line.empty()) continue;
        std::istringstream iss(line); Hit hit;  // This is used to use >> operator to easily extract space-separated values 
        std::string p, n, v, io; double x,y,z; long long pid,sid; double t,e,px,py,pz;  // temporary variables for parsing
        if (!(iss>>hit.eventID>>pid>>sid>>hit.trackID>>p>>n>>v>>hit.copyNo>>io>>x>>y>>z>>t>>e>>px>>py>>pz)) continue;  // skip malformed (e.g., it has too few columns, or text where a number should be) lines
        if (io == "in") {  // only consider "in" hits
            hit.position=TVector3(x,y,z); // it takes the x,y,z coordinates and makes a TVector3 out of them
            events[hit.eventID].push_back(hit);  // events[hit.eventID]: this creates a new vector<Hit> if eventID is new, or accesses existing vector if eventID already exists
            // then push_back(hit) adds the current hit to that vector
        }
    }
    inFile.close();
    std::cout << "Finished reading file. " << events.size() << " events found.\n";

    // --- RECONSTRUCTION LOGIC ---
    int vertexCount = 0;
    std::cout << "Finding tracks and extrapolating to source...\n";
    for (const auto& [current_eventID, all_hits_in_event] : events) {  // It iterates over each event in the map created above
        // current_eventID is the eventID (the key), all_hits_in_event is the vector<Hit> for that event
        std::unordered_map<int, std::vector<Hit>> tracks_in_event;  // inside each event, we created new dictionary. This will sort the hits in current event by trackID
        for (const auto& hit : all_hits_in_event) { tracks_in_event[hit.trackID].push_back(hit); }  // This loops over all hits in the current event, grouping them by trackID
         // tracks_in_event[hit.trackID] creates a new vector<Hit> if trackID is new, or accesses existing vector if trackID already exists
         // then push_back(hit) adds the current hit to that vector

        for (const auto& [current_trackID, hits_for_this_track] : tracks_in_event) {  // Now we loop over each track in the current event
            // current_trackID is the trackID (the key), hits_for_this_track is the vector<Hit> for that track
            if (hits_for_this_track.size() < 2) continue;  // skip tracks with fewer than 2 hits
            
            std::optional<Hit> leftFrontHit, leftBackHit, rightFrontHit, rightBackHit;  // optional can hold either a Hit or nothing (std::nullopt). They start empty.
            for (const auto& hit : hits_for_this_track) {  // loop over all hits for this track
                // The following lines check the copyNo of each hit to see if it matches one of the four detector types
                // If it does, it assigns that hit to the corresponding optional variable
                if (isLeftFront(hit.copyNo))  leftFrontHit = hit;
                if (isLeftBack(hit.copyNo))   leftBackHit = hit;
                if (isRightFront(hit.copyNo)) rightFrontHit = hit;
                if (isRightBack(hit.copyNo))  rightBackHit = hit;
            }
            
            std::optional<std::pair<TVector3, TVector3>> track_to_extrapolate;  // This will hold the track (point and direction) to be extrapolated, if we can form one
            if (leftFrontHit && leftBackHit) { // this checks if both leftFrontHit and leftBackHit have been assigned (i.e., they are not nullopt)
                track_to_extrapolate = getTrack(*leftFrontHit, *leftBackHit); // if both exist, we can form a track from them by calling getTrack
            } else if (rightFrontHit && rightBackHit) { // similarly for the right side
                track_to_extrapolate = getTrack(*rightFrontHit, *rightBackHit); 
            }

            if (track_to_extrapolate) {  // this checks if we successfully formed a track to extrapolate
                // If track_to_extrapolate has a value, we proceed with extrapolation
            } else {
                continue; // If we couldn't form a track, skip to the next track
            }
            
            // Loop over the 3D boxes to find a match
            for (const auto& box : sourceBoxes) {  // loop over each source box
                auto vertex_opt = extrapolateToBox(track_to_extrapolate->first, track_to_extrapolate->second, box);   // try to extrapolate the track to this box
                
                // If an intersection is found with this box...
                if (vertex_opt) {
                    // ...store the result immediately.
                    TVector3 vertex = *vertex_opt;
                    eventID = current_eventID; trackID = current_trackID;
                    vx = vertex.X(); vy = vertex.Y(); vz = vertex.Z();
                    tree->Fill();
                    h3_vertex->Fill(vx, vy, vz);
                    vertexCount++;
                    
                    // CRITICAL STEP: Stop searching for other source boxes for this track.
                    break; 
                }
            } // End of loop over sourceBoxes
         }
        
    }
    

    // --- FINALIZATION ---
    std::cout << "Track extrapolation completed.\n";
    std::cout << "Total extrapolated vertices found: " << vertexCount << "\n";
    outFile->cd();
    tree->Write();
    h3_vertex->Write();
    outFile->Close();
    delete outFile;

    return 0;   
}




/****** Testing Code*********/

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

// // --- Geometry Helper Functions (RE-ACTIVATED) ---
// // We bring these back to identify the module type.
// bool isLeftFront(int copyNo)  { return copyNo >= 200 && copyNo <= 212; }
// bool isLeftBack(int copyNo)   { return copyNo >= 300 && copyNo <= 312; }
// bool isRightFront(int copyNo) { return copyNo >= 0 && copyNo <= 12;   }
// bool isRightBack(int copyNo)  { return copyNo >= 100 && copyNo <= 112; }

// // --- Mathematical Functions (Unchanged) ---
// std::pair<TVector3, TVector3> getTrack(const Hit& frontHit, const Hit& backHit) {
//     TVector3 p_front = frontHit.position;
//     TVector3 p_back  = backHit.position;
//     TVector3 dir = (p_back - p_front).Unit();
//     return {p_front, dir};
// }

// std::optional<TVector3> extrapolateToBox(const TVector3& trackPoint, const TVector3& trackDir, const SourceBox& box) {
//     double xmin = box.center.X() - box.halfWidthX, xmax = box.center.X() + box.halfWidthX;
//     double ymin = box.center.Y() - box.halfWidthY, ymax = box.center.Y() + box.halfWidthY;
//     double zmin = box.center.Z() - box.halfWidthZ, zmax = box.center.Z() + box.halfWidthZ;
//     double t_near = -std::numeric_limits<double>::infinity();
//     double t_far  =  std::numeric_limits<double>::infinity();
//     if (std::abs(trackDir.X()) < 1e-9) { if (trackPoint.X() < xmin || trackPoint.X() > xmax) return std::nullopt; } 
//     else {
//         double t1 = (xmin - trackPoint.X()) / trackDir.X(); double t2 = (xmax - trackPoint.X()) / trackDir.X();
//         if (t1 > t2) std::swap(t1, t2);
//         t_near = std::max(t_near, t1); t_far  = std::min(t_far, t2);
//     }
//     if (std::abs(trackDir.Y()) < 1e-9) { if (trackPoint.Y() < ymin || trackPoint.Y() > ymax) return std::nullopt; } 
//     else {
//         double t1 = (ymin - trackPoint.Y()) / trackDir.Y(); double t2 = (ymax - trackPoint.Y()) / trackDir.Y();
//         if (t1 > t2) std::swap(t1, t2);
//         t_near = std::max(t_near, t1); t_far  = std::min(t_far, t2);
//     }
//     if (std::abs(trackDir.Z()) < 1e-9) { if (trackPoint.Z() < zmin || trackPoint.Z() > zmax) return std::nullopt; } 
//     else {
//         double t1 = (zmin - trackPoint.Z()) / trackDir.Z(); double t2 = (zmax - trackPoint.Z()) / trackDir.Z();
//         if (t1 > t2) std::swap(t1, t2);
//         t_near = std::max(t_near, t1); t_far  = std::min(t_far, t2);
//     }
//     if (t_near > t_far || t_far < 0) return std::nullopt;
//     return trackPoint + trackDir * t_far;
// }


// // --- Main Program ---
// int main() {
//     // --- SETUP (Unchanged) ---
//     std::ifstream inFile("FilteredEvent_20250820_185858.dat");
//     if (!inFile) {
//         std::cerr << "Error: Could not open input file. Please check path.\n";
//         return 1;
//     }
//     const TVector3 stlPosition(-8.0 * cm, 3.5 * cm, 0.0 * cm);
//     const double boxHalfX = 7.0 * cm / 2.0;
//     const double boxHalfY = 7.0 * cm / 2.0;
//     const double boxHalfZ = 250.0 * micrometer / 2.0;
//     std::vector<SourceBox> sourceBoxes;
//     TVector3 center1 = stlPosition + TVector3(0, 0, 0);
//     TVector3 center2 = stlPosition + TVector3(0, 0, 50 * cm);
//     TVector3 center3 = stlPosition + TVector3(0, 0, -50 * cm);
//     sourceBoxes.push_back({center1, boxHalfX, boxHalfY, boxHalfZ});
//     sourceBoxes.push_back({center2, boxHalfX, boxHalfY, boxHalfZ});
//     sourceBoxes.push_back({center3, boxHalfX, boxHalfY, boxHalfZ});
//     TFile* outFile = new TFile("ExtrapolatedVertices_Hybrid.root", "RECREATE");
//     TTree* tree = new TTree("ExtrapolatedVertexTree", "3D Extrapolated Track Origins");
//     long long eventID; int trackID; double vx, vy, vz;
//     tree->Branch("eventID", &eventID, "eventID/L"); tree->Branch("trackID", &trackID, "trackID/I");
//     tree->Branch("vx", &vx, "vx/D"); tree->Branch("vy", &vy, "vy/D"); tree->Branch("vz", &vz, "vz/D");
//     TH3D* h3_vertex = new TH3D("h3_extrap_vertex", "Extrapolated Vertex Origins;X (cm);Y (cm);Z (cm)", 50, -12.0, -4.0, 50, -2.0, 8.0, 100, -65.0, 65.0);

//     // --- FILE PARSING (Unchanged) ---
//     std::unordered_map<long long, std::vector<Hit>> events;
//     std::string line;
//     std::cout << "Reading and parsing data file...\n";
//     while (std::getline(inFile, line)) {
//         if (line.empty()) continue;
//         std::istringstream iss(line); Hit hit;
//         std::string p, n, v, io; double x,y,z; long long pid,sid; double t,e,px,py,pz;
//         if (!(iss>>hit.eventID>>pid>>sid>>hit.trackID>>p>>n>>v>>hit.copyNo>>io>>x>>y>>z>>t>>e>>px>>py>>pz)) continue;
//         if (io == "in") { 
//             hit.position=TVector3(x,y,z);
//             events[hit.eventID].push_back(hit);
//         }
//     }
//     inFile.close();
//     std::cout << "Finished reading file. " << events.size() << " events found.\n";

//     // --- RECONSTRUCTION LOGIC ---
//     int vertexCount = 0;
//     std::cout << "Finding tracks and extrapolating to source...\n";
//     for (const auto& [current_eventID, all_hits_in_event] : events) {
//         std::unordered_map<int, std::vector<Hit>> tracks_in_event;
//         for (const auto& hit : all_hits_in_event) { tracks_in_event[hit.trackID].push_back(hit); }

//         for (const auto& [current_trackID, hits_for_this_track] : tracks_in_event) {
//             if (hits_for_this_track.size() < 2) continue;
            
//             // ============================================================================
//             // --- HYBRID TRACK FINDING LOGIC: Using copyNo AND X-Coordinate ---
//             // ============================================================================
            
//             std::optional<Hit> leftFrontHit, leftBackHit, rightFrontHit, rightBackHit;
//             const double frontPlaneX = 19.55 * cm;
//             const double backPlaneX  = 29.55 * cm;
//             const double epsilon = 0.5 * cm; // Increased tolerance to 5mm for robustness

//             for (const auto& hit : hits_for_this_track) {
//                 // To be a valid front hit, BOTH its copyNo AND its X-position must be correct.
//                 if (isLeftFront(hit.copyNo) && std::abs(hit.position.X() - frontPlaneX) < epsilon) {
//                     leftFrontHit = hit;
//                 }
//                 if (isRightFront(hit.copyNo) && std::abs(hit.position.X() - frontPlaneX) < epsilon) {
//                     rightFrontHit = hit;
//                 }

//                 // To be a valid back hit, BOTH its copyNo AND its X-position must be correct.
//                 if (isLeftBack(hit.copyNo) && std::abs(hit.position.X() - backPlaneX) < epsilon) {
//                     leftBackHit = hit;
//                 }
//                 if (isRightBack(hit.copyNo) && std::abs(hit.position.X() - backPlaneX) < epsilon) {
//                     rightBackHit = hit;
//                 }
//             }
            
//             // The logic to form the track is now the same as the original code
//             std::optional<std::pair<TVector3, TVector3>> track_to_extrapolate;
//             if (leftFrontHit && leftBackHit) {
//                 track_to_extrapolate = getTrack(*leftFrontHit, *leftBackHit);
//             } else if (rightFrontHit && rightBackHit) {
//                 track_to_extrapolate = getTrack(*rightFrontHit, *rightBackHit); 
//             }

//             // The extrapolation logic is also the same as the original "Find First" code
//             if (track_to_extrapolate) {
//                 for (const auto& box : sourceBoxes) {
//                     auto vertex_opt = extrapolateToBox(track_to_extrapolate->first, track_to_extrapolate->second, box);
                    
//                     if (vertex_opt) {
//                         TVector3 vertex = *vertex_opt;
//                         eventID = current_eventID; trackID = current_trackID;
//                         vx = vertex.X(); vy = vertex.Y(); vz = vertex.Z();
//                         tree->Fill();
//                         h3_vertex->Fill(vx, vy, vz);
//                         vertexCount++;
//                         break; 
//                     }
//                 }
//             }
//         }
//     }
    
//     // --- FINALIZATION ---
//     std::cout << "Track extrapolation completed.\n";
//     std::cout << "Total extrapolated vertices found: " << vertexCount << "\n";
//     outFile->cd();
//     tree->Write();
//     h3_vertex->Write();
//     outFile->Close();
//     delete outFile;

//     return 0;   
// }
