//
// PLY (Stanford Triangle Format) mesh loader
//
// Supports:
//   - ASCII, binary little-endian, binary big-endian
//   - Per-vertex positions (x, y, z)
//   - Per-vertex normals (nx, ny, nz) — optional
//   - Per-vertex texture coordinates (s/u/texture_u, t/v/texture_v) — optional
//   - Triangles and quads (quads are fan-triangulated)
//   - Arbitrary unknown properties are skipped
//
// References:
//   http://paulbourke.net/dataformats/ply/
//

#include <cstdint>
#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "TriangleMesh.h"
#include "slab.h"

namespace {

enum class PlyFormat { ASCII, BINARY_LE, BINARY_BE };

enum class PlyType { INT8, UINT8, INT16, UINT16, INT32, UINT32, FLOAT32, FLOAT64 };

static PlyType parsePlyType(const std::string & s)
{
    if(s == "char"   || s == "int8")    return PlyType::INT8;
    if(s == "uchar"  || s == "uint8")   return PlyType::UINT8;
    if(s == "short"  || s == "int16")   return PlyType::INT16;
    if(s == "ushort" || s == "uint16")  return PlyType::UINT16;
    if(s == "int"    || s == "int32")   return PlyType::INT32;
    if(s == "uint"   || s == "uint32")  return PlyType::UINT32;
    if(s == "float"  || s == "float32") return PlyType::FLOAT32;
    if(s == "double" || s == "float64") return PlyType::FLOAT64;
    throw std::runtime_error("PLY: Unknown type: " + s);
}

static size_t plyTypeSize(PlyType t)
{
    switch(t) {
        case PlyType::INT8:   case PlyType::UINT8:              return 1;
        case PlyType::INT16:  case PlyType::UINT16:             return 2;
        case PlyType::INT32:  case PlyType::UINT32:
        case PlyType::FLOAT32:                                   return 4;
        case PlyType::FLOAT64:                                   return 8;
    }
    return 0;
}

struct PlyProperty {
    std::string name;
    PlyType type     = PlyType::FLOAT32;
    bool    isList   = false;
    PlyType countType = PlyType::UINT8;   // type of list-length prefix
};

struct PlyElement {
    std::string              name;
    size_t                   count = 0;
    std::vector<PlyProperty> properties;

    int findProp(const std::string & n) const {
        for(int i = 0; i < (int)properties.size(); ++i)
            if(properties[i].name == n) return i;
        return -1;
    }
};

// Read a single typed value from a binary stream, with optional byte-swap.
static double readBinVal(std::istream & in, PlyType type, bool swap)
{
    const size_t sz = plyTypeSize(type);
    uint8_t buf[8] = {};
    in.read(reinterpret_cast<char *>(buf), sz);
    if(swap) {
        for(size_t i = 0, j = sz - 1; i < j; ++i, --j)
            std::swap(buf[i], buf[j]);
    }
    switch(type) {
        case PlyType::INT8:    { int8_t   v; memcpy(&v, buf, sz); return double(v); }
        case PlyType::UINT8:   { uint8_t  v; memcpy(&v, buf, sz); return double(v); }
        case PlyType::INT16:   { int16_t  v; memcpy(&v, buf, sz); return double(v); }
        case PlyType::UINT16:  { uint16_t v; memcpy(&v, buf, sz); return double(v); }
        case PlyType::INT32:   { int32_t  v; memcpy(&v, buf, sz); return double(v); }
        case PlyType::UINT32:  { uint32_t v; memcpy(&v, buf, sz); return double(v); }
        case PlyType::FLOAT32: { float    v; memcpy(&v, buf, sz); return double(v); }
        case PlyType::FLOAT64: { double   v; memcpy(&v, buf, sz); return double(v); }
    }
    return 0.0;
}

} // namespace

bool loadTriangleMeshFromPLY(TriangleMesh & mesh,
                             MaterialArray & materials,
                             TriangleMeshDataCache & meshDataCache,
                             TextureCache & textureCache,
                             const std::string & path, const std::string & filename)
{
    const std::string fullPath = path.empty() ? filename : path + "/" + filename;

    // Open in binary mode so the stream position after the header is exact
    // for binary-format files. ASCII text works fine in binary mode too.
    std::ifstream file(fullPath, std::ios::binary);
    if(!file) {
        std::cerr << "PLY: Cannot open " << fullPath << '\n';
        return false;
    }

    // ---- Parse header (always ASCII) ----

    auto readLine = [&]() -> std::string {
        std::string line;
        std::getline(file, line);
        if(!line.empty() && line.back() == '\r') line.pop_back(); // handle CRLF
        return line;
    };

    if(readLine() != "ply") {
        std::cerr << "PLY: Not a PLY file: " << fullPath << '\n';
        return false;
    }

    PlyFormat              format = PlyFormat::ASCII;
    std::vector<PlyElement> elements;

    for(std::string line = readLine(); line != "end_header"; line = readLine()) {
        if(file.eof()) {
            std::cerr << "PLY: Unexpected end of file in header\n";
            return false;
        }
        std::istringstream iss(line);
        std::string kw;
        iss >> kw;

        if(kw == "format") {
            std::string fmt; iss >> fmt;
            if     (fmt == "ascii")                format = PlyFormat::ASCII;
            else if(fmt == "binary_little_endian") format = PlyFormat::BINARY_LE;
            else if(fmt == "binary_big_endian")    format = PlyFormat::BINARY_BE;
        }
        else if(kw == "element") {
            PlyElement e;
            iss >> e.name >> e.count;
            elements.push_back(e);
        }
        else if(kw == "property" && !elements.empty()) {
            PlyProperty p;
            std::string typeStr; iss >> typeStr;
            if(typeStr == "list") {
                p.isList = true;
                std::string cntStr, valStr;
                iss >> cntStr >> valStr;
                p.countType = parsePlyType(cntStr);
                p.type      = parsePlyType(valStr);
            } else {
                p.type = parsePlyType(typeStr);
            }
            iss >> p.name;
            elements.back().properties.push_back(p);
        }
        // Silently ignore: comment, obj_info, and any unknown keywords
    }

    // ---- Locate vertex and face elements ----

    PlyElement * vertElem = nullptr;
    for(auto & e : elements) {
        if(e.name == "vertex") { vertElem = &e; break; }
    }

    if(!vertElem) {
        std::cerr << "PLY: No vertex element found\n";
        return false;
    }

    // Identify vertex property indices
    const int xi  = vertElem->findProp("x"),  yi  = vertElem->findProp("y"),  zi  = vertElem->findProp("z");
    const int nxi = vertElem->findProp("nx"), nyi = vertElem->findProp("ny"), nzi = vertElem->findProp("nz");

    int si = vertElem->findProp("s");
    if(si < 0) si = vertElem->findProp("u");
    if(si < 0) si = vertElem->findProp("texture_u");
    int ti = vertElem->findProp("t");
    if(ti < 0) ti = vertElem->findProp("v");
    if(ti < 0) ti = vertElem->findProp("texture_v");

    if(xi < 0 || yi < 0 || zi < 0) {
        std::cerr << "PLY: Vertex element is missing x/y/z properties\n";
        return false;
    }

    const bool hasNormals   = (nxi >= 0 && nyi >= 0 && nzi >= 0);
    const bool hasTexCoords = (si  >= 0 && ti  >= 0);

    // Determine whether byte-swapping is needed for binary formats
    const uint32_t endianProbe = 1;
    const bool sysIsLE = (*reinterpret_cast<const char *>(&endianProbe) == 1);
    const bool swapEndian = (format == PlyFormat::BINARY_LE && !sysIsLE)
                          || (format == PlyFormat::BINARY_BE &&  sysIsLE);

    // ---- Unified per-property reader ----
    // For ASCII: beginRow() reads the next line into a stream; readVal() parses a token.
    // For binary: beginRow() is a no-op; readVal() reads bytes from the file.

    std::string       asciiLine;
    std::istringstream asciiStream;

    auto beginRow = [&]() {
        if(format == PlyFormat::ASCII) {
            asciiLine  = readLine();
            asciiStream = std::istringstream(asciiLine);
        }
    };

    auto readVal = [&](PlyType type) -> double {
        if(format == PlyFormat::ASCII) {
            double v; asciiStream >> v; return v;
        }
        return readBinVal(file, type, swapEndian);
    };

    auto skipList = [&](const PlyProperty & p) {
        const int n = int(readVal(p.countType));
        for(int j = 0; j < n; ++j) readVal(p.type);
    };

    auto skipRow = [&](const PlyElement & elem) {
        beginRow();
        for(const auto & p : elem.properties) {
            if(p.isList) skipList(p);
            else         readVal(p.type);
        }
    };

    // ---- Build mesh data ----

    mesh.meshData = std::make_shared<TriangleMeshData>();
    auto & md = *mesh.meshData;

    // Process elements in file order
    for(const auto & elem : elements) {

        if(elem.name == "vertex") {
            md.vertices.reserve(elem.count);
            if(hasNormals)   md.normals.reserve(elem.count);
            if(hasTexCoords) md.texcoords.reserve(elem.count);

            for(size_t vi = 0; vi < elem.count; ++vi) {
                beginRow();
                float x = 0, y = 0, z = 0;
                float nx = 0, ny = 0, nz = 0;
                float s = 0, t = 0;
                for(int pi = 0; pi < (int)elem.properties.size(); ++pi) {
                    const auto & p = elem.properties[pi];
                    if(p.isList) { skipList(p); continue; }
                    const double v = readVal(p.type);
                    if     (pi == xi)  x  = float(v);
                    else if(pi == yi)  y  = float(v);
                    else if(pi == zi)  z  = float(v);
                    else if(pi == nxi) nx = float(v);
                    else if(pi == nyi) ny = float(v);
                    else if(pi == nzi) nz = float(v);
                    else if(pi == si)  s  = float(v);
                    else if(pi == ti)  t  = float(v);
                }
                md.vertices.push_back(Position3(x, y, z));
                if(hasNormals) {
                    Direction3 n(nx, ny, nz);
                    n.normalize();
                    md.normals.push_back(n);
                }
                if(hasTexCoords) md.texcoords.push_back(TextureCoordinate{s, t});
            }

            md.bounds = boundingBox(md.vertices);
        }

        else if(elem.name == "face") {
            // Find the vertex index list property
            int faceListIdx = elem.findProp("vertex_indices");
            if(faceListIdx < 0) faceListIdx = elem.findProp("vertex_index");

            for(size_t fi = 0; fi < elem.count; ++fi) {
                beginRow();
                std::vector<uint32_t> faceVerts;

                for(int pi = 0; pi < (int)elem.properties.size(); ++pi) {
                    const auto & p = elem.properties[pi];
                    if(p.isList) {
                        const int n = int(readVal(p.countType));
                        if(pi == faceListIdx) {
                            faceVerts.resize(n);
                            for(int j = 0; j < n; ++j)
                                faceVerts[j] = uint32_t(readVal(p.type));
                        } else {
                            for(int j = 0; j < n; ++j) readVal(p.type);
                        }
                    } else {
                        readVal(p.type); // skip scalar face properties
                    }
                }

                if(faceVerts.size() < 3) continue;

                // Fan-triangulate: (0,1,2), (0,2,3), (0,3,4), ...
                for(size_t i = 1; i + 1 < faceVerts.size(); ++i) {
                    const uint32_t a = faceVerts[0];
                    const uint32_t b = faceVerts[i];
                    const uint32_t c = faceVerts[i + 1];

                    md.indices.vertex.push_back(a);
                    md.indices.vertex.push_back(b);
                    md.indices.vertex.push_back(c);

                    // Normal indices mirror vertex indices (normals are per-vertex)
                    if(hasNormals) {
                        md.indices.normal.push_back(a);
                        md.indices.normal.push_back(b);
                        md.indices.normal.push_back(c);
                    }

                    // Texcoord indices mirror vertex indices, or NoTexCoord
                    const uint32_t noTc = TriangleMeshData::NoTexCoord;
                    md.indices.texcoord.push_back(hasTexCoords ? a : noTc);
                    md.indices.texcoord.push_back(hasTexCoords ? b : noTc);
                    md.indices.texcoord.push_back(hasTexCoords ? c : noTc);

                    md.faces.material.push_back(NoMaterial);
                }
            }
        }

        else {
            // Unknown element — skip all rows
            for(size_t i = 0; i < elem.count; ++i)
                skipRow(elem);
        }
    }

    printf("PLY Mesh: vertices=%d normals=%d texcoords=%d triangles=%d\n",
           (int)md.vertices.size(), (int)md.normals.size(),
           (int)md.texcoords.size(), (int)(md.indices.vertex.size() / 3));
    printf("Mesh bounds: "); md.bounds.print();

    return true;
}
