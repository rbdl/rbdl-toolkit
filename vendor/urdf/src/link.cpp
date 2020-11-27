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


#include <urdf_parser/urdf_parser.h>
#include <urdf_model/link.h>
#include <urdf_model/pose.h>
//#include <fstream>
//#include <sstream>
#include <boost/lexical_cast.hpp>

#include <algorithm>
#include <tinyxml/tinyxml.h>


namespace urdf{


void parseMaterial(Material &material, TiXmlElement *config, bool only_name_is_ok)
{
  bool has_rgb = false;
  bool has_filename = false;

  material.clear();

  if (!config->Attribute("name"))
  {
    std::ostringstream error_msg;
    error_msg << "Error! Material without a name attribute detected!";
    throw URDFParseError(error_msg.str());
  }
  
  material.name = config->Attribute("name");

  // texture
  TiXmlElement *t = config->FirstChildElement("texture");
  if (t)
  {
    if (t->Attribute("filename"))
    {
      material.texture_filename = t->Attribute("filename");
      has_filename = true;
    }
  }

  // color
  TiXmlElement *c = config->FirstChildElement("color");
  if (c)
  {
    if (c->Attribute("rgba")) {

      try {
        material.color.init(c->Attribute("rgba"));
        has_rgb = true;
      }
      catch (URDFParseError &e) {
        std::ostringstream error_msg;
        error_msg << "Material [" << material.name
                  << "] has malformed color rgba values: "
                  << e.what() << "!";
        throw URDFParseError(error_msg.str());
      }
    }
  }

  if (!has_rgb && !has_filename) {
    if (!only_name_is_ok) // no need for an error if only name is ok
    {
      std::ostringstream error_msg;
      error_msg << "Material [" << material.name
                << "] has neither a texture nor a color defined!";
      throw URDFParseError(error_msg.str());
    }
  }
}

const char* getParentLinkName(TiXmlElement *c) {
  TiXmlElement* e = c->Parent()->ToElement();
  while(e->Value() != "link" && e->Parent()!=NULL) {
    e = e->Parent()->ToElement();
  }
  return e->Attribute("name");
}


void parseSphere(Sphere &s, TiXmlElement *c)
{
  s.clear();

  s.type = Geometry::SPHERE;
  if (!c->Attribute("radius"))
  {
    std::ostringstream error_msg;
    error_msg << "Error while parsing link '" << getParentLinkName(c)
              << "': Sphere shape must have a radius attribute";
    throw URDFParseError(error_msg.str());
  }

  try
  {
    s.radius = boost::lexical_cast<double>(c->Attribute("radius"));
  }
  catch (boost::bad_lexical_cast &e)
  {
    std::ostringstream error_msg;
    error_msg << "Error while parsing link '" << getParentLinkName(c)
              << "': sphere radius [" << c->Attribute("radius")
              << "] is not a valid float: " << e.what() << "!";
    throw URDFParseError(error_msg.str());
  }
}

void parseBox(Box &b, TiXmlElement *c)
{
  b.clear();
  
  b.type = Geometry::BOX;
  if (!c->Attribute("size"))
  {
    std::ostringstream error_msg;
    error_msg << "Error while parsing link '" << getParentLinkName(c)
              << "': Sphere shape must have a size attribute";
    throw URDFParseError(error_msg.str());
  }
  try
  {
    b.dim.init(c->Attribute("size"));
  }
  catch (URDFParseError &e)
  {
    std::ostringstream error_msg;
    error_msg << "Error while parsing link '" << getParentLinkName(c)
              << "': box size [" << c->Attribute("size")
              << "] is not a valid: " << e.what() << "!";
    throw URDFParseError(error_msg.str());
  }
}

void parseCylinder(Cylinder &y, TiXmlElement *c)
{
  y.clear();

  y.type = Geometry::CYLINDER;
  if (!c->Attribute("length") ||
      !c->Attribute("radius"))
  {
    std::ostringstream error_msg;
    error_msg << "Error while parsing link '" << getParentLinkName(c)
              << "': Cylinder shape must have both length and radius attributes!";
    throw URDFParseError(error_msg.str());
  }

  try
  {
    y.length = boost::lexical_cast<double>(c->Attribute("length"));
  }
  catch (boost::bad_lexical_cast &e)
  {
    std::ostringstream error_msg;
    error_msg << "Error while parsing link '" << getParentLinkName(c)
              << "': cylinder length [" << c->Attribute("length")
              << "] is not a valid float: " << e.what() << "!";
    throw URDFParseError(error_msg.str());
  }

  try
  {
    y.radius = boost::lexical_cast<double>(c->Attribute("radius"));
  }
  catch (boost::bad_lexical_cast &e)
  {
    std::ostringstream error_msg;
    error_msg << "Error while parsing link '" << getParentLinkName(c)
              << "': cylinder radius [" << c->Attribute("radius")
              << "] is not a valid float: " << e.what() << "!";
    throw URDFParseError(error_msg.str());
  }
}


void parseMesh(Mesh &m, TiXmlElement *c)
{
  m.clear();

  m.type = Geometry::MESH;
  if (!c->Attribute("filename")) {
    std::ostringstream error_msg;
    error_msg << "Error while parsing link '" << getParentLinkName(c)
              << "Mesh must contain a filename attribute!";
    throw URDFParseError(error_msg.str());
  }

  m.filename = c->Attribute("filename");

  if (c->Attribute("scale")) {
    try {
      m.scale.init(c->Attribute("scale"));
    }
    catch (URDFParseError &e) {
      std::ostringstream error_msg;
      error_msg << "Error while parsing link '" << getParentLinkName(c)
                << "': mesh scale [" << c->Attribute("scale")
                << "] is not a valid: " << e.what() << "!";
      throw URDFParseError(error_msg.str());
    }
  }
  else
  {
    // if not defined use unit scaling
    m.scale.x = m.scale.y = m.scale.z = 1;
  }
}

my_shared_ptr<Geometry> parseGeometry(TiXmlElement *g)
{
  my_shared_ptr<Geometry> geom;
  if (!g) return geom;

  TiXmlElement *shape = g->FirstChildElement();
  if (!shape)
  {
    std::ostringstream error_msg;
    error_msg << "Error while parsing link '" << getParentLinkName(g)
              << "' geometry does not contain any shape information!";
    throw URDFParseError(error_msg.str());
  }

  const std::string type_name = shape->ValueTStr().c_str();
  if (type_name == "sphere")
  {
    Sphere *s = new Sphere();
    geom.reset(s);
    parseSphere(*s, shape);
    return geom;
  }
  else if (type_name == "box")
  {
    Box *b = new Box();
    geom.reset(b);
    parseBox(*b, shape);
    return geom;
  }
  else if (type_name == "cylinder")
  {
    Cylinder *c = new Cylinder();
    geom.reset(c);
    parseCylinder(*c, shape);
    return geom;
  }
  else if (type_name == "mesh")
  {
    Mesh *m = new Mesh();
    geom.reset(m);
    parseMesh(*m, shape);
    return geom;
  }
  else
  {
    std::ostringstream error_msg;
    error_msg << "Error while parsing link '" << getParentLinkName(g)
              << "' unknown shape type '" << type_name << "'!";
    throw URDFParseError(error_msg.str());
  }
  
  return my_shared_ptr<Geometry>();
}

void parseInertial(Inertial &i, TiXmlElement *config)
{
  i.clear();

  // Origin
  TiXmlElement *o = config->FirstChildElement("origin");
  if (o)
  {
    parsePose(i.origin, o);
  }

  TiXmlElement *mass_xml = config->FirstChildElement("mass");
  if (!mass_xml)
  {
    std::ostringstream error_msg;
    error_msg << "Error while parsing link '" << getParentLinkName(config)
              << "' inertial element must have a <mass> element!";
    throw URDFParseError(error_msg.str());
  }

  if (!mass_xml->Attribute("value"))
  {
    std::ostringstream error_msg;
    error_msg << "Error while parsing link '" << getParentLinkName(config)
              << "' <mass> element must have a value attribute!";
    throw URDFParseError(error_msg.str());
  }

  try
  {
    i.mass = boost::lexical_cast<double>(mass_xml->Attribute("value"));
  }
  catch (boost::bad_lexical_cast &e)
  {
    std::ostringstream error_msg;
    error_msg << "Error while parsing link '" << getParentLinkName(config)
              << "': inertial mass [" << mass_xml->Attribute("value")
              << "] is not a valid double: " << e.what() << "!";
    throw URDFParseError(error_msg.str());
  }

  TiXmlElement *inertia_xml = config->FirstChildElement("inertia");
  if (!inertia_xml)
  {
    std::ostringstream error_msg;
    error_msg << "Error while parsing link '" << getParentLinkName(config)
              << "' inertial element must have a <inertia> element!";
    throw URDFParseError(error_msg.str());
  }
  if (!(inertia_xml->Attribute("ixx") && inertia_xml->Attribute("ixy") && inertia_xml->Attribute("ixz") &&
        inertia_xml->Attribute("iyy") && inertia_xml->Attribute("iyz") &&
        inertia_xml->Attribute("izz")))
  {
    std::ostringstream error_msg;
    error_msg << "Error while parsing link '" << getParentLinkName(config)
              << "' <inertia> element must have ixx,ixy,ixz,iyy,iyz,izz attributes!";
    throw URDFParseError(error_msg.str());
  }
  try
  {
    i.ixx  = boost::lexical_cast<double>(inertia_xml->Attribute("ixx"));
    i.ixy  = boost::lexical_cast<double>(inertia_xml->Attribute("ixy"));
    i.ixz  = boost::lexical_cast<double>(inertia_xml->Attribute("ixz"));
    i.iyy  = boost::lexical_cast<double>(inertia_xml->Attribute("iyy"));
    i.iyz  = boost::lexical_cast<double>(inertia_xml->Attribute("iyz"));
    i.izz  = boost::lexical_cast<double>(inertia_xml->Attribute("izz"));
  }
  catch (boost::bad_lexical_cast &e)
  {
    std::ostringstream error_msg;
    error_msg << "Error while parsing link '" << getParentLinkName(config)
              << "Inertial: one of the inertia elements is not a valid double:"
              << " ixx [" << inertia_xml->Attribute("ixx") << "]"
              << " ixy [" << inertia_xml->Attribute("ixy") << "]"
              << " ixz [" << inertia_xml->Attribute("ixz") << "]"
              << " iyy [" << inertia_xml->Attribute("iyy") << "]"
              << " iyz [" << inertia_xml->Attribute("iyz") << "]"
              << " izz [" << inertia_xml->Attribute("izz") << "]\n\n"
              << e.what();
    throw URDFParseError(error_msg.str());
  }
}

void parseVisual(Visual &vis, TiXmlElement *config)
{
  vis.clear();

  // Origin
  TiXmlElement *o = config->FirstChildElement("origin");
  if (o) {
    try {
      parsePose(vis.origin, o);
    } catch (URDFParseError& e) {
      std::ostringstream error_msg;
      error_msg << "Error while parsing link '" << getParentLinkName(config)
                << "': visual origin is not a valid: " << e.what() << "!";
      throw URDFParseError(error_msg.str());
    }
  } else {
    // no origin specified assuming (0, 0, 0)
    vis.origin.clear();
  }

  // Geometry
  TiXmlElement *geom = config->FirstChildElement("geometry");
  vis.geometry = parseGeometry(geom);

  const char *name_char = config->Attribute("name");
  if (name_char)
    vis.name = name_char;

  // Material
  TiXmlElement *mat = config->FirstChildElement("material");
  if (mat) {
    // get material name
    if (!mat->Attribute("name")) {
      std::ostringstream error_msg;
      error_msg << "Error while parsing link '" << getParentLinkName(config)
                << "': visual material must contain a name attribute!";
      throw URDFParseError(error_msg.str());
    }
    vis.material_name = mat->Attribute("name");
    
    // try to parse material element in place
    vis.material.reset(new Material());
    parseMaterial(*vis.material, mat, true);
  }
}

void parseCollision(Collision &col, TiXmlElement* config)
{  
  col.clear();

  // Origin
  TiXmlElement *o = config->FirstChildElement("origin");
  if (o) {
    try {
      parsePose(col.origin, o);
    } catch (URDFParseError& e) {
      std::ostringstream error_msg;
      error_msg << "Error while parsing link '" << getParentLinkName(config)
                << "': collision origin is not a valid: " << e.what() << "!";
      throw URDFParseError(error_msg.str());
    }
  }
  
  // Geometry
  TiXmlElement *geom = config->FirstChildElement("geometry");
  col.geometry = parseGeometry(geom);

  const char *name_char = config->Attribute("name");
  if (name_char)
    col.name = name_char;

}

void parseLink(Link &link, TiXmlElement* config)
{
  
  link.clear();

  const char *name_char = config->Attribute("name");
  if (!name_char)
  {
    std::ostringstream error_msg;
    error_msg << "Error! Link without a name attribute detected!";
    throw URDFParseError(error_msg.str());
  }
  link.name = std::string(name_char);

  // Inertial (optional)
  TiXmlElement *i = config->FirstChildElement("inertial");
  if (i)
  {
    link.inertial.reset(new Inertial());
    parseInertial(*link.inertial, i);
  }

  // Multiple Visuals (optional)
  for (TiXmlElement* vis_xml = config->FirstChildElement("visual"); vis_xml; vis_xml = vis_xml->NextSiblingElement("visual"))
  {

    my_shared_ptr<Visual> vis;
    vis.reset(new Visual());
    parseVisual(*vis, vis_xml);
    link.visual_array.push_back(vis);
  }

  // Visual (optional)
  // Assign the first visual to the .visual ptr, if it exists
  if (!link.visual_array.empty())
    link.visual = link.visual_array[0];
  
  // Multiple Collisions (optional)
  for (TiXmlElement* col_xml = config->FirstChildElement("collision"); col_xml; col_xml = col_xml->NextSiblingElement("collision"))
  {
    my_shared_ptr<Collision> col;
    col.reset(new Collision());
    parseCollision(*col, col_xml);
    link.collision_array.push_back(col);
  }
  
  // Collision (optional)  
  // Assign the first collision to the .collision ptr, if it exists
  if (!link.collision_array.empty())
    link.collision = link.collision_array[0];
}

}
