
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
#include <limits>     // Required for the new math function

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

// MODIFICATION 1: Replaced SourcePlane with SourceBox to represent a 3D volume
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


// This function implements the robust Slab Test algorithm for ray-box intersection.
std::optional<TVector3> extrapolateToBox(const TVector3& trackPoint, const TVector3& trackDir, const SourceBox& box) {
    // Define the boundaries of the box
    double xmin = box.center.X() - box.halfWidthX, xmax = box.center.X() + box.halfWidthX;
    double ymin = box.center.Y() - box.halfWidthY, ymax = box.center.Y() + box.halfWidthY;
    double zmin = box.center.Z() - box.halfWidthZ, zmax = box.center.Z() + box.halfWidthZ;

    // Initialize overall entry and exit "times"
    double t_near = -std::numeric_limits<double>::infinity();
    double t_far  =  std::numeric_limits<double>::infinity();
 
    // Perform the Slab Test for each axis
    // X-Slab
    if (std::abs(trackDir.X()) < 1e-9) { if (trackPoint.X() < xmin || trackPoint.X() > xmax) return std::nullopt; } 
    else {
        double t1 = (xmin - trackPoint.X()) / trackDir.X(); double t2 = (xmax - trackPoint.X()) / trackDir.X();
        if (t1 > t2) std::swap(t1, t2);
        t_near = std::max(t_near, t1); t_far  = std::min(t_far, t2);
    }
    // Y-Slab
    if (std::abs(trackDir.Y()) < 1e-9) { if (trackPoint.Y() < ymin || trackPoint.Y() > ymax) return std::nullopt; }
    else {
        double t1 = (ymin - trackPoint.Y()) / trackDir.Y(); double t2 = (ymax - trackPoint.Y()) / trackDir.Y();
        if (t1 > t2) std::swap(t1, t2);
        t_near = std::max(t_near, t1); t_far  = std::min(t_far, t2);
    }
    // Z-Slab
    if (std::abs(trackDir.Z()) < 1e-9) { if (trackPoint.Z() < zmin || trackPoint.Z() > zmax) return std::nullopt; }
    else {
        double t1 = (zmin - trackPoint.Z()) / trackDir.Z(); double t2 = (zmax - trackPoint.Z()) / trackDir.Z();
        if (t1 > t2) std::swap(t1, t2);
        t_near = std::max(t_near, t1); t_far  = std::min(t_far, t2);
    }

    // If the latest entry is after the earliest exit, it's a miss.
    // Also, the intersection must happen at t<0 for a backward extrapolation.
    if (t_near > t_far || t_far < 0) {
        return std::nullopt;
    }

    // Success! The origin point is the last intersection point (t_far).
    return trackPoint + trackDir * t_far;
}


// --- Main Program ---
int main() {
    // --- SETUP ---
    std::ifstream inFile("PionInteractions_20250814_173141_10Cr_Moire_new.dat");
    if (!inFile) {
        std::cerr << "Error: Could not open input file. Please check path.\n";
        return 1;
    }
    
    // MODIFICATION 3: Define the source geometry using SourceBox and physical dimensions
    // const TVector3 stlPosition(-8.0 * cm, 3.5 * cm, 8.0 * cm);  // old position
    const TVector3 stlPosition(-8.0 * cm, 3.5 * cm, 0.0 * cm);  // new corrected position
    const double boxHalfX = 7.0 * cm / 2.0;
    const double boxHalfY = 7.0 * cm / 2.0;
    const double boxHalfZ = 250.0 * micrometer / 2.0;

    std::vector<SourceBox> sourceBoxes;

    /*OLD POSITIONS*/
    // TVector3 center1 = stlPosition + TVector3(0, 0, 0);
    // TVector3 center2 = stlPosition + TVector3(0, 0, 49.9 * cm);
    // TVector3 center3 = stlPosition + TVector3(0, 0, -49.9 * cm);

    /*NEW POSITIONS*/
    TVector3 center1 = stlPosition + TVector3(0, 0, 0);
    TVector3 center2 = stlPosition + TVector3(0, 0, 50 * cm);
    TVector3 center3 = stlPosition + TVector3(0, 0, -50 * cm);
    sourceBoxes.push_back({center1, boxHalfX, boxHalfY, boxHalfZ});
    sourceBoxes.push_back({center2, boxHalfX, boxHalfY, boxHalfZ});
    sourceBoxes.push_back({center3, boxHalfX, boxHalfY, boxHalfZ});
    
    // --- ROOT file setup ---
    TFile* outFile = new TFile("ExtrapolatedVertices_3D_MoireSource_new_position.root", "RECREATE");
    TTree* tree = new TTree("ExtrapolatedVertexTree", "3D Extrapolated Track Origins");
    long long eventID; int trackID; double vx, vy, vz;
    tree->Branch("eventID", &eventID, "eventID/L"); tree->Branch("trackID", &trackID, "trackID/I");
    tree->Branch("vx", &vx, "vx/D"); tree->Branch("vy", &vy, "vy/D"); tree->Branch("vz", &vz, "vz/D");
    TH3D* h3_vertex = new TH3D("h3_extrap_vertex", "Extrapolated Vertex Origins;X (cm);Y (cm);Z (cm)", 50, -12.0, -4.0, 50, 0.0, 7.0, 100, -50.0, 65.0);

    // --- FILE PARSING (Unchanged) ---
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

    // --- RECONSTRUCTION LOGIC ---
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
            std::optional<std::pair<TVector3, TVector3>> track_to_extrapolate;
            if (leftFrontHit && leftBackHit) { track_to_extrapolate = getTrack(*leftFrontHit, *leftBackHit); }
            else if (rightFrontHit && rightBackHit) { track_to_extrapolate = getTrack(*rightFrontHit, *rightBackHit); }

            if (track_to_extrapolate) {
                // MODIFICATION 4: Loop over the 3D boxes and call the new function
                for (const auto& box : sourceBoxes) {
                    auto vertex_opt = extrapolateToBox(track_to_extrapolate->first, track_to_extrapolate->second, box);
                    if (vertex_opt) {
                        TVector3 vertex = *vertex_opt;
                        eventID = current_eventID; trackID = current_trackID;
                        vx = vertex.X(); vy = vertex.Y(); vz = vertex.Z();
                        tree->Fill(); h3_vertex->Fill(vx, vy, vz);
                        vertexCount++;
                        break;
                    }
                }
            }
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

// // MODIFICATION 1: Replaced SourcePlane with SourceBox to represent a 3D volume (kept for compatibility if needed)
// struct SourceBox {
//     TVector3 center;
//     double   halfWidthX;
//     double   halfWidthY;
//     double   halfWidthZ;
// };

// // --- Geometry Helper Functions (Unchanged) ---
// bool isLeftFront(int copyNo)  { return copyNo >= 200 && copyNo <= 212; }
// bool isLeftBack(int copyNo)   { return copyNo >= 300 && copyNo <= 312; }
// bool isRightFront(int copyNo) { return copyNo >= 0 && copyNo <= 12;   }
// bool isRightBack(int copyNo)  { return copyNo >= 100 && copyNo <= 112; }

// // --- Mathematical Functions ---
// std::pair<TVector3, TVector3> getTrack(const Hit& frontHit, const Hit& backHit) {
//     TVector3 p_front = frontHit.position;
//     TVector3 p_back  = backHit.position;
//     TVector3 dir = (p_back - p_front).Unit();
//     return {p_front, dir};
// }

// // Intersect ray p + t*d with a fixed vertical plane x = x_plane.
// // Returns std::nullopt if no intersection (parallel and not on plane or intersection behind start).
// std::optional<TVector3> intersectWithXPlane(const TVector3& p, const TVector3& d, double x_plane, double eps = 1e-9) {
//     // If direction nearly parallel to X axis (d.X() == 0)
//     if (std::abs(d.X()) < eps) {
//         // If already on the plane (within tolerance) consider t = 0 intersection; otherwise no intersection
//         if (std::abs(p.X() - x_plane) < 1e-6) return p; // on-plane at t=0
//         return std::nullopt;
//     }
//     double t = (x_plane - p.X()) / d.X();
//     // Require intersection in front of the track point (t >= 0). If you want backward extrapolation, adjust this.
//     if (t < 0) return std::nullopt;
//     return p + d * t;
// }

// // Compute both front/back intersections (front_x and back_x). Returns pair(frontOpt, backOpt)
// std::pair<std::optional<TVector3>, std::optional<TVector3>>
// intersectFrontBack(const TVector3& p, const TVector3& d, double front_x = 14.55, double back_x = 24.55) {
//     std::optional<TVector3> front = intersectWithXPlane(p, d, front_x);
//     std::optional<TVector3> back  = intersectWithXPlane(p, d, back_x);
//     return {front, back};
// }

// // --- Main Program ---
// int main() {
//     // --- SETUP ---
//     std::ifstream inFile("PionInteractions_20250814_173141_10Cr_Moire_new.dat");
//     if (!inFile) {
//         std::cerr << "Error: Could not open input file. Please check path.\n";
//         return 1;
//     }

//     // MODIFICATION 3: Define the source geometry using SourceBox and physical dimensions (kept for context)
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

//     // --- ROOT file setup ---
//     TFile* outFile = new TFile("ExtrapolatedVertices_3D_MoireSource_new_position.root", "RECREATE");
//     TTree* tree = new TTree("ExtrapolatedVertexTree", "3D Extrapolated Track Origins");
//     long long eventID; int trackID; int planeID; double vx, vy, vz;
//     tree->Branch("eventID", &eventID, "eventID/L");
//     tree->Branch("trackID", &trackID, "trackID/I");
//     // planeID: 0 -> front (x=14.55), 1 -> back (x=24.55)
//     tree->Branch("planeID", &planeID, "planeID/I");
//     tree->Branch("vx", &vx, "vx/D"); tree->Branch("vy", &vy, "vy/D"); tree->Branch("vz", &vz, "vz/D");
//     TH3D* h3_vertex = new TH3D("h3_extrap_vertex",
//                                "Extrapolated Vertex Origins;X (cm);Y (cm);Z (cm)",
//                                50, -12.0, -4.0,
//                                50, 0.0, 7.0,
//                                100, -50.0, 65.0);

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

//     // --- RECONSTRUCTION LOGIC ---
//     int vertexCount = 0;
//     std::cout << "Finding tracks and extrapolating to X planes (14.55 cm front, 24.55 cm back)...\n";

//     // constants for front/back planes
//     const double front_x = 14.55; // cm
//     const double back_x  = 24.55; // cm

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
//                 // Optional logging (comment/uncomment as needed)
//                 // std::cout << "Left Track Hit Zs: " << leftFrontHit->position.Z()
//                 //           << ", " << leftBackHit->position.Z() << std::endl;
//                 track_to_extrapolate = getTrack(*leftFrontHit, *leftBackHit);
//             } else if (rightFrontHit && rightBackHit) {
//                 // std::cout << "Right Track Hit Zs: " << rightFrontHit->position.Z()
//                 //           << ", " << rightBackHit->position.Z() << std::endl;
//                 track_to_extrapolate = getTrack(*rightFrontHit, *rightBackHit);
//             }

//             if (!track_to_extrapolate) continue;

//             const TVector3 trackPoint = track_to_extrapolate->first;
//             const TVector3 trackDir   = track_to_extrapolate->second;

//             // Compute intersections with front/back X planes
//             auto [frontOpt, backOpt] = intersectFrontBack(trackPoint, trackDir, front_x, back_x);

//             // We'll fill entries for whichever intersections exist (front and/or back).
//             // Use planeID = 0 for front_x, planeID = 1 for back_x.
//             if (frontOpt) {
//                 TVector3 vertex = *frontOpt;
//                 // compute squared distance from trackPoint to vertex correctly
//                 double dist_sq = (vertex - trackPoint).Mag2();

//                 eventID = current_eventID; trackID = current_trackID; planeID = 0;
//                 vx = vertex.X(); vy = vertex.Y(); vz = vertex.Z();
//                 tree->Fill(); h3_vertex->Fill(vx, vy, vz);
//                 vertexCount++;
//             }

//             if (backOpt) {
//                 TVector3 vertex = *backOpt;
//                 double dist_sq = (vertex - trackPoint).Mag2();

//                 eventID = current_eventID; trackID = current_trackID; planeID = 1;
//                 vx = vertex.X(); vy = vertex.Y(); vz = vertex.Z();
//                 tree->Fill(); h3_vertex->Fill(vx, vy, vz);
//                 vertexCount++;
//             }

//             // Note: If you only want to record the front intersection (and ignore back),
//             // remove the backOpt block above (or vice versa).
//         }
//     }

//     // --- FINALIZATION ---
//     std::cout << "Track extrapolation completed.\n";
//     std::cout << "Total extrapolated vertex-plane intersections found: " << vertexCount << "\n";
//     outFile->cd();
//     tree->Write();
//     h3_vertex->Write();
//     outFile->Close();
//     delete outFile;

//     return 0;
// }
