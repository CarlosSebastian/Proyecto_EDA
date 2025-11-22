#ifndef UCRINFO_H
#define UCRINFO_H

#include <string>
#include <unordered_map>

class UCRInfo {
public:
    static int getWindow(const std::string& data) {
        if (windowMap.empty()) {
            initUCRInfo();
        }
        
        auto it = windowMap.find(data);
        if (it != windowMap.end()) {
            return it->second;
        }
        return 0;
    }

private:
    static void initUCRInfo() {
        windowMap["50words"] = 16;
        windowMap["Adiac"] = 5;
        windowMap["ArrowHead"] = 0;
        windowMap["Beef"] = 0;
        windowMap["BeetleFly"] = 35;
        windowMap["BirdChicken"] = 30;
        windowMap["Car"] = 5;
        windowMap["CBF"] = 14;
        windowMap["ChlorineConcentration"] = 0;
        windowMap["CinC_ECG_torso"] = 16;
        windowMap["Coffee"] = 0;
        windowMap["Computers"] = 93;
        windowMap["Cricket_X"] = 30;
        windowMap["Cricket_Y"] = 51;
        windowMap["Cricket_Z"] = 15;
        windowMap["DiatomSizeReduction"] = 0;
        windowMap["DistalPhalanxOutlineAgeGroup"] = 0;
        windowMap["DistalPhalanxOutlineCorrect"] = 1;
        windowMap["DistalPhalanxTW"] = 0;
        windowMap["Earthquakes"] = 112;
        windowMap["ECG200"] = 0;
        windowMap["ECG5000"] = 1;
        windowMap["ECGFiveDays"] = 0;
        windowMap["ElectricDevices"] = 13;
        windowMap["FaceAll"] = 3;
        windowMap["FaceFour"] = 7;
        windowMap["FacesUCR"] = 15;
        windowMap["FISH"] = 18;
        windowMap["FordA"] = 0;
        windowMap["FordB"] = 5;
        windowMap["Gun_Point"] = 0;
        windowMap["Ham"] = 0;
        windowMap["HandOutlines"] = 27;
        windowMap["Haptics"] = 21;
        windowMap["Herring"] = 25;
        windowMap["InlineSkate"] = 263;
        windowMap["InsectWingbeatSound"] = 5;
        windowMap["ItalyPowerDemand"] = 0;
        windowMap["LargeKitchenAppliances"] = 676;
        windowMap["Lighting2"] = 38;
        windowMap["Lighting7"] = 15;
        windowMap["MALLAT"] = 0;
        windowMap["Meat"] = 0;
        windowMap["MedicalImages"] = 19;
        windowMap["MiddlePhalanxOutlineAgeGroup"] = 4;
        windowMap["MiddlePhalanxOutlineCorrect"] = 0;
        windowMap["MiddlePhalanxTW"] = 1;
        windowMap["MoteStrain"] = 0;
        windowMap["NonInvasiveFatalECG_Thorax1"] = 7;
        windowMap["NonInvasiveFatalECG_Thorax2"] = 7;
        windowMap["OliveOil"] = 0;
        windowMap["OSULeaf"] = 29;
        windowMap["PhalangesOutlinesCorrect"] = 0;
        windowMap["Phoneme"] = 143;
        windowMap["Plane"] = 8;
        windowMap["ProximalPhalanxOutlineAgeGroup"] = 0;
        windowMap["ProximalPhalanxOutlineCorrect"] = 0;
        windowMap["ProximalPhalanxTW"] = 4;
        windowMap["RefrigerationDevices"] = 57;
        windowMap["ScreenType"] = 122;
        windowMap["ShapeletSim"] = 15;
        windowMap["ShapesAll"] = 20;
        windowMap["SmallKitchenAppliances"] = 108;
        windowMap["SonyAIBORobotSurface"] = 0;
        windowMap["SonyAIBORobotSurfaceII"] = 0;
        windowMap["StarLightCurves"] = 163;
        windowMap["Strawberry"] = 0;
        windowMap["SwedishLeaf"] = 2;
        windowMap["Symbols"] = 31;
        windowMap["synthetic_control"] = 3;
        windowMap["ToeSegmentation1"] = 22;
        windowMap["ToeSegmentation2"] = 17;
        windowMap["Trace"] = 8;
        windowMap["TwoLeadECG"] = 4;
        windowMap["Two_Patterns"] = 5;
        windowMap["UWaveGestureLibraryAll"] = 37;
        windowMap["uWaveGestureLibrary_X"] = 12;
        windowMap["uWaveGestureLibrary_Y"] = 12;
        windowMap["uWaveGestureLibrary_Z"] = 18;
        windowMap["wafer"] = 1;
        windowMap["Wine"] = 0;
        windowMap["WordsSynonyms"] = 21;
        windowMap["Worms"] = 27;
        windowMap["WormsTwoClass"] = 81;
        windowMap["yoga"] = 8;
    }
    
    static std::unordered_map<std::string, int> windowMap;
};

std::unordered_map<std::string, int> UCRInfo::windowMap;

#endif // UCRINFO_H