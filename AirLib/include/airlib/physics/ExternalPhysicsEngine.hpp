// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#ifndef airsim_core_ExternalPhysicsEngine_hpp
#define airsim_core_ExternalPhysicsEngine_hpp

#include <cinttypes>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>

#include "airlib/common/Common.hpp"
#include "airlib/common/CommonStructs.hpp"
#include "airlib/common/SteppableClock.hpp"
#include "airlib/physics/PhysicsEngineBase.hpp"

namespace msr {
namespace airlib {

class ExternalPhysicsEngine : public PhysicsEngineBase {
  public:
  ExternalPhysicsEngine()
  {
  }

  //*** Start: UpdatableState implementation ***//
  virtual void resetImplementation() override
  {
  }

  virtual void update() override
  {
    PhysicsEngineBase::update();

    for (PhysicsBody* body_ptr : *this) {
      body_ptr->updateKinematics();
      body_ptr->update();
    }
  }
  virtual void reportState(StateReporter& reporter) override
  {
    for (PhysicsBody* body_ptr : *this) {
      reporter.writeValue("ExternalPhysicsEngine", true);
      reporter.writeValue("Is Grounded", body_ptr->isGrounded());
    }
    // call base
    UpdatableObject::reportState(reporter);
  }
  //*** End: UpdatableState implementation ***//
};

}  // namespace airlib
}  // namespace msr
#endif
