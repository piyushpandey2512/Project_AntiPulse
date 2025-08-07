#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <unordered_map>
#include <sstream>
#include <algorithm>
#include <string>
#include <optional>
#include <limits> // Required for the extrapolateToBox function

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
struct Hit { long long eventID; int trackID; int copyNo; TVector3 position; };

// MODIFICATION 1: Use the SourceBox struct to represent a 3D volume
struct SourceBox { TVector3 center; double halfWidthX, halfWidthY, halfWidthZ; };

// --- Geometry Helper Functions (Now simpler and more robust) ---
bool isFront(int c) { return (c >= 200 && c <= 212) || (c >= 0 && c <= 12); }
bool isBack(int c)  { return (c >= 300 && c <= 312) || (c >= 100 && c <= 112); }

// --- Mathematical Functions ---
std::pair<TVector3, TVector3> getTrack(const Hit& f, const Hit& b) {
    // We now ensure 'f' is the min-X hit and 'b' is the max-X hit
    return {f.position, (b.position - f.position).Unit()};
}

// MODIFICATION 2: Use the robust line-box intersection logic
std::optional<TVector3> extrapolateToBox(const TVector3& p, const TVector3& d, const SourceBox& b) {
    double xmin=b.center.X()-b.halfWidthX, xmax=b.center.X()+b.halfWidthX;
    double ymin=b.center.Y()-b.halfWidthY, ymax=b.center.Y()+b.halfWidthY;
    double zmin=b.center.Z()-b.halfWidthZ, zmax=b.center.Z()+b.halfWidthZ;
    double tn=-std::numeric_limits<double>::infinity(), tf=std::numeric_limits<double>::infinity();
    if(std::abs(d.X())<1e-9){if(p.X()<xmin||p.X()>xmax)return std::nullopt;}else{double t1=(xmin-p.X())/d.X(),t2=(xmax-p.X())/d.X();if(t1>t2)std::swap(t1,t2);tn=std::max(tn,t1);tf=std::min(tf,t2);}
    if(std::abs(d.Y())<1e-9){if(p.Y()<ymin||p.Y()>ymax)return std::nullopt;}else{double t1=(ymin-p.Y())/d.Y(),t2=(ymax-p.Y())/d.Y();if(t1>t2)std::swap(t1,t2);tn=std::max(tn,t1);tf=std::min(tf,t2);}
    if(std::abs(d.Z())<1e-9){if(p.Z()<zmin||p.Z()>zmax)return std::nullopt;}else{double t1=(zmin-p.Z())/d.Z(),t2=(zmax-p.Z())/d.Z();if(t1>t2)std::swap(t1,t2);tn=std::max(tn,t1);tf=std::min(tf,t2);}
    if(tn > tf || tf < 0) return std::nullopt;
    return p + tf * d;
}

// --- Main Program ---
int main() {
    // --- SETUP ---
    std::ifstream inFile("FilteredEvent_20250807_152941_Testing_No_STL.dat");
    if (!inFile) {
        std::cerr << "Error: Could not open input file.\n"; return 1;
    }

    // --- GEOMETRY DEFINITION (Matches the working code's logic) ---
    const TVector3 stlPosition(-8.0 * cm, 3.5 * cm, 8.0 * cm);

    // MODIFICATION 3: Define the source box with its physical X-thickness
    const double boxHalfX = 7.0 * cm / 2.0;
    const double boxHalfY = 7.0 * cm / 2.0;
    const double boxHalfZ = 250.0 * micrometer / 2.0;

    std::vector<SourceBox> sourceBoxes;
    TVector3 center1 = stlPosition + TVector3(0, 0, 0);
    TVector3 center2 = stlPosition + TVector3(0, 0, 49.9 * cm);
    TVector3 center3 = stlPosition + TVector3(0, 0, -49.9 * cm);
    sourceBoxes.push_back({center1, boxHalfX, boxHalfY, boxHalfZ});
    sourceBoxes.push_back({center2, boxHalfX, boxHalfY, boxHalfZ});
    sourceBoxes.push_back({center3, boxHalfX, boxHalfY, boxHalfZ});

    // --- ROOT file setup ---
    TFile* outFile = new TFile("ExtrapolatedVertices_ThickSource.root", "RECREATE");
    TTree* tree = new TTree("VertexTree", "Vertices");
    long long evID; int trkID; double vx,vy,vz;
    tree->Branch("eventID",&evID,"evID/L"); tree->Branch("trackID",&trkID,"trkID/I");
    tree->Branch("vx",&vx,"vx/D"); tree->Branch("vy",&vy,"vy/D"); tree->Branch("vz",&vz,"vz/D");
    TH3D* h3_vertex = new TH3D("h3_vertex", "Vertex Origins (Thick Source);X (cm);Y (cm);Z (cm)", 50, -12.0, -4.0, 50, 0.0, 7.0, 100, -50.0, 65.0);

    // --- PARSING ---
    std::unordered_map<long long, std::vector<Hit>> events;
    std::string line; std::cout << "Reading and parsing data file...\n";
    while (std::getline(inFile, line)) {
        if (line.empty()) continue;
        std::istringstream iss(line); Hit hit;
        std::string p, n, v, io; double x,y,z; long long pid,sid; double t,e,px,py,pz;
        if (!(iss>>hit.eventID>>pid>>sid>>hit.trackID>>p>>n>>v>>hit.copyNo>>io>>x>>y>>z>>t>>e>>px>>py>>pz)) continue;
        if (io == "in") { hit.position=TVector3(x,y,z); events[hit.eventID].push_back(hit); }
    }
    std::cout << "Finished reading file. Found " << events.size() << " unique events.\n";

    // --- RECONSTRUCTION WITH FINAL LOGIC ---
    int vertexCount = 0;
    std::cout << "Finding tracks and extrapolating to source...\n";
    for (const auto& [current_eventID, all_hits_in_event] : events) {
        std::unordered_map<int, std::vector<Hit>> tracks_in_event;
        for (const auto& hit : all_hits_in_event) { tracks_in_event[hit.trackID].push_back(hit); }
        for (const auto& [current_trackID, hits_for_this_track] : tracks_in_event) {
            if (hits_for_this_track.size() < 2) continue;

            // MODIFICATION 4: Use the robust min/max X-hit finding logic
            auto minmax_hits = std::minmax_element(hits_for_this_track.begin(), hits_for_this_track.end(),
                [](const Hit& a, const Hit& b) { return a.position.X() < b.position.X(); });
            const Hit& minX_hit = *minmax_hits.first;
            const Hit& maxX_hit = *minmax_hits.second;

            if (isFront(minX_hit.copyNo) && isBack(maxX_hit.copyNo)) {
                auto track = getTrack(minX_hit, maxX_hit);
                for (const auto& box : sourceBoxes) {
                    if (auto vertex_opt = extrapolateToBox(track.first, track.second, box)) {
                        TVector3 vertex = *vertex_opt;
                        vx=vertex.X(); vy=vertex.Y(); vz=vertex.Z();
                        evID=current_eventID; trkID=current_trackID;
                        tree->Fill(); h3_vertex->Fill(vx,vy,vz);
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
    outFile->cd(); tree->Write(); h3_vertex->Write(); outFile->Close(); delete outFile;

    return 0;
}




/****** Working Code*********

#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <unordered_map>
#include <sstream>
#include <algorithm>
#include <string>
#include <optional>

// ROOT Headers
#include "TVector3.h"
#include "TFile.h"
#include "TTree.h"
#include "TH3D.h"

// --- Unit Definitions (to replace Geant4 units) ---
// We define these as constants so we can write code that looks like the Geant4 source.
static const double cm = 1.0; // We work in cm, so cm = 1.
static const double micrometer = 0.0001 * cm; // 1 micrometer = 0.0001 cm

// --- Data Structures and Constants ---
struct Hit {
    long long eventID;
    int       trackID;
    int       copyNo;
    TVector3  position;
};

// Represents a source plane for extrapolation.
// CORRECTED: Added halfWidthZ.
struct SourcePlane {
    TVector3 center;
    double   halfWidthY; // The check is on the YZ plane
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

// CORRECTED: Now uses target.halfWidthZ correctly.
std::optional<TVector3> extrapolateToTarget(const TVector3& trackPoint, const TVector3& trackDir, const SourcePlane& target) {
    if (std::abs(trackDir.X()) < 1e-9) {
        return std::nullopt;
    }

    double t = (target.center.X() - trackPoint.X()) / trackDir.X();
    TVector3 intersectionPoint = trackPoint + t * trackDir;

    if (std::abs(intersectionPoint.Y() - target.center.Y()) <= target.halfWidthY &&
        std::abs(intersectionPoint.Z() - target.center.Z()) <= target.halfWidthZ) {
        return intersectionPoint;
    }

    return std::nullopt;
}

// --- Main Program ---
int main() {
    // --- SETUP ---
    std::ifstream inFile("FilteredEvent_20250807_152941_Testing_No_STL.dat"); // IMPORTANT: CHANGE THIS FILENAME
    if (!inFile) {
        std::cerr << "Error: Could not open input file. Please check the path and name.\n";
        return 1;
    }
    
    // CORRECTED: Replaced G4 types with TVector3 and our defined units.
    const TVector3 stlPosition(-8.0 * cm, 3.5 * cm, 8.0 * cm);
    const double boxHalfY = 7.0 * cm / 2.0;
    const double boxHalfZ = 250.0 * micrometer / 2.0;

    std::vector<SourcePlane> sourcePlanes;
    // Define the center of the three source planes
    TVector3 center1 = stlPosition + TVector3(0, 0, 0);
    TVector3 center2 = stlPosition + TVector3(0, 0, 49.9 * cm);
    TVector3 center3 = stlPosition + TVector3(0, 0, -49.9 * cm);
    
    // The target for extrapolation is the Y-Z plane of the boxes.
    // The "center" of the target plane has the X-coordinate of the box center.
    sourcePlanes.push_back({center1, boxHalfY, boxHalfZ});
    sourcePlanes.push_back({center2, boxHalfY, boxHalfZ});
    sourcePlanes.push_back({center3, boxHalfY, boxHalfZ});
    
    // Set up the ROOT output file and TTree
    TFile* outFile = new TFile("ExtrapolatedVertices.root", "RECREATE");
    TTree* tree = new TTree("ExtrapolatedVertexTree", "Extrapolated Track Origins");

    long long eventID;
    int trackID;
    double vx, vy, vz;
    tree->Branch("eventID", &eventID, "eventID/L");
    tree->Branch("trackID", &trackID, "trackID/I");
    tree->Branch("vx", &vx, "vx/D");
    tree->Branch("vy", &vy, "vy/D");
    tree->Branch("vz", &vz, "vz/D");

    TH3D* h3_vertex = new TH3D("h3_extrap_vertex", "Extrapolated Vertex Origins;X (cm);Y (cm);Z (cm)",
                               50, -12.0, -4.0, 50, 0.0, 7.0, 100, -50.0, 65.0);

    // --- FILE PARSING ---
    std::unordered_map<long long, std::vector<Hit>> events;
    std::string line;
    std::cout << "Reading and parsing data file...\n";
    while (std::getline(inFile, line)) {
        if (line.empty()) continue;
        std::istringstream iss(line);
        Hit hit;
        std::string dummy, inout;

        iss >> hit.eventID >> dummy >> dummy >> hit.trackID >> dummy >> dummy >> dummy >> hit.copyNo >> inout
            >> hit.position[0] >> hit.position[1] >> hit.position[2];

        if (iss && inout == "in") {
            events[hit.eventID].push_back(hit);
        }
    }
    inFile.close();
    std::cout << "Finished reading file. " << events.size() << " events found.\n";

    // --- RECONSTRUCTION LOGIC ---
    int vertexCount = 0;
    std::cout << "Finding tracks and extrapolating to source...\n";

    for (const auto& [current_eventID, all_hits_in_event] : events) {
        std::unordered_map<int, std::vector<Hit>> tracks_in_event;
        for (const auto& hit : all_hits_in_event) {
            tracks_in_event[hit.trackID].push_back(hit);
        }

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

            if (leftFrontHit && leftBackHit) {
                track_to_extrapolate = getTrack(*leftFrontHit, *leftBackHit);
            } else if (rightFrontHit && rightBackHit) {
                track_to_extrapolate = getTrack(*rightFrontHit, *rightBackHit);
            }

            if (track_to_extrapolate) {
                for (const auto& plane : sourcePlanes) {
                    auto vertex_opt = extrapolateToTarget(track_to_extrapolate->first, track_to_extrapolate->second, plane);
                    if (vertex_opt) {
                        TVector3 vertex = *vertex_opt;
                        eventID = current_eventID;
                        trackID = current_trackID;
                        vx = vertex.X();
                        vy = vertex.Y();
                        vz = vertex.Z();
                        
                        tree->Fill();
                        h3_vertex->Fill(vx, vy, vz);
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



*****// End of working code