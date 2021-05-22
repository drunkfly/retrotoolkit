#include "Compiler/Java/JavaClasses.h"
#include "Compiler/Java/JNIStringRef.h"
#include "Compiler/Java/Exceptions.h"
#include "Common/IO.h"
#include "Common/Xml.h"

namespace
{
    struct Tileset
    {
        int firstId;
        int lastId;
        JNIRef ref;
    };
}

void JNICALL NATIVE_drunkfly_Tilemap_loadXml(JNIEnv* env, jobject self, jstring fileName)
{
    JTRY
        JNIStringRef fileNameRef{fileName, JNIRef::Unknown};
        auto path = fileNameRef.toPath();

        auto xml = xmlLoad(path);
        ROOT(map);

        const auto& version = REQ_STRING(version, map);
        const auto& tiledVersion = REQ_STRING(tiledversion, map);
        const auto& orientation = REQ_STRING(orientation, map);
        const auto& renderOrder = REQ_STRING(renderorder, map);
        int width = REQ_INT(width, map);
        int height = REQ_INT(height, map);
        int tileWidth = REQ_INT(tilewidth, map);
        int tileHeight = REQ_INT(tileheight, map);
        int infinite = REQ_INT(infinite, map);
        int nextLayerId = REQ_INT(nextlayerid, map);
        int nextObjectId = REQ_INT(nextobjectid, map);

        if (orientation != "orthogonal")
            INVALID(orientation, map);
        if (renderOrder != "left-up")
            INVALID(renderorder, map);
        if (infinite != 0)
            INVALID(infinite, map);

        JavaClasses::drunkfly_Tilemap.setWidth(self, width);
        JavaClasses::drunkfly_Tilemap.setHeight(self, height);
        JavaClasses::drunkfly_Tilemap.setTileWidth(self, tileWidth);
        JavaClasses::drunkfly_Tilemap.setTileHeight(self, tileHeight);

        std::vector<Tileset> tilesets;

        FOR_EACH(tileset, map) {
            std::filesystem::path tilesetPath = path;
            tilesetPath.remove_filename();
            tilesetPath = tilesetPath / pathFromUtf8(REQ_STRING(source, tileset));

            Tileset tileset;
            tileset.ref = JavaClasses::drunkfly_Tileset.construct(JNIStringRef::from(tilesetPath).toJNI());
            tileset.firstId = REQ_INT(firstgid, tileset);
            tileset.lastId = tileset.firstId + JavaClasses::drunkfly_Tileset.getTileCount(tileset.ref.toJNI()) - 1;
            tilesets.emplace_back(std::move(tileset));
        }

        FOR_EACH(layer, map) {
            int id = REQ_INT(id, layer);
            const auto& name = REQ_STRING(name, layer);
            int layerWidth = REQ_INT(width, layer);
            int layerHeight = REQ_INT(height, layer);
            if (layerWidth != width)
                INVALID(width, layer);
            if (layerHeight != height)
                INVALID(height, layer);

            JNIRef layer = JavaClasses::drunkfly_Tilemap.addLayer(self, id, JNIStringRef::from(name).toJNI());

            REQ_ELEMENT(data, layer) {
                const auto& encoding = REQ_STRING(encoding, data);
                if (encoding != "csv")
                    INVALID(encoding, data);

                std::unique_ptr<int[]> data{ new int[layerWidth * layerHeight] };

                const char* text = REQ_TEXT(data);
                int off = 0;
                while (*text) {
                    const char* str = text;
                    char* p = (char*)strchr(text, ',');
                    if (!p)
                        text += strlen(text);
                    else {
                        *p = 0;
                        text = p + 1;
                    }

                    data[off++] = atoi(str);
                }

                for (int y = 0; y < layerHeight; y++) {
                    for (int x = 0; x < layerWidth; x++) {
                        if (data[y * layerWidth + x] == 0) {
                            std::stringstream ss;
                            ss << "Missing tile at (" << x << "," << y << ").";
                            throw std::runtime_error(ss.str());
                        }

                        int tileIndex = data[y * layerWidth + x];
                        const Tileset* tileset = nullptr;
                        for (const auto& ts : tilesets) {
                            if (tileIndex >= ts.firstId && tileIndex <= ts.lastId) {
                                tileset = &ts;
                                break;
                            }
                        }

                        if (!tileset) {
                            std::stringstream ss;
                            ss << "Invalid tile index " << tileIndex << " at (" << x << "," << y << ").";
                            throw std::runtime_error(ss.str());
                        }

                        tileIndex -= tileset->firstId;

                        JNIRef tile = JavaClasses::drunkfly_Tileset.getTile(tileset->ref.toJNI(), tileIndex);
                        if (!tile)
                            tile = JavaClasses::drunkfly_Tileset.setTile(tileset->ref.toJNI(), tileIndex, nullptr);

                        JavaClasses::drunkfly_Tilemap_Layer.setTile(layer.toJNI(), x, y, tile.toJNI());
                    }
                }
            }
        }

        FOR_EACH(objectgroup, map) {
            int groupId = REQ_INT(id, objectgroup);
            const auto& groupName = REQ_STRING(name, objectgroup);

            FOR_EACH(object, objectgroup) {
                int objectId = REQ_INT(id, object);
                const auto& objectName = REQ_STRING(name, object);
                int objectX = REQ_INT(x, object);
                int objectY = REQ_INT(y, object);

                REQ_ELEMENT(point, object);

                JavaClasses::drunkfly_Tilemap.addObject(self,
                    objectId, JNIStringRef::from(objectName).toJNI(), objectX, objectY);
            }
        }

        xmlCheckAllAccessed(xml);
    JCATCH
}
