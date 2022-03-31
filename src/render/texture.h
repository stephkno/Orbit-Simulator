#pragma once
#include <string>

#include <glm/serialize.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>
#include <boost/archive/xml_iarchive.hpp>
#include <stb_image.h>
#include <filesystem>

class Texture {
private:
  // Allow serialization to access non-public data members.
  friend class boost::serialization::access;

  // Serialize the std::vector member of Info
  template<class Archive>
  void serialize(Archive & ar, const unsigned int version)
  {
    ar & unit & type & filename;

    if(Archive::is_loading::value){
      std::cout << "Loading texture: " << filename << std::endl;
      Load(filename);
    }
  }
public:
  Texture(){

  }
  Texture(std::string path){
    Load(path);
  }
  ~Texture(){

  }
  unsigned int id=0;
  unsigned int unit=0;
  std::string type;
  std::string filename;
  int width;
  int height;
  int nrComponents;

  void Load(const std::string file, bool gamma = false)
  {
    std::string cwd = std::filesystem::current_path();
  	filename = file;//cwd + "/assets/textures/surface/" + path + "/" + file;

  	Print << "Loading texture from " << filename.c_str() << Endl;
  	glGenTextures(1, &id);

  	unsigned char * data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
  	if (data)
  	{
  		Print << "Loaded texture from " << filename.c_str() << Endl;
  		GLenum format;
  		if (nrComponents == 1){
        format = GL_RED;
      }
  		else if (nrComponents == 3){
        format = GL_RGB;
      }
  		else if (nrComponents == 4){
        format = GL_RGBA;
      }

  		glBindTexture(GL_TEXTURE_2D, id);
  		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
  		glGenerateMipmap(GL_TEXTURE_2D);

  		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    }

  	else
  	{
  		Print << "Texture failed to load at path: " << filename << Endl;
  		stbi_image_free(data);
  	}

  }
};
