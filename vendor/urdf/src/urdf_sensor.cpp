/*********************************************************************
* Software License Agreement (BSD License)
* 
*  Copyright (c) 2008, Willow Garage, Inc.
*  All rights reserved.
* 
*  Redistribution and use in source and binary forms, with or without
*  modification, are permitted provided that the following conditions
*  are met:
* 
*   * Redistributions of source code must retain the above copyright
*     notice, this list of conditions and the following disclaimer.
*   * Redistributions in binary form must reproduce the above
*     copyright notice, this list of conditions and the following
*     disclaimer in the documentation and/or other materials provided
*     with the distribution.
*   * Neither the name of the Willow Garage nor the names of its
*     contributors may be used to endorse or promote products derived
*     from this software without specific prior written permission.
* 
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
*  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
*  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
*  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
*  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
*  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
*  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
*  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
*  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
*  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
*  POSSIBILITY OF SUCH DAMAGE.
*********************************************************************/

/* Author: John Hsu */


#include <urdf_sensor/sensor.h>
#include <urdf_model/model.h>
#include <fstream>
#include <sstream>
#include <boost/lexical_cast.hpp>
#include <algorithm>
#include <tinyxml.h>

namespace urdf{


void parseCamera(Camera &camera, TiXmlElement* config)
{
  camera.clear();
  camera.type = VisualSensor::CAMERA;

  TiXmlElement *image = config->FirstChildElement("image");
  if (image)
  {
    const char* width_char = image->Attribute("width");
    if (width_char)
    {
      try
      {
        camera.width = boost::lexical_cast<unsigned int>(width_char);
      }
      catch (boost::bad_lexical_cast &e)
      {
        std::ostringstream error_msg;
        error_msg << "Error while parsing camera width attribute value (" << width_char
                  << ") is not a float: " << e.what() << "!";
        throw URDFParseError(error_msg.str());
      }
    }
    else
    {
      std::string error_msg = "Camera sensor needs an image width attribute";
      throw URDFParseError(error_msg);
    }

    const char* height_char = image->Attribute("height");
    if (height_char)
    {
      try
      {
        camera.height = boost::lexical_cast<unsigned int>(height_char);
      }
      catch (boost::bad_lexical_cast &e)
      {
        std::ostringstream error_msg;
        error_msg << "Error while parsing camera height attribute value (" << height_char
                  << ") is not a float: " << e.what() << "!";
        throw URDFParseError(error_msg.str());
      }
    }
    else
    {
      std::string error_msg = "Camera sensor needs an image height attribute";
      throw URDFParseError(error_msg);
    }

    const char* format_char = image->Attribute("format");
    if (format_char)
      camera.format = std::string(format_char);
    else
    {
      std::string error_msg = "Camera sensor needs an image format attribute";
      throw URDFParseError(error_msg);
    }

    const char* hfov_char = image->Attribute("hfov");
    if (hfov_char)
    {
      try
      {
        camera.hfov = boost::lexical_cast<double>(hfov_char);
      }
      catch (boost::bad_lexical_cast &e)
      {
        std::ostringstream error_msg;
        error_msg << "Error while parsing camera hfov attribute value (" << hfov_char
                  << ") is not a float: " << e.what() << "!";
        throw URDFParseError(error_msg.str());
      }
    }
    else
    {
      std::string error_msg = "Camera sensor needs an image hfov attribute";
      throw URDFParseError(error_msg);
    }

    const char* near_char = image->Attribute("near");
    if (near_char)
    {
      try
      {
        camera.near = boost::lexical_cast<double>(near_char);
      }
      catch (boost::bad_lexical_cast &e)
      {
        std::ostringstream error_msg;
        error_msg << "Error while parsing camera near attribute value (" << near_char
                  << ") is not a float: " << e.what() << "!";
        throw URDFParseError(error_msg.str());
      }
    }
    else
    {
      std::string error_msg = "Camera sensor needs an image near attribute";
      throw URDFParseError(error_msg);
    }

    const char* far_char = image->Attribute("far");
    if (far_char)
    {
      try
      {
        camera.far = boost::lexical_cast<double>(far_char);
      }
      catch (boost::bad_lexical_cast &e)
      {
        std::ostringstream error_msg;
        error_msg << "Error while parsing camera far attribute value (" << far_char
                  << ") is not a float: " << e.what() << "!";
        throw URDFParseError(error_msg.str());
      }
    }
    else
    {
      std::string error_msg = "Camera sensor needs an image far attribute";
      throw URDFParseError(error_msg);
    }
    
  }
  else
  {
    std::string error_msg = "Camera sensor has no <image> element!";
    throw URDFParseError(error_msg);
  }
}

void parseRay(Ray &ray, TiXmlElement* config)
{
  ray.clear();
  ray.type = VisualSensor::RAY;

  TiXmlElement *horizontal = config->FirstChildElement("horizontal");
  if (horizontal)
  {
    const char* samples_char = horizontal->Attribute("samples");
    if (samples_char)
    {
      try
      {
        ray.horizontal_samples = boost::lexical_cast<unsigned int>(samples_char);
      }
      catch (boost::bad_lexical_cast &e)
      {
        std::ostringstream error_msg;
        error_msg << "Error while parsing ray horizontal samples attribute value ("
                  << samples_char << ") is not a float: " << e.what() << "!";
        throw URDFParseError(error_msg.str());
      }
    }

    const char* resolution_char = horizontal->Attribute("resolution");
    if (resolution_char)
    {
      try
      {
        ray.horizontal_resolution = boost::lexical_cast<double>(resolution_char);
      }
      catch (boost::bad_lexical_cast &e)
      {
        std::ostringstream error_msg;
        error_msg << "Error while parsing ray horizontal resolution attribute value ("
                  << resolution_char << ") is not a float: " << e.what() << "!";
        throw URDFParseError(error_msg.str());
      }
    }   
    
    const char* min_angle_char = horizontal->Attribute("min_angle");
    if (min_angle_char)
    {
      try
      {
        ray.horizontal_min_angle = boost::lexical_cast<double>(min_angle_char);
      }
      catch (boost::bad_lexical_cast &e)
      {
        std::ostringstream error_msg;
        error_msg << "Error while parsing ray horizontal min_angle attribute value ("
                  << min_angle_char << ") is not a float: " << e.what() << "!";
        throw URDFParseError(error_msg.str());
      }
    }

    const char* max_angle_char = horizontal->Attribute("max_angle");
    if (max_angle_char)
    {
      try
      {
        ray.horizontal_max_angle = boost::lexical_cast<double>(max_angle_char);
      }
      catch (boost::bad_lexical_cast &e)
      {
        std::ostringstream error_msg;
        error_msg << "Error while parsing ray horizontal max_angle attribute value ("
                  << max_angle_char << ") is not a float: " << e.what() << "!";
        throw URDFParseError(error_msg.str());
      }
    }
  }
  
  TiXmlElement *vertical = config->FirstChildElement("vertical");
  if (vertical)
  {
    const char* samples_char = vertical->Attribute("samples");
    if (samples_char)
    {
      try
      {
        ray.vertical_samples = boost::lexical_cast<unsigned int>(samples_char);
      }
      catch (boost::bad_lexical_cast &e)
      {
        std::ostringstream error_msg;
        error_msg << "Error while parsing ray vertical samples attribute value ("
                  << samples_char << ") is not a float: " << e.what() << "!";
        throw URDFParseError(error_msg.str());
      }
    }

    const char* resolution_char = vertical->Attribute("resolution");
    if (resolution_char)
    {
      try
      {
        ray.vertical_resolution = boost::lexical_cast<double>(resolution_char);
      }
      catch (boost::bad_lexical_cast &e)
      {
        std::ostringstream error_msg;
        error_msg << "Error while parsing ray vertical resolution attribute value ("
                  << resolution_char << ") is not a float: " << e.what() << "!";
        throw URDFParseError(error_msg.str());
      }
    }   
    
    const char* min_angle_char = vertical->Attribute("min_angle");
    if (min_angle_char)
    {
      try
      {
        ray.vertical_min_angle = boost::lexical_cast<double>(min_angle_char);
      }
      catch (boost::bad_lexical_cast &e)
      {
        std::ostringstream error_msg;
        error_msg << "Error while parsing ray vertical min_angle attribute value ("
                  << min_angle_char << ") is not a float: " << e.what() << "!";
        throw URDFParseError(error_msg.str());
      }
    }

    const char* max_angle_char = vertical->Attribute("max_angle");
    if (max_angle_char)
    {
      try
      {
        ray.vertical_max_angle = boost::lexical_cast<double>(max_angle_char);
      }
      catch (boost::bad_lexical_cast &e)
      {
        std::ostringstream error_msg;
        error_msg << "Error while parsing ray vertical max_angle attribute value ("
                  << max_angle_char << ") is not a float: " << e.what() << "!";
        throw URDFParseError(error_msg.str());
      }
    }
  }
}

boost::shared_ptr<VisualSensor> parseVisualSensor(TiXmlElement *g)
{
  boost::shared_ptr<VisualSensor> visual_sensor;

  // get sensor type
  TiXmlElement *sensor_xml;
  if (g->FirstChildElement("camera"))
  {
    Camera *camera = new Camera();
    visual_sensor.reset(camera);
    sensor_xml = g->FirstChildElement("camera");
    parseCamera(*camera, sensor_xml);
  }
  else if (g->FirstChildElement("ray"))
  {
    Ray *ray = new Ray();
    visual_sensor.reset(ray);
    sensor_xml = g->FirstChildElement("ray");
    parseRay(*ray, sensor_xml);
  }
  return visual_sensor;
}


void parseSensor(Sensor &sensor, TiXmlElement* config)
{
  sensor.clear();

  const char *name_char = config->Attribute("name");
  if (!name_char)
  {
    std::string error_msg = "No name given for the sensor.";
    throw URDFParseError(error_msg);
  }
  sensor.name = std::string(name_char);

  // parse parent_link_name
  const char *parent_link_name_char = config->Attribute("parent_link_name");
  if (!parent_link_name_char)
  {
    std::string error_msg = "No parent_link_name given for the sensor.";
    throw URDFParseError(error_msg);
  }
  sensor.parent_link_name = std::string(parent_link_name_char);

  // parse origin
  TiXmlElement *o = config->FirstChildElement("origin");
  if (o)
  {
    parsePose(sensor.origin, o);
  }

  // parse sensor
  sensor.sensor = parseVisualSensor(config);
}

}


