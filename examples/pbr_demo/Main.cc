/*
 * Copyright (C) 2018 Open Source Robotics Foundation
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

#if defined(__APPLE__)
  #include <OpenGL/gl.h>
  #include <GLUT/glut.h>
#elif not defined(_WIN32)
  #include <GL/glew.h>
  #include <GL/gl.h>
  #include <GL/glut.h>
#endif

#include <iostream>
#include <vector>

#include <ignition/common/Console.hh>
#include <ignition/common/MeshManager.hh>
#include <ignition/rendering.hh>

#include "example_config.hh"
#include "GlutWindow.hh"

using namespace ignition;
using namespace rendering;


const std::string RESOURCE_PATH =
    common::joinPaths(std::string(PROJECT_BINARY_PATH), "media");


//////////////////////////////////////////////////
void buildScene(ScenePtr _scene)
{
  // initialize _scene
  _scene->SetAmbientLight(0.4, 0.4, 0.4);
  _scene->SetBackgroundColor(0.0, 0.0, 0.0);
  VisualPtr root = _scene->RootVisual();

  std::vector<std::string> meshes;
  meshes.push_back("drill");
  meshes.push_back("extinguisher");
  meshes.push_back("rescue_randy");
  meshes.push_back("valve");

  std::map<std::string, VisualPtr> visuals;

  // create PBR material
  double x = 3;
  double y = -1.0 * meshes.size() * 0.5;
  for (auto mesh : meshes)
  {
    MaterialPtr matPBR = _scene->CreateMaterial();
    std::string textureMap = common::joinPaths(RESOURCE_PATH, mesh,
        mesh + "_albedo.png");
    std::string normalMap = common::joinPaths(RESOURCE_PATH, mesh,
        mesh + "_normal.png");
    std::string roughnessMap = common::joinPaths(RESOURCE_PATH, mesh,
        mesh + "_roughness.png");
    std::string metalnessMap = common::joinPaths(RESOURCE_PATH, mesh,
       mesh + "_metallic.png");
    matPBR->SetTexture(textureMap);
    matPBR->SetNormalMap(normalMap);
    matPBR->SetRoughnessMap(roughnessMap);
    matPBR->SetMetalnessMap(metalnessMap);

    // create mesh for PBR
    VisualPtr meshPBR = _scene->CreateVisual(mesh);
    meshPBR->SetLocalPosition(x, y, 0.0);
    meshPBR->SetLocalRotation(0, 0, 0);
//    meshPBR->SetLocalScale(0.5, 0.5, 0.5);
    MeshDescriptor descriptorPBR;
    descriptorPBR.meshName = common::joinPaths(RESOURCE_PATH, mesh,
        mesh + ".dae");
    common::MeshManager *meshManager = common::MeshManager::Instance();
    descriptorPBR.mesh = meshManager->Load(descriptorPBR.meshName);
    MeshPtr meshPBRGeom = _scene->CreateMesh(descriptorPBR);
    meshPBRGeom->SetMaterial(matPBR);
    meshPBR->AddGeometry(meshPBRGeom);
    root->AddChild(meshPBR);
    y += 1.0;

    visuals[mesh] = meshPBR;
  }

  // manually position and scale the meshes

  // create white material
  MaterialPtr white = _scene->CreateMaterial();
  white->SetDiffuse(1.0, 1.0, 1.0);
  white->SetSpecular(1.0, 1.0, 1.0);

  // create plane visual
  VisualPtr plane = _scene->CreateVisual("plane");
  plane->AddGeometry(_scene->CreatePlane());
  plane->SetLocalScale(10, 10, 1);
  plane->SetLocalPosition(3, 0, 0.0);
  plane->SetMaterial(white);
  root->AddChild(plane);

  // create directional light
  DirectionalLightPtr light0 = _scene->CreateDirectionalLight();
  light0->SetDirection(0.5, 0.5, -1);
  light0->SetDiffuseColor(1.0, 1.0, 1.0);
  light0->SetSpecularColor(0.3, 0.3, 0.3);
  light0->SetCastShadows(true);
  root->AddChild(light0);

/*  // create spot light
  SpotLightPtr light1 = _scene->CreateSpotLight();
  light1->SetDiffuseColor(0.8, 0.8, 0.3);
  light1->SetSpecularColor(0.2, 0.2, 0.2);
  light1->SetLocalPosition(0, 3, 3);
  light1->SetDirection(1, -1, -1);
  light1->SetCastShadows(true);
  root->AddChild(light1);

  // create spot light that does not cast shadows
  SpotLightPtr light3 = _scene->CreateSpotLight();
  light3->SetDiffuseColor(0.3, 0.3, 0.3);
  light3->SetSpecularColor(0.2, 0.2, 0.2);
  light3->SetLocalPosition(0, -3, 3);
  light3->SetDirection(1, 1, -1);
  light3->SetCastShadows(false);
  root->AddChild(light3);
*/
  // create point light
  PointLightPtr pointLight = _scene->CreatePointLight();
  pointLight->SetDiffuseColor(0.5, 0.8, 0.8);
  pointLight->SetSpecularColor(0.2, 0.2, 0.2);
  pointLight->SetLocalPosition(0, 0, 2);
  pointLight->SetCastShadows(true);
  root->AddChild(pointLight);

  // create camera
  CameraPtr camera = _scene->CreateCamera("camera");
  camera->SetLocalPosition(0.0, 0.0, 3.0);
  camera->SetLocalRotation(0.0, 0.5, 0.0);
  camera->SetImageWidth(1280);
  camera->SetImageHeight(1024);
  camera->SetAntiAliasing(2);
  camera->SetAspectRatio(1.333);
  camera->SetHFOV(IGN_PI / 2);
  root->AddChild(camera);
}

//////////////////////////////////////////////////
CameraPtr createCamera(const std::string &_engineName)
{
  // create and populate scene
  RenderEngine *engine = rendering::engine(_engineName);
  if (!engine)
  {
    std::cout << "Engine '" << _engineName
              << "' is not supported" << std::endl;
    return CameraPtr();
  }
  ScenePtr scene = engine->CreateScene("scene");
  buildScene(scene);

  // return camera sensor
  SensorPtr sensor = scene->SensorByName("camera");
  // get render pass system
  CameraPtr camera = std::dynamic_pointer_cast<Camera>(sensor);
  RenderPassSystemPtr rpSystem = engine->RenderPassSystem();
  if (rpSystem)
  {
    // add gaussian noise pass
    RenderPassPtr pass = rpSystem->Create<GaussianNoisePass>();
    GaussianNoisePassPtr noisePass =
        std::dynamic_pointer_cast<GaussianNoisePass>(pass);
    noisePass->SetMean(0.1);
    noisePass->SetStdDev(0.08);
    noisePass->SetEnabled(false);
    camera->AddRenderPass(noisePass);
  }

  return camera;
}

//////////////////////////////////////////////////
int main(int _argc, char** _argv)
{
  glutInit(&_argc, _argv);

  common::Console::SetVerbosity(4);
  std::vector<std::string> engineNames;
  std::vector<CameraPtr> cameras;

  engineNames.push_back("ogre2");
  for (auto engineName : engineNames)
  {
    try
    {
      CameraPtr camera = createCamera(engineName);
      if (camera)
      {
        cameras.push_back(camera);
      }
    }
    catch (...)
    {
      // std::cout << ex.what() << std::endl;
      std::cerr << "Error starting up: " << engineName << std::endl;
    }
  }
  run(cameras);
  return 0;
}
