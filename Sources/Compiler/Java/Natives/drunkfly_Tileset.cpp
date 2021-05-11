#include "Compiler/Java/JavaClasses.h"
#include "Common/Xml.h"
#include "Common/IO.h"
#include "Compiler/Java/Exceptions.h"
#include "Compiler/Java/JNIStringRef.h"

void JNICALL NATIVE_drunkfly_Tileset_loadXml(JNIEnv* env, jobject self, jstring fileName)
{
    JTRY
        JNIStringRef fileNameRef{fileName, JNIRef::Unknown};
        auto path = fileNameRef.toPath();

        auto xml = xmlLoad(path);
        ROOT(tileset);

        const auto& version = REQ_STRING(version, tileset);
        const auto& tiledVersion = REQ_STRING(tiledversion, tileset);
        const auto& name = REQ_STRING(name, tileset);
        int tileWidth = REQ_INT(tilewidth, tileset);
        int tileHeight = REQ_INT(tileheight, tileset);
        int tileCount = REQ_INT(tilecount, tileset);
        int columnCount = REQ_INT(columns, tileset);

        JavaClasses::drunkfly_Tileset.setTileWidth(self, tileWidth);
        JavaClasses::drunkfly_Tileset.setTileHeight(self, tileHeight);
        JavaClasses::drunkfly_Tileset.setTileCount(self, tileCount);
        JavaClasses::drunkfly_Tileset.setColumnCount(self, columnCount);

        REQ_ELEMENT(image, tileset) {
            const auto& source = REQ_STRING(source, image);
            int imageWidth = REQ_INT(width, image);
            int imageHeight = REQ_INT(height, image);

            std::filesystem::path imagePath = path;
            imagePath.remove_filename();
            imagePath = imagePath / pathFromUtf8(REQ_STRING(source, image));
            JavaClasses::drunkfly_Tileset.setImagePath(self, JNIStringRef::from(imagePath).toJNI());
        }

        FOR_EACH(tile, tileset) {
            int id = REQ_INT(id, tile);
            std::string stringId;

            IF_HAS(properties, tile) {
                FOR_EACH(property, properties) {
                    const auto& name = REQ_STRING(name, property);
                    const auto& value = REQ_STRING(value, property);

                    if (name == "id")
                        stringId = value;
                    else
                        INVALID(name, property);
                }
            }

            int x = id % columnCount;
            int y = id / columnCount;
            JavaClasses::drunkfly_Tileset.setTile(self, x, y, JNIStringRef::from(stringId).toJNI());
        }

        xmlCheckAllAccessed(xml);
    JCATCH
}
