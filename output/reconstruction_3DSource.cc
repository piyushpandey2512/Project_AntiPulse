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
#include <map>
#include <utility>

// ROOT Headers
#include "TVector3.h"
#include "TFile.h"
#include "TTree.h"
#include "TH3D.h"
#include "TH1D.h"
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
    double    time;
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

std::optional<TVector3> extrapolateToBox(const TVector3& trackPoint, const TVector3& trackDir, const SourceBox& box) {
    double xmin = box.center.X() - box.halfWidthX, xmax = box.center.X() + box.halfWidthX;
    double ymin = box.center.Y() - box.halfWidthY, ymax = box.center.Y() + box.halfWidthY;
    double zmin = box.center.Z() - box.halfWidthZ, zmax = box.center.Z() + box.halfWidthZ;

    double t_near = -std::numeric_limits<double>::infinity();
    double t_far  =  std::numeric_limits<double>::infinity();

    //For X slab
    if (std::abs(trackDir.X()) < 1e-9) { if (trackPoint.X() < xmin || trackPoint.X() > xmax) return std::nullopt; }
    else { 
        double t1 = (xmin - trackPoint.X()) / trackDir.X(); double t2 = (xmax - trackPoint.X()) / trackDir.X(); 
        if (t1 > t2) std::swap(t1, t2); 
        t_near = std::max(t_near, t1); t_far  = std::min(t_far, t2); 
    }

    // For Y slab
    if (std::abs(trackDir.Y()) < 1e-9) { if (trackPoint.Y() < ymin || trackPoint.Y() > ymax) return std::nullopt; }
    else { 
        double t1 = (ymin - trackPoint.Y()) / trackDir.Y(); double t2 = (ymax - trackPoint.Y()) / trackDir.Y(); 
        if (t1 > t2) std::swap(t1, t2); 
        t_near = std::max(t_near, t1); t_far  = std::min(t_far, t2); 
    }

    // For Z slab
    if (std::abs(trackDir.Z()) < 1e-9) { if (trackPoint.Z() < zmin || trackPoint.Z() > zmax) return std::nullopt; }
    else { 
        double t1 = (zmin - trackPoint.Z()) / trackDir.Z(); double t2 = (zmax - trackPoint.Z()) / trackDir.Z(); 
        if (t1 > t2) std::swap(t1, t2); 
        t_near = std::max(t_near, t1); t_far  = std::min(t_far, t2); 
    }
    // The only failure condition is if the ray misses the box entirely.
    if (t_near > t_far) {
        return std::nullopt;
    }
    // We want the vertex, which is the FIRST intersection point (t_near).
    return trackPoint + trackDir * t_near;
}

// --- Toggle switches ---
bool useMoireGratingReco = false;
bool useMoireSourceReco  = true;
bool useTestReco         = false;

// --- Main Program ---
int main() {
    if (useMoireGratingReco) {
        std::ifstream inFile("../build/imp_files/PionInteractions_20250924_094041_1Cr_FM_OW.dat");
        if (!inFile) { std::cerr << "Error: Could not open input file.\n"; return 1; }

        const TVector3 basePosition(0.1*cm, 0.0*cm, 0.55*cm);
        const double gratingHalfX = 7.0 * cm / 2.0;
        const double gratingHalfY = 7.0 * cm / 2.0;
        const double gratingHalfZ = 250.0 * micrometer / 2.0;

        std::vector<SourceBox> sourceBoxes;
        TVector3 center_grating1 = basePosition;
        sourceBoxes.push_back({center_grating1, gratingHalfX, gratingHalfY, gratingHalfZ});
        TVector3 center_grating2 = basePosition + TVector3(0, 0, -45 * cm);
        sourceBoxes.push_back({center_grating2, gratingHalfX, gratingHalfY, gratingHalfZ});

        TFile* outFile = new TFile("ExtrapolatedVertices_FromGratings1.root", "RECREATE");
        TTree* tree = new TTree("ExtrapolatedVertexTree", "Pion Origins Extrapolated to Gratings");
        long long eventID; int trackID; double vx, vy, vz;
        tree->Branch("eventID", &eventID, "eventID/L"); tree->Branch("trackID", &trackID, "trackID/I");
        tree->Branch("vx", &vx, "vx/D"); tree->Branch("vy", &vy, "vy/D"); tree->Branch("vz", &vz, "vz/D");
        TH3D* h3_vertex = new TH3D("h3_extrap_vertex", "Extrapolated Pion Origins;X (cm);Y (cm);Z (cm)",
                                50, -5.0, 5.0, 
                                50, -5.0, 5.0, 
                                101, -50.0, 5.0);

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
        std::cout << "Finding pion tracks and extrapolating to gratings...\n";
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
                if (leftFrontHit && leftBackHit) {
                    track_to_extrapolate = getTrack(*leftFrontHit, *leftBackHit);
                } else if (rightFrontHit && rightBackHit) {
                    track_to_extrapolate = getTrack(*rightFrontHit, *rightBackHit);
                }

                if (track_to_extrapolate) {
                    for (const auto& box : sourceBoxes) {
                        auto found_vertex = extrapolateToBox(track_to_extrapolate->first, track_to_extrapolate->second, box);
                        if (found_vertex) {
                            eventID = current_eventID; trackID = current_trackID;
                            vx = found_vertex->X(); vy = found_vertex->Y(); vz = found_vertex->Z();
                            tree->Fill();
                            h3_vertex->Fill(vx, vy, vz);
                            vertexCount++;
                            break;
                        }
                    }
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

/**************************************************************************************************************/

    if (useMoireSourceReco) {
        std::ifstream inFile("../build/PionInteractions_20251023_122607_onlyPi+.dat");
        if (!inFile) { std::cerr << "Error: Could not open input file.\n"; return 1; }

        const TVector3 stlPosition(-8.0 * cm, 3.5 * cm, 0.0 * cm);
        const double reconBoxHalfX = 5.0 * cm; 
        const double reconBoxHalfY = 5.0 * cm; 
        const double boxHalfZ = 250.0 * micrometer / 2.0;

        SourceBox centerBox = {stlPosition + TVector3(0, 0, 0),    reconBoxHalfX, reconBoxHalfY, boxHalfZ};
        SourceBox rightBox  = {stlPosition + TVector3(0, 0, 45 * cm),  reconBoxHalfX, reconBoxHalfY, boxHalfZ};
        SourceBox leftBox   = {stlPosition + TVector3(0, 0, -45 * cm), reconBoxHalfX, reconBoxHalfY, boxHalfZ};

        TFile* outFile = new TFile("ExtrapolatedVertices_onlyPi+.root", "RECREATE");
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

/**************************************************************************************************************/

    if (useTestReco) {
        std::ifstream inFile("../build/PionInteractions_20251009_130508.dat");
        if (!inFile) { std::cerr << "Error: Could not open input file.\n"; return 1; }
        
        std::vector<double> velocities;
        std::map<long long, int> recoCountPerEvent;

        const TVector3 stlPosition(-8.0 * cm, 3.5 * cm, 0.0 * cm);
        const double reconBoxHalfX = 5.0 * cm; 
        const double reconBoxHalfY = 5.0 * cm; 
        const double boxHalfZ = 250.0 * micrometer / 2.0;

        SourceBox centerBox = {stlPosition + TVector3(0, 0, 0),    reconBoxHalfX, reconBoxHalfY, boxHalfZ};
        SourceBox rightBox  = {stlPosition + TVector3(0, 0, 45 * cm),  reconBoxHalfX, reconBoxHalfY, boxHalfZ};
        SourceBox leftBox   = {stlPosition + TVector3(0, 0, -45 * cm), reconBoxHalfX, reconBoxHalfY, boxHalfZ};

        TFile* outFile = new TFile("ExtrapolatedVertices_velocity.root", "RECREATE");
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
            if (p != "pi+") continue;
            if (io == "in") { hit.position=TVector3(x,y,z); hit.time = t; events[hit.eventID].push_back(hit); }
        }
        inFile.close();
        std::cout << "Finished reading file. " << events.size() << " events found.\n";

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

                double velocity = -1;
                if (leftFrontHit && leftBackHit) {
                    double dt = leftBackHit->time - leftFrontHit->time;
                    double dx = (leftBackHit->position - leftFrontHit->position).Mag();
                    if (dt > 0) velocity = dx / dt; // cm/ns
                } else if (rightFrontHit && rightBackHit) {
                    double dt = rightBackHit->time - rightFrontHit->time;
                    double dx = (rightBackHit->position - rightFrontHit->position).Mag();
                    if (dt > 0) velocity = dx / dt; // cm/ns
                }

                if (velocity > 0) velocities.push_back(velocity);
                
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
                    recoCountPerEvent[current_eventID]++;
                }
            }
        }

        std::ofstream recoFile("RecoCountPerEvent.txt");
        for (const auto& [evt, count] : recoCountPerEvent) {
            recoFile << evt << " " << count << "\n";
        }
        recoFile.close();
        
        std::cout << "Track extrapolation completed.\n";
        std::cout << "Total extrapolated vertices found: " << vertexCount << "\n";
        outFile->cd();
        tree->Write();
        h3_vertex->Write();

        TH1D* h_velocity = new TH1D("h_velocity", "Track Velocity Distribution;Velocity (cm/ns);Counts", 100, 0, 30);
        for (double v : velocities) h_velocity->Fill(v);
        h_velocity->Write();
        outFile->Close();
        delete outFile;

        return 0;   
    }
    return 0;
}