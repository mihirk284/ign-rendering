/*
 * Copyright (C) 2015 Open Source Robotics Foundation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */
#include <iostream>
#include <ignition/common/Console.hh>
#include <gazebo/transport/TransportIface.hh>

#include "ignition/rendering/rendering.hh"
#include "CameraWindow.hh"
#include "SceneManager.hh"

using namespace ignition;
using namespace rendering;

void Connect()
{
  ignition::common::Console::SetVerbosity(4);
  gazebo::transport::init();
  gazebo::transport::run();

  SceneManager* manager = SceneManager::Instance();
  manager->Load();
  manager->Init();
}

ScenePtr CreateScene(const std::string &_engine)
{
  RenderEngine *engine = rendering::get_engine(_engine);
  ScenePtr scene = engine->CreateScene("scene");
  SceneManager::Instance()->AddScene(scene);
  return scene;
}

CameraPtr CreateCamera(const std::string &_engine)
{
  ScenePtr scene = CreateScene(_engine);
  VisualPtr root = scene->RootVisual();

  CameraPtr camera = scene->CreateCamera("camera");
  camera->SetLocalPosition(-1.0, 1.0, 0.0);
  camera->SetLocalRotation(0.0, 0.35, -0.175);
  camera->SetImageWidth(640);
  camera->SetImageHeight(480);
  camera->SetAntiAliasing(2);
  camera->SetAspectRatio(1.333);
  camera->SetHFOV(M_PI / 3);
  root->AddChild(camera);

  return camera;
}

int main(int, char**)
{
  Connect();
  std::vector<CameraPtr> cameras;
  // cameras.push_back(CreateCamera("ogre"));
  cameras.push_back(CreateCamera("optix"));
  GlutRun(cameras);
  return 0;
}
