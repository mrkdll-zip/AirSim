// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "airlib/common/common_utils/StrictMode.hpp"
STRICT_MODE_OFF
#ifndef RPCLIB_MSGPACK
#define RPCLIB_MSGPACK clmdep_msgpack
#endif  // !RPCLIB_MSGPACK
#include "rpc/rpc_error.h"
STRICT_MODE_ON

#include <chrono>
#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "airlib/common/common_utils/FileSystem.hpp"
#include "airlib/vehicles/multirotor/api/MultirotorRpcLibClient.hpp"

bool descend(msr::airlib::MultirotorRpcLibClient& client, float speed, int fps)
{
  float delta_z = speed / fps;

  auto pose         = client.simGetVehiclePose();
  pose.position.z() = pose.position.z() + delta_z;

  client.simSetVehiclePose(pose, false);
  auto collision_state = client.simGetCollisionInfo();
  return collision_state.has_collided;
}

bool takeoff(msr::airlib::MultirotorRpcLibClient& client, float height)
{
  auto pose         = client.simGetVehiclePose();
  pose.position.z() = height;
  pose.position.y() = 0;
  pose.position.x() = 0;
  client.simSetVehiclePose(pose, false);

  auto collision_state = client.simGetCollisionInfo();
  return collision_state.has_collided;
}

int main()
{
  using namespace msr::airlib;

  msr::airlib::MultirotorRpcLibClient client("192.168.86.52");
  typedef ImageCaptureBase::ImageRequest ImageRequest;
  typedef common_utils::FileSystem FileSystem;

  try {
    client.confirmConnection();

    const float duration = 0.2;

    std::cout << "Sim moving vehicle to pose: 0, 0, -30" << std::endl;
    std::this_thread::sleep_for(std::chrono::duration<double>(duration));

    bool collided = takeoff(client, -30);
    while (not collided) {
      collided = descend(client, 2.5, 15);
      std::this_thread::sleep_for(std::chrono::duration<double>(duration));

      // get images back
      vector<ImageRequest> request = {
          ImageRequest("BottomLeft", ImageCaptureBase::ImageType::Scene),
          ImageRequest("BottomRight", ImageCaptureBase::ImageType::Scene)};
      const vector<ImageCaptureBase::ImageResponse>& response =
          client.simGetImages(request);

      if (response.size() > 0) {
        std::cout << "Enter path with ending separator to save images (leave empty for "
                     "no save) "
                  << std::endl;

        cv::Mat left  = cv::Mat(response.at(0).height, response.at(0).width, CV_8UC3);
        left          = cv::imdecode(response.at(0).image_data_uint8, 1);
        cv::Mat right = cv::Mat(response.at(1).height, response.at(1).width, CV_8UC3);
        right         = cv::imdecode(response.at(1).image_data_uint8, 1);

        auto imu_data = client.getImuData();

        cv::imshow("image", left);
        cv::imshow("novel", right);
        cv::waitKey(10);
      }
    }

    std::cout << "Landed" << std::endl;
    client.landAsync()->waitOnLastTask();
    client.armDisarm(false);
  }
  catch (rpc::rpc_error& e) {
    std::string msg = e.get_error().as<std::string>();
    std::cout << "Exception raised by the API, something went wrong." << std::endl
              << msg << std::endl;
  }

  return 0;
}
