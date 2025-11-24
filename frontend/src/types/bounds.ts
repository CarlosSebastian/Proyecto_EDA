export interface Bound {
  id: string;
  name: string;
  displayName: string;
  hasParameter?: boolean; // Para Enhanced y EnhancedWebb
  defaultParameter?: number;
}

export const AVAILABLE_BOUNDS: Bound[] = [
  { id: 'keogh', name: 'Keogh', displayName: 'LB_Keogh' },
  { id: 'improved', name: 'Improved', displayName: 'LB_Improved' },
  { id: 'enhanced', name: 'Enhanced', displayName: 'LB_Enhanced', hasParameter: true, defaultParameter: 5 },
  { id: 'petitjean', name: 'Petitjean', displayName: 'LB_Petitjean' },
  { id: 'webb', name: 'Webb', displayName: 'LB_Webb' },
  { id: 'enhancedwebb', name: 'EnhancedWebb', displayName: 'LB_EnhancedWebb', hasParameter: true, defaultParameter: 3 },
];

export const AVAILABLE_DATASETS = [
  '50words', 'Adiac', 'ArrowHead', 'Beef', 'BeetleFly', 'BirdChicken',
  'Car', 'CBF', 'ChlorineConcentration', 'CinC_ECG_torso', 'Coffee',
  'Computers', 'Cricket_X', 'Cricket_Y', 'Cricket_Z', 'DiatomSizeReduction',
  'DistalPhalanxOutlineAgeGroup', 'DistalPhalanxOutlineCorrect', 'DistalPhalanxTW',
  'Earthquakes', 'ECG200', 'ECG5000', 'ECGFiveDays', 'ElectricDevices',
  'FaceAll', 'FaceFour', 'FacesUCR', 'FISH', 'FordA', 'FordB',
  'Gun_Point', 'Ham', 'HandOutlines', 'Haptics', 'Herring', 'InlineSkate',
  'InsectWingbeatSound', 'ItalyPowerDemand', 'LargeKitchenAppliances',
  'Lighting2', 'Lighting7', 'MALLAT', 'Meat', 'MedicalImages',
  'MiddlePhalanxOutlineAgeGroup', 'MiddlePhalanxOutlineCorrect', 'MiddlePhalanxTW',
  'MoteStrain', 'NonInvasiveFatalECG_Thorax1', 'NonInvasiveFatalECG_Thorax2',
  'OliveOil', 'OSULeaf', 'PhalangesOutlinesCorrect', 'Phoneme', 'Plane',
  'ProximalPhalanxOutlineAgeGroup', 'ProximalPhalanxOutlineCorrect', 'ProximalPhalanxTW',
  'RefrigerationDevices', 'ScreenType', 'ShapeletSim', 'ShapesAll',
  'SmallKitchenAppliances', 'SonyAIBORobotSurface', 'SonyAIBORobotSurfaceII',
  'StarLightCurves', 'Strawberry', 'SwedishLeaf', 'Symbols', 'synthetic_control',
  'ToeSegmentation1', 'ToeSegmentation2', 'Trace', 'Two_Patterns', 'TwoLeadECG',
  'uWaveGestureLibrary_X', 'uWaveGestureLibrary_Y', 'uWaveGestureLibrary_Z',
  'UWaveGestureLibraryAll', 'wafer', 'Wine', 'WordsSynonyms', 'Worms',
  'WormsTwoClass', 'yoga'
];

