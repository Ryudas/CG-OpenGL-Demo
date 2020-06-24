#include <memory>
#include <exception>
#include <stdexcept>
#include "icamera.h"

void ICamera::render(const Scene& scene)
{
   // this -> prerender();
  //  scene.render(*this);
   // this -> postrender();
}

void ICamera::draw(const ICamera& camera,const Scene& scene, const DrawableLight &light) const {}
void ICamera::drawDepth(const ICamera &projection, const Scene &scene) const {}

void ICamera::postrender(){}

ICamera::~ICamera(){}