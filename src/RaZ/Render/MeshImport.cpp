#include "RaZ/Render/Mesh.hpp"
#include "RaZ/Utils/FilePath.hpp"
#include "RaZ/Utils/StrUtils.hpp"

#include <fstream>

namespace Raz {

void Mesh::import(const FilePath& filePath) {
  // Resetting the mesh to an empty state before importing
  m_submeshes.clear();
  m_submeshes.resize(1);
  m_materials.clear();

  std::ifstream file(filePath, std::ios_base::in | std::ios_base::binary);

  if (file) {
    const std::string format = StrUtils::toLowercaseCopy(filePath.recoverExtension().toUtf8());

    if (format == "obj")
      importObj(file, filePath);
    else if (format == "off")
      importOff(file);
    else if (format == "fbx")
#if defined(FBX_ENABLED)
      importFbx(filePath);
#else
      throw std::runtime_error("Error: FBX disabled; check that you allowed its usage when building RaZ");
#endif
    else
      throw std::runtime_error("Error: '" + format + "' format is not supported");
  } else {
    throw std::runtime_error("Error: Couldn't open the file '" + filePath + "'");
  }
}

void Mesh::save(const FilePath& filePath) const {
  std::ofstream file(filePath, std::ios_base::out | std::ios_base::binary);
  const std::string format = StrUtils::toLowercaseCopy(filePath.recoverExtension().toUtf8());

  if (file) {
    if (format == "obj")
      saveObj(file, filePath);
    else
      throw std::runtime_error("Error: '" + format + "' format is not supported");
  } else {
    throw std::runtime_error("Error: Unable to create a file as '" + filePath + "'; path to file must exist");
  }
}

} // namespace Raz
