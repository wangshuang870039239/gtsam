/* ----------------------------------------------------------------------------

 * GTSAM Copyright 2010, Georgia Tech Research Corporation, 
 * Atlanta, Georgia 30332-0415
 * All Rights Reserved
 * Authors: Frank Dellaert, et al. (see THANKS for the full author list)

 * See LICENSE for the license information

 * -------------------------------------------------------------------------- */

/**
 * @file SharedGaussian.h
 * @brief Class that wraps a shared Gaussian noise model
 * @author Frank Dellaert
 * @date Jan 22, 2010
 */

#pragma once

#include <gtsam/linear/NoiseModel.h>
#include <gtsam/linear/SharedNoiseModel.h>

namespace gtsam { // note, deliberately not in noiseModel namespace

	/**
	 * A useful convenience class to refer to a shared Gaussian model
	 * Also needed to make noise models in matlab
	 */
	class SharedGaussian: public SharedNoiseModel {
	public:

	  typedef SharedNoiseModel Base;

		SharedGaussian() {}
		SharedGaussian(const noiseModel::Gaussian::shared_ptr& p) : Base(p) {}
		SharedGaussian(const noiseModel::Diagonal::shared_ptr& p) : Base(p) {}
		SharedGaussian(const noiseModel::Constrained::shared_ptr& p) : Base(p) {}
		SharedGaussian(const noiseModel::Isotropic::shared_ptr& p) : Base(p) {}
		SharedGaussian(const noiseModel::Unit::shared_ptr& p) : Base(p) {}

		// Define GTSAM_MAGIC_GAUSSIAN to have access to slightly
		// dangerous and non-shared (inefficient, wasteful) noise models.
		// Intended to be used only in tests (if you must) and the MATLAB wrapper
#ifdef GTSAM_MAGIC_GAUSSIAN
		SharedGaussian(const Matrix& covariance)
		  :	Base(noiseModel::Gaussian::Covariance(covariance)) {}
		SharedGaussian(const Vector& sigmas)
		  : Base(noiseModel::Diagonal::Sigmas(sigmas)) {}
#endif

// Define GTSAM_DANGEROUS_GAUSSIAN to have access to bug-prone fixed dimension Gaussians
// Not intended for human use, only for backwards compatibility of old unit tests
#ifdef GTSAM_DANGEROUS_GAUSSIAN
		SharedGaussian(const double& s) :
			Base(noiseModel::Isotropic::Sigma(GTSAM_DANGEROUS_GAUSSIAN, s)) {}
#endif

		/// Print
		inline void print(const std::string &s) const { (*this)->print(s); }

		/** Serialization function */
		friend class boost::serialization::access;
		template<class ARCHIVE>
		void serialize(ARCHIVE & ar, const unsigned int version) {
			ar & boost::serialization::make_nvp("SharedGaussian",
			     boost::serialization::base_object<noiseModel::Gaussian::shared_ptr >(*this));
		}
	};
}