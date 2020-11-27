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

/* Author: Wim Meeussen */
//#include <boost/algorithm/string.hpp>
#include <vector>
#include <string>
#include "urdf_parser/urdf_parser.h"
#include "urdf_model/link.h"

namespace urdf{

my_shared_ptr<ModelInterface>  parseURDF(const std::string &xml_string)
{
  my_shared_ptr<ModelInterface> model(new ModelInterface);
  model->clear();

  TiXmlDocument xml_doc;
  xml_doc.Parse(xml_string.c_str());
  if (xml_doc.Error())
  {
    std::string error_msg = xml_doc.ErrorDesc();
    xml_doc.ClearError();
    throw URDFParseError(error_msg);
  }

  TiXmlElement *robot_xml = xml_doc.FirstChildElement("robot");
  if (!robot_xml)
  {
    std::string error_msg = "Error! Could not find the <robot> element in the xml file";
    throw URDFParseError(error_msg);
  }

  // Get robot name
  const char *name = robot_xml->Attribute("name");
  if (!name)
  {
    std::string error_msg = "No name given for the robot. Please add a name attribute to the robot element!";
    throw URDFParseError(error_msg);
  }
  model->name_ = std::string(name);

  // Get all Material elements
  for (TiXmlElement* material_xml = robot_xml->FirstChildElement("material"); material_xml; material_xml = material_xml->NextSiblingElement("material"))
  {
    my_shared_ptr<Material> material;
    material.reset(new Material);

    parseMaterial(*material, material_xml, false); // material needs to be fully defined here
    if (model->getMaterial(material->name)) {
      std::ostringstream error_msg;
      error_msg << "Duplicate materials '" << material->name << "' found!";
      throw URDFParseError(error_msg.str());
    } else {
        model->materials_.insert(make_pair(material->name,material));
    }
  }

  // Get all Link elements
  for (TiXmlElement* link_xml = robot_xml->FirstChildElement("link"); link_xml; link_xml = link_xml->NextSiblingElement("link")) {
    my_shared_ptr<Link> link;
    link.reset(new Link);
    model->m_numLinks++;

    parseLink(*link, link_xml);
    if (model->getLink(link->name)) {
      std::ostringstream error_msg;
      error_msg << "Error! Duplicate links '" << link->name << "' found!";
      throw URDFParseError(error_msg.str());
    } else {
      // set link visual material
      if (link->visual) {
        if (!link->visual->material_name.empty()) {
          //find the correct material in model
          if (model->getMaterial(link->visual->material_name)) {
              link->visual->material = model->getMaterial( link->visual->material_name.c_str() );
          } else {
            // if no model matrial found use the one defined in the visual
            if (link->visual->material) {
                model->materials_.insert(make_pair(link->visual->material->name,link->visual->material));
            } else {
              // no matrial information available for this visual -> error
              std::ostringstream error_msg;
              error_msg << "Error! Link '" << link->name
                        << "' material '" << link->visual->material_name
                        <<" ' undefined!";
              throw URDFParseError(error_msg.str());
            }
          }
        }
      }
      model->links_.insert(make_pair(link->name,link));
    }
  }

  if (model->links_.empty()){
    std::string error_msg = "Error! No link elements found in the urdf file.";
    throw URDFParseError(error_msg);
  }

  // Get all Joint elements
  for (TiXmlElement* joint_xml = robot_xml->FirstChildElement("joint"); joint_xml; joint_xml = joint_xml->NextSiblingElement("joint")) {
    my_shared_ptr<Joint> joint;
    joint.reset(new Joint);
    model->m_numJoints++;

    parseJoint(*joint, joint_xml);
    if (model->getJoint(joint->name)) {
      std::ostringstream error_msg;
      error_msg << "Error! Duplicate joints '" << joint->name << "' found!";
      throw URDFParseError(error_msg.str());
    } else {
        model->joints_.insert(make_pair(joint->name,joint));
    }
  }

  // every link has children links and joints, but no parents, so we create a
  // local convenience data structure for keeping child->parent relations
  std::map<std::string, std::string> parent_link_tree;
  parent_link_tree.clear();

  // building tree: name mapping
  model->initTree(parent_link_tree);

  // find the root link
  model->initRoot(parent_link_tree);

  return model;
}

}

