#pragma once
#include <string>
#include <fstream>
#include <GL/glew.h>
#include <stb_image.h>
#include <iostream>

class File{
public:
	/*static Texture * LoadTexture(std::string path, std::string typeName, bool repeat=true)
	{
		// if texture hasn't been loaded already, load it
		Texture * texture = new Texture();
		//std::string directory = path.substr(0, path.find_last_of('/'));
		texture->id = File::TextureFromFile(path, typeName, repeat);
		if(texture->id==0){
			return nullptr;
		}
		texture->type = typeName.c_str();
		texture->filepath = path;
		texture->filename = typeName;
		return texture;
	}*/

	static unsigned int TextureFromFile(const std::string path, const std::string file, bool gamma = false)
	{
		std::string filename = path + "/" + file;
		Print << "Loading texture from " << filename.c_str() << Endl;

		unsigned int textureID;
		glGenTextures(1, &textureID);

		int width, height, nrComponents;
		unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
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

			glBindTexture(GL_TEXTURE_2D, textureID);
			glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			stbi_image_free(data);
		}
		else
		{
			Print << "Texture failed to load at path: " << filename << Endl;
			stbi_image_free(data);
		}

		return textureID;
	}

	// Reads a text file and outputs a std::string with everything in the text file
	static std::string LoadTextFile(const char* filename)
	{

		std::ifstream in(filename, std::ios::binary);
		if (in)
		{

			std::string contents;

			in.seekg(0, std::ios::end);
			contents.resize(in.tellg());

			in.seekg(0, std::ios::beg);
			in.read(&contents[0], contents.size());
			in.close();

			return(contents);

		}

		throw(errno);

	}

};
