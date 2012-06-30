/* ----------------------------------------------------------------------------

 * GTSAM Copyright 2010, Georgia Tech Research Corporation,
 * Atlanta, Georgia 30332-0415
 * All Rights Reserved
 * Authors: Frank Dellaert, et al. (see THANKS for the full author list)

 * See LICENSE for the license information

 * -------------------------------------------------------------------------- */

#pragma once

#include <gtsam/linear/NoiseModel.h>

namespace gtsam { // note, deliberately not in noiseModel namespace

  struct SharedNoiseModel: public noiseModel::Base::shared_ptr {

    typedef noiseModel::Base::shared_ptr Base;

    SharedNoiseModel() {}
    SharedNoiseModel(const noiseModel::Robust::shared_ptr& p): Base(p) {}
    SharedNoiseModel(const noiseModel::Gaussian::shared_ptr& p): Base(p) {}
    SharedNoiseModel(const noiseModel::Diagonal::shared_ptr& p): Base(p) {}
    SharedNoiseModel(const noiseModel::Constrained::shared_ptr& p): Base(p) {}
    SharedNoiseModel(const noiseModel::Isotropic::shared_ptr& p): Base(p) {}
    SharedNoiseModel(const noiseModel::Unit::shared_ptr& p): Base(p) {}

    #ifdef GTSAM_MAGIC_GAUSSIAN
    SharedNoiseModel(const Matrix& covariance) :
      Base(boost::static_pointer_cast<noiseModel::Base>(
             noiseModel::Gaussian::Covariance(covariance))) {}

    SharedNoiseModel(const Vector& sigmas) :
      Base(boost::static_pointer_cast<noiseModel::Base>(
             noiseModel::Diagonal::Sigmas(sigmas))) {}
    #endif

    // Define GTSAM_DANGEROUS_GAUSSIAN to have access to bug-prone fixed dimension Gaussians
    // Not intended for human use, only for backwards compatibility of old unit tests
    #ifdef GTSAM_DANGEROUS_GAUSSIAN
    SharedNoiseModel(const double& s) :
      Base(boost::static_pointer_cast<noiseModel::Base>(
             noiseModel::Isotropic::Sigma(GTSAM_DANGEROUS_GAUSSIAN, s))) {}
    #endif

		/// Print
		inline void print(const std::string &s) const { (*this)->print(s); }

    // Static syntactic sugar functions to create noisemodels directly
    // These should only be used with the Matlab interface
  	static inline SharedNoiseModel Sigmas(const Vector& sigmas, bool smart=false) {
  		return noiseModel::Diagonal::Sigmas(sigmas, smart);
  	}

  	static inline SharedNoiseModel Sigma(size_t dim, double sigma) {
  		return noiseModel::Isotropic::Sigma(dim, sigma);
  	}

  	static inline SharedNoiseModel Precisions(const Vector& precisions) {
  	  return noiseModel::Diagonal::Precisions(precisions);
  	}

  	static inline SharedNoiseModel Precision(size_t dim, double precision) {
  	  return noiseModel::Isotropic::Precision(dim, precision);
  	}

  	static inline SharedNoiseModel Unit(size_t dim) {
  	  return noiseModel::Unit::Create(dim);
  	}

  	static inline SharedNoiseModel SqrtInformation(const Matrix& R) {
  	  return noiseModel::Gaussian::SqrtInformation(R);
  	}

  	static inline SharedNoiseModel Covariance(const Matrix& covariance, bool smart=false) {
  		return noiseModel::Gaussian::Covariance(covariance, smart);
  	}

    /** Serialization function */
    friend class boost::serialization::access;
    template<class ARCHIVE>
    void serialize(ARCHIVE & ar, const unsigned int version) {
      ar & boost::serialization::make_nvp("SharedNoiseModel",
             boost::serialization::base_object<Base>(*this));
    }
  };
}