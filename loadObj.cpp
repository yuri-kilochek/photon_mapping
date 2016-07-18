#include "loadObj.hpp"

#include <QRegularExpression>
#include <QTextStream>
#include <QStringList>
#include <QDir>
#include <QFile>

#include "Material.hpp"

namespace photon_mapping {
    static void loadMtl(std::map<QString, std::shared_ptr<Material>>& materials, QTextStream& textStream, QString const& dirPath) {
        std::shared_ptr<Material> material;
        while (!textStream.atEnd()) {
            QString line = textStream.readLine();
            line.remove(QRegularExpression("#.*"));
            line.replace('\t', ' ');
            QStringList l = line.split(' ', QString::SkipEmptyParts);

            if (l.isEmpty()) {
                continue;
            }

            if (l[0] == "newmtl") {
                material = std::make_shared<Material>();
                QString materialName = l[1];
                materials[materialName] = material;
                continue;
            }

            if (l[0] == "Ke") {
                float r = l[1].toFloat();
                float g = l[2].toFloat();
                float b = l[3].toFloat();
                material->setEmission(std::make_shared<Texture<TextureType::TrueColor>>(Color(r, g, b)));
                continue;
            }

            if (l[0] == "map_Ke") {
                QString mapFilePath = QDir::cleanPath(QDir(dirPath).filePath(l.last()));
                material->setEmission(std::make_shared<Texture<TextureType::TrueColor>>(mapFilePath));
                continue;
            }

            if (l[0] == "Pe") {
                float ep = l[1].toFloat();
                material->setEmissionPower(ep);
                continue;
            }

            if (l[0] == "Kd") {
                float r = l[1].toFloat();
                float g = l[2].toFloat();
                float b = l[3].toFloat();
                material->setDiffuse(std::make_shared<Texture<TextureType::TrueColor>>(Color(r, g, b)));
                continue;
            }

            if (l[0] == "map_Kd") {
                QString mapFilePath = QDir::cleanPath(QDir(dirPath).filePath(l.last()));
                material->setDiffuse(std::make_shared<Texture<TextureType::TrueColor>>(mapFilePath));
                continue;
            }

            if (l[0] == "Ks") {
                float r = l[1].toFloat();
                float g = l[2].toFloat();
                float b = l[3].toFloat();
                material->setSpecular(std::make_shared<Texture<TextureType::TrueColor>>(Color(r, g, b)));
                continue;
            }

            if (l[0] == "map_Ks") {
                QString mapFilePath = QDir::cleanPath(QDir(dirPath).filePath(l.last()));
                material->setSpecular(std::make_shared<Texture<TextureType::TrueColor>>(mapFilePath));
                continue;
            }

            if (l[0] == "Ns") {
                float sp = l[1].toFloat();
                material->setSpecularPower(sp);
                continue;
            }

            if (l[0] == "Ni") {
                float ri = l[1].toFloat();
                material->setRefractionIndex(ri);
                continue;
            }

            if (l[0] == "Tf") {
                float r = l[1].toFloat();
                float g = l[2].toFloat();
                float b = l[3].toFloat();
                material->setTransmission(Color(r, g, b));
                continue;
            }

            if (l[0] == "bump") {
                QString mapFilePath = QDir::cleanPath(QDir(dirPath).filePath(l.last()));
                material->setNormal(std::make_shared<Texture<TextureType::NormalMap>>(mapFilePath));
                continue;
            }
        }
    }
    
    static void loadMtl(std::map<QString, std::shared_ptr<Material>>& materials, QString const& mtlFilePath) {
        QFile file(mtlFilePath);
        if (file.open(QFile::ReadOnly | QFile::Text)) {
            QTextStream textStream(&file);
            loadMtl(materials, textStream, QDir(mtlFilePath).filePath("../"));
        }
    }

    static void loadObj(std::shared_ptr<Scene> const& scene, QTextStream& textStream, QString const& dirPath) {
        std::vector<Point3f> vertices;
        std::vector<Vector3f> tangents;
        std::vector<Vector3f> bitangents;
        std::vector<Vector3f> normals;
        std::vector<Point2f> texcoords;
        std::map<QString, std::shared_ptr<Material>> materials;
        int currentMaterialIndex = -1;
        struct Face {
            int vertices[3];
            int texcoords[3];
            int spaces[3];
            int materialIndex;
        };
        std::map<QString, std::vector<Face>> objects;
        std::vector<Face>* currentObjectFaces = nullptr;
        
        while (!textStream.atEnd()) {
            QString line = textStream.readLine();
            line.remove(QRegularExpression("#.*"));
            line.replace('\t', ' ');
            QStringList l = line.split(' ', QString::SkipEmptyParts);

            if (l.isEmpty()) {
                continue;
            }

            if (l[0] == "v") {
                float x = l[1].toFloat();
                float y = l[2].toFloat();
                float z = l[3].toFloat();
                vertices.push_back(Point3f(x, y, z));
                continue;
            }

            if (l[0] == "vn") {
                float x = l[1].toFloat();
                float y = l[2].toFloat();
                float z = l[3].toFloat();
                normals.push_back(Vector3f(x, y, z));
                continue;
            }

            if (l[0] == "vt") {
                float x = l[1].toFloat();
                while (x < 0.0f) {
                    x += 1.0f;
                }
                while (x > 1.0f) {
                    x -= 1.0f;
                }
                float y = l[2].toFloat();
                while (y < 0.0f) {
                    y += 1.0f;
                }
                while (y > 1.0f) {
                    y -= 1.0f;
                }
                texcoords.push_back(Point2f(x, y));
                continue;
            }

            if (l[0] == "f") {
                if (!currentObjectFaces) {
                    currentObjectFaces = &objects[""];
                }
                switch (l.length() - 1) {
                    case 4: {
                        Face face;
                        for (int i = 0; i < 3; ++i) {
                            int j = (2 + i) % 4;
                            QStringList fi = (l[1 + j] + "//").split("/");
                            face.vertices[i] = fi[0].toInt();
                            if (face.vertices[i] < 0) {
                                face.vertices[i] += vertices.size() + 1;
                            }
                            face.texcoords[i] = (fi[1] == "") ? 0 : fi[1].toInt();
                            if (face.texcoords[i] < 0) {
                                face.texcoords[i] += texcoords.size() + 1;
                            }
                            face.spaces[i] = (fi[2] == "") ? 0 : fi[2].toInt();
                            if (face.spaces[i] < 0) {
                                face.spaces[i] += normals.size() + 1;
                            }
                        }
                        face.materialIndex = currentMaterialIndex;
                        currentObjectFaces->push_back(face);
                    }
                    case 3: {
                        Face face;
                        for (int i = 0; i < 3; ++i) {
                            QStringList fi = (l[1 + i] + "//").split("/");
                            face.vertices[i] = fi[0].toInt();
                            if (face.vertices[i] < 0) {
                                face.vertices[i] += vertices.size() + 1;
                            }
                            face.texcoords[i] = (fi[1] == "") ? 0 : fi[1].toInt();
                            if (face.texcoords[i] < 0) {
                                face.texcoords[i] += texcoords.size() + 1;
                            }
                            face.spaces[i] = (fi[2] == "") ? 0 : fi[2].toInt();
                            if (face.spaces[i] < 0) {
                                face.spaces[i] += normals.size() + 1;
                            }
                        }
                        face.materialIndex = currentMaterialIndex;
                        currentObjectFaces->push_back(face);
                    }
                }
                continue;
            }

            if (l[0] == "usemtl") {
                QString materialName = l[1];
                auto materialIt = materials.find(materialName);
                if (materialIt != materials.end()) {
                    currentMaterialIndex = std::distance(materials.begin(), materialIt);
                } else {
                    currentMaterialIndex = -1;
                }
                continue;
            }

            if (l[0] == "mtllib") {
                for (int i = 1; i < l.length(); ++i) {
                    QString mtlFilePath = QDir::cleanPath(QDir(dirPath).filePath(l[i]));
                    loadMtl(materials, mtlFilePath);
                }
                continue;
            }

            if (l[0] == "o") {
                currentObjectFaces = &objects[l[1]];
                continue;
            }
        }

        for (auto& object : objects) {
            auto& objectFaces = object.second;
            std::vector<Mesh::Triangle> objectTriangles(objectFaces.size()); {
                for (int i = 0; i < objectTriangles.size(); ++i) {
                    objectTriangles[i].vertex = vertices[objectFaces[i].vertices[0] - 1];
                    objectTriangles[i].edge01 = vertices[objectFaces[i].vertices[1] - 1] - objectTriangles[i].vertex;
                    objectTriangles[i].edge02 = vertices[objectFaces[i].vertices[2] - 1] - objectTriangles[i].vertex;
                    for (int j = 0; j < 3; ++j) {
                        if (objectFaces[i].texcoords[j] != 0) {
                            objectTriangles[i].texcoords[j] = texcoords[objectFaces[i].texcoords[j] - 1];
                        } else {
                            objectTriangles[i].texcoords[j] = Point2f::ORIGIN;
                        }
                    }
                    objectTriangles[i].materialIndex = objectFaces[i].materialIndex;
                }
                tangents.resize(normals.size(), Vector3f::ZERO);
                bitangents.resize(normals.size(), Vector3f::ZERO);
                for (int i = 0; i < objectFaces.size(); ++i) {
                    Vector2f texcoordEdge01 = objectTriangles[i].texcoords[1] - objectTriangles[i].texcoords[0];
                    Vector2f texcoordEdge02 = objectTriangles[i].texcoords[2] - objectTriangles[i].texcoords[0];
                    float d = texcoordEdge01.x * texcoordEdge02.y - texcoordEdge01.y * texcoordEdge02.x;
                    if (d != 0.0f) {
                        float id = 1.0f / d;
                        Vector3f tangent = (objectTriangles[i].edge01 * (+texcoordEdge02.y) + objectTriangles[i].edge02 * (-texcoordEdge01.y)) * id;
                        Vector3f bitangent = (objectTriangles[i].edge01 * (-texcoordEdge02.x) + objectTriangles[i].edge02 * (+texcoordEdge01.x)) * id;
                        for (int j = 0; j < 3; ++j) {
                            if (objectFaces[i].spaces[j] != 0) {
                                tangents[objectFaces[i].spaces[j] - 1] += tangent;
                                bitangents[objectFaces[i].spaces[j] - 1] += bitangent;
                            }
                        }
                    }
                }
                for (int i = 0; i < objectTriangles.size(); ++i) {
                    for (int j = 0; j < 3; ++j) {
                        Vector3f normal;
                        if (objectFaces[i].spaces[j] != 0) {
                            normal = normals[objectFaces[i].spaces[j] - 1];
                        } else {
                            normal = unit(cross(objectTriangles[i].edge01, objectTriangles[i].edge02));
                        }
                        Vector3f tangent;
                        if (objectFaces[i].spaces[j] != 0 && sqrLength(tangents[objectFaces[i].spaces[j] - 1]) > 0.0f) {
                            tangent = tangents[objectFaces[i].spaces[j] - 1];
                            tangent = unit(tangent - normal * dot(normal, tangent));
                        } else {
                            tangent = unit(objectTriangles[i].edge01);
                        }
                        Vector3f bitangent;
                        if (objectFaces[i].spaces[j] != 0 && sqrLength(bitangents[objectFaces[i].spaces[j] - 1]) > 0.0f) {
                            bitangent = bitangents[objectFaces[i].spaces[j] - 1];
                            bitangent = unit(bitangent - normal * dot(normal, bitangent));
                        } else {
                            bitangent = cross(normal, tangent);
                        }
                        objectTriangles[i].tangents[j] = tangent;
                        objectTriangles[i].bitangents[j] = bitangent;
                        objectTriangles[i].normals[j] = normal; 
                    }
                }
            }
            scene->addObject([&](QString const& objectName){
                auto object = std::make_shared<Object>();
                object->setName(objectName);
                std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>(objectTriangles.begin(), objectTriangles.end());
                object->setMesh(mesh);
                for (auto& material = materials.begin(); material != materials.end(); ++material) {
                    object->setMaterial(std::distance(materials.begin(), material), material->second);
                }
                return object;
            }(object.first));
        }
    }

    void loadObj(std::shared_ptr<Scene> const& scene, QString const& objFilePath) {
        QFile file(objFilePath);
        if (file.open(QFile::ReadOnly | QFile::Text)) {
            QTextStream textStream(&file);
            loadObj(scene, textStream, QDir(objFilePath).filePath("../"));
        }
    }
}