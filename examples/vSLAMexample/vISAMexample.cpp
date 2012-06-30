/* ----------------------------------------------------------------------------

 * GTSAM Copyright 2010, Georgia Tech Research Corporation,
 * Atlanta, Georgia 30332-0415
 * All Rights Reserved
 * Authors: Frank Dellaert, et al. (see THANKS for the full author list)

 * See LICENSE for the license information

 * -------------------------------------------------------------------------- */

/**
 * @file    vISAMexample.cpp
 * @brief   An ISAM example for synthesis sequence
 * single camera
 * @author  Duy-Nguyen Ta
 */

#include <boost/shared_ptr.hpp>
#include <boost/foreach.hpp>
using namespace boost;

#include <gtsam/nonlinear/NonlinearFactorGraph.h>
#include <gtsam/nonlinear/NonlinearOptimizer.h>
#include <gtsam/slam/visualSLAM.h>
#include <gtsam/slam/PriorFactor.h>
#include <gtsam/nonlinear/NonlinearISAM.h>

#include "vSLAMutils.h"
#include "Feature2D.h"

using namespace std;
using namespace gtsam;
using namespace visualSLAM;
using namespace boost;

/* ************************************************************************* */
#define CALIB_FILE          "calib.txt"
#define LANDMARKS_FILE      "landmarks.txt"
#define POSES_FILE          "poses.txt"
#define MEASUREMENTS_FILE    "measurements.txt"

// Base data folder
string g_dataFolder;

// Store groundtruth values, read from files
shared_ptrK g_calib;
map<int, Point3> g_landmarks;        // map: <landmark_id, landmark_position>
map<int, Pose3> g_poses;            // map: <camera_id, pose>
std::map<int, std::vector<Feature2D> > g_measurements;    // feature sets detected at each frame

// Noise models
SharedNoiseModel measurementSigma(noiseModel::Isotropic::Sigma(2, 5.0f));
SharedNoiseModel poseSigma(noiseModel::Unit::Create(1));


/* ************************************************************************* */
/**
 * Read all data: calibration file, landmarks, poses, and all features measurements
 * Data is stored in global variables, which are used later to simulate incremental updates.
 */
void readAllDataISAM() {
  g_calib = readCalibData(g_dataFolder + CALIB_FILE);

  // Read groundtruth landmarks' positions. These will be used later as intial estimates and priors for landmark nodes.
  g_landmarks = readLandMarks(g_dataFolder + LANDMARKS_FILE);

  // Read groundtruth camera poses. These will be used later as intial estimates for pose nodes.
  g_poses = readPoses(g_dataFolder, POSES_FILE);

  // Read all 2d measurements. Those will become factors linking their associating pose and the corresponding landmark.
  g_measurements = readAllMeasurementsISAM(g_dataFolder, MEASUREMENTS_FILE);
}

/* ************************************************************************* */
/**
 * Setup newFactors and initialValues for each new pose and set of measurements at each frame.
 */
void createNewFactors(shared_ptr<Graph>& newFactors, boost::shared_ptr<Values>& initialValues,
    const Values& currentEstimate, int pose_id, const Pose3& pose,
    const std::vector<Feature2D>& measurements, SharedNoiseModel measurementSigma, shared_ptrK calib) {

  // Create a graph of newFactors with new measurements
  newFactors = shared_ptr<Graph> (new Graph());
  for (size_t i = 0; i < measurements.size(); i++) {
    newFactors->addMeasurement(
        measurements[i].m_p,
        measurementSigma,
        pose_id,
        measurements[i].m_idLandmark,
        calib);
  }

  // Add prior on the first pose
  if(pose_id == 0)
    newFactors->addPosePrior(pose_id, Pose3(), poseSigma);

  // Create initial values for all new variables and add priors on new landmarks
  initialValues = shared_ptr<Values> (new Values());
  initialValues->insert(PoseKey(pose_id), pose);
  for (size_t i = 0; i < measurements.size(); i++) {
    if(!currentEstimate.exists(PointKey(measurements[i].m_idLandmark))) {
      initialValues->insert(PointKey(measurements[i].m_idLandmark), g_landmarks[measurements[i].m_idLandmark]);
      newFactors->addPointPrior(measurements[i].m_idLandmark, g_landmarks[measurements[i].m_idLandmark]);
    }
  }
}

/* ************************************************************************* */
int main(int argc, char* argv[]) {
  if (argc < 2) {
    cout << "Usage: vISAMexample <DataFolder>" << endl << endl;
    cout << "\tPlease specify <DataFolder>, which contains calibration file, initial\n"
      "\tlandmarks, initial poses, and feature data." << endl;
    cout << "\tSample folder is in $gtsam_source_folder$/examples/Data/" << endl << endl;
    cout << "Example usage: vISAMexample '$gtsam_source_folder$/examples/Data/'" << endl;
    exit(0);
  }

  g_dataFolder = string(argv[1]) + "/";
  readAllDataISAM();

  // Create a NonlinearISAM which will be relinearized and reordered after every "relinearizeInterval" updates
  int relinearizeInterval = 3;
  NonlinearISAM<> isam(relinearizeInterval);

  // At each frame (poseId) with new camera pose and set of associated measurements,
  // create a graph of new factors and update ISAM
  typedef std::map<int, std::vector<Feature2D> > FeatureMap;
  Values currentEstimate = isam.estimate();
  BOOST_FOREACH(const FeatureMap::value_type& features, g_measurements) {
    const int poseId = features.first;
    shared_ptr<Graph> newFactors;
    shared_ptr<Values> initialValues;
    createNewFactors(newFactors, initialValues, currentEstimate, poseId, g_poses[poseId],
            features.second, measurementSigma, g_calib);

    isam.update(*newFactors, *initialValues);
    currentEstimate = isam.estimate();
    cout << "****************************************************" << endl;
    currentEstimate.print("Current estimate: ");
  }

  return 1;
}
/* ************************************************************************* */
