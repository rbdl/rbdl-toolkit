/*********************************************************************
* Software Ligcense Agreement (BSD License)
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

#include <sstream>
#include <urdf_model/joint.h>
#include <urdf_model/model.h>
#include <boost/lexical_cast.hpp>
#include <urdf_model/pose.h>


#include <tinyxml/tinyxml.h>
#include <urdf_parser/urdf_parser.h>

namespace urdf{

const char* getJointName(TiXmlElement* xml) {
  // this should always be set since we check for the joint name in parseJoint already
  return ((TiXmlElement*)xml->Parent())->Attribute("name");
}

void parseJointDynamics(JointDynamics &jd, TiXmlElement* config)
{
  jd.clear();

  // Get joint damping
  const char* damping_str = config->Attribute("damping");
  if (damping_str == NULL){
    //urdfdom.joint_dynamics: no damping, defaults to 0
    jd.damping = 0;
  }
  else
  {
    try
    {
      jd.damping = boost::lexical_cast<double>(damping_str);
    }
    catch (boost::bad_lexical_cast &e)
    {
      std::ostringstream error_msg;
      error_msg << "Error while parsing joint '" << getJointName(config)
                << "': dynamics damping value (" << damping_str
                << ") is not a float: " << e.what() << "!";
      throw URDFParseError(error_msg.str());
    }
  }

  // Get joint friction
  const char* friction_str = config->Attribute("friction");
  if (friction_str == NULL){
    //urdfdom.joint_dynamics: no friction, defaults to 0
    jd.friction = 0;
  }
  else
  {
    try
    {
      jd.friction = boost::lexical_cast<double>(friction_str);
    }
    catch (boost::bad_lexical_cast &e)
    {
      std::ostringstream error_msg;
      error_msg << "Error while parsing joint '" << getJointName(config)
                << "': dynamics friction value (" << friction_str
                << ") is not a float: " << e.what() << "!";
      throw URDFParseError(error_msg.str());
    }
  }

  if (damping_str == NULL && friction_str == NULL)
  {
    std::ostringstream error_msg;
    error_msg << "Error while parsing joint '" << getJointName(config)
              << "': joint dynamics element specified with no damping and no friction!";
    throw URDFParseError(error_msg.str());
  }
}

void parseJointLimits(JointLimits &jl, TiXmlElement* config)
{
  jl.clear();

  // Get lower joint limit
  const char* lower_str = config->Attribute("lower");
  if (lower_str == NULL){
    // urdfdom.joint_limit: no lower, defaults to 0;
    jl.lower = 0;
  }
  else
  {
    try
    {
      jl.lower = boost::lexical_cast<double>(lower_str);
    }
    catch (boost::bad_lexical_cast &e)
    {
      std::ostringstream error_msg;
      error_msg << "Error while parsing joint '" << getJointName(config)
                << "': limits lower value (" << lower_str
                << ") is not a float: " << e.what() << "!";
      throw URDFParseError(error_msg.str());
    }
  }

  // Get upper joint limit
  const char* upper_str = config->Attribute("upper");
  if (upper_str == NULL){
    //urdfdom.joint_limit: no upper, , defaults to 0;
    jl.upper = 0;
  }
  else
  {
    try
    {
      jl.upper = boost::lexical_cast<double>(upper_str);
    }
    catch (boost::bad_lexical_cast &e)
    {
      std::ostringstream error_msg;
      error_msg << "Error while parsing joint '" << getJointName(config)
                << "': limits upper value (" << upper_str
                << ") is not a float: " << e.what() << "!";
      throw URDFParseError(error_msg.str());
    }
  }

  // Get joint effort limit
  const char* effort_str = config->Attribute("effort");
  if (effort_str == NULL){
    std::ostringstream error_msg;
    error_msg << "Error while parsing joint '" << getJointName(config)
              << "' joint limit: no effort!";
    throw URDFParseError(error_msg.str());
  }
  else
  {
    try
    {
      jl.effort = boost::lexical_cast<double>(effort_str);
    }
    catch (boost::bad_lexical_cast &e)
    {
      std::ostringstream error_msg;
      error_msg << "Error while parsing joint '" << getJointName(config)
                << "' limits effort value (" << effort_str
                << ") is not a float: " << e.what() << "!";
      throw URDFParseError(error_msg.str());
    }
  }

  // Get joint velocity limit
  const char* velocity_str = config->Attribute("velocity");
  if (velocity_str == NULL){
    std::ostringstream error_msg;
    error_msg << "Error while parsing joint '" << getJointName(config)
              << "' joint limit: no velocity!";
    throw URDFParseError(error_msg.str());
  }
  else
  {
    try
    {
      jl.velocity = boost::lexical_cast<double>(velocity_str);
    }
    catch (boost::bad_lexical_cast &e)
    {
      std::ostringstream error_msg;
      error_msg << "Error while parsing joint '" << getJointName(config)
                << "' limits velocity value (" << velocity_str
                << ") is not a float: " << e.what() << "!";
      throw URDFParseError(error_msg.str());
    }
  }
}

void parseJointSafety(JointSafety &js, TiXmlElement* config)
{
  js.clear();

  // Get soft_lower_limit joint limit
  const char* soft_lower_limit_str = config->Attribute("soft_lower_limit");
  if (soft_lower_limit_str == NULL)
  {
    //urdfdom.joint_safety: no soft_lower_limit, using default value
    js.soft_lower_limit = 0;
  }
  else
  {
    try
    {
      js.soft_lower_limit = boost::lexical_cast<double>(soft_lower_limit_str);
    }
    catch (boost::bad_lexical_cast &e)
    {
      std::ostringstream error_msg;
      error_msg << "Error while parsing joint '" << getJointName(config)
                << "' safety soft_lower_limit value (" << soft_lower_limit_str
                << ") is not a float: " << e.what() << "!";
      throw URDFParseError(error_msg.str());
    }
  }

  // Get soft_upper_limit joint limit
  const char* soft_upper_limit_str = config->Attribute("soft_upper_limit");
  if (soft_upper_limit_str == NULL)
  {
    //urdfdom.joint_safety: no soft_upper_limit, using default value
    js.soft_upper_limit = 0;
  }
  else
  {
    try
    {
      js.soft_upper_limit = boost::lexical_cast<double>(soft_upper_limit_str);
    }
    catch (boost::bad_lexical_cast &e)
    {
      std::ostringstream error_msg;
      error_msg << "Error while parsing joint '" << getJointName(config)
                << "' safety soft_upper_limit value (" << soft_upper_limit_str
                << ") is not a float: " << e.what() << "!";
      throw URDFParseError(error_msg.str());
    }
  }

  // Get k_position_ safety "position" gain - not exactly position gain
  const char* k_position_str = config->Attribute("k_position");
  if (k_position_str == NULL)
  {
    //urdfdom.joint_safety: no k_position, using default value
    js.k_position = 0;
  }
  else
  {
    try
    {
      js.k_position = boost::lexical_cast<double>(k_position_str);
    }
    catch (boost::bad_lexical_cast &e)
    {
      std::ostringstream error_msg;
      error_msg << "Error while parsing joint '" << getJointName(config)
                << "' safety k_position value (" << k_position_str
                << ") is not a float: " << e.what() << "!";
      throw URDFParseError(error_msg.str());
    }
  }
  // Get k_velocity_ safety velocity gain
  const char* k_velocity_str = config->Attribute("k_velocity");
  if (k_velocity_str == NULL)
  {
    std::ostringstream error_msg;
    error_msg << "Error while parsing joint '" << getJointName(config)
              << "' joint safety no k_velocity !";
    throw URDFParseError(error_msg.str());
  }
  else
  {
    try
    {
      js.k_velocity = boost::lexical_cast<double>(k_velocity_str);
    }
    catch (boost::bad_lexical_cast &e)
    {
      std::ostringstream error_msg;
      error_msg << "Error while parsing joint '" << getJointName(config)
                << "' safety k_velocity value (" << k_velocity_str
                << ") is not a float: " << e.what() << "!";
      throw URDFParseError(error_msg.str());
    }
  }
}

void parseJointCalibration(JointCalibration &jc, TiXmlElement* config)
{
  jc.clear();

  // Get rising edge position
  const char* rising_position_str = config->Attribute("rising");
  if (rising_position_str == NULL)
  {
    //urdfdom.joint_calibration: no rising, using default value
    jc.rising.reset();
  }
  else
  {
    try
    {
      jc.rising.reset(new double(boost::lexical_cast<double>(rising_position_str)));
    }
    catch (boost::bad_lexical_cast &e)
    {
      std::ostringstream error_msg;
      error_msg << "Error while parsing joint '" << getJointName(config)
                << "' calibration rising_position value (" << rising_position_str
                << ") is not a float: " << e.what() << "!";
      throw URDFParseError(error_msg.str());
    }
  }

  // Get falling edge position
  const char* falling_position_str = config->Attribute("falling");
  if (falling_position_str == NULL)
  {
    //urdfdom.joint_calibration: no falling, using default value
    jc.falling.reset();
  }
  else
  {
    try
    {
      jc.falling.reset(new double(boost::lexical_cast<double>(falling_position_str)));
    }
    catch (boost::bad_lexical_cast &e)
    {
      std::ostringstream error_msg;
      error_msg << "Error while parsing joint '" << getJointName(config)
                << "' calibration falling_position value (" << falling_position_str
                << ") is not a float: " << e.what() << "!";
      throw URDFParseError(error_msg.str());
    }
  }
}

void parseJointMimic(JointMimic &jm, TiXmlElement* config)
{
  jm.clear();

  // Get name of joint to mimic
  const char* joint_name_str = config->Attribute("joint");

  if (joint_name_str == NULL)
  {
    std::ostringstream error_msg;
    error_msg << "Error while parsing joint '" << getJointName(config)
              << "joint mimic: no mimic joint specified!";
    throw URDFParseError(error_msg.str());
  }
  else
    jm.joint_name = joint_name_str;
  
  // Get mimic multiplier
  const char* multiplier_str = config->Attribute("multiplier");

  if (multiplier_str == NULL)
  {
    //urdfdom.joint_mimic: no multiplier, using default value of 1
    jm.multiplier = 1;    
  }
  else
  {
    try
    {
      jm.multiplier = boost::lexical_cast<double>(multiplier_str);
    }
    catch (boost::bad_lexical_cast &e)
    {
      std::ostringstream error_msg;
      error_msg << "Error while parsing joint '" << getJointName(config)
                << "' mimic multiplier value (" << multiplier_str
                << ") is not a float: " << e.what() << "!";
      throw URDFParseError(error_msg.str());
    }
  }

  
  // Get mimic offset
  const char* offset_str = config->Attribute("offset");
  if (offset_str == NULL)
  {
    //urdfdom.joint_mimic: no offset, using default value of 0
    jm.offset = 0;
  }
  else
  {
    try
    {
      jm.offset = boost::lexical_cast<double>(offset_str);
    }
    catch (boost::bad_lexical_cast &e)
    {
      std::ostringstream error_msg;
      error_msg << "Error while parsing joint '" << getJointName(config)
                << "' mimic offset value (" << offset_str
                << ") is not a float: " << e.what() << "!";
      throw URDFParseError(error_msg.str());
    }
  }
}

void parseJoint(Joint &joint, TiXmlElement* config)
{
  joint.clear();

  // Get Joint Name
  const char *name = config->Attribute("name");
  if (!name)
  {
    std::ostringstream error_msg;
    error_msg << "Error while parsing model: unnamed joint found!";
    throw URDFParseError(error_msg.str());
  }
  joint.name = name;

  // Get transform from Parent Link to Joint Frame
  TiXmlElement *origin_xml = config->FirstChildElement("origin");
  if (!origin_xml)
  {
    //urdfdom: Joint missing origin tag under parent describing transform
    //from Parent Link to Joint Frame, (using Identity transform)
    joint.parent_to_joint_origin_transform.clear();
  }
  else
  {
    try {
      parsePose(joint.parent_to_joint_origin_transform, origin_xml);
    } catch (urdf::URDFParseError& e) {
      std::ostringstream error_msg;
      error_msg << "Error! Malformed parent origin element for joint '" <<joint.name
                << "': " << e.what() << "!";
      throw URDFParseError(error_msg.str());
    }
  }

  // Get Parent Link
  TiXmlElement *parent_xml = config->FirstChildElement("parent");
  if (parent_xml)
  {
    const char *pname = parent_xml->Attribute("link");
    if (!pname)
    {
      //no parent link name specified for Joint link. this might be the root?
    }
    else
    {
      joint.parent_link_name = std::string(pname);
    }
  }

  // Get Child Link
  TiXmlElement *child_xml = config->FirstChildElement("child");
  if (child_xml)
  {
    const char *pname = child_xml->Attribute("link");
    if (!pname)
    {
      //no child link name specified for Joint link
    }
    else
    {
      joint.child_link_name = std::string(pname);
    }
  }

  // Get Joint type
  const char* type_char = config->Attribute("type");
  if (!type_char)
  {
    std::ostringstream error_msg;
    error_msg << "Error! Joint " << joint.name
              <<" has no type, check to see if it's a reference.";
    throw URDFParseError(error_msg.str());
  }
  
  std::string type_str = type_char;
  if (type_str == "planar")
    joint.type = Joint::PLANAR;
  else if (type_str == "floating")
    joint.type = Joint::FLOATING;
  else if (type_str == "revolute")
    joint.type = Joint::REVOLUTE;
  else if (type_str == "continuous")
    joint.type = Joint::CONTINUOUS;
  else if (type_str == "prismatic")
    joint.type = Joint::PRISMATIC;
  else if (type_str == "fixed")
    joint.type = Joint::FIXED;
  else
  {
    std::ostringstream error_msg;
    error_msg << "Error! Joint '" << joint.name
              <<"' has unknown type (" << type_str << ")!";
    throw URDFParseError(error_msg.str());
  }

  // Get Joint Axis
  if (joint.type != Joint::FLOATING && joint.type != Joint::FIXED)
  {
    // axis
    TiXmlElement *axis_xml = config->FirstChildElement("axis");
    if (!axis_xml){
      //urdfdom: no axis elemement for joint
      //defaulting to (1,0,0) axis
      joint.axis = Vector3(1.0, 0.0, 0.0);
    }
    else{
      if (axis_xml->Attribute("xyz")){
        try {
          joint.axis.init(axis_xml->Attribute("xyz"));
        }
        catch (URDFParseError &e) {
          std::ostringstream error_msg;
          error_msg << "Error! Malformed axis element for joint ["<< joint.name
                    <<"]: " << e.what();
          throw URDFParseError(error_msg.str());
        }
      }
    }
  }

  // Get limit
  TiXmlElement *limit_xml = config->FirstChildElement("limit");
  if (limit_xml)
  {
    joint.limits.reset(new JointLimits());
    parseJointLimits(*joint.limits, limit_xml);
  }

  // Get safety
  TiXmlElement *safety_xml = config->FirstChildElement("safety_controller");
  if (safety_xml)
  {
    joint.safety.reset(new JointSafety());
    parseJointSafety(*joint.safety, safety_xml);
  }

  // Get calibration
  TiXmlElement *calibration_xml = config->FirstChildElement("calibration");
  if (calibration_xml)
  {
    joint.calibration.reset(new JointCalibration());
    parseJointCalibration(*joint.calibration, calibration_xml);
  }

  // Get Joint Mimic
  TiXmlElement *mimic_xml = config->FirstChildElement("mimic");
  if (mimic_xml)
  {
    joint.mimic.reset(new JointMimic());
    parseJointMimic(*joint.mimic, mimic_xml);
  }

  // Get Dynamics
  TiXmlElement *prop_xml = config->FirstChildElement("dynamics");
  if (prop_xml)
  {
    joint.dynamics.reset(new JointDynamics());
    parseJointDynamics(*joint.dynamics, prop_xml);
  }
}

// end namespace urdf
}
