#include <util/assets.h>
#include <util/file.h>

AssetManager * AssetManager::assman = nullptr;

void AssetManager::ImportModel(ImportFile file){
  Debug::msg("importing model ", file.path.filename());
  Model m = ModelLoader::Load(file.path);
  m.name = file.path.filename();
  assman->models[file.path.filename()] = m;
  Debug::msg("Added model ", file.path.filename());
}
